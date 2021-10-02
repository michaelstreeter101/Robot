/*
Web Server v4
Authors: Michael Streeter & Anthony Streeter
*/

/*This code is to run on the Arduino Mega 2560 + WiFi*/
#define USBCON x
//define DEBUG x

#include <WiFiEsp.h>
#include <elapsedMillis.h>
#include <SabertoothSimplified.h>

char ssid[] = "TP-Link_8049";   // your network SSID (name)
char pass[] = "27481752";       // your network password
int status = WL_IDLE_STATUS;    // the Wifi radio's status
int reqCount = 0;               // number of requests received
int reqBufferSize = 1000;       // buffer to receive request size in bytes
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
  ST.motor(1, 0);    // Stop. In an emergency,
  ST.motor(2, 0);    // just hit the 'reset' button on the Arduino.

  #if defined(DEBUG)
  Serial.begin(115200); // initialize serial for debugging 115200 Baud
  #else
  pinMode(LED_BUILTIN, OUTPUT);
  blink(5);
  #endif
  
  Serial3.begin(115200); // initialize serial for ESP module 115200 Baud
  WiFi.init(&Serial3); // initialize ESP module

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    #if defined(DEBUG)
    Serial.println("WiFi shield not present");
    #endif
    
    while (true){
      blink('w');
      delay(2000);
      }; // HALT
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    #if defined(DEBUG)
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    #endif
    
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  #if defined(DEBUG)
  Serial.println("You're connected to the network");
  printWifiStatus();
  Serial.print("Accessing Sabertooth on ");
  Serial.println(SabertoothTXPinSerial);
  #endif
  
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
  client.print("    req.open('GET', url, false);\r\n"); // NB: 3rd parameter, asyncProc, false specifies syncronous process (wait for response). 
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
  client.print("    console.log('POST...');\r\n");
  client.print("    req.send(data);\r\n");
  client.print("    console.log(req.status);\r\n");
  client.print("    //if (req.status == 200) {\r\n");
  client.print("      return req.responseText;\r\n");
  client.print("    //}\r\n");
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
    #if defined(DEBUG)
    Serial.println("New client");
    #endif
    
    int reqBufferPtr = 0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    Timer1 = 0; // reset Timer1. Fix for Esp32 buffer overrun bug.
    while (client.connected()) { 
      if (client.available() && reqBufferPtr < reqBufferSize) {
        char c = client.read();
        reqBuffer[reqBufferPtr++] = c;
        #if defined(DEBUG)
        Serial.write(c);
        #endif
        
        if (Timer1 > 100){ // Fix for Esp32 buffer overrun bug: assume the HTTP request is over after 0.63 s.
          #if defined(DEBUG)
          Serial.println("Esp32 buffer overrun timeout");
          #endif
          
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
          //Serial.println("My sReqBuffer =");
          //Serial.println(sReqBuffer);
          
          // Q. was there a command (e.g. 'POST /forward HTTP/1.1')?
          int PostPtr = sReqBuffer.indexOf("POST ");
          int HttpPtr = sReqBuffer.indexOf(" HTTP/1.1", PostPtr);
          #if defined(DEBUG)
          Serial.println("PostPtr: " + String(PostPtr) + ", HttpPtr: " + String(HttpPtr));
          #endif
          
          if (PostPtr>=0 && HttpPtr > PostPtr){
            String sCmd = sReqBuffer.substring(PostPtr+6, HttpPtr); // +6 in order to look past the "POST /" part.
            #if defined(DEBUG)
            Serial.print("Command detected: ");
            Serial.println(sCmd);
            #endif
            
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
            //break;
          }
          
          else {
            #if defined(DEBUG)
            Serial.println("No command!");
            Serial.println("Sending response");
            #endif
            
            homePage( client );
            // close the connection:
            client.stop();
            #if defined(DEBUG)
            Serial.println("Client disconnected");
            Serial.println(" ");
            #endif
            
            // https://github.com/bportaluri/WiFiEsp/issues/53 ...?
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
    #if defined(DEBUG)
    Serial.println("Not connected");    
    Serial.println("delay(10)");
    #endif
    
    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    //client.stop(); moved up...
    //Serial.println("Client disconnected");
    //Serial.println(" ");
    // https://github.com/bportaluri/WiFiEsp/issues/53 ...?
  }
}

void stop(){
  #if defined(DEBUG)
  Serial.println("function stop");
  #endif
  ST.motor(1, 0);    // Stop.
  ST.motor(2, 0);    // Stop.
  blink('s');
}

void forward(){
  #if defined(DEBUG)
  Serial.println("function forward");
  #endif
  blink('f'); // ..-.
  ST.motor(1, 16);  // Go forward at 16.
  ST.motor(2, 16);  // Go forward at 16.
  delay(20);        // Wait 0.02 seconds.
  ST.motor(1, 32);  // Go forward at 32.
  ST.motor(2, 32);  // Go forward at 32.
}

void backward(){
  #if defined(DEBUG)
  Serial.println("function backward");
  #endif
  blink('b'); // -...
  ST.motor(1, -16); // Go backward at 16
  ST.motor(2, -16); // Go backward at 16
  stop(); // Stop
}

void clockwise(){
  #if defined(DEBUG)
  Serial.println("function clockwise");
  #endif
  blink('c'); // -.-.
  ST.motor(1, 16); // Go forward at 16
  ST.motor(2, -16); // Go forward at 16
  delay(3000); // Delay
  stop(); // Stop
}

void anticlockwise(){
  #if defined(DEBUG)
  Serial.println("function anticlockwise");
  #endif
  blink('a'); // .-
  ST.motor(1, -16); // Rotation one wheel back one wheel forward
  ST.motor(2, 16);
  delay(3000); // Delay
  stop(); // Stop
}

void right(){
  #if defined(DEBUG)
  Serial.println("right turn");
  #endif
  blink('r'); // .-.
  ST.motor(1, 36); // Go forward at 36
  ST.motor(2, 16); // Go forward at 16
  delay(3000); // Delay
  stop(); // Stop
}

void left(){
  #if defined(DEBUG)
  Serial.println("left turn");
  #endif
  blink('l'); // .-..
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

void blink(int n) {
  for( int blinkCount = 0 ; blinkCount <= n ; blinkCount++ ){
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);                       // wait for a second
  }
}

void blink(char c) {
  switch (c) {
    case 'a': // .-
      blinkDit();
      blinkDa();
      break;
     case 'b': // -...
      blinkDa();
      blinkDit();
      blinkDit();
      blinkDit();
      break;
    case 'c': // -.-.
      blinkDa();
      blinkDit();
      blinkDa();
      blinkDit();
      break;
    case 'd': // -..
      blinkDa();
      blinkDit();
      blinkDit();
      break;
    case 'e': // .
      blinkDit();
      break;
    case 'f': // ..-.
      blinkDit();
      blinkDit();
      blinkDa();
      blinkDit();
      break;
    case 'g': // --.
      blinkDa();
      blinkDa();
      blinkDit();
      break;
    case 'h': // ....
      blinkDit();
      blinkDit();
      blinkDit();
      blinkDit();
      break;
    case 'i': // ..
      blinkDit();
      blinkDit();
      break;
    case 'j': // .---
      blinkDit();
      blinkDa();
      blinkDa();
      blinkDa();
      break;
    case 'k': // -.-
      blinkDa();
      blinkDit();
      blinkDa();
      break;
    case 'l': // .-..
      blinkDit();
      blinkDa();
      blinkDit();
      blinkDit();
      break;
    case 'm': // --
      blinkDa();
      blinkDa();
      break;
    case 'n': // -.
      blinkDa();
      blinkDit();
      break;
    case 'o': // ---
      blinkDa();
      blinkDa();
      blinkDa();
      break;
    case 'p': // .--.
      blinkDit();
      blinkDa();
      blinkDa();
      blinkDit();
      break;
    case 'q': // --.-
      blinkDa();
      blinkDa();
      blinkDit();
      blinkDa();
      break;
    case 'r': // .-.
      blinkDit();
      blinkDa();
      blinkDit();
      break;
    case 's': // ...
      blinkDit();
      blinkDit();
      blinkDit();
      break;
    case 't': // -
      blinkDa();
      break;
    case 'u': // ..-
      blinkDit();
      blinkDit();
      blinkDa();
      break;
    case 'v': // ...-
      blinkDit();
      blinkDit();
      blinkDit();
      blinkDa();
      break;
    case 'w': // .--
      blinkDit();
      blinkDa();
      blinkDa();
      break;
    case 'x': // -..-
      blinkDa();
      blinkDit();
      blinkDit();
      blinkDa();
      break;
    case 'y': // -.--
      blinkDa();
      blinkDit();
      blinkDa();
      blinkDa();
      break;
    case 'z': // --..
      blinkDa();
      blinkDa();
      blinkDit();
      blinkDit();
      break;
    case '1': // .----
      blinkDit();
      blinkDa();
      blinkDa();
      blinkDa();
      blinkDa();
      break;
    case '2': // ..---
      blinkDit();
      blinkDit();
      blinkDa();
      blinkDa();
      blinkDa();
      break;
    case '3': // ...--
      blinkDit();
      blinkDit();
      blinkDit();
      blinkDa();
      blinkDa();
      break;
    case '4': // ....-
      blinkDit();
      blinkDit();
      blinkDit();
      blinkDit();
      blinkDa();
      break;
    case '5': // .....
      blinkDit();
      blinkDit();
      blinkDit();
      blinkDit();
      blinkDit();
      break;
    case '6': // -....
      blinkDa();
      blinkDit();
      blinkDit();
      blinkDit();
      blinkDit();
      break;
    case '7': // --...
      blinkDa();
      blinkDa();
      blinkDit();
      blinkDit();
      blinkDit();
      break;
    case '8': // ---..
      blinkDa();
      blinkDa();
      blinkDa();
      blinkDit();
      blinkDit();
      break;
    case '9': // ---.
      blinkDa();
      blinkDa();
      blinkDa();
      blinkDa();
      blinkDit();
      break;
    case '0': // ---- 
      blinkDa();
      blinkDa();
      blinkDa();
      blinkDa();
      blinkDa();
      break;
    //default:
        }
}

void blinkDit() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);
}

void blinkDa() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);
}

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
