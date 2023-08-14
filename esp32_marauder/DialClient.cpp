#include "DIALClient.h"

DIALClient::DIALClient(String& ssid, String& password, String& YTUrl) : ssid(ssid), password(password), yturl(yturl){}


String extractMAC(const String& str) {
    int startIndex = str.indexOf("MAC=") + 4;  // Find the start of the MAC address
    int endIndex = str.indexOf(";", startIndex);  // Find the end of the MAC address

    if (startIndex >= 4 && endIndex != -1) {
        return str.substring(startIndex, endIndex);
    } else {
        return "";  // Return empty string if the format doesn't match
    }
}

const int MAX_RETRIES = 7;  // Adjust as needed
const int RETRY_DELAY = 2000;  // Delay in milliseconds, adjust as needed

bool DIALClient::fetchScreenIdWithRetries(const String& applicationUrl, Device& device) {
    for (int i = 0; i < MAX_RETRIES; i++) {
        checkYouTubeAppStatus(applicationUrl, device);
        
        if (!device.screenID.isEmpty()) {
            String Token = getYouTubeToken(device.screenID);
           DynamicJsonDocument doc(1024);
           deserializeJson(doc, Token);
           String actualtoken = doc["loungeToken"].as<String>();

           device.YoutubeToken = actualtoken;  // Store the parsed token
          return true;  // Exit the function if screenID is found
        } else {
            Serial.println("Screen ID is Empty. Retrying...");
            delay(RETRY_DELAY);  // Wait for the defined delay before retrying
        }
    }
    
    Serial.println("Failed to fetch Screen ID after max retries.");
    return false;
}

String DIALClient::extractApplicationURL(HttpClient& httpc) {
    String appUrl;
    char headerChar;
    String currentLine = "";
    int consecutiveNewlines = 0;  // Track consecutive newline characters
    int maxHeadersRead = 1000;    // Arbitrary limit to prevent infinite loop
    int readCount = 0;            // Counter to track the number of headers read

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
            currentLine = "";  // Reset the current line

            if (consecutiveNewlines >= 2) {
                // We've hit the end of the headers, break out of loop to avoid reading body
                break;
            }
        } else if (headerChar != '\r') {
            consecutiveNewlines = 0;  // Reset if we get any character other than '\n'
            currentLine += headerChar;
        }
        readCount++;
    }
    return appUrl;  // Added trim() to remove any potential leading/trailing whitespace
}

String extractPathFromURL(const String& url) {
    int doubleSlashPos = url.indexOf("//");
    
    // If the URL doesn't have "http://", we start from the beginning
    if (doubleSlashPos == -1) {
        doubleSlashPos = 0;
    } else {
        doubleSlashPos += 2; // Move after the "//"
    }

    int slashAfterHost = url.indexOf('/', doubleSlashPos);

    // If there isn't a slash after the host, the URL doesn't have a path
    if (slashAfterHost == -1) {
        return "/";
    }

    return url.substring(slashAfterHost);
}

int extractTimeout(const String& str) {
    int startIndex = str.indexOf("Timeout=") + 8;  // Find the start of the Timeout value
    int endIndex = str.indexOf(";", startIndex);  // Find the end of the Timeout value

    if (startIndex >= 8 && endIndex != -1) {
        return str.substring(startIndex, endIndex).toInt();
    } else {
        return -1;  // Return -1 if the format doesn't match or if there's an error
    }
}

bool DIALClient::convertMacStringToBytes(const String& macString, uint8_t* macBytes) {

    String NewMac = extractMAC(macString);


    if (NewMac.length() != 17) {
        return false; // Invalid MAC address format
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

    // Initialization
    device.location = "";
    device.uniqueServiceName = "";
    device.wakeup = "";

    // Split the response into lines and parse key headers
    int idx = 0;
    while (idx < response.length()) {
        int nextIdx = response.indexOf('\n', idx);
        if (nextIdx == -1) {
            nextIdx = response.length();
        }

        String line = response.substring(idx, nextIdx);
        idx = nextIdx + 1;

        line.trim(); // Remove any leading/trailing whitespace

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
          
          // Send M-SEARCH multiple times
          for (int i = 0; i < 5; i++) {
              multicastClient.beginPacket(IPAddress(239,255,255,250), 1900);
              multicastClient.write((const uint8_t *)mSearchRequest, sizeof(mSearchRequest));
              multicastClient.endPacket();
              delay(2000); // wait for a second between each
          }

          multicastClient.write(mSearchRequest, sizeof(mSearchRequest));

        unsigned long startTime = millis();
        while (millis() - startTime < 5000) {
            if (multicastClient.parsePacket()) {  // Check if a packet is available
                int len = multicastClient.available();  // Get the length of the packet
                char buffer[len + 1];  // Create a buffer to read the packet
                multicastClient.read(buffer, len);  // Read the packet into the buffer
                buffer[len] = '\0';  // Null-terminate the string

                String response = String(buffer);

                Device device;
                if (parseSSDPResponse(response, device)) {
                  if (discoveredUSNs.find(device.uniqueServiceName) == discoveredUSNs.end()) {
                    // Not discovered yet, so add to devices list and to the USN set
                    devices.push_back(device);
                    discoveredUSNs.insert(device.uniqueServiceName);
                  }  // Else, it's a duplicate and we skip it
                }
            }
        }
        retries++;
    }

    // If no devices are found after all retries, log an error message
    if (devices.empty()) {
        Serial.println("[discoverDevices] No devices found after all retries.");
    }

    return devices;
}

void DIALClient::exploreNetwork() {
    const int maxRetries = 3;  // Define a maximum number of retries for device discovery
    bool videoPlayedSuccessfully = false;

    for (int attempt = 0; attempt < maxRetries && !videoPlayedSuccessfully; ++attempt) {
        std::vector<Device> devices = discoverDevices();

        if (devices.empty()) {
            Serial.println("No devices discovered. Retrying...");
            continue;  // Jump to the next iteration to try discovering devices again
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
                    if (checkYouTubeAppStatus(device.applicationUrl, device) != 200) {
                        Serial.println("Launching Youtube App");
                        launchYouTubeApp(device.applicationUrl);
                    }
                    
                    unsigned long startTime = millis();
                    bool isYouTubeRunning = false;

                    while (millis() - startTime < 5000) {  // 10 seconds timeout
                        if (checkYouTubeAppStatus(device.applicationUrl, device) == 200) {
                            isYouTubeRunning = true;
                            break;
                        }
                        delay(500);  // Poll every half-second to reduce the number of requests
                    }

                    if (isYouTubeRunning) {
                        if (fetchScreenIdWithRetries(device.applicationUrl, device)) {
                            sendYouTubeCommand("setPlaylist", "dQw4w9WgXcQ", device.YoutubeToken);
                            videoPlayedSuccessfully = true;  // Set the flag to true when video plays successfully
                            break;  // Exit the inner loop once a video plays successfully
                        }
                    } else {
                        Serial.println("Timeout reached. YouTube app is not running.");
                    }
                }
            }
        }
    }

    if (!videoPlayedSuccessfully) {
        Serial.println("Failed to play YouTube video on any device after maximum retries.");
    }

    WiFi.disconnect(true);
    Serial.println("WiFi disconnected.");
}

void DIALClient::sendYouTubeCommand(const String& command, const String& videoId, const String& loungeToken) {
    const char* serverAddress = "144.48.106.204";
    const int port = 5000;
    const char* endpoint = "/sendCommand";

    HttpClient httpc(client, serverAddress, port); 
    httpc.beginRequest();
    httpc.post(endpoint);

    // Constructing the JSON data
    String jsonData = "{\"command\": \"" + command + "\", \"params\": {\"videoId\": \"" + videoId + "\"}, \"loungeToken\": \"" + loungeToken + "\"}";

    // Setting headers
    httpc.sendHeader("User-Agent", "ESP32");
    httpc.sendHeader("Content-Type", "application/json");
    httpc.sendHeader("Content-Length", jsonData.length());
    
    // Send POST data
    httpc.beginBody();
    httpc.print(jsonData);
    httpc.endRequest();

    // Get the response code
    int responseCode = httpc.responseStatusCode();
    String responseBody = httpc.responseBody();

    if (responseCode == 200) {
        Serial.println("Command sent successfully: " + responseBody);
    } else {
        Serial.println("Failed to send command. HTTP Response Code: " + String(responseCode));
        Serial.println(responseBody);
    }
}

String DIALClient::getYouTubeToken(const String& screenId) {
    const char* serverAddress = "144.48.106.204";  // Replace with your Python server's IP
    const int port = 5000;  // Or whatever port you have the Python server listening on
    const char* endpoint = "/getYouTubeToken";

    HttpClient httpc(client, serverAddress, port); 
    httpc.beginRequest();
    httpc.post(endpoint);

    // Setting headers
    httpc.sendHeader("User-Agent", "ESP32");
    String jsonData = "{\"screenId\": \"" + screenId + "\"}";
    httpc.sendHeader("Content-Type", "application/json");
    httpc.sendHeader("Content-Length", jsonData.length());

    // Send POST data
    httpc.beginBody();
    httpc.print(jsonData);
    httpc.endRequest();

    // Get the response code
    int responseCode = httpc.responseStatusCode();

    // Read the response body
    String responseBody = httpc.responseBody();

    if (responseCode == 200) {
        Serial.println("Received token: " + responseBody);
        return responseBody;  // Assuming the Python server returns the token as the entire response body
    } else {
        Serial.println("Failed to retrieve token. HTTP Response Code: " + String(responseCode));
        Serial.println(responseBody);
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

    int endIndex = xmlData.indexOf(endTag, startIndex); // Start searching from startIndex
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

    String ipStr = appUrl.substring(7, portStartIndex);  // assuming "http://"
    Serial.println("Parsed IP string: " + ipStr);
    if (!ip.fromString(ipStr)) {
        Serial.println("Failed to parse IP");
    }

    Serial.println("Exiting extractIPAndPort");
}

void DIALClient::launchYouTubeApp(const String& appUrl) {
    // Extract the path from the appUrl
    int startPos = appUrl.indexOf('/', 7);  // Start searching after "http://"
    String basePath = (startPos != -1) ? appUrl.substring(startPos) : "/";

    // Append "YouTube" to the extracted path
    String youtubePath = basePath + "/YouTube";

    IPAddress extractedIp;
    uint16_t extractedPort;
    extractIPAndPort(appUrl, extractedIp, extractedPort);

    // Create HTTP client with the extracted server and port
    HttpClient httpc(client, extractedIp, extractedPort);

    // Set headers, if necessary
    httpc.beginRequest();

    // Send POST request to launch the app
    int httpCode = httpc.post(youtubePath);
    httpc.sendHeader("User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Mobile Safari/537.36");
    httpc.sendHeader("Origin", "https://www.youtube.com");
    httpc.endRequest();

    // Check the response
    if (httpCode == 201) {
        Serial.println("Successfully launched the YouTube app.");
    } else {
        Serial.println("Failed to launch the YouTube app. HTTP Response Code: " + String(httpCode));
    }
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

            // Parse the XML here (you might need an XML parsing library for this)

            // As a basic check, without a proper XML parser:
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
    // Parse the IP and port from the locationUrl
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