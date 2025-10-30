// Libs
#include "SPI.h"
#include "RF24.h"
#include "printf.h"


// Transceiver pins
const byte CE_PIN = 9;
const byte CSN_PIN = 10;

// Controller pins
const byte LEFT_POT_PIN = A0;
const byte RIGHT_POT_PIN = A1;
const byte VRX_PIN = A2;
const byte VRY_PIN = A3;
const byte BTN_PIN = 8;

const int ZeroRange = 25;

struct TransData {
    int leftMotorSpeed = 0;
    int rightMotorSpeed = 0;
    bool tV = false;
};
const int DataSize = sizeof(TransData);

// Defining transceiver params
RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000001";
const byte writingPipe[6] = "000002";

void setup() {
    // Begin serial
    Serial.begin(9600);
    // And wait for it to be ready
    while (!Serial) {}

    // Transceiver setup
    if (!transceiver.begin()) {
        Serial.println(F("Radio hardware not responding"));
        while (1) {} // Hold program in infinite loop until transceiver is ready
    }
    transceiver.openWritingPipe(writingPipe);
    transceiver.setPALevel(RF24_PA_MIN);
    transceiver.setPayloadSize(DataSize); // This isn't nessesary, but it potentially makes transmission faster

    // Set pins
    pinMode(VRX_PIN, INPUT);
    pinMode(VRY_PIN, INPUT);
    pinMode(BTN_PIN, INPUT);
}

void loop() {
    TransData tData;

    // Read joystick values
    int x = analogRead(VRX_PIN);
    int y = analogRead(VRY_PIN);
    int btn = digitalRead(BTN_PIN);

    // Map joystick values
    x = map(x, 0, 1023, -1, 1);
    y = map(y, 0, 1023, -1, 1);

    if (x == -1) {
        tData.leftMotorSpeed = 255;
        tData.rightMotorSpeed = -255;
    } else if (x == 1) {
        tData.leftMotorSpeed = -255;
        tData.rightMotorSpeed = 255;
    }

    if (y == -1) {
        tData.leftMotorSpeed = -255;
        tData.rightMotorSpeed = -255;
    } else if (y == 1) {
        tData.leftMotorSpeed = 255;
        tData.rightMotorSpeed = 255;
    }

    if (btn) {
        tData.tV = true;
        Serial.println("Exp trig");
        //Serial.read(); // Must read to consume the serial data and prevent it constantly triggering
    }

    bool transmitted = transceiver.write(&tData, DataSize);
    if (transmitted) {
        Serial.print("Transmitted: Left motor speed: ");
        Serial.print(tData.leftMotorSpeed);
        Serial.print(", right motor speed: ");
        Serial.println(tData.rightMotorSpeed);
    } else {
        Serial.println("Failed to transmit");
    }
}
