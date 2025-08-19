#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 9
#define CSN_PIN 10

#define VRX_PIN 5
#define VRY_PIN 6
#define SW_PIN 4


bool swit = false;

RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000001";
const byte writingPipe[6] = "000002";

void setup() {
  transceiver.begin();
  transceiver.openWritingPipe(writingPipe);
  transceiver.setPALevel(RF24_PA_MIN);

  transceiver.setPayloadSize(sizeof(char));

  Serial.begin(9600);

  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT);
}

void loop() {
  int x = analogRead(VRX_PIN);
  int y = analogRead(VRY_PIN);
  int sw = digitalRead(SW_PIN);

  if (Serial.available()) {
    char input = Serial.parseInt();
    transceiver.write(&input, sizeof(char));
  }


  //Serial.println(x);
  //Serial.println(y);
  //Serial.println(sw);
  delay(250);
}

class Test {
public:
  static const int a = 0;
};
