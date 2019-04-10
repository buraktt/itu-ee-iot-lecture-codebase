#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


#define WLAN_SSID       "SSID"
#define WLAN_PASS       "PASS"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883
#define AIO_USERNAME    "USERNAME"
#define AIO_KEY         "KEY"

//WiFiClient client;
WiFiClientSecure client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe infrared = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/infrared");

#define RELAY_PIN D3

void MQTT_connect();

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(RELAY_PIN, OUTPUT);

  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqtt.subscribe(&infrared);
}

int x = 260;

void loop() {

  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &infrared) {
      x = atoi((char *)infrared.lastread);
      Serial.print(F("Got: "));
      Serial.println(x);
      if (x < 260) {
        digitalWrite(RELAY_PIN, HIGH);
      } else {
        digitalWrite(RELAY_PIN, LOW);
      }
    }
  }

  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}


