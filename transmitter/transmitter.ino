#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Access Point credentials
const char* ssid = "ESP32_AP";
const char* password = "12345678";

// Receiver MAC addresses (optional for logging)
const char* receiver1MAC = "A0:A3:B3:30:4A:D4";
const char* receiver2MAC = "XX:XX:XX:XX:XX:XX";

// Web server on port 80
WebServer server(80);

// HTML interface
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
 <head>
  <title>ESP32 Servo Control</title>
  <style>
    body { font-family: sans-serif; text-align: center; margin-top: 50px; }
    button { padding: 15px; font-size: 18px; margin: 10px; }
  </style>
</head>
<body>
  <div> 
    <h2>ESP32 Servo Control 1</h2>
    <button onclick="location.href='/servo1?pos1=0'">Move to 0°</button>
    <button onclick="location.href='/servo1?pos1=90'">Move to 90°</button>
    <button onclick="location.href='/servo1?pos1=180'">Move to 180°</button>
  </div>
  <div> 
    <h2>ESP32 Servo Control 2</h2>
    <button onclick="location.href='/servo2?pos2=0'">Move to 0°</button>
    <button onclick="location.href='/servo2?pos2=90'">Move to 90°</button>
    <button onclick="location.href='/servo2?pos2=180'">Move to 180°</button>
  </div>
</body>
</html>
)rawliteral";

// URL encode helper
String urlEncode(String str) {
  String encoded = "";
  for (unsigned int i = 0; i < str.length(); i++) {
    char c = str[i];
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += c;
    } else {
      encoded += '%' + String(c, HEX);
    }
  }
  return encoded;
}

// Route: homepage
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// Route: /servo1?pos1=XX
void handleSend1() {
  if (server.hasArg("pos1")) {
    String pos = server.arg("pos1");
    String encodedPos = urlEncode(pos);
    IPAddress targetIP(192, 168, 4, 68);  // Receiver 1 IP

    WiFiClient client;
    if (client.connect(targetIP, 80)) {
      client.print("GET /servo1?pos1=" + encodedPos + " HTTP/1.1\r\nHost: " + targetIP.toString() + "\r\nConnection: close\r\n\r\n");
      client.stop();
      Serial.println("Sent to Receiver 1: " + pos);
    } else {
      Serial.println("Failed to connect to Receiver 1");
    }

    server.send(200, "text/html", htmlPage + "<p>Sent to Receiver 1: " + pos + "°</p>");
  } else {
    server.send(400, "text/html", "Missing pos1 parameter");
  }
}

// Route: /servo2?pos2=XX
void handleSend2() {
  if (server.hasArg("pos2")) {
    String pos = server.arg("pos2");
    String encodedPos = urlEncode(pos);
    IPAddress targetIP(192, 168, 4, 69);  // Receiver 2 IP

    WiFiClient client;
    if (client.connect(targetIP, 80)) {
      client.print("GET /servo2?pos2=" + encodedPos + " HTTP/1.1\r\nHost: " + targetIP.toString() + "\r\nConnection: close\r\n\r\n");
      client.stop();
      Serial.println("Sent to Receiver 2: " + pos);
    } else {
      Serial.println("Failed to connect to Receiver 2");
    }

    server.send(200, "text/html", htmlPage + "<p>Sent to Receiver 2: " + pos + "°</p>");
  } else {
    server.send(400, "text/html", "Missing pos2 parameter");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Setting up Access Point...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  // Routes
  server.on("/", handleRoot);
  server.on("/servo1", HTTP_GET, handleSend1);
  server.on("/servo2", HTTP_GET, handleSend2);
  server.begin();
  Serial.println("Transmitter web server started");
}

void loop() {
  server.handleClient();
}
