#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi credentials (must match the transmitter's AP)
const char* ssid = "ESP32_AP";
const char* password = "12345678";


// Servo setup
#define SERVO_PIN 18
Servo myServo;

// Web server on port 80
WebServer server(80);

// Handle servo1 requests: /servo1?pos1=90
void handleServo2() {
  if (server.hasArg("pos2")) {
    int pos = server.arg("pos2").toInt();
    pos = constrain(pos, 0, 180);
    myServo.write(pos);
    Serial.printf("Moving servo to %d degrees\n", pos);
    server.send(200, "text/plain", "Moved to " + String(pos) + "°");
  } else {
    server.send(400, "text/plain", "Missing pos2 parameter");
  }
}

// (Optional) second handler if this device controls another servo
// void handleServo2() { ... }

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN);

  IPAddress local_IP(192, 168, 4, 69);  // ESP32's IP
  IPAddress gateway(192, 168, 4, 1);    // Your router
  IPAddress subnet(255, 255, 255, 0);   // Subnet mask

  WiFi.config(local_IP, gateway, subnet);

  // Connect to transmitter's Access Point
  Serial.println("Connecting to transmitter's AP...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  server.on("/servo2", handleServo2);
  server.begin();
  Serial.println("Receiver web server started");
}

void loop() {
  server.handleClient();
}
