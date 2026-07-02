#include <DHT11.h>

#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN 2         // DHT11 data pin
#define DHTTYPE DHT11

#define PUMP_RELAY_PIN 9 // Relay module controlling the pump
#define HUM_THRESHOLD 50 // Humidity threshold (%)

// Time settings
#define SPRAY_INTERVAL 300000UL // 5 minutes in milliseconds
#define SPRAY_DURATION 30000UL  // 30 seconds in milliseconds

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned long lastSprayTime = 0; // Track last spray
bool sprayActive = false;
unsigned long sprayStartTime = 0;

void setup() {
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, HIGH); // Pump OFF initially (active LOW)

  lcd.begin(16, 2);
  lcd.print("System Init...");
  dht.begin();
  delay(2000);
  lcd.clear();
}

void loop() {
  // --- Read DHT11 ---
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!   ");
    delay(2000);
    return;
  }

  unsigned long currentTime = millis();

  // --- Automatic periodic spray ---
  if (!sprayActive && currentTime - lastSprayTime >= SPRAY_INTERVAL) {
    sprayActive = true;
    sprayStartTime = currentTime;
    digitalWrite(PUMP_RELAY_PIN, LOW); // Pump ON
  }

  // --- Stop spray after SPRAY_DURATION ---
  if (sprayActive && currentTime - sprayStartTime >= SPRAY_DURATION) {
    sprayActive = false;
    lastSprayTime = currentTime;
    digitalWrite(PUMP_RELAY_PIN, HIGH); // Pump OFF
  }

  // --- Humidity-based spray (active LOW relay) ---
  if (humidity < HUM_THRESHOLD && !sprayActive) {
    digitalWrite(PUMP_RELAY_PIN, LOW); // Pump ON
  } else if (!sprayActive) {
    digitalWrite(PUMP_RELAY_PIN, HIGH); // Pump OFF
  }

  // --- Display on LCD ---
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print((char)223); // Degree symbol
  lcd.print("C H:");
  lcd.print(humidity);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  lcd.print("Spray:");
  lcd.print(digitalRead(PUMP_RELAY_PIN) == LOW ? "ON " : "OFF");

  delay(1000); // Refresh every 1 second
}
