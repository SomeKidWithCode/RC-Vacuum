#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 9
#define CSN_PIN 10

RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000001";
const byte writingPipe[6] = "000002";

void setup() {
  transceiver.begin();
  transceiver.openReadingPipe(0, readingPipe);
  transceiver.setPALevel(RF24_PA_MIN);
  transceiver.startListening();

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
}

