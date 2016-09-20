#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <ArduinoJson.h>

#define wifi_ssid "TNCAP7B297F"
#define wifi_password "4AF8E22F53"
#define mqtt_server "192.168.1.69"
#define mqtt_user "hass"
#define mqtt_password "hasspass"
#define script_name "MQTT_ESP8266"
#define script_version "1.0.0"
//Set topic to device id that client will subscribe to
#define topic1 "80001"
#define topic2 "t2"
WiFiClient espClient;
PubSubClient client(espClient);

char data[80];
// int fsrAnalogPin = "A0";
int fsrReading = 0;
int prevReading;
int bagNo = 80001;


void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print(script_name);
  Serial.println(script_version);
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("TestMQTT", mqtt_user, mqtt_password)) { //* See //NOTE below
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void myDelay(int ms) {
  int i;
  for (i = 1; i != ms; i++) {
    delay(1);
    if (i % 100 == 0) {
      ESP.wdtFeed();
      yield();
    }
  }
}


//Variables used in loop()
long lastMsg = 0;
float t1 = 75.5;
float t2 = 50.5;

unsigned long time0 = 0,
              time1 = 0,
              Intvl = 0;
              
void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  //2 seconds minimum between Read Sensors and Publish
  // if (now - lastMsg > 2000) {
  //     lastMsg = now;
  //NOTE: read sensors, check for a change, test time difference
  prevReading = fsrReading;

  fsrReading = analogRead(A0);
  Serial.print("Analog reading = ");

  if ((fsrReading - prevReading > 7) || (fsrReading - prevReading >-7)) {
    if (fsrReading > 8) {
      time1 = millis();
      Intvl = time1 - time0;
      
      time0 = time1;

      Serial.println(fsrReading);
      Serial.println(Intvl);
      if ((Intvl < 5000) && (Intvl > 200)){

      //create Json formmatted information
      String t1Json = "\"BagNo\":" +  String(bagNo)  + ", \"Tchnq\": " + String(1) + ", \"Pwr\": " + String(fsrReading) + ", \"Intvl\": " + String(Intvl);
      //Publish Values to MQTT broker
      String payload = "{" + t1Json + "}";
      payload.toCharArray(data, (payload.length() + 1));
      client.publish(topic1, data);

      myDelay(150);
      //delay(100);
      }
    }
  }
  // }
}
