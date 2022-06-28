#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#define LED  D4
int lampu1 = D5;
int lampu2 = D6;

WiFiClient espClient;
PubSubClient client(espClient);
const char* ssid = "Peaberry Coffee & Tea"; //melakukan connect ke wifi mengisikan nama dan password
const char* password = "11333356";
const char *mqtt_server = "138.2.87.224"; // isikan server broker
bool lamp1 = false, lamp2 = false;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //melakukan looping untuk connect ke wifi
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection..."); //melakukan connect ke server 
    if (client.connect("smart-home")) {
      Serial.println("connected");
      client.subscribe("monitor/lamp1");
      client.subscribe("monitor/lamp2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) { //mengirimkan topik ke server (nodered)
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  if (String(topic) == "monitor/lamp1") {
    Serial.println("Topic : monitor/lamp1");
    if (messageTemp == "false") {
      lamp1 = false;
    } else if (messageTemp == "true") {
      lamp1 = true;
    }
  }
  if (String(topic) == "monitor/lamp2") {
    Serial.println("Topic : monitor/lamp2");
    if (messageTemp == "false") {
      lamp2 = false;
    } else if (messageTemp == "true") {
      lamp2 = true;
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Mqtt Node-RED");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(lampu1, OUTPUT);
  pinMode(lampu2, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop()) {
    client.connect("smart-home");
  }
  if (lamp1) {
    digitalWrite(lampu1, LOW);
  } else {
    digitalWrite(lampu1, HIGH);
  }
  if (lamp2) {
    digitalWrite(lampu2, LOW);
  } else {
    digitalWrite(lampu2, HIGH);
  }
}
