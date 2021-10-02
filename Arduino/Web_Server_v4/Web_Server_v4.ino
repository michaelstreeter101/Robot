  /*
Web Server v4
Authors: Michael Streeter & Anthony Streeter*/

#include "WiFiEsp.h"
#include "elapsedMillis.h"
#include <SabertoothSimplified.h>

char ssid[] = "";          // your network SSID (name)
char pass[] = "";       // your network password
int status = WL_IDLE_STATUS;        // the Wifi radio's status
int reqCount = 0;                   // number of requests received
int reqBufferSize = 1000;           // buffer to receive request size in bytes
//char *reqBuffer = (char*)malloc(reqBufferSize); // area to hold the HTTP request
char reqBuffer[1000] = {'\0'};

WiFiEspServer server(80);
elapsedMillis Timer1;

SabertoothSimplified ST; // We'll name the Sabertooth object ST.
// This sample uses a baud rate of 9600.
//
// Connections to make:
//   Arduino TX->1  ->  Sabertooth S1
//   Arduino GND    ->  Sabertooth 0V
//   Arduino VIN    ->  Sabertooth 5V (OPTIONAL, if you want the Sabertooth to power the Arduino)
//
// NB: can use a pin other than TX->1, see the SoftwareSerial example for how.
void setup()
{
  SabertoothTXPinSerial.begin(9600); // This is the baud rate you chose with the DIP switches.
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  ST.motor(1, 0);    // Stop. In an emergency,
  ST.motor(2, 0);    // just hit the 'reset' button on the Arduino.

  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial3.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial3);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  printWifiStatus();
  
  // start the web server on port 80
  server.begin();
}

void homePage( WiFiEspClient client ){
  // send a standard http response header
  // use \r\n instead of many println statements to speedup data send
  client.print(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: Keep-Alive\r\n"  // the connection will be left open after completion of the response
    "\r\n");
  client.print("<!DOCTYPE HTML>\r\n");
  client.print("<html>\r\n");
  client.print("<head>\r\n");
  client.print("<meta charset='utf-8'/>\r\n");
  client.print("<title>Telepresence</title>\r\n");
  client.print("<script>\r\n");
  client.print("function HttpRequest(url, method = 'get', data = '') {\r\n");
  client.print("  url = url.toString();\r\n");
  client.print("  method = method.toString();\r\n");
  client.print("  data = data.toString();\r\n");
  client.print("  //CHECK CSRF-Token\r\n");
  client.print("  function getCookie(name) {\r\n");
  client.print("    var cookieValue = null;\r\n");
  client.print("    if (document.cookie && document.cookie !== '') {\r\n");
  client.print("      var cookies = document.cookie.split(';');\r\n");
  client.print("      for (var i = 0; i < cookies.length; i++) {\r\n");
  client.print("        var cookie = cookies[i].trim();\r\n");
  client.print("        // Does this cookie string begin with the name we want?\r\n");
  client.print("        if (cookie.substring(0, name.length + 1) === (name + '=')) {\r\n");
  client.print("          cookieValue = decodeURIComponent(cookie.substring(name.length + 1));\r\n");
  client.print("          break;\r\n");
  client.print("        }\r\n");
  client.print("      }\r\n");
  client.print("    }\r\n");
  client.print("    return cookieValue;\r\n");
  client.print("  }\r\n");
  client.print("  //Check if there is a hidden csrf-input-field, if so, get the value\r\n");
  client.print("  try {\r\n");
  client.print("    var csrffield = document.getElementsByTagName('input');\r\n");
  client.print("    var l = csrffield.length;\r\n");
  client.print("    var i = 0;\r\n");
  client.print("    while (true) {\r\n");
  client.print("      if (i > l) {\r\n");
  client.print("        break;\r\n");
  client.print("      }\r\n");
  client.print("      if (csrffield[i].name.toLowerCase().includes('csrf') == true) {\r\n");
  client.print("        csrftoken = csrffield[i].value;\r\n");
  client.print("        break;\r\n");
  client.print("      }\r\n");
  client.print("      i++;\r\n");
  client.print("    }\r\n");
  client.print("  } catch (err) {\r\n");
  client.print("    var csrftoken = getCookie('csrftoken');\r\n");
  client.print("  }\r\n");
  client.print("  var req = new XMLHttpRequest();\r\n");
  client.print("  if (method == 'get' || method == 'GET' || method == 'Get') {\r\n");
  client.print("    req.open('GET', url, false);\r\n");
  client.print("    req.send();\r\n");
  client.print("    if (req.status == 200) {\r\n");
  client.print("      return req.responseText;\r\n");
  client.print("    }\r\n");
  client.print("  }\r\n");
  client.print("  if (method == 'post' || method == 'POST' || method == 'Post') {\r\n");
  client.print("    req.open('POST', url, false);\r\n");
  client.print("    if (data != '') {\r\n");
  client.print("      req.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');\r\n");
  client.print("    }\r\n");
  client.print("    if (csrftoken != '' || csrftoken != null) {\r\n");
  client.print("      //Set it twice with a different header-name so that different apps can deal with it\r\n");
  client.print("      req.setRequestHeader('X-CSRF-Token', csrftoken);\r\n");
  client.print("      req.setRequestHeader('X-CSRFToken', csrftoken);\r\n");
  client.print("    }\r\n");
  client.print("    req.send(data);\r\n");
  client.print("    if (req.status == 200) {\r\n");
  client.print("      return req.responseText;\r\n");
  client.print("    }\r\n");
  client.print("  }\r\n");
  client.print("}\r\n");
  client.print("\r\n");
  client.print("function addListeners() {\r\n");
  client.print("  console.log('addListeners')\r\n");
  client.print("  document.getElementById('forward').addEventListener('mousedown', e => {\r\n");
  client.print("    console.log('mousedown');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/forward', method = 'post', data = 'command=forward');\r\n");
  client.print("  });\r\n");
  client.print("  document.getElementById('forward').addEventListener('mouseup', e => {\r\n");
  client.print("    console.log('mouseup');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/stop', method = 'post', data = 'command=stop');\r\n");
  client.print("  });\r\n");

  client.print("  document.getElementById('backward').addEventListener('mousedown', e => {\r\n");
  client.print("    console.log('mousedown');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/backward', method = 'post', data = 'command=backward');\r\n");
  client.print("  });\r\n");
  client.print("  document.getElementById('backward').addEventListener('mouseup', e => {\r\n");
  client.print("    console.log('mouseup');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/stop', method = 'post', data = 'command=stop');\r\n");
  client.print("  });\r\n");

  client.print("  document.getElementById('stop').addEventListener('mousedown', e => {\r\n");
  client.print("    console.log('mousedown');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/stop', method = 'post', data = 'command=stop');\r\n");
  client.print("  });\r\n");

  client.print("  document.getElementById('clockwise').addEventListener('mousedown', e => {\r\n");
  client.print("    console.log('mousedown');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/clockwise', method = 'post', data = 'command=clockwise');\r\n");
  client.print("  });\r\n");
  client.print("  document.getElementById('clockwise').addEventListener('mouseup', e => {\r\n");
  client.print("    console.log('mouseup');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/stop', method = 'post', data = 'command=stop');\r\n");
  client.print("  });\r\n");

  client.print("  document.getElementById('anticlockwise').addEventListener('mousedown', e => {\r\n");
  client.print("    console.log('mousedown');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/anticlockwise', method = 'post', data = 'command=anticlockwise');\r\n");
  client.print("  });\r\n");
  client.print("  document.getElementById('anticlockwise').addEventListener('mouseup', e => {\r\n");
  client.print("    console.log('mouseup');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/stop', method = 'post', data = 'command=stop');\r\n");
  client.print("  });\r\n");

  client.print("  document.getElementById('right').addEventListener('mousedown', e => {\r\n");
  client.print("    console.log('mousedown');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/right', method = 'post', data = 'command=right');\r\n");
  client.print("  });\r\n");
  client.print("  document.getElementById('right').addEventListener('mouseup', e => {\r\n");
  client.print("    console.log('mouseup');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/stop', method = 'post', data = 'command=stop');\r\n");
  client.print("  });\r\n");

  client.print("  document.getElementById('left').addEventListener('mousedown', e => {\r\n");
  client.print("    console.log('mousedown');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/left', method = 'post', data = 'command=left');\r\n");
  client.print("  });\r\n");
  client.print("  document.getElementById('left').addEventListener('mouseup', e => {\r\n");
  client.print("    console.log('mouseup');\r\n");
  client.print("    HttpRequest('http://192.168.0.97/stop', method = 'post', data = 'command=stop');\r\n");
  client.print("  });\r\n");
  
  client.print("}\r\n");
  client.print("</script>\r\n");
  client.print("</head>\r\n");
  client.print("<body onload='addListeners();'>\r\n");
  client.print("<h1>Telepresence Robot!</h1>\r\n");
  client.print("Requests received: ");
  client.print(++reqCount);
  client.print("<br>\r\n");
  //client.print("Analog input A0: ");
  //client.print(analogRead(0));
  //client.print("<br>\r\n");
  //client.print("<input id='csrf' name='csrf' type='text'>\r\n");
  client.print("<button type='button' id='stop' name='stop' formmethod='post'>Stop</button>\r\n");
  client.print("<button type='button' id='forward' name='forward' formmethod='post'>Forward</button>\r\n");
  client.print("<br />");
  client.print("<button type='button' id='anticlockwise' name='anticlockwise' formmethod='post'>Anticlockwise Rotation</button>\r\n");
  client.print("<button type='button' id='clockwise' name='clockwise' formmethod='post'>Clockwise Rotation</button>\r\n");
  client.print("<br />");
  client.print("<button type='button' id='left' name='left' formmethod='post'>Left Corner</button>\r\n");
  client.print("<button type='button' id='right' name='right' formmethod='post'>Right Corner</button>\r\n");
  client.print("<br />");
  client.print("<button type='button' id='backward' name='backward' formmethod='post'>Backward</button>\r\n");
  client.print("</body>\r\n");
  client.print("</html>\r\n");
}

void loop()
{
  // listen for incoming clients
  WiFiEspClient client = server.available();
  if (client) {
    Serial.println("New client");
    int reqBufferPtr = 0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    Timer1 = 0; // reset Timer1. Fix for Esp32 buffer overrun bug.
    while (client.connected()) { 
      if (client.available()) {
        char c = client.read();
        reqBuffer[reqBufferPtr++] = c; // todo: check for buffer overrun if reqBufferPtr > reqBufferSize
        //Serial.write(c);
        if (Timer1 > 630){ // Fix for Esp32 buffer overrun bug: assume the HTTP request is over after 0.63 s.
          break;
        }
        // if you've reached the end of the line (received a newline
        // character) and the line is blank, then the http request has ended,
        // so you can send a reply.
        if (c == '\n' && currentLineIsBlank) {
          reqBuffer[reqBufferPtr+1] = '\0';
          //Serial.println("My reqBuffer =");
          //Serial.println(reqBuffer);
          String sReqBuffer = String(reqBuffer);
          Serial.println("My sReqBuffer =");
          Serial.println(sReqBuffer);
          
          // Q. was there a command (e.g. 'POST /forward HTTP/1.1')?
          int PostPtr = sReqBuffer.indexOf("POST ");
          int HttpPtr = sReqBuffer.indexOf(" HTTP/1.1", PostPtr);
          Serial.println("PostPtr: " + String(PostPtr) + ", HttpPtr: " + String(HttpPtr));
          if (PostPtr>=0 && HttpPtr > PostPtr){
            String sCmd = sReqBuffer.substring(PostPtr+6, HttpPtr); // +6 in order to look past the "POST /" part.
            Serial.print("Command detected: ");
            Serial.println(sCmd);

            /*
            stop
            forward
            clockwise rotation
            anticlockwise rotation
            right corner
            left corner
            backward
            */
            if (sCmd == "stop"){
              stop();
            } else if (sCmd == "forward"){
              forward();
            } else if (sCmd == "clockwise"){
              clockwise();
            } else if (sCmd == "anticlockwise"){
              anticlockwise();
            } else if (sCmd == "right"){
              right();
            } else if (sCmd == "left"){
              left();
            } else if (sCmd == "backward"){
              backward();
            } else {;
            }
            
            client.println("HTTP/1.1 304 Not Modified");
            client.println("");
            break;
          }
          
          else {
            Serial.println("No command!");

            Serial.println("Sending response");
            homePage( client );
            break;
          }
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
      // Serial.println("Client not available"); in a tight loop when CNA.
    }
    Serial.println("Not connected");
    // give the web browser time to receive the data
    Serial.println("delay(10)");
    delay(10);

    // close the connection:
    client.stop();
    Serial.println("Client disconnected");
    Serial.println(" ");
    // https://github.com/bportaluri/WiFiEsp/issues/53 ...?
  }
}

void stop(){
  Serial.println("function stop");
  ST.motor(1, 0);    // Stop.
  ST.motor(2, 0);    // Stop.
}

void forward(){
  Serial.println("function forward");
  ST.motor(1, 16);  // Go forward at 16.
  ST.motor(2, 16);  // Go forward at 16.
  delay(20);       // Wait 0.002 seconds.
  ST.motor(1, 32);  // Go forward at 32.
  ST.motor(2, 32);  // Go forward at 32.
}

void backward(){
  Serial.println("function backward");
  ST.motor(1, -16); // Go backward at 16
  ST.motor(2, -16); // Go backward at 16
  stop(); // Stop
}

void clockwise(){
  Serial.println("function clockwise");
  ST.motor(1, 16); // Go forward at 16
  ST.motor(2, -16); // Go forward at 16
  delay(3000); // Delay
  stop(); // Stop
}

void anticlockwise(){
  Serial.println("function anticlockwise");
  ST.motor(1, -16); // Rotation one wheel back one wheel forward
  ST.motor(2, 16);
  delay(3000); // Delay
  stop(); // Stop
}

void right(){
  Serial.println("right turn");
  ST.motor(1, 36); // Go forward at 36
  ST.motor(2, 16); // Go forward at 16
  delay(3000); // Delay
  stop(); // Stop
}

void left(){
  Serial.println("left turn");
  ST.motor(1, 16); // Go forward at 16
  ST.motor(2, 36); // Go forward at 36
  delay(3667); // Delay
  stop(); // Stop 
}

/*
stop
forward
back
clockwise rotation
anticlockwise rotation
right corner
left corner
*/

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  // print where to go in the browser
  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
}
