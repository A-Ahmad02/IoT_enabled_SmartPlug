#include <WiFi.h>
#include <PubSubClient.h>

// Your WiFi credentials
const char *ssid = "wifi_name";
const char *password = "wifi_password";

// MQTT broker settings
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "CN_CEP/SmartPlug";
const int mqtt_port = 1883;

int msg[5];
int pin = 13;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *m_topic, byte *payload, unsigned int length) {
    for (int i = 0; i < length; i++) {
        msg[i] = (char) payload[i] - '0';
    }
    
    if (msg[4]) {
        Serial.println("Turn ON");
        digitalWrite(pin, HIGH);
    } else {
        Serial.println("Turn OFF");
        digitalWrite(pin, LOW);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);  
    
    pinMode(pin, OUTPUT);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the Wi-Fi network");

    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    digitalWrite(LED_BUILTIN, LOW);  

    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());

        if (client.connect(client_id.c_str())) {
            Serial.println("Public EMQX MQTT broker connected");
            client.subscribe(topic);
        } else {
            Serial.println("Failed connection");
            delay(2000);
        }
    }
}

void loop() {
    client.loop();
}

