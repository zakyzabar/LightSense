#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Abayazid";
const char* password = "19201920";

// MQTT broker
const char* mqtt_server = "rmq.corex.id";
const int mqtt_port = 1883;
const char* mqtt_user = "kelompok11";
const char* mqtt_password = "kelompok11";
const char* mqtt_client_id = "esp_kelompok11";
const char* mqtt_topic = "iot/cahaya";

WiFiClient espClient;
PubSubClient client(espClient);

const int sensorPin = 36;     // VP pin
const int relayPin = 5;       // GPIO5 (D5)
int cahaya;
String statusLampu;

void setup_wifi() {
  Serial.print("Menghubungkan ke WiFi: ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Tersambung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Menghubungkan ke MQTT...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("Terhubung ke MQTT Broker");
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" → Coba lagi 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Relay OFF (Active LOW relay)

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  cahaya = analogRead(sensorPin);

  if (cahaya < 200) {
    digitalWrite(relayPin, LOW);   // Relay ON → Lampu Menyala
    statusLampu = "GELAP - Lampu NYALA";
  } else {
    digitalWrite(relayPin, HIGH);  // Relay OFF → Lampu Mati
    statusLampu = "TERANG - Lampu MATI";
  }

  String payload = "Cahaya: " + String(cahaya) + " | Status: " + statusLampu;
  Serial.println(payload);
  client.publish(mqtt_topic, payload.c_str());

  delay(1000); // Baca tiap 1 detik
}