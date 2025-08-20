// Libs
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// Transceiver pins
#define CE_PIN 9
#define CSN_PIN 10

// Motor pins
#define MOTOR_A_IN_1 5
#define MOTOR_A_IN_2 4
#define MOTOR_B_IN_1 3
#define MOTOR_B_IN_2 2





// Abstract away the motor control
class Motor {
    int pinA;
    int pinB;
    bool reversed = false;
    
  public:
    Motor(int _pinA, int _pinB) {
      pinA = _pinA;
      pinB = _pinB;
    }
    
    void begin() {
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
    }
    
    void reverse() {
      reversed = true;
    }

    void start() {
      digitalWrite(pinA, reversed ? LOW : HIGH);
      digitalWrite(pinA, reversed ? HIGH : LOW);
    }

    void stop() {
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, LOW);
    }
};





// Defining transceiver things
RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000001";
const byte writingPipe[6] = "000002";

// Defining motors
Motor leftMotor(MOTOR_A_IN_1, MOTOR_A_IN_2);
Motor rightMotor(MOTOR_B_IN_1, MOTOR_B_IN_2);


void setup() {
  // Begin serial
  Serial.begin(115200);
  // Wait for serial to be ready
  while (!Serial) {}

  // Transceiver setup
  transceiver.begin();
  transceiver.openReadingPipe(0, readingPipe);
  transceiver.setPALevel(RF24_PA_MIN);
  transceiver.startListening();
  transceiver.setPayloadSize(sizeof(int));

  // Set up motors
  leftMotor.begin();
  rightMotor.begin();
  leftMotor.reverse();
}

void loop() {
  if (transceiver.available()) {
    char text[32] = "";
    transceiver.read(&text, sizeof(text));
    
  }

  // Start motors
  leftMotor.start();
  rightMotor.start();
  delay(500);

  // Stop motors
  leftMotor.stop();
  rightMotor.stop();
  delay(500);
}
