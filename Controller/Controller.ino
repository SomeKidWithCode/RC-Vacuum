// Libs
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// Transceiver pins
#define CE_PIN 9
#define CSN_PIN 10

// Joystick pins
#define VRX_PIN A0
#define VRY_PIN A1
#define SW_PIN 4

// Defining transceiver things
RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000001";
const byte writingPipe[6] = "000002";


void setup() {
  // Begin serial
  Serial.begin(115200);
  // Wait for serial to be ready
  while (!Serial) {}

  // Transceiver setup
  transceiver.begin();
  transceiver.openWritingPipe(writingPipe);
  transceiver.setPALevel(RF24_PA_MIN);
  transceiver.setPayloadSize(sizeof(char));

  // Set pins
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT);
}

void loop() {
  // Read joystick values
  int x = analogRead(VRX_PIN);
  int y = analogRead(VRY_PIN);
  int sw = digitalRead(SW_PIN);

  if (Serial.available()) {
    char input = Serial.parseInt();
    transceiver.write(&input, sizeof(char));
  }


  Serial.println(x);
  //Serial.println(y);
  //Serial.println(sw);
  delay(250);
}

