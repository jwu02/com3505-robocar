/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

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
    .button {
      padding: 15px;
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

    .grid-container {
      display: grid;
      align-items: center;
      gap: 0.5em;
    }

    #robot-controls-grid {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
    }
    .arrow {
      border: solid black;
      border-width: 0 3px 3px 0;
      display: inline-block;
      padding: 3px;
    }
    .right {
      transform: rotate(-45deg);
      -webkit-transform: rotate(-45deg);
    }
    .left {
      transform: rotate(135deg);
      -webkit-transform: rotate(135deg);
    }
    .up {
      transform: rotate(-135deg);
      -webkit-transform: rotate(-135deg);
    }
    .down {
      transform: rotate(45deg);
      -webkit-transform: rotate(45deg);
    }
    #forward-btn {
      grid-column-start: 2;
      grid-column-end: 3;
      grid-row-start: 1;
      grid-row-end: 2;
    }
    #backward-btn {
      grid-column-start: 2;
      grid-column-end: 3;
      grid-row-start: 3;
      grid-row-end: 4;
    }
    #left-btn {
      grid-column-start: 1;
      grid-column-end: 2;
      grid-row-start: 2;
      grid-row-end: 3;
    }
    #right-btn {
      grid-column-start: 3;
      grid-column-end: 4;
      grid-row-start: 2;
      grid-row-end: 3;
    }
  </style>
  
  <title>Robocar</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>

<body>
  <div class="topnav">
    <h1>Robocar</h1>
  </div>
  <div class="content">
    <div class="grid-container">
      <!-- <div>
        <h2>Mode</h2>
        <div id="robot-mode-grid">
          Exploration Mode<input type="checkbox">
        </div>
      </div> -->
      
      <div>
        <h2>Controls</h2>
        <div id="robot-controls-grid">
          <button id="forward-btn" class="button"><i class="arrow up"></i></button>
          <button id="left-btn" class="button"><i class="arrow left"></i></button>
          <button id="right-btn" class="button"><i class="arrow right"></i></button>
          <button id="backward-btn" class="button"><i class="arrow down"></i></button>
        </div>
      </div>
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