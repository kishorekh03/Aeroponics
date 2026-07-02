#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "DHTesp.h"

#define DHTpin 14       // D5 = GPIO14
#define LDR A0
#define pump 5          // D1 = GPIO5

#define ACTIVE_LOW_RELAY 1   // Most relay modules are active LOW

DHTesp dht;
float humidity, temperature;

// --- WiFi and ThingSpeak ---
const char* ssid = "OnePlus Nord CE 3 Lite 5G";
const char* password = "tommybuffy";
WiFiClient client;
unsigned long myChannelNumber = 3141746;
const char* myWriteAPIKey = "ZDIWSEMPXDSUDM0I";

unsigned long initialTime = 0;
unsigned long nowTime;

void setup() {
    Serial.begin(115200);
    pinMode(pump, OUTPUT);

  // Pump OFF initially
#if ACTIVE_LOW_RELAY
    digitalWrite(pump, HIGH);
#else
    digitalWrite(pump, LOW);
#endif

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
}

    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    dht.setup(DHTpin, DHTesp::DHT11);
    WiFi.mode(WIFI_STA);
    ThingSpeak.begin(client);
}



void loop() {
  // --- Read Sensors ---
    int ldrValue = analogRead(LDR);
    float light = ldrValue * (5.0 / 1023.0);
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();

    Serial.println("-----------------------------");
    Serial.print("Temperature (°C): "); Serial.println(temperature);
    Serial.print("Humidity (%): "); Serial.println(humidity);
    Serial.print("Light Value: "); Serial.println(light);

  // --- Upload to ThingSpeak ---
    if (!isnan(temperature)) ThingSpeak.writeField(myChannelNumber, 1, temperature, myWriteAPIKey);
    delay(2000);
    if (!isnan(humidity)) ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);
    delay(2000);
    ThingSpeak.writeField(myChannelNumber, 3, light, myWriteAPIKey);
    delay(2000);

  // --- Pump Control using millis() ---
    nowTime = millis();
    if ((nowTime - initialTime) / 1000 >= 30) {  // Every 30 seconds
#if ACTIVE_LOW_RELAY
     digitalWrite(pump, LOW);   // Turn pump ON
#else
    digitalWrite(pump, HIGH);  // Turn pump ON
#endif
    Serial.println("Pump ON");
    delay(5000);               // Keep pump ON for 5 seconds
#if ACTIVE_LOW_RELAY
digitalWrite(pump, HIGH);  // Turn pump OFF
#else
    digitalWrite(pump, LOW);   // Turn pump OFF
#endif
    Serial.println("Pump OFF");
    initialTime = nowTime;     // Reset timer
     }

    delay(1000);
}