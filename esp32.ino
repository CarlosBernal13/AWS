#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "INFINITUM2FAC";
const char* password = "RHKvZ3uTWm";
//const char* ssid = "Wokwi-GUEST";
//const char* password = "";
const char* mqtt_server = "18.217.172.191";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado - IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conexion MQTT...");

    if (client.connect("ESP32_Arturo_UAEM")) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static unsigned long lastMsg = 0;
  unsigned long now = millis();

  if (now - lastMsg > 5000) {
    lastMsg = now;

    float temp = random(20, 30);
    float hum = random(40, 60);

    StaticJsonDocument<200> doc;
    doc["sensor"] = "BME680";
    doc["temp"] = temp;
    doc["hum"] = hum;

    char buffer[256];
    serializeJson(doc, buffer);

    Serial.print("Publicando mensaje: ");
    Serial.println(buffer);

    client.publish("casa/sensores", buffer);
  }
}
