#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include "CommandLine.h"
#include <WiFiManager.h>

#define MAX_HTML_SIZE 20000

class EvilPortal {
public:

    char ssid[100];
    const char *password;
    String accumulatedHTML;
    bool has_html;
    bool has_ap;
    char HTML[MAX_HTML_SIZE];
    
    const char* removeLineBreaks(const char* input) {
        char* buffer = new char[strlen(input) + 1];
        char* p = buffer;

        while (*input) {
            if (*input != '\n' && *input != '\r') {
                *p++ = *input;
            }
            input++;
        }
        *p = '\0';

        return buffer;
    }


    EvilPortal(const char* inssid, const char* InPassword) {
        ssid = removeLineBreaks(inssid);
        password = InPassword;
    }

    ~EvilPortal() {
       
    }

    void startPortal() {
        

        Serial.printf("Evil Portal Started at %s", WiFi.softAPIP().toString().c_str());
    }

    void stopPortal() {
       
    }

    void captureCredentials() {
       
    }

    void redirectTraffic() {
       
    }

    void displayFakeLogin() {
       
    }
};
