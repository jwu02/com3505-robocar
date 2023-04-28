#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_MotorShield.h>

// globals for a wifi access point and webserver
String apSSID = "mea20jw_robocar";  // SSID of the access point
String apPassword = "password";

WebServer webServer(80); // create a webserver object that listens for HTTP request on port 80

// Create the motor shield object with the default I2C address 0x60
// https://learn.adafruit.com/adafruit-stepper-dc-motor-featherwing/pinouts#i2c-addressing-1883531
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4
Adafruit_DCMotor *leftMotor = AFMS.getMotor(3); // 2 and 3
Adafruit_DCMotor *rightMotor = AFMS.getMotor(4); // 1 and 4

void setup() {
    Serial.begin(115200); // initialise the serial line

    startAP();
    startWebServer();

    if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }
    Serial.println("Motor Shield found.");

    // Set the speed to start, from 0 (off) to 255 (max speed)
    leftMotor->setSpeed(150);
    leftMotor->run(FORWARD);
    // RELEASE stops the motor
    // removes power from the motor and is equivalent to setSpeed(0)
    leftMotor->run(RELEASE);

    rightMotor->setSpeed(150);
    rightMotor->run(FORWARD);
    rightMotor->run(RELEASE);
}

void loop() {
    

    webServer.handleClient(); // serve pending web requests
}

// start the access point
void startAP() {
    if(! WiFi.mode(WIFI_AP_STA))
        Serial.println("Failed to set Wifi mode");
    if(! WiFi.softAP(apSSID.c_str(), apPassword.c_str()))
        Serial.println("Failed to start soft AP");
}

// start the web server
void startWebServer() {
    // register callbacks to handle different routes
    webServer.on("/", handleRoot);
    webServer.on("left", []{handleRobotMotion(BACKWARD, FORWARD, 150);});
    webServer.on("right", []{handleRobotMotion(FORWARD, BACKWARD, 150);});
    webServer.on("forward", []{handleRobotMotion(FORWARD, FORWARD, 150);});
    webServer.on("backward", []{handleRobotMotion(BACKWARD, BACKWARD, 150);});
    webServer.onNotFound(handleNotFound);

    webServer.begin();
    Serial.println("HTTP server started");
}

// webserver handler callbacks
void handleNotFound() {
    Serial.println("URI Not Found: "+webServer.uri());
    webServer.send(200, "text/plain", "URI Not Found");
}

const char *pageHeader =
    "<html>\
        <head>\
            <title>Robot Control</title>\
            <meta charset='utf-8'>\
            <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
            <style>body{background:#FFF; color: #000; font-family: sans-serif; font-size: 150%;}</style>\
        </head>\
        \
        <body>";

const char *pageFooter =
    "   </body>\
    </html>";

void handleRoot() {
    Serial.println("Serving /");
    String htmlPage = ""; // a String to hold the resultant page
    htmlPage += pageHeader;
    const char *pageBody = "<h1>Robot Control</h1>\
    ";
    htmlPage += pageBody;
    htmlPage += pageFooter;
    webServer.send(200, "text/html", htmlPage);
}

void handleRobotMotion(int leftMotorDirection, int rightMotorDirection, int motorSpeed) {
    leftMotor->setSpeed(motorSpeed);
    rightMotor->setSpeed(motorSpeed);

    leftMotor->run(leftMotorDirection);
    rightMotor->run(rightMotorDirection);

    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}
