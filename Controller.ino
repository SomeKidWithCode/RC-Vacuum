#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 9
#define CSN_PIN 10

#define VRX_PIN 5
#define VRY_PIN 6
#define SW_PIN 8


bool swit = false;

RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000001";
const byte writingPipe[6] = "000002";

void setup() {
  transceiver.begin();
  transceiver.setPALevel(RF24_PA_MIN);

  transceiver.setPayloadSize(sizeof(int));

  Serial.begin(9600);

  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT);
}

void loop() {
  int x = analogRead(VRX_PIN);
  int y = analogRead(VRY_PIN);
  int sw = digitalRead(SW_PIN);

  int va = 69;

  transceiver.write(&va, sizeof(int));

  //Serial.println(x);
  //Serial.println(y);
  //Serial.println(sw);
}

class Test {
public:
  static const int a = 0;
};
