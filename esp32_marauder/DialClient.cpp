#include "DIALClient.h"

DIALClient::DIALClient(String& ssid, String& password, String& app, String& YTUrl) : ssid(ssid), password(password), App(app), YTUrl(YTUrl){}

String extractMAC(const String& str) {
    int startIndex = str.indexOf("MAC=") + 4;
    int endIndex = str.indexOf(";", startIndex);

    if (startIndex >= 4 && endIndex != -1) {
        return str.substring(startIndex, endIndex);
    } else {
        return "";
    }
}

String extractJSON(const String& response) {
  // Find the end of the headers
  int startIndex = response.indexOf("\r\n\r\n");
  if (startIndex == -1) return ""; // Headers not found
  
  // Skip headers
  startIndex += 4;

  // Skip chunk sizes (assuming they are always 4 characters long for simplicity)
  startIndex += 4;

  // Find the end of the JSON content
  int endIndex = response.lastIndexOf("]");

  // Extract the JSON
  return response.substring(startIndex, endIndex + 1);
}

String zx() {
      String result = "";
      const char* characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      const int charactersLength = strlen(characters);
      const int stringLength = 12;

      for (int i = 0; i < stringLength; i++) {
        char randomChar = characters[random(0, charactersLength)];
        result += randomChar;
      }

      return result;
}

String generateUUID() {
  String uuid = "";

  
  randomSeed(analogRead(0) + millis());

  for (int i = 0; i < 4; i++) {
    uuid += String(random(0xFFFFFFF), HEX);
  }

  return uuid;
}

std::string getRandomString(const std::vector<std::string>& vec) {
    
    size_t randomIndex = random(0, vec.size());

    
    return vec[randomIndex];
}

const int MAX_RETRIES = 7;
const int RETRY_DELAY = 2000;

bool DIALClient::fetchScreenIdWithRetries(const String& applicationUrl, Device& device) {
    for (int i = 0; i < MAX_RETRIES; i++) {
        checkYouTubeAppStatus(applicationUrl, device);
        
        if (!device.screenID.isEmpty()) {
          String Token = getYouTubeToken(device.screenID);
          device.YoutubeToken = Token;
          BindSessionID(device);
          return true;
        } else {
            Serial.println("Screen ID is Empty. Retrying...");
            delay(RETRY_DELAY);
        }
    }
    
    Serial.println("Failed to fetch Screen ID after max retries.");
    return false;
}

String DIALClient::extractApplicationURL(HttpClient& httpc) {
    String appUrl;
    char headerChar;
    String currentLine = "";
    int consecutiveNewlines = 0; 
    int maxHeadersRead = 1000;
    int readCount = 0;

    while (httpc.connected() && readCount < maxHeadersRead) {
        headerChar = httpc.readHeader();
        if (headerChar == '\n') {
            consecutiveNewlines++;
            
            currentLine.trim();
            currentLine.toLowerCase();
            if (currentLine.startsWith("application-url:")) {
                appUrl = currentLine.substring(currentLine.indexOf(':') + 2); // +2 to skip over the colon and potential space
                break;
            }
            currentLine = "";

            if (consecutiveNewlines >= 2) {
               
                break;
            }
        } else if (headerChar != '\r') {
            consecutiveNewlines = 0;  
            currentLine += headerChar;
        }
        readCount++;
    }
    return appUrl;
}

String extractPathFromURL(const String& url) {
    int doubleSlashPos = url.indexOf("//");
    
   
    if (doubleSlashPos == -1) {
        doubleSlashPos = 0;
    } else {
        doubleSlashPos += 2;
    }

    int slashAfterHost = url.indexOf('/', doubleSlashPos);

    
    if (slashAfterHost == -1) {
        return "/";
    }

    return url.substring(slashAfterHost);
}

int extractTimeout(const String& str) {
    int startIndex = str.indexOf("Timeout=") + 8;
    int endIndex = str.indexOf(";", startIndex);  

    if (startIndex >= 8 && endIndex != -1) {
        return str.substring(startIndex, endIndex).toInt();
    } else {
        return -1;
    }
}

bool DIALClient::convertMacStringToBytes(const String& macString, uint8_t* macBytes) {

    String NewMac = extractMAC(macString);


    if (NewMac.length() != 17) {
        return false;
    }

    for (int i = 0; i < 6; i++) {
        macBytes[i] = strtol(NewMac.substring(i*3, i*3+2).c_str(), NULL, 16);
    }
    return true;
}

void DIALClient::connectWiFi() {
    Serial.println("[connectWiFi] Initiating WiFi connection.");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("[connectWiFi] Connecting to WiFi...");
    }
    Serial.println("[connectWiFi] Connected to WiFi.");
    secureClient.setInsecure();

    exploreNetwork();
}

void DIALClient::Execute() {
  Serial.println("[Execute] Starting execution.");
  connectWiFi();
}


bool DIALClient::parseSSDPResponse(const String& response, Device& device) {
    Serial.println("[parseSSDPResponse] Parsing SSDP response.");

    if (response.length() == 0) {
        return false;
    }

    
    device.location = "";
    device.uniqueServiceName = "";
    device.wakeup = "";

    
    int idx = 0;
    while (idx < response.length()) {
        int nextIdx = response.indexOf('\n', idx);
        if (nextIdx == -1) {
            nextIdx = response.length();
        }

        String line = response.substring(idx, nextIdx);
        idx = nextIdx + 1;

        line.trim();

        if (line.startsWith("LOCATION:")) {
            device.location = line.substring(9);
            device.location.trim();
        } else if (line.startsWith("USN:")) {
            device.uniqueServiceName = line.substring(4);
            device.uniqueServiceName.trim();
        } else if (line.startsWith("WAKEUP:")) {
            device.wakeup = line.substring(7);
            device.wakeup.trim();
        }
    }
    return !device.location.isEmpty() && !device.uniqueServiceName.isEmpty();
}

std::vector<DIALClient::Device> DIALClient::discoverDevices() {
    const int MAX_RETRIES = 5; 
    const int RETRY_DELAY = 5000;

    std::vector<Device> devices;
     std::set<String> discoveredUSNs;

    const uint8_t mSearchRequest[] = {
    0x4d, 0x2d, 0x53, 0x45, 0x41, 0x52, 0x43, 0x48, 0x20, 0x2a, 0x20, 0x48, 0x54, 0x54, 0x50, 0x2f,
    0x31, 0x2e, 0x31, 0x0d, 0x0a, 0x48, 0x4f, 0x53, 0x54, 0x3a, 0x20, 0x32, 0x33, 0x39, 0x2e, 0x32,
    0x35, 0x35, 0x2e, 0x32, 0x35, 0x35, 0x2e, 0x32, 0x35, 0x30, 0x3a, 0x31, 0x39, 0x30, 0x30, 0x0d,
    0x0a, 0x4d, 0x41, 0x4e, 0x3a, 0x20, 0x22, 0x73, 0x73, 0x64, 0x70, 0x3a, 0x64, 0x69, 0x73, 0x63,
    0x6f, 0x76, 0x65, 0x72, 0x22, 0x0d, 0x0a, 0x53, 0x54, 0x3a, 0x20, 0x75, 0x72, 0x6e, 0x3a, 0x64,
    0x69, 0x61, 0x6c, 0x2d, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x2d,
    0x6f, 0x72, 0x67, 0x3a, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x3a, 0x64, 0x69, 0x61, 0x6c,
    0x3a, 0x31, 0x0d, 0x0a, 0x4d, 0x58, 0x3a, 0x20, 0x33, 0x0d, 0x0a, 0x0d, 0x0a
    };

    int retries = 0;
    while (devices.empty() && retries < MAX_RETRIES) {
        if (retries > 0) {
            Serial.println("[discoverDevices] Retrying device discovery...");
            delay(RETRY_DELAY);
        }

          multicastClient.beginMulticast(IPAddress(239,255,255,250), 1900);
          
          
          for (int i = 0; i < 5; i++) {
              multicastClient.beginPacket(IPAddress(239,255,255,250), 1900);
              multicastClient.write((const uint8_t *)mSearchRequest, sizeof(mSearchRequest));
              multicastClient.endPacket();
              delay(2000); 
          }

          multicastClient.write(mSearchRequest, sizeof(mSearchRequest));

        unsigned long startTime = millis();
        while (millis() - startTime < 5000) {
            if (multicastClient.parsePacket()) {
                int len = multicastClient.available(); 
                char buffer[len + 1];  
                multicastClient.read(buffer, len);  
                buffer[len] = '\0';  

                String response = String(buffer);

                Device device;
                if (parseSSDPResponse(response, device)) {
                  if (discoveredUSNs.find(device.uniqueServiceName) == discoveredUSNs.end()) {
                    
                    devices.push_back(device);
                    discoveredUSNs.insert(device.uniqueServiceName);
                  }
                }
            }
        }
        retries++;
    }


    if (devices.empty()) {
        Serial.println("[discoverDevices] No devices found after all retries.");
    }

    return devices;
}

void DIALClient::exploreNetwork() {
    std::vector<std::string> strings;


    if (YTUrl.isEmpty())
    {
      strings = {"dQw4w9WgXcQ", "k1BneeJTDcU", "fJ9rUzIMcZQ"};
    }
    else 
    {
      strings.push_back(YTUrl.c_str());
    }


    const int maxRetries = 3;
    bool videoPlayedSuccessfully = false;

    for (int attempt = 0; attempt < maxRetries && !videoPlayedSuccessfully; ++attempt) {
        std::vector<Device> devices = discoverDevices();

        if (devices.empty()) {
            Serial.println("No devices discovered. Retrying...");
            continue;
        }

        for (Device& device : devices) {
            Serial.println("Discovered Device at URL: " + device.location);
            Serial.println(device.friendlyName.c_str());
            Serial.println(device.wakeup.c_str());

            uint8_t macBytes[6];
            if (convertMacStringToBytes(device.wakeup, macBytes)) {
                wol.send(macBytes, IPAddress(255,255,255,255), 9, 3, extractTimeout(device.wakeup));

                device.applicationUrl = getDialApplicationUrl(device.location);

                if (!device.applicationUrl.isEmpty()) {
                  if (App == "Youtube")
                  {
                    if (checkYouTubeAppStatus(device.applicationUrl, device) != 200) {
                        Serial.println("Launching Youtube App");
                        launchYouTubeApp(device.applicationUrl);
                    }
                    
                    unsigned long startTime = millis();
                    bool isYouTubeRunning = false;

                    while (millis() - startTime < 5000) {
                        if (checkYouTubeAppStatus(device.applicationUrl, device) == 200) {
                            isYouTubeRunning = true;
                            break;
                        }
                        delay(500);
                    }

                    if (isYouTubeRunning) {
                        if (fetchScreenIdWithRetries(device.applicationUrl, device)) {
                            sendYouTubeCommand("setPlaylist", getRandomString(strings).c_str(), device);
                        }
                    } else {
                        Serial.println("Timeout reached. YouTube app is not running.");
                    }
                  }
                  else if (App == "Netflix")
                  {
                    Serial.println("Launching Netflix App");
                    launchNetflixApp(device.applicationUrl);
                  }
                }
            }
        }
    }

    WiFi.disconnect(true);
    Serial.println("WiFi disconnected.");

    delete this;
}

void DIALClient::BindSessionID(Device& Device)
{
    const char* serverAddress = "www.youtube.com";
    const int port = 443;
    const char* endpoint = "/api/lounge/bc/bind";

    
    if (!secureClient.connect(serverAddress, port)) {
        Serial.println("Connection failed!");
        return;
    }
    
    String UUID = generateUUID();


    String urlParams = "device=REMOTE_CONTROL";
    urlParams += "&mdx-version=3";
    urlParams += "&ui=1";
    urlParams += "&v=2";
    urlParams += "&name=Flipper_0";
    urlParams += "&app=youtube-desktop";
    urlParams += "&loungeIdToken=" + Device.YoutubeToken;
    urlParams += "&id=" + UUID;
    urlParams += "&VER=8";
    urlParams += "&CVER=1";
    urlParams += "&zx=" + zx();
    urlParams += "&RID=" + String(rid.next());

    
    String jsonData = "{count: 0 }";

    
    secureClient.print("POST " + String(endpoint) + "?" + urlParams + " HTTP/1.1\r\n");
    secureClient.print("Host: " + String(serverAddress) + "\r\n");
    secureClient.print("User-Agent: ESP32\r\n");
    secureClient.print("Content-Type: application/json\r\n");
    secureClient.print("Content-Length: " + String(jsonData.length()) + "\r\n");
    secureClient.print("\r\n");
    secureClient.print(jsonData);

    
    while (!secureClient.available()) {
        delay(10);
    }

    
    String response = secureClient.readString();
    Serial.println(response);

    DynamicJsonDocument doc(4096);
    Serial.println(extractJSON(response));
    DeserializationError error = deserializeJson(doc, extractJSON(response));

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }

    JsonArray array = doc.as<JsonArray>();
    String gsession;
    String SID;

    for (JsonVariant v : array) {
      if (v[0] == 1 && v[1][0] == "S") {
        gsession = v[1][1].as<String>();
      }
      if (v[0] == 0 && v[1][0] == "c") {
        SID = v[1][1].as<String>();
      }
    }

    Serial.println("gsession: " + gsession);
    Serial.println("SID: " + SID);

    Device.gsession = gsession;
    Device.UUID = UUID;
    Device.SID = SID;

    secureClient.stop();
}

void DIALClient::sendYouTubeCommand(const String& command, const String& videoId, const Device& device) {
    const char* serverAddress = "www.youtube.com";
    const int port = 443;
    const char* endpoint = "/api/lounge/bc/bind";

    
    if (!secureClient.connect(serverAddress, port)) {
        Serial.println("Connection failed!");
        return;
    }
    
    String urlParams = "device=REMOTE_CONTROL";
    urlParams += "&loungeIdToken=" + device.YoutubeToken;
    urlParams += "&id=" + device.UUID;
    urlParams += "&VER=8";
    urlParams += "&zx=" + zx();
    urlParams += "&SID=" + device.SID;
    urlParams += "&RID=" + String(rid.next());
    urlParams += "&AID=" + String("-1");
    urlParams += "&gsessionid=" + device.gsession;

    
    String jsonData = "{\"command\": \"" + command + "\", \"params\": {\"videoId\": \"" + videoId + "\"}, \"loungeToken\": \"" + device.YoutubeToken + "\"}";

    
    secureClient.print("POST " + String(endpoint) + "?" + urlParams + " HTTP/1.1\r\n");
    secureClient.print("Host: " + String(serverAddress) + "\r\n");
    secureClient.print("User-Agent: ESP32\r\n");
    secureClient.print("Content-Type: application/json\r\n");
    secureClient.print("Content-Length: " + String(jsonData.length()) + "\r\n");
    secureClient.print("\r\n");
    secureClient.print(jsonData);

    
    while (!secureClient.available()) {
        delay(10);
    }

    
    String response = secureClient.readString();
    Serial.println(response);
}

String DIALClient::getYouTubeToken(const String& screenId) {
    const char* serverAddress = "www.youtube.com";
    const int port = 443;
    const char* endpoint = "/api/lounge/pairing/get_lounge_token_batch";

    if (secureClient.connect(serverAddress, port)) {
        Serial.println("Connected to server.");


        String postData = "screen_ids=" + screenId;
        secureClient.print("POST " + String(endpoint) + " HTTP/1.1\r\n");
        secureClient.print("Host: " + String(serverAddress) + "\r\n");
        secureClient.print("User-Agent: ESP32\r\n");
        secureClient.print("Content-Type: application/x-www-form-urlencoded\r\n");
        secureClient.print("Content-Length: " + String(postData.length()) + "\r\n");
        secureClient.print("\r\n");
        secureClient.print(postData);

        
        while (!secureClient.available()) {
          delay(10);
        }

        
        String entireResponse = secureClient.readString();

        
        int firstNewline = entireResponse.indexOf('\n');
        String statusLine = entireResponse.substring(0, firstNewline);
        int statusCode = statusLine.substring(9, 12).toInt();
        Serial.println("YouTube API response status: " + String(statusCode));

        String responseBody = "";

        
        int endOfHeaders = entireResponse.indexOf("\n\r\n");
        if (endOfHeaders != -1) {
            responseBody = entireResponse.substring(endOfHeaders + 3);
            Serial.println("YouTube API response content: " + responseBody);
        } else {
            Serial.println("Failed to parse response headers.");
        }

        
        int startOfJson = responseBody.indexOf('{');
        if (startOfJson != -1) {
            responseBody = responseBody.substring(startOfJson);
        } else {
            Serial.println("Failed to find start of JSON content.");
            return "";
        }

        if (statusCode == 200) {

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, responseBody);
            String loungeToken = doc["screens"][0]["loungeToken"].as<String>();
            Serial.println("Lounge Token: " + loungeToken);
            secureClient.stop();
            return loungeToken;
        } else {
            Serial.println("Failed to retrieve token. HTTP Response Code: " + String(statusCode));
            return "";
        }
    } else {
        Serial.println("Failed to connect to server.");
        return "";
    }
}

String DIALClient::extractScreenId(const String& xmlData) {
    String startTag = "<screenId>";
    String endTag = "</screenId>";

    int startIndex = xmlData.indexOf(startTag);
    if (startIndex == -1) {
        Serial.println("Start tag not found.");
        return "";
    }
    startIndex += startTag.length();

    int endIndex = xmlData.indexOf(endTag, startIndex);
    if (endIndex == -1) {
        Serial.println("End tag not found.");
        return "";
    }

    String extractedId = xmlData.substring(startIndex, endIndex);
    if (extractedId.length() == 0) {
        Serial.println("Extracted screenId is empty.");
        return "";
    }

    Serial.println("Extracted screenId: " + extractedId);
    return extractedId;
}

void DIALClient::extractIPAndPort(const String& appUrl, IPAddress& ip, uint16_t& port) {
    Serial.println("Entering extractIPAndPort");
    
    int portStartIndex = appUrl.lastIndexOf(':');
    if (portStartIndex != -1 && portStartIndex < appUrl.length() - 1) {
        String portStr = appUrl.substring(portStartIndex + 1);
        port = portStr.toInt();
        Serial.println("Parsed port: " + String(port));
    }

    String ipStr = appUrl.substring(7, portStartIndex);
    Serial.println("Parsed IP string: " + ipStr);
    if (!ip.fromString(ipStr)) {
        Serial.println("Failed to parse IP");
    }

    Serial.println("Exiting extractIPAndPort");
}

void DIALClient::launchYouTubeApp(const String& appUrl) {
    int startPos = appUrl.indexOf('/', 7);
    String basePath = (startPos != -1) ? appUrl.substring(startPos) : "/";

    String youtubePath = basePath + "/YouTube";

    IPAddress extractedIp;
    uint16_t extractedPort;
    extractIPAndPort(appUrl, extractedIp, extractedPort);

    HttpClient httpc(client, extractedIp, extractedPort);

    httpc.beginRequest();

    int httpCode = httpc.post(youtubePath);
    httpc.sendHeader("User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Mobile Safari/537.36");
    httpc.sendHeader("Origin", "https://www.youtube.com");
    httpc.endRequest();

    if (httpCode == 201) {
        Serial.println("Successfully launched the YouTube app.");
    } else {
        Serial.println("Failed to launch the YouTube app. HTTP Response Code: " + String(httpCode));
    } 
}

void DIALClient::launchNetflixApp(const String& appUrl) {
    int startPos = appUrl.indexOf('/', 7);
    String basePath = (startPos != -1) ? appUrl.substring(startPos) : "/";

    String Netflixpath = basePath + "/Netflix";

    IPAddress extractedIp;
    uint16_t extractedPort;
    extractIPAndPort(appUrl, extractedIp, extractedPort);

    HttpClient httpc(client, extractedIp, extractedPort);

    httpc.beginRequest();

    int httpCode = httpc.post(Netflixpath);
    httpc.sendHeader("User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Mobile Safari/537.36");
    httpc.endRequest();

    if (httpCode == 0) { // 0 Means Success For Netflix
        Serial.println("Successfully launched the Netflix app.");
    } else {
        Serial.println("Failed to launch the Netflix app. HTTP Response Code: " + String(httpCode));
  }
}

DIALClient::~DIALClient() {

    client.stop();


    secureClient.stop();


    multicastClient.stop();
}

int DIALClient::checkYouTubeAppStatus(const String& appUrl, Device& device_I) {
    IPAddress extractedIp;
    uint16_t extractedPort;
    extractIPAndPort(appUrl, extractedIp, extractedPort);

    int startPos = appUrl.indexOf('/', 7);
    String basePath = (startPos != -1) ? appUrl.substring(startPos) : "/";
    String youtubePath = basePath + "/YouTube";

    Serial.println("[checkYouTubeAppStatus] Connecting to IP: " + extractedIp.toString() + " Port: " + String(extractedPort));
    Serial.println("[checkYouTubeAppStatus] Trying to access: " + youtubePath);

    HttpClient httpc(client, extractedIp, extractedPort);
    httpc.beginRequest();

    int resultCode = httpc.get(youtubePath);
    httpc.sendHeader("User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Mobile Safari/537.36");
    httpc.sendHeader("Origin", "https://www.youtube.com");
    httpc.endRequest();

    if (resultCode == HTTP_SUCCESS) {
        Serial.println("Successfully connected and received a response.");
        int responseCode = httpc.responseStatusCode();
        Serial.println("[checkYouTubeAppStatus] HTTP Response Code: " + String(responseCode));
        
        if (responseCode == 200) {
            Serial.println("Got Sucessful Response");

            String responseBody = httpc.responseBody();

           

           
            if (responseBody.indexOf("<state>running</state>") != -1) {
                device_I.screenID = extractScreenId(responseBody);
                Serial.println("YouTube app is running.");
                return responseCode;
            } else if (responseBody.indexOf("<state>stopped</state>") != -1) {
                Serial.println("YouTube app is not running.");
                responseCode = 404;
                return responseCode;
            } else {
                Serial.println("Unable to determine the status of the YouTube app.");
                responseCode = 500;
                return responseCode;
            }
        } else if (responseCode == 404) {
            Serial.println("YouTube app is not running.");
            return responseCode;
        } else {
            Serial.println("Received unexpected HTTP Response Code: " + String(responseCode));
            return responseCode;
        }
    } else {
        Serial.println("Failed to establish a connection or send the request. Result code: " + String(resultCode));
        return resultCode;
    }
}

String DIALClient::getDialApplicationUrl(const String& locationUrl) {
    int ip_start = locationUrl.indexOf("//") + 2;
    int ip_end = locationUrl.indexOf(":", ip_start);
    int port_end = locationUrl.indexOf("/", ip_end);

    String ip = locationUrl.substring(ip_start, ip_end);
    String portStr = locationUrl.substring(ip_end + 1, port_end);
    uint16_t port = portStr.toInt();

    String path = extractPathFromURL(locationUrl);

    HttpClient httpc(client, ip, port);

    httpc.beginRequest();
    int getcode = httpc.get(path);
    httpc.sendHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8");
    httpc.sendHeader("User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Mobile Safari/537.36");
    httpc.sendHeader("Content-Type", "text/xml; charset=\"utf-8\"");
    httpc.endRequest();
    
    int httpCode = httpc.responseStatusCode();

    if (httpCode == 200) { 
    String appUrl = extractApplicationURL(httpc);

      if (!appUrl.isEmpty()) {
          Serial.println("[getDialApplicationUrl] Application-URL: " + appUrl);
      } else {
          Serial.println("[getDialApplicationUrl] Couldn't find Application-URL in the headers.");
      }
      return appUrl;
    } 
    else 
    {
      Serial.println("Failed to fetch device description from " + locationUrl + ". HTTP Response Code: " + String(httpCode));
      return "";
    }
}