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

// Ultrasonic pins
const byte FRONT_TRIG_PIN = A0;
const byte FRONT_ECHO_PIN = A1;
const byte LEFT_TRIG_PIN = A2;
const byte LEFT_ECHO_PIN = A3;
const byte RIGHT_TRIG_PIN = A4;
const byte RIGHT_ECHO_PIN = A5;

struct TransData {
    int leftMotorSpeed;
    int rightMotorSpeed;
    bool tV = false;
};
const int DataSize = sizeof(TransData);

// Defining transceiver things
RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000002";
const byte writingPipe[6] = "000001";

// Defining ESC controller
Servo ESC;

// Must be floats for correct decimals when printing
const float ESC_ARMING_VAL = 1060;
const float ESC_MAX_VAL = 1860;
const float ESC_RANGE = ESC_MAX_VAL - ESC_ARMING_VAL;

long front_dist = 0;
long left_dist = 0;
long right_dist = 0;

bool autoVac = false;

bool isNegative(int n) { return n < 0; }

void setup() {
    // Begin serial (we don't wait because if it's not connect to a computer, it will wait forever)
    Serial.begin(9600);

    // Transceiver setup
    if (!transceiver.begin()) {
        Serial.println(F("Transceiver hardware not responding"));
        while (1) {} // Hold program in infinite loop until transceiver is ready
    }
    transceiver.openReadingPipe(0, readingPipe);
    transceiver.setPALevel(RF24_PA_MIN);
    transceiver.startListening();
    transceiver.setPayloadSize(DataSize);

    // ESC setup
    ESC.attach(ESC_SIG_PIN);
    ESC.writeMicroseconds(ESC_ARMING_VAL);

    // Set up motor pins
    // Left
    pinMode(MOTOR_LEFT_IN_1, OUTPUT);
    pinMode(MOTOR_LEFT_IN_2, OUTPUT);
    pinMode(MOTOR_LEFT_SPEED, OUTPUT);
    // Right
    pinMode(MOTOR_RIGHT_IN_1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN_2, OUTPUT);
    pinMode(MOTOR_RIGHT_SPEED, OUTPUT);

    // Set up ultrasonic pins
    pinMode(FRONT_TRIG_PIN, OUTPUT);
    pinMode(FRONT_ECHO_PIN, INPUT);
    pinMode(LEFT_TRIG_PIN, OUTPUT);
    pinMode(LEFT_ECHO_PIN, INPUT);
    pinMode(RIGHT_TRIG_PIN, OUTPUT);
    pinMode(RIGHT_ECHO_PIN, INPUT);

    // Set initial speeds
    // Left
    analogWrite(MOTOR_LEFT_SPEED, 0);
    // Right
    analogWrite(MOTOR_RIGHT_SPEED, 0);
}

void loop() {
    if (Serial.available()) {
        long n = Serial.parseInt();
        if (n == 0)
            return;
        Serial.print("Motor set to ");
        Serial.println(n);
        stopMotors();
        ESC.writeMicroseconds(1060 + n);
    }

    if (autoVac) {
        getDistancesFromSonics();

        Serial.print("Front: ");
        Serial.print(front_dist);
        Serial.print(", Left: ");
        Serial.print(left_dist);
        Serial.print(", Right: ");
        Serial.println(right_dist);

        // Facing wall head on
        if (front_dist < 16) {
            // Right has more space 
            if (left_dist < right_dist)
                setMotors(255, -255);
            // Left has more space
            else if (left_dist > right_dist)
                setMotors(-255, 255);
            // Neither has more space
            else
                setMotors(-255, -255);
        } else
            setMotors(255, 255);
    } else {
        if (transceiver.available()) {
            TransData tData;
            transceiver.read( & tData, DataSize);
            Serial.print(F("Received: Left motor speed: "));
            Serial.print(tData.leftMotorSpeed);
            Serial.print(F(", right motor speed: "));
            Serial.println(tData.rightMotorSpeed);

            setMotors(tData.leftMotorSpeed, tData.rightMotorSpeed);

            // Experiment
            if (tData.tV) {
                return;
                Serial.println("Exp trig received");
                Serial.read();
                stopMotors();
                activateVac();
            }
        }
    }
}

void setMotors(int left, int right) {
    // Set direction
    // Left
    digitalWrite(MOTOR_LEFT_IN_1, isNegative(left) ? HIGH : LOW);
    digitalWrite(MOTOR_LEFT_IN_2, isNegative(left) ? LOW : HIGH);
    // Right
    digitalWrite(MOTOR_RIGHT_IN_1, isNegative(right) ? HIGH : LOW);
    digitalWrite(MOTOR_RIGHT_IN_2, isNegative(right) ? LOW : HIGH);

    // Set speed to the absolute of their value
    analogWrite(MOTOR_LEFT_SPEED, abs(left));
    analogWrite(MOTOR_RIGHT_SPEED, abs(right));
}

// An alias for setMotors(0, 0)
void stopMotors() { setMotors(0, 0); }

void activateVac() {
    // Ramp up motor
    for (int i = 0; i < 800; i++) {
        ESC.writeMicroseconds(1060 + i);
        delay(10);
    }
    delay(3000);
    deactivateVac();
}

void deactivateVac() {
    ESC.writeMicroseconds(ESC_ARMING_VAL);
}

void getDistancesFromSonics() {
    digitalWrite(FRONT_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(FRONT_TRIG_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(FRONT_TRIG_PIN, LOW);
    long front_duration = pulseIn(FRONT_ECHO_PIN, HIGH);

    digitalWrite(LEFT_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(LEFT_TRIG_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(LEFT_TRIG_PIN, LOW);
    long left_duration = pulseIn(LEFT_ECHO_PIN, HIGH);

    digitalWrite(RIGHT_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(RIGHT_TRIG_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(RIGHT_TRIG_PIN, LOW);
    long right_duration = pulseIn(RIGHT_ECHO_PIN, HIGH);

    // You can't trigger them in parallel due to pulseIn
    // And for some reason this now executes extrodinarily fast

    front_dist = microsecondsToCentimetres(front_duration);
    left_dist = microsecondsToCentimetres(left_duration);
    right_dist = microsecondsToCentimetres(right_duration);
}

long microsecondsToCentimetres(long microseconds) { return microseconds / 29 / 2; }
