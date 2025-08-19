#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 9
#define CSN_PIN 10

#define MOTOR_A_IN_1 5
#define MOTOR_A_IN_2 4
#define MOTOR_B_IN_1 3
#define MOTOR_B_IN_2 2

RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000001";
const byte writingPipe[6] = "000002";

void setup() {
  transceiver.begin();
  transceiver.openReadingPipe(0, readingPipe);
  transceiver.setPALevel(RF24_PA_MIN);
  transceiver.startListening();

  pinMode(MOTOR_A_IN_1, OUTPUT);
  pinMode(MOTOR_A_IN_2, OUTPUT);
  pinMode(MOTOR_B_IN_1, OUTPUT);
  pinMode(MOTOR_B_IN_2, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  transceiver.setPayloadSize(sizeof(int));
}

void loop() {
  if (transceiver.available()) {
    char text[32] = "";
    transceiver.read(&text, sizeof(text));
    
  }

  digitalWrite(LED_BUILTIN, HIGH);

  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);

  // Left forward
  digitalWrite(MOTOR_B_IN_1, 0);
  digitalWrite(MOTOR_B_IN_2, 1);
  delay(500);

  digitalWrite(MOTOR_B_IN_1, 0);
  digitalWrite(MOTOR_B_IN_2, 0);
  delay(500);
  // Right forward
  digitalWrite(MOTOR_A_IN_1, 1);
  digitalWrite(MOTOR_A_IN_2, 0);
  delay(500);

  digitalWrite(MOTOR_A_IN_1, 0);
  digitalWrite(MOTOR_A_IN_2, 0);
  delay(500);
}

