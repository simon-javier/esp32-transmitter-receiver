#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Access Point credentials
const char* ssid = "ESP32-12341";
const char* password = "12345678";

String serverName = "";

// Receiver MAC addresses (optional for logging)
const char* receiver1MAC = "A0:A3:B3:30:4A:D4";
const char* receiver2MAC = "XX:XX:XX:XX:XX:XX";

// Web server on port 80
WebServer server(80);

// HTML interface
String htmlPage = R"rawliteral(<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Servo Control Server</title>
  </head>
  <style>
    * {
      margin: 0;
      padding: 0;
      width: 100%;
      box-sizing: border-box;
    }
    main {
      display: flex;
      justify-content: space-between;
      gap: 1em;
    }
    .container {
      max-width: 700px;
      margin: 0 auto;
    }
    h1 {
      font-size: 28px;
      font-weight: 600;
      margin-bottom: 24px;
      text-align: center;
    }
    h2 {
      font-size: 20px;
      font-weight: 500;
      margin: 16px 0;
    }
    .server-section {
      background-color: white;
      padding: 16px;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      margin-bottom: 24px;
    }
    input[type="text"] {
      width: 100%;
      padding: 8px;
      margin: 6px 0;
      border: 1px solid #e2e8f0;
      border-radius: 6px;
      font-size: 14px;
      box-sizing: border-box;
    }
    input[type="text"]:focus {
      outline: none;
      border-color: #3182ce;
      box-shadow: 0 0 0 2px rgba(49, 130, 206, 0.2);
    }
    button {
      padding: 8px 16px;
      margin: 6px;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      font-size: 14px;
      font-weight: 500;
      transition:
        background-color 0.2s,
        transform 0.1s;
    }
    button:hover {
      transform: translateY(-1px);
    }
    .btn-connect {
      background-color: #4a5568;
      color: white;
    }
    .btn-connect:hover {
      background-color: #2d3748;
    }
    .btn-send {
      background-color: #3182ce;
      color: white;
    }
    .btn-send:hover {
      background-color: #2b6cb0;
    }
    .btn-delete {
      background-color: #e53e3e;
      color: white;
    }
    .btn-delete:hover {
      background-color: #c53030;
    }
    .btn-add {
      background-color: #38a169;
      color: white;
    }
    .btn-add:hover {
      background-color: #2f855a;
    }
    .status,
    .error {
      padding: 10px;
      margin: 12px 0;
      border-radius: 6px;
      font-size: 14px;
      animation: fadeIn 0.5s ease-in;
      text-align: center;
    }
    .status {
      background-color: #e6fffa;
      color: #2f855a;
    }
    .error {
      background-color: #fff5f5;
      color: #c53030;
    }
    .receiver-table {
      width: 100%;
      border-collapse: separate;
      border-spacing: 0;
      background-color: white;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      margin-bottom: 24px;
    }
    .receiver-table th,
    .receiver-table td {
      padding: 10px;
      text-align: left;
      border-bottom: 1px solid #e2e8f0;
    }
    .receiver-table th {
      background-color: #edf2f7;
      font-weight: 500;
      font-size: 14px;
      color: #4a5568;
    }
    .receiver-table td input[type="text"] {
      margin: 0;
    }
    .receiver-table td button {
      margin: 0 4px;
    }
    .history-table {
      width: 100%;
      border-collapse: separate;
      border-spacing: 0;
      background-color: #f7fafc;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      animation: fadeIn 0.5s ease-in;
    }
    .history-table th,
    .history-table td {
      padding: 10px;
      text-align: left;
      border-bottom: 1px solid #e2e8f0;
    }
    .history-table th {
      background-color: #e2e8f0;
      font-weight: 500;
      font-size: 14px;
      color: #2d3748;
    }
    @keyframes fadeIn {
      from {
        opacity: 0;
      }
      to {
        opacity: 1;
      }
    }

    .servo-main {
      margin-inline: 1em;
    }

    .table-main {
      margin-inline: 1em;
    }
  </style>
  <body>
    <div class="server-section">
      <h2>Database Server</h2>
      <form id="connectForm" action="/connect" method="POST">
        <input
          type="text"
          name="serverIP"
          id="serverIP"
          placeholder="Last number, e.g., 2 for 192.168.4.2"
        />
        <button type="submit" class="btn-connect">Connect to Server</button>
      </form>
    </div>
    <main>
      <div class="servo-main">
        <div>
          <h2>ESP32 Servo Control 1</h2>
          <button id='btn-servo1-0' onclick="location.href='/servo1?pos1=0&ip1=192.168.4.68'">Move to 0°</button>
          <button id='btn-servo1-90' onclick="location.href='/servo1?pos1=90&ip1=192.168.4.68'">Move to 90°</button>
          <button id='btn-servo1-180' onclick="location.href='/servo1?pos1=180&ip1=192.168.4.68'">
            Move to 180°
          </button>
        </div>
        <div>
          <h2>ESP32 Servo Control 2</h2>
          <button onclick="location.href='/servo2?pos2=0&ip2=192.168.4.69'">Move to 0°</button>
          <button onclick="location.href='/servo2?pos2=90&ip2=192.168.4.69'">Move to 90°</button>
          <button onclick="location.href='/servo2?pos2=180&ip2=192.168.4.69'">
            Move to 180°
          </button>
        </div>
      </div>
      <div class="table-main">
        <h2>Text History</h2>
        <table class="history-table">
          <thead>
            <tr>
              <th style='width: 10%;'>ID</th>
              <th style='width: 30%;'>Receiver</th>
              <th style='width: 10%;'>Pos</th>
              <th style='width: 50%;'>Timestamp</th>
            </tr>
          </thead>
          <tbody id="historyTable"></tbody>
        </table>
      </div>
      <script>
        let receivers = [
          { id: 1, ip: "192.168.4.68", pos: "" },
          { id: 2, ip: "192.168.4.69", pos: "" },
        ];

        try {
          const stored = localStorage.getItem("receivers");
          if (stored) {
            receivers = JSON.parse(stored);
            renderReceivers();
          }
        } catch (e) {
          console.log("localStorage not available");
        }

        function saveReceivers() {
          try {
            localStorage.setItem("receivers", JSON.stringify(receivers));
          } catch (e) {
            console.log("Failed to save to localStorage");
          }
        }

        function fetchHistory() {
          fetch("/history")
            .then((response) => response.text())
            .then((data) => {
              document.getElementById("historyTable").innerHTML = data;
            })
            .catch(() => {
              document.getElementById("historyTable").innerHTML =
                '<tr><td colspan="4">Error fetching history</td></tr>';
            });
        }

        fetchHistory();
        setInterval(fetchHistory, 5000);
      </script>
    </main>
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

String getHistory() {
  if (serverName == "") {
    return "<tr><td colspan='4' class='p-3'>No server connected</td></tr>";
  }
  HTTPClient http;
  http.begin(serverName);
  http.setTimeout(10000);
  int httpCode = http.GET();
  String historyHtml = "";
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      for (JsonObject row : doc.as<JsonArray>()) {
        historyHtml += "<tr>";
        historyHtml += "<td class='p-3' style='width: 10%;'>" + String(row["id"].as<int>()) + "</td>";
        historyHtml += "<td class='p-3' style='width: 30%;'>" + String(row["receiver"].as<String>()) + "</td>";
        historyHtml += "<td class='p-3' style='width: 10%;'>" + String(row["text"].as<String>()) + "</td>";
        historyHtml += "<td class='p-3' style='width: 50%;'>" + String(row["timestamp"].as<String>()) + "</td>";
        historyHtml += "</tr>";
      }
    } else {
      historyHtml = "<tr><td colspan='4' class='p-3'>Error parsing history: " + String(error.c_str()) + "</td></tr>";
      Serial.println("History parse error: " + String(error.c_str()));
    }
  } else {
    historyHtml = "<tr><td colspan='4' class='p-3'>Error fetching history: HTTP " + String(httpCode) + "</td></tr>";
    Serial.println("History fetch failed: HTTP " + String(httpCode) + " for " + serverName);
  }
  http.end();
  return historyHtml;
}

// Route: homepage
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleHistory() {
  server.send(200, "text/html", getHistory());
}

void handleConnect() {
  if (server.hasArg("serverIP")) {
    String lastOctet = server.arg("serverIP");
    if (!lastOctet || lastOctet.length() == 0) {
      server.send(400, "text/html", htmlPage + "<div class='error'>No server IP provided</div>");
      Serial.println("Connect failed: No server IP provided");
      return;
    }
    int val = lastOctet.toInt();
    if (val < 2 || val > 254 || lastOctet != String(val)) {
      server.send(400, "text/html", htmlPage + "<div class='error'>Invalid server IP. Use last number 2-254 (e.g., 2 for 192.168.4.2)</div>");
      Serial.println("Connect failed: Invalid server IP last octet: " + lastOctet);
      return;
    }
    String fullServerUrl = "http://192.168.4." + lastOctet + ":42069/post-text.php";
    Serial.println("Attempting to connect to server: " + fullServerUrl);
    HTTPClient http;
    http.begin(fullServerUrl);
    http.setTimeout(10000);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      serverName = fullServerUrl;
      server.send(200, "text/html", htmlPage + "<div class='status'>Connection successful to 192.168.4." + lastOctet + "</div>");
      Serial.println("Connected to server: " + fullServerUrl);
    } else {
      server.send(500, "text/html", htmlPage + "<div class='error'>Failed to connect to server 192.168.4." + lastOctet + ": HTTP " + String(httpCode) + "</div>");
      Serial.println("Connect failed: HTTP " + String(httpCode) + " for " + fullServerUrl);
    }
    http.end();
  } else {
    server.send(400, "text/html", htmlPage + "<div class='error'>No server IP provided</div>");
    Serial.println("Connect failed: No server IP provided");
  }
}

bool validateIP(String lastOctet) {
  int val = lastOctet.toInt();
  return val >= 2 && val <= 254 && lastOctet == String(val);
}

// Route: /servo1?pos1=XX
void handleSend1() {

  if (server.hasArg("pos1") && server.hasArg("ip1")) {
    String receiverId = "1";
    String pos = server.arg("pos1");
    String ipArg = server.arg("ip1");

    String encodedPos = urlEncode(pos);
    IPAddress targetIP(192, 168, 4, 68);  // Receiver 1 IP

    WiFiClient client;
    if (client.connect(targetIP, 80)) {
      client.print("GET /servo1?pos1=" + encodedPos + " HTTP/1.1\r\nHost: " + targetIP.toString() + "\r\nConnection: close\r\n\r\n");
      HTTPClient http;
      http.begin(serverName);
      http.setTimeout(10000);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String postData = "receiver=Receiver%20" + String(receiverId) + "&text=" + pos;
      int httpCode = http.POST(postData);
      http.end();
      if (httpCode != HTTP_CODE_OK) {
        server.send(500, "text/html", htmlPage + "<div class='error'>Failed to log to database: HTTP " + String(httpCode) + "</div>");
        Serial.println("Send failed for Receiver " + String(receiverId) + ": Database log failed, HTTP " + String(httpCode));
        return;
      }
      client.stop();
      Serial.println("Sent to Receiver 1: " + pos);
    } else {
      Serial.println("Failed to connect to Receiver 1");
    }

    server.send(200, "text/html", htmlPage);
  } else {
    server.send(400, "text/html", "Missing pos1 parameter");
  }
}

// Route: /servo2?pos2=XX
void handleSend2() {
  if (server.hasArg("pos2") && server.hasArg("ip2")) {
    String receiverId = "2";
    String pos = server.arg("pos2");
    String ipArg = server.arg("ip2");

    String encodedPos = urlEncode(pos);
    IPAddress targetIP(192, 168, 4, 69);  // Receiver 2 IP

    WiFiClient client;
    if (client.connect(targetIP, 80)) {
      client.print("GET /servo2?pos2=" + encodedPos + " HTTP/1.1\r\nHost: " + targetIP.toString() + "\r\nConnection: close\r\n\r\n");
      HTTPClient http;
      http.begin(serverName);
      http.setTimeout(10000);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String postData = "receiver=Receiver%20" + String(receiverId) + "&text=" + pos;
      int httpCode = http.POST(postData);
      http.end();
      if (httpCode != HTTP_CODE_OK) {
        server.send(500, "text/html", htmlPage + "<div class='error'>Failed to log to database: HTTP " + String(httpCode) + "</div>");
        Serial.println("Send failed for Receiver " + String(receiverId) + ": Database log failed, HTTP " + String(httpCode));
        return;
      }
      client.stop();
      Serial.println("Sent to Receiver 2: " + pos);
    } else {
      Serial.println("Failed to connect to Receiver 2");
    }

    server.send(200, "text/html", htmlPage);
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
  server.on("/history", handleHistory);
  server.on("/connect", HTTP_POST, handleConnect);
  server.on("/servo1", HTTP_GET, handleSend1);
  server.on("/servo2", HTTP_GET, handleSend2);
  server.begin();
  Serial.println("Transmitter web server started");
}

void loop() {
  server.handleClient();
}
