#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "Hidden"
#define WLAN_PASS       "Mare1234"
/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "catal4n"
#define AIO_KEY         "aio_iCoF74elXHSSd82sGbqW80ifiLdj"
/****************************************************************************/
char watt[5];
WiFiClient client;
float bill_amount;   
float energyTariff = 0.2; //Leu per constanta / 10

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Power = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Power");
Adafruit_MQTT_Publish bill = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bill");

void MQTT_connect();
void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  delay(10);
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}



void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
 int i=0;
 float watt1;
 digitalWrite(2, LOW); //Porneste ledul 2

 if(Serial.available() > 0 ){
   delay(100); //allows all serial sent to be received together
    while(Serial.available() && i<5) {

     watt[i++] = Serial.read();

    }
    watt[i++]='\0';
  }
  watt1 = atof(watt);
  bill_amount = watt1 * energyTariff;     //1000 // 1unit = 1kwH
  Serial.print(F("\nSending Power val "));
  Serial.println(watt1);
  Serial.print(F("\nSending bill val "));
  Serial.println(bill_amount);
  Serial.print("...");

  if (! Power.publish(watt1)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
   if (! bill.publish(bill_amount)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }


if (bill_amount==4){
for (int i =0; i<=2; i++)
{
  bill.publish(bill_amount);
delay(5000);
}
bill_amount =6;
} 
delay(5000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.

void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

