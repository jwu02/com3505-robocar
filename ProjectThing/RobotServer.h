/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "webpage.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// web sockets establishes and maintains a two-way communication between client and server
// for more response real-time communication, i.e. quicker response to robot commands
// without the need for page refreshes on the client side
AsyncWebSocket ws("/robocar_control");


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (strcmp((char*)data, "release") == 0) handleRelease();
        if (strcmp((char*)data, "forward") == 0) goForward();
        if (strcmp((char*)data, "backward") == 0) goBackward();
        if (strcmp((char*)data, "left") == 0) turnLeft();
        if (strcmp((char*)data, "right") == 0) turnRight();
    }
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
    return String();
}

// defining a FreeRTOS task for periodically calling the cleanupClients() function 
// to limit the number of clients by closing the oldest client when the maximum 
// number has been exceeded
void cleanupWebsocketClients(void *parameters) {
    while(1) {
        ws.cleanupClients();
        // Serial.println("Cleaned up websocket clients");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setupServer() {
    initWebSocket();

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });

    // Start server
    server.begin();

    xTaskCreate(
        cleanupWebsocketClients,   // Task function
        "cleanupWebsocketClients", // Task name
        1000,                     // Stack size (bytes)
        NULL,                      // Task parameters
        1,                         // Task priority, higher number = higher priority
        NULL                       // Task handle, allows you to interact with task from other tasks
    );
}

void loopServer() {
}