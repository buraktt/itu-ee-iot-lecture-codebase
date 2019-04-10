#include <Wire.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "SI114X.h"

#define WLAN_SSID       "SSID"
#define WLAN_PASS       "PASS"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "USERNAME"
#define AIO_KEY         "KEY"

WiFiClient client;
//WiFiClientSecure client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish infrared = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/infrared");

SI114X SI1145 = SI114X();



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

  while (!SI1145.Begin()) {
    Serial.println("Si1145 is not ready!");
    delay(1000);
  }

  Serial.println("Si1145 is ready!");

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
}

uint32_t x=0;

void loop() {

  MQTT_connect();

  x = SI1145.ReadIR();

  Serial.print(F("\nSending infrared val "));
  Serial.print(x);
  Serial.print("...");
  if (! infrared.publish(x)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  delay(2000);
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}


