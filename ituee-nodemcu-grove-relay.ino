#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// wi-fi credentials
#define WLAN_SSID "SSID"
#define WLAN_PASS "PASS"

// Adafruit IO
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 8883
#define AIO_USERNAME "USERNAME"
#define AIO_KEY "KEY"

// io.adafruit.com SHA1 fingerprint
static const char *fingerprint PROGMEM = "59 3C 48 0A B1 8B 39 4E 0D 58 50 47 9A 13 55 60 CC A0 1D AF";

//WiFiClient client;
WiFiClientSecure client;

// init adafruit client & topic subscription
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe infrared = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/infrared");

// relay pin
#define RELAY_PIN D3

void MQTT_connect();

void MQTT_connect()
{
  int8_t ret;
  if (mqtt.connected())
  {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0)
    {
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}

void setup()
{
  // Start serial connection
  Serial.begin(115200);
  delay(10);

  // set relay pin as output
  pinMode(RELAY_PIN, OUTPUT);

  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  // connect to wifi
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  // loop until connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // check the fingerprint of io.adafruit.com's SSL cert
  client.setFingerprint(fingerprint);

  // subscribe to topic for new messages
  mqtt.subscribe(&infrared);
}

int x = 260;

void loop()
{

  // always keep connected
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;

  // listen for new messages
  while ((subscription = mqtt.readSubscription(5000)))
  {
    if (subscription == &infrared)
    {
      // get last reading
      x = atoi((char *)infrared.lastread);
      Serial.print(F("Got: "));
      Serial.println(x);
      // if ir level is lower than 260 (sun is setting)
      // open relay
      if (x < 260)
      {
        digitalWrite(RELAY_PIN, HIGH);
      }
      else
      {
        digitalWrite(RELAY_PIN, LOW);
      }
    }
  }

  // if no ping from server, disconnect
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}
