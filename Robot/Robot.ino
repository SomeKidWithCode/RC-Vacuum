// Libs
#include "Servo.h"
#include "SPI.h"
#include "RF24.h"
#include "printf.h"


// Transceiver pins
const byte CE_PIN = 9;
const byte CSN_PIN = 10;

// Motor pins
const byte MOTOR_LEFT_IN_1 = 8;
const byte MOTOR_LEFT_IN_2 = 7;
const byte MOTOR_LEFT_SPEED = 6;

const byte MOTOR_RIGHT_IN_1 = 4;
const byte MOTOR_RIGHT_IN_2 = 2;
const byte MOTOR_RIGHT_SPEED = 3;

// ESC pin
const byte ESC_SIG_PIN = 5;


struct TransData {
    int leftMotorSpeed;
    int rightMotorSpeed;
    bool reverse;
    bool tV = false;
};


// Defining transceiver things
RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000002";
const byte writingPipe[6] = "000001";

// Defining ESC controller
Servo ESC;

// Must be floats for correct decimals when printing
const float ESC_ARMING_VAL = 50;
const float ESC_MAX_VAL = 179;
const float ESC_RANGE = ESC_MAX_VAL - ESC_ARMING_VAL;


void setup() {
    // Begin serial (we don't wait because if it's not connect to a computer, it will wait forever)
    Serial.begin(9600);

    // Transceiver setup
    if (!transceiver.begin()) {
        Serial.println(F("Transceiver hardware not responding"));
        while (1) { }  // Hold program in infinite loop until transceiver is ready
    }
    transceiver.openReadingPipe(0, readingPipe);
    transceiver.setPALevel(RF24_PA_MIN);
    transceiver.startListening();

    // ESC setup
    ESC.attach(ESC_SIG_PIN);
    ESC.write(ESC_ARMING_VAL);

    // Set up motor pins
    // Left
    pinMode(MOTOR_LEFT_IN_1, OUTPUT);
    pinMode(MOTOR_LEFT_IN_2, OUTPUT);
    pinMode(MOTOR_LEFT_SPEED, OUTPUT);
    // Right
    pinMode(MOTOR_RIGHT_IN_1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN_2, OUTPUT);
    pinMode(MOTOR_RIGHT_SPEED, OUTPUT);

    // Set up motor directions and initial speeds
    // Left
    digitalWrite(MOTOR_LEFT_IN_1, LOW);
    digitalWrite(MOTOR_LEFT_IN_2, HIGH);
    analogWrite(MOTOR_LEFT_SPEED, 0);
    // Right
    digitalWrite(MOTOR_RIGHT_IN_1, LOW);
    digitalWrite(MOTOR_RIGHT_IN_2, HIGH);
    analogWrite(MOTOR_RIGHT_SPEED, 0);
}

void loop() {
    if (transceiver.available()) {
        TransData tData;
        transceiver.read(&tData, sizeof(tData));
        Serial.print(F("Received: Left motor speed: "));
        Serial.print(tData.leftMotorSpeed);
        Serial.print(F(", right motor speed: "));
        Serial.print(tData.rightMotorSpeed);
        Serial.print(F(", reversed: "));
        Serial.println(tData.reverse);

        setMotorDirection(tData.reverse);

        if (tData.leftMotorSpeed == 0 && tData.rightMotorSpeed == 0)
            stopMotors();
        else
            setMotorSpeed(tData.leftMotorSpeed, tData.rightMotorSpeed);


        // Experiment
        if (tData.tV) {
            Serial.println("Exp trig received");
            stopMotors();
            activateVac();
        }
    }
}


void setMotorSpeed(int left, int right) {
    analogWrite(MOTOR_LEFT_SPEED, left);
    analogWrite(MOTOR_RIGHT_SPEED, right);
}

// An alias for setMotorSpeed(0, 0)
void stopMotors() { setMotorSpeed(0, 0); }

// This prevents rapid writing of digital values when not nessesary
bool dirRevFlag = false;
void setMotorDirection(bool reversed) {
    if (dirRevFlag != reversed) {
        if (reversed) {
            // Left
            digitalWrite(MOTOR_LEFT_IN_1, HIGH);
            digitalWrite(MOTOR_LEFT_IN_2, LOW);
            // Right
            digitalWrite(MOTOR_RIGHT_IN_1, HIGH);
            digitalWrite(MOTOR_RIGHT_IN_2, LOW);
        }
        else {
            // Left
            digitalWrite(MOTOR_LEFT_IN_1, LOW);
            digitalWrite(MOTOR_LEFT_IN_2, HIGH);
            // Right
            digitalWrite(MOTOR_RIGHT_IN_1, LOW);
            digitalWrite(MOTOR_RIGHT_IN_2, HIGH);
        }
        dirRevFlag = reversed;
    }
}


void activateVac() {
    ESC.write(100);
    delay(1000);
    deactivateVac();
}

void deactivateVac() {
    ESC.write(ESC_ARMING_VAL);
}
