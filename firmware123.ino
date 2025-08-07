#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const int rotaryClkPin = 11;
const int rotaryDtPin = 12;
const int buttonPin = 13;
const int mosfetPins[] = {8, 9, 10};
int lastClkState;
int menuIndex = 0;
unsigned long durations[] = {60000, 120000, 180000};  // in ms
String labels[] = {"60s", "120s", "180s"};
bool selectionMade = false;
unsigned long startTime = 0;
void setup() {
  pinMode(rotaryClkPin, INPUT_PULLUP);
  pinMode(rotaryDtPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);

  for (int i = 0; i < 3; i++) {
    pinMode(mosfetPins[i], OUTPUT);
    digitalWrite(mosfetPins[i], LOW);
  }

  lastClkState = digitalRead(rotaryClkPin);

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found"));
    while (true);
  }

  displayMenu();
}
void loop() {
  if (!selectionMade) {
    int clkState = digitalRead(rotaryClkPin);
    int dtState = digitalRead(rotaryDtPin);

    if (clkState != lastClkState && clkState == LOW) {
      if (dtState != clkState) {
        menuIndex++;
      } else {
        menuIndex--;
      }

      if (menuIndex < 0) menuIndex = 2;
      if (menuIndex > 2) menuIndex = 0;

      displayMenu();
    }

    lastClkState = clkState;

    if (digitalRead(buttonPin) == LOW) {
      delay(200); 
      if (digitalRead(buttonPin) == LOW) {
        startTime = millis();
        selectionMade = true;

        for (int i = 0; i < 3; i++) {
          digitalWrite(mosfetPins[i], HIGH);
        }

        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 20);
        display.print("Running: ");
        display.print(labels[menuIndex]);
        display.display();
      }
    }
  } else {
    if (millis() - startTime >= durations[menuIndex]) {
      selectionMade = false;

      for (int i = 0; i < 3; i++) {
        digitalWrite(mosfetPins[i], LOW);
      }

      displayMenu();
    }
  }
}
void displayMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Select Time:");
  display.setCursor(0, 25);
  display.print(">");
  display.println(labels[menuIndex]);
  display.display();
}

                