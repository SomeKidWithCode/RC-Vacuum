// Libs
#include "SPI.h"
#include "RF24.h"
#include "printf.h"
#include "stdarg.h"


// Transceiver pins
const byte CE_PIN = 9;
const byte CSN_PIN = 10;

// Controller pins
const byte LEFT_POT_PIN = A0;
const byte RIGHT_POT_PIN = A1;
const byte BTN_PIN = 8;

const int ZeroRange = 25;


struct TransData {
    int leftMotorSpeed;
    int rightMotorSpeed;
    bool tV = false;
};
const int DataSize = sizeof(TransData);


// Defining transceiver params
RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000001";
const byte writingPipe[6] = "000002";


bool isEven(int n) { return (n & 1) == 0; }
bool between(int n, int min, int max) { return n > min && max > n; }

// Magic function for performing magic... if it would work
void printn(String types...) {
    va_list args;
    int count = sizeof(types) - 1;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        if (types[i] == 's')
            if (i == count - 1)
                Serial.println(va_arg(args, String));
            else
                Serial.print(va_arg(args, String));
        else if (types[i] == 'i')
            if (i == count - 1)
                Serial.println(va_arg(args, int));
            else
                Serial.print(va_arg(args, int));
        else if (types[i] == 'f')
            if (i == count - 1)
                Serial.println(va_arg(args, float));
            else
                Serial.print(va_arg(args, float));
        else if (types[i] == 'b')
            if (i == count - 1)
                Serial.println(va_arg(args, bool));
            else
                Serial.print(va_arg(args, bool));
    }

    va_end(args);
}


void setup() {
    // Begin serial
    Serial.begin(9600);
    // And wait for it to be ready
    while (!Serial) { }

    // Transceiver setup
    if (!transceiver.begin()) {
        Serial.println(F("Radio hardware not responding"));
        while (1) { }  // Hold program in infinite loop until transceiver is ready
    }
    transceiver.openWritingPipe(writingPipe);
    transceiver.setPALevel(RF24_PA_MIN);
    transceiver.setPayloadSize(DataSize); // This isn't nessesary, but it potentially makes transmission faster


    // Set pins
    pinMode(LEFT_POT_PIN, INPUT);
    pinMode(RIGHT_POT_PIN, INPUT);
    pinMode(BTN_PIN, INPUT);
}

void loop() {
    // Read potentiometer values
    int left = analogRead(LEFT_POT_PIN);
    int right = analogRead(RIGHT_POT_PIN);
    int btn = digitalRead(BTN_PIN);
    

    if (true) {
        Serial.print("Left: ");
        Serial.print(left);
        Serial.print(", right: ");
        Serial.print(right);
        Serial.print(", btn: ");
        Serial.println(btn);
        /*printn("sisi", 
            "Joystick: x: ",
            x,
            ", y: ",
            y
        );*/
    }

    // Map joystick values
    int mapped_left = map(left, 0, 1023, -255, 255);
    int mapped_right = map(right, 0, 1023, -255, 255);

    // Because the standby position is not perfectly 0, we have to add a buffer range
    if (between(mapped_left, -ZeroRange, ZeroRange))
      mapped_left = 0;
    if (between(mapped_right, -ZeroRange, ZeroRange))
      mapped_right = 0;


    TransData tData;

    tData.leftMotorSpeed = mapped_left;
    tData.rightMotorSpeed = mapped_right;


    // Experiment
    if (btn) {
        tData.tV = true;
        Serial.println("Exp trig");
        //Serial.read(); // Must read in order to consume the serial data
    }


    bool transmitted = transceiver.write(&tData, DataSize);
    if (transmitted) {
        Serial.print("Transmitted: Left motor speed: ");
        Serial.print(tData.leftMotorSpeed);
        Serial.print(", right motor speed: ");
        Serial.println(tData.rightMotorSpeed);
        /*printn("sisi",
            "Transmitted: Left motor speed: ",
            tData.leftMotorSpeed,
            ", right motor speed: ",
            tData.rightMotorSpeed
        );*/
    }
    else {
        Serial.println("Failed to transmit");
    }

    delay(500);
}
