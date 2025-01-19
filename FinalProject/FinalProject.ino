#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define trigPin 13
#define echoPin 12
#define buzzerPin 9
#define resetPin 8

LiquidCrystal_I2C lcd(0x27, 16, 2);

int counter = 0, currentState = 0, previousState = 0;
long duration, distance;
unsigned long debounceTimer = 0;
const unsigned long debounceDelay = 300; // Delay for debounce in milliseconds

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Jumlah Pengunjung");
  delay(1000);
  lcd.clear();

  // Initialize pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(resetPin, INPUT_PULLUP);

  // Load saved counter value from EEPROM
  counter = EEPROM.read(0);
  if (counter < 0 || counter > 255) counter = 0; // Handle invalid EEPROM values
}

void loop() {
  // Handle Reset Button
  if (digitalRead(resetPin) == LOW) {
    counter = 0;
    EEPROM.write(0, counter); // Save to EEPROM
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Counter Reset!");
    delay(200);
    lcd.clear();
  }

  // Ultrasonic distance measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1; // Calculate distance in cm

  // Debounce and state update
  currentState = (distance <= 100) ? 1 : 0; // Updated to detect up to 100 cm
  if (currentState != previousState && millis() - debounceTimer > debounceDelay) {
    if (currentState == 1) {
      counter++;
      EEPROM.write(0, counter); // Save counter to EEPROM
      tone(buzzerPin, 1000, 200); // Play sound
    }
    previousState = currentState;
    debounceTimer = millis();
  }

  // Update LCD only if counter changes
  static int lastCounter = -1;
  if (counter != lastCounter) {
    lcd.setCursor(0, 0);
    lcd.print("Pengunjung Masuk:");
    lcd.setCursor(0, 1);
    lcd.print("Total: ");
    lcd.print(counter);
    lcd.print("   "); // Clear trailing characters
    lastCounter = counter;
  }

  delay(50);
}
