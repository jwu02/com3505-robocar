// Import required libraries
#include <Arduino.h>
#include <WiFi.h>
#include "RobotActions.h"
#include "RobotServer.h"
#include "UltrasonicSensor.h"

#include <time.h>

// access point credentials
const char* ssid = "Robocar (http://192.168.4.1/)";
const char* password = "password";

bool explorationMode = false;


void setupAP() {
    if(! WiFi.mode(WIFI_AP_STA))
        Serial.println("Failed to set Wifi mode");
    // Remove the password parameter, if you want the AP to be open
    if(! WiFi.softAP(ssid))
        Serial.println("Failed to start soft AP");
}

void setup(){
    Serial.begin(115200); // Serial port for debugging purposes

    setupAP();

    setupServer();
    setupMotorShield();
    setupUltrasonicSensor();
}

void loop() {
    loopServer();
    // if (explorationMode == true) {
    //     explorationControl();
    // }

    loopUltrasonicSensor();
    if (pulseDistance < SAFETY_MARGIN) {
        // maintain a distance of 10cm from an obstacle in front
        do {goBackward();loopUltrasonicSensor();} while (pulseDistance < SAFETY_MARGIN+10);
        handleRelease();
    }
}

void explorationControl() {
    // if obstacle detected, stop the robot
    // if no obstacle detected, continue
    srand ( time(NULL) );

    // if (ultrasonicDistance < 10) {
    //     handleRelease();
    // }
}
