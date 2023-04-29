/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_MotorShield.h>

// Replace with your network credentials
const char* ssid = "kazdesu";
const char* password = "password";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/robocar_control");

// Create the motor shield object with the default I2C address 0x60
// https://learn.adafruit.com/adafruit-stepper-dc-motor-featherwing/pinouts#i2c-addressing-1883531
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4
Adafruit_DCMotor *leftMotor = AFMS.getMotor(2); // 2 and 3
Adafruit_DCMotor *rightMotor = AFMS.getMotor(1); // 1 and 4

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>Robocar Control</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>Robocar Control</h1>
  </div>
  <div class="content">
    <div class="card">
      <table>
        <tr>
          <td></td>
          <td><button id="forward-btn" class="button">Forward</button></td>
          <td></td>
        </tr>

        <tr>
          <td><button id="left-btn" class="button">Left</button></td>
          <td></td>
          <td><button id="right-btn" class="button">Right</button></td>
        </tr>

        <tr>
          <td></td>
          <td><button id="backward-btn" class="button">Backward</button></td>
          <td></td>
        </tr>
      </table>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/robocar_control`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    ['forward','backward','left','right'].forEach( direction => {
      document.getElementById(direction+'-btn').addEventListener('mousedown', function(){
        websocket.send(direction);
      });
      document.getElementById(direction+'-btn').addEventListener('mouseup', function(){
        websocket.send('release');
      });

      document.getElementById(direction+'-btn').addEventListener('touchstart', function(){
        websocket.send(direction);
      });
      document.getElementById(direction+'-btn').addEventListener('touchend', function(){
        websocket.send('release');
      });
    });
  }
</script>
</body>
</html>
)rawliteral";

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (strcmp((char*)data, "release") == 0) {
            handleRelease();
            Serial.println("STOP");
        }
        if (strcmp((char*)data, "forward") == 0) {
            handleRobotMotion(FORWARD, FORWARD, 100);
            Serial.println("FORWARD");
        }
        if (strcmp((char*)data, "backward") == 0) {
            handleRobotMotion(BACKWARD, BACKWARD, 100);
            Serial.println("BACKWARD");
        }
        if (strcmp((char*)data, "left") == 0) {
            handleRobotMotion(BACKWARD, FORWARD, 100);
            Serial.println("LEFT");
        }
        if (strcmp((char*)data, "right") == 0) {
            handleRobotMotion(FORWARD, BACKWARD, 100);
            Serial.println("RIGHT");
        }
    }
}

void handleRobotMotion(int leftMotorDirection, int rightMotorDirection, int motorSpeed) {
  // Set the speed to start, from 0 (off) to 255 (max speed)
    leftMotor->setSpeed(motorSpeed);
    rightMotor->setSpeed(motorSpeed);

    leftMotor->run(leftMotorDirection);
    rightMotor->run(rightMotorDirection);
}

void handleRelease() {
    // RELEASE stops the motor
    // removes power from the motor and is equivalent to setSpeed(0)
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

String processor(const String& var){
    Serial.println(var);
    // if(var == "STATE"){
    //     if (ledState){
    //         return "ON";
    //     } else{
    //         return "OFF";
    //     }
    // }
    return String();
}

void setup(){
    // Serial port for debugging purposes
    Serial.begin(115200);

    if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }
    Serial.println("Motor Shield found.");

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    // Print ESP Local IP Address
    Serial.println(WiFi.localIP()); // 172.20.10.7 for my mobile hotspot

    initWebSocket();

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });

    // Start server
    server.begin();

}

void loop() {
    // Periodically calling the cleanupClients() function from the main loop(), 
    // to limit the number of clients by closing the oldest client when the 
    // maximum number has been exceeded
    ws.cleanupClients();
    delay(10); // adding a delay makes actions received from client more responsive
}
