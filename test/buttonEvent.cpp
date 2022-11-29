#include <Arduino.h>

#define BUTTON_PIN 7

int lastButtonState;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // enable the internal pull-up resistor
  lastButtonState = digitalRead(BUTTON_PIN);
}

void loop() {
  // read the value of the button
  int buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState != buttonState) { // state changed
    delay(50); // debounce time

    if (buttonState == LOW)
      Serial.println("The button pressed event");
    else
      Serial.println("The button released event");

    lastButtonState = buttonState;
  }
}