/*
 * Lab 1 - ESP32 + DHT11 -> serveur Node.js via TCP (sockets)
 * Carte : ESP32 Dev Module
 */

#include <WiFi.h>
#include <DHT.h>

const char* WIFI_SSID = "IoTLab";
const char* WIFI_PASS = "12345678";
const char* SERVER_IP = "10.42.0.1";

const uint16_t SERVER_PORT = 8080;

#define DHTPIN  17
#define DHTTYPE DHT11

const unsigned long SEND_INTERVAL_MS = 10000;  // une mesure toutes les 10 s

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
unsigned long lastSend = 0;

void connectWiFi() {
  Serial.print("Connexion au WiFi ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
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

void setup() {
  Serial.begin(115200);
  delay(500);
  dht.begin();
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

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

    if (connectServer()) {
      // Un message JSON par ligne ('\n' sert de separateur cote serveur)
      String msg = "{\"temperature\":" + String(temperature, 1) +
                   ",\"humidity\":" + String(humidity, 1) + "}\n";
      client.print(msg);
      Serial.print("Envoye : ");
      Serial.print(msg);
    }
  }
}
