#include <Servo.h>

const byte ESC_SIG_PIN = 5;
const byte POT_PIN = A0;

const float ESC_ARMING_VAL = 50;
const float ESC_MAX_VAL = 179;
const float ESC_RANGE = ESC_MAX_VAL - ESC_ARMING_VAL;

Servo esc;

bool escArmed = false;


// For this to work, do the following:
// Get a breadboard, Arduino and some cables
// Get a button and potentiometer
// Get a 10k resistor
// Put the button into the breadboard
// Connect 5V to one side of the button and the 10k to the same edge
// Connect the one of the other edges (opposite side of the 5V pin) and to pin 3
// Connect the potentiometer, specifically the middle to A0 (with the pins facing down, -ve on the left, +ve on the right)
// Finally, connect the ESC yellow cable to pin 5
// Once this has all been done, we should have a fully functional test platform
// https://cdn-global-hk.hobbyking.com/media/file/214281780X260742X20.pdf


void setup() {
    // Begin serial
    Serial.begin(9600);
    // And wait for it to be ready
    while (!Serial) { }
    
    esc.attach(ESC_SIG_PIN);
    esc.writeMicroseconds(0);

    pinMode(POT_PIN,     INPUT);

    Serial.println("Press enter on the serial monitor to arm.");
    Serial.println("Ensure the potentiometer is at its minimum before arming.");
}

void loop() {
    if (Serial.available() && !escArmed) {
        Serial.println("ARMING ESC IN 3 SECONDS");
        delay(1000);
        Serial.println("ARMING ESC IN 2 SECONDS");
        delay(1000);
        Serial.println("ARMING ESC IN 1 SECONDS");
        delay(1000);
        //esc.write(ESC_ARMING_VAL);
        esc.writeMicroseconds(1060); // Also works
        Serial.println("THE ESC HAS BEEN ARMED");
        escArmed = true;
        delay(1000);
    }

    if (escArmed) {
        int potVal = analogRead(POT_PIN);
        int mappedPot = map(potVal, 0, 1023, ESC_ARMING_VAL, ESC_MAX_VAL);
        esc.write(mappedPot);
        Serial.print("ESC power: ");
        Serial.print(((mappedPot - ESC_ARMING_VAL) / ESC_RANGE) * 100);
        Serial.println("%");
    }
}
