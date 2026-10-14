/*
 * Lab 1 - Arduino Nano 33 IoT : accelerometre
 *   - HTTP POST -> serveur Node.js (port 3000)              [partie principale]
 *   - client TCP -> ESP32 (port 5000)                       [bonus : chatting]
 * Carte : Arduino NANO 33 IoT
 */

#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <Arduino_LSM6DS3.h>

const char* WIFI_SSID = "Freebox-0961B1";
const char* WIFI_PASS = "4vnhzq5wxtqmz3xdqd6d76";
const char* SERVER_IP = "192.168.1.186";

const int SERVER_PORT = 3000;
const char* ENDPOINT  = "/api/arduino-data";

// Bonus : IP fixe de l'ESP32 (doit correspondre a STATIC_IP du sketch ESP32,
// ou a l'IP affichee sur son moniteur serie si le DHCP est utilise)
const char* ESP32_IP = "192.168.1.47";
const int ESP32_PORT = 5000;
const unsigned long PEER_RETRY_MS = 5000;

const unsigned long SEND_INTERVAL_MS = 2000;  // une mesure toutes les 2 s

WiFiClient wifi;                        // utilise par le client HTTP
HttpClient http(wifi, SERVER_IP, SERVER_PORT);
WiFiClient peer;                        // connexion TCP vers l'ESP32
String peerBuf = "";
unsigned long lastSend = 0;
unsigned long lastPeerAttempt = 0;

void connectWiFi() {
  Serial.print("Connexion au WiFi ");
  Serial.println(WIFI_SSID);
  while (WiFi.begin(WIFI_SSID, WIFI_PASS) != WL_CONNECTED) {
    Serial.print(".");
    delay(2000);
  }
  Serial.println();
  Serial.print("Connecte, IP Nano : ");
  Serial.println(WiFi.localIP());
}

// ---- Bonus : client TCP vers l'ESP32 ----

void ensurePeer() {
  if (peer.connected()) return;
  if (millis() - lastPeerAttempt < PEER_RETRY_MS) return;  // pas de spam
  lastPeerAttempt = millis();

  peer.stop();
  Serial.print("Connexion TCP a l'ESP32 ");
  Serial.print(ESP32_IP);
  Serial.print(":");
  Serial.print(ESP32_PORT);
  Serial.print(" ... ");
  if (peer.connect(ESP32_IP, ESP32_PORT)) {
    Serial.println("OK");
    peerBuf = "";
  } else {
    Serial.println("echec");
  }
}

// Lit les lignes envoyees par l'ESP32 ({"temperature":..,"humidity":..}\n)
void readPeer() {
  while (peer.available()) {
    char c = (char)peer.read();
    if (c == '\n') {
      Serial.print("[ESP32] Temp/Hum recues : ");
      Serial.println(peerBuf);
      peerBuf = "";
    } else if (c != '\r') {
      peerBuf += c;
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);  // laisse le temps d'ouvrir le moniteur serie (pas de while(!Serial) : la carte doit marcher sans PC)

  if (!IMU.begin()) {
    Serial.println("Echec de l'initialisation de l'IMU (LSM6DS3) !");
    while (true) { delay(1000); }
  }
  Serial.print("Frequence d'echantillonnage de l'accelerometre : ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  connectWiFi();
  lastPeerAttempt = millis() - PEER_RETRY_MS;  // premiere tentative immediate
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Bonus : maintenir la connexion vers l'ESP32 et lire ses messages
  ensurePeer();
  readPeer();

  if (millis() - lastSend < SEND_INTERVAL_MS) return;

  float x, y, z;
  if (!IMU.accelerationAvailable()) return;
  IMU.readAcceleration(x, y, z);  // en g
  lastSend = millis();

  // Corps JSON : {"x":0.01,"y":-0.02,"z":1.00}
  String body = "{\"x\":" + String(x, 2) +
                ",\"y\":" + String(y, 2) +
                ",\"z\":" + String(z, 2) + "}";

  // 1) vers l'ESP32 en TCP (une ligne par message)
  if (peer.connected()) {
    peer.print(body + "\n");
    Serial.print("Envoye a l'ESP32 : ");
    Serial.println(body);
  }

  // 2) vers Node.js en HTTP
  Serial.print("Envoi HTTP : ");
  Serial.println(body);

  http.post(ENDPOINT, "application/json", body);

  int status = http.responseStatusCode();
  String response = http.responseBody();  // lit la reponse pour liberer la connexion

  Serial.print("Reponse HTTP : ");
  Serial.print(status);
  Serial.print(" ");
  Serial.println(response);
}
