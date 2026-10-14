/*
 * Lab 1 - ESP32 + DHT11
 *   - client TCP  -> serveur Node.js (port 8080)          [partie principale]
 *   - serveur TCP <- Arduino Nano 33 IoT (port 5000)      [bonus : chatting]
 * Carte : ESP32 Dev Module
 */

#include <WiFi.h>
#include <DHT.h>

const char* WIFI_SSID = "Freebox-0961B1";
const char* WIFI_PASS = "4vnhzq5wxtqmz3xdqd6d76";
const char* SERVER_IP = "192.168.1.186";

const uint16_t SERVER_PORT = 8080;   // serveur Node.js
const uint16_t PEER_PORT   = 5000;   // port d'ecoute pour le Nano (bonus)

// IP fixe : le Nano doit connaitre l'adresse de l'ESP32.
#define USE_STATIC_IP false // false pour la détection automatique

#define DHTPIN  17
#define DHTTYPE DHT11

const unsigned long SEND_INTERVAL_MS = 10000;  // une mesure toutes les 10 s

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;          // vers Node.js
WiFiServer peerServer(PEER_PORT);
WiFiClient peer;            // le Nano connecte
String peerBuf = "";
unsigned long lastSend = 0;

void connectWiFi() {
  Serial.print("Connexion au WiFi ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
#if USE_STATIC_IP
  WiFi.config(STATIC_IP, GATEWAY, SUBNET, DNS_IP);
#endif
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connecte, IP ESP32 : ");
  Serial.println(WiFi.localIP());
}

bool connectServer() {
  if (client.connected()) return true;
  Serial.print("Connexion TCP a ");
  Serial.print(SERVER_IP);
  Serial.print(":");
  Serial.println(SERVER_PORT);
  if (client.connect(SERVER_IP, SERVER_PORT)) {
    Serial.println("Connexion TCP etablie");
    return true;
  }
  Serial.println("Echec de la connexion TCP");
  return false;
}

// ---- Bonus : serveur TCP pour le Nano ----

void acceptPeer() {
  if (peer && peer.connected()) return;       // deja un Nano connecte
  WiFiClient incoming = peerServer.available();
  if (incoming) {
    peer = incoming;
    peerBuf = "";
    Serial.print("Nano connecte depuis ");
    Serial.println(peer.remoteIP());
  }
}

// Lit les lignes envoyees par le Nano ({"x":..,"y":..,"z":..}\n)
void readPeer() {
  if (!peer) return;
  while (peer.available()) {
    char c = (char)peer.read();
    if (c == '\n') {
      Serial.print("[Nano] Accelerometre recu : ");
      Serial.println(peerBuf);
      peerBuf = "";
    } else if (c != '\r') {
      peerBuf += c;
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  dht.begin();
  connectWiFi();
  peerServer.begin();
  Serial.print("Serveur TCP pour le Nano sur le port ");
  Serial.println(PEER_PORT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    peerServer.begin();
  }

  acceptPeer();
  readPeer();

  if (millis() - lastSend >= SEND_INTERVAL_MS) {
    lastSend = millis();

    float humidity    = dht.readHumidity();
    float temperature = dht.readTemperature();  // en degres Celsius

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Erreur de lecture du DHT11 (verifie le cablage et le GPIO)");
      return;
    }

    Serial.print("Temperature : ");
    Serial.print(temperature);
    Serial.print(" C | Humidite : ");
    Serial.print(humidity);
    Serial.println(" %");

    // Un message JSON par ligne ('\n' sert de separateur)
    String msg = "{\"temperature\":" + String(temperature, 1) +
                 ",\"humidity\":" + String(humidity, 1) + "}\n";

    // 1) vers le Nano (bonus)
    if (peer && peer.connected()) {
      peer.print(msg);
      Serial.print("Envoye au Nano : ");
      Serial.print(msg);
    }

    // 2) vers Node.js
    if (connectServer()) {
      client.print(msg);
      Serial.print("Envoye a Node : ");
      Serial.print(msg);
    }
  }
}
