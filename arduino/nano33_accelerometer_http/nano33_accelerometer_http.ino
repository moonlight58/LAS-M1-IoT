/*
 * Lab 1 - Arduino Nano 33 IoT : accelerometre -> serveur Node.js via HTTP
 * Carte : Arduino NANO 33 IoT
 */

#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <Arduino_LSM6DS3.h>

const char* WIFI_SSID = "IoTLab";
const char* WIFI_PASS = "12345678";
const char* SERVER_IP = "10.42.0.1"; 

const int SERVER_PORT = 3000;
const char* ENDPOINT  = "/api/arduino-data";

const unsigned long SEND_INTERVAL_MS = 2000;  // une mesure toutes les 2 s

WiFiClient wifi;
HttpClient http(wifi, SERVER_IP, SERVER_PORT);
unsigned long lastSend = 0;

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
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (millis() - lastSend < SEND_INTERVAL_MS) return;

  float x, y, z;
  if (!IMU.accelerationAvailable()) return;
  IMU.readAcceleration(x, y, z);  // en g
  lastSend = millis();

  // Corps JSON : {"x":0.01,"y":-0.02,"z":1.00}
  String body = "{\"x\":" + String(x, 2) +
                ",\"y\":" + String(y, 2) +
                ",\"z\":" + String(z, 2) + "}";

  Serial.print("Envoi : ");
  Serial.println(body);

  http.post(ENDPOINT, "application/json", body);

  int status = http.responseStatusCode();
  String response = http.responseBody();  // lit la reponse pour liberer la connexion

  Serial.print("Reponse HTTP : ");
  Serial.print(status);
  Serial.print(" ");
  Serial.println(response);
}
