// Libs
#include "SPI.h"
#include "RF24.h"
#include "printf.h"
#include "stdarg.h"


// Transceiver pins
const byte CE_PIN = 9;
const byte CSN_PIN = 10;

// Joystick pins
const byte VRX_PIN = A0;
const byte VRY_PIN = A1;
const byte SW_PIN = 2;


struct TransData {
    int leftMotorSpeed;
    int rightMotorSpeed;
    bool reverse;
    bool tV = false;
};


// Defining transceiver params
RF24 transceiver(CE_PIN, CSN_PIN);
const byte readingPipe[6] = "000001";
const byte writingPipe[6] = "000002";


bool isEven(int n) { return (n & 1) == 0; }
bool isNegative(int n) { return n < 0; }
bool between(int n, int min, int max) { return n > min && max > n; }

// Magic function for performing magic
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
    //transceiver.setPayloadSize(sizeof(TransData)); // This isn't nessesary, but it potentially makes transmission faster

    // Set pins
    pinMode(VRX_PIN, INPUT);
    pinMode(VRY_PIN, INPUT);
    pinMode(SW_PIN, INPUT);
}

void loop() {
    // Read joystick values
    // x is left and right (pos-neg)
    // y is up and down (pos-neg)
    int x = analogRead(VRX_PIN);
    int y = analogRead(VRY_PIN);
    int sw = digitalRead(SW_PIN);
    

    if (true) {
        Serial.print("Joystick: x: ");
        Serial.print(x);
        Serial.print(", y: ");
        Serial.print(y);
        Serial.print(", switch: ");
        Serial.println(sw);
        /*printn("sisisi", 
            "Joystick: x: ",
            x,
            ", y: ",
            y,
            ", switch: ",
            sw
        );*/
    }

    // Map joystick values
    int mapped_x = map(x, 0, 1023, -255, 255);
    int mapped_y = map(y, 0, 1023, -255, 255);

    // Because the standby position is not perfectly 0, we have to add a buffer range
    if (between(mapped_y, -10, 10))
      mapped_y = 0;
    if (between(mapped_x, -10, 10))
      mapped_x = 0;


    TransData tData;

    // Step 1, reverse directions if needed
    tData.reverse = isNegative(mapped_y);

    // Step 2, *magic*
    // Set the inital motor speeds to the absolute y value
    // Because this ranges from -255 to 255, we'll get the speed which can only
    // be 0-255
    int leftSpeed = abs(mapped_y);
    int rightSpeed = abs(mapped_y);

    
    if (isNegative(mapped_x))
        // += because mapped_x is already -ve
        rightSpeed += mapped_x;
    else
        leftSpeed -= mapped_x;

    tData.leftMotorSpeed = leftSpeed;
    tData.rightMotorSpeed = rightSpeed;


    // Experiment
    if (Serial.available()) {
        tData.tV = true;
        Serial.println("Exp trig");
        Serial.read();
    }


    bool transmitted = transceiver.write(&tData, sizeof(TransData));
    if (transmitted) {
        Serial.print("Transmitted: Left motor speed: ");
        Serial.print(tData.leftMotorSpeed);
        Serial.print(", right motor speed: ");
        Serial.print(tData.rightMotorSpeed);
        Serial.print(", reversed: ");
        Serial.println(tData.reverse);
        /*printn("sisisb",
            "Transmitted: Left motor speed: ",
            tData.leftMotorSpeed,
            ", right motor speed: ",
            tData.rightMotorSpeed,
            ", reversed: ",
            tData.reverse
        );*/
    }
    else {
        Serial.println("Failed to transmit");
    }

    delay(500);
}
