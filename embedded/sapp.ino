
#include <WiFi.h>
#define relay  12


// Create the objects for server and client
WiFiServer server(80);
WiFiClient client;
bool itOn = false;

const char* ssid   = "AutoHome";// This is the SSID that ESP32 will broadcast
const char* password = "12345678";     // password should be atleast 8 characters to make it work

// Create the global variable
String http;


void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);

  Serial.println( "" );
  Serial.println( "WiFi AP is now running" );
  Serial.println( "IP address: " );
  Serial.println( WiFi.softAPIP() );

  // Start our ESP32 server
  server.begin();
}



void sendResponse() {
  // Send the HTTP response headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
}

void updateWebpage() {

  // Send the whole HTML
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<title>autoAlarm</title>");
  client.println("<style>*,body {margin: 0;padding: 0;background-color:");
  client.println("#c7f9cc;font-family:Tahoma, Geneva, Verdana, sans-serif;}");
  client.println(".center {text-align: center;padding: 17px 0;font-size: 1.7rem;font-weight: bolder;");
  client.println("background-color: #57cc99;color: #22577a;margin-bottom: 20px;}");
  client.println(".on {width: 200px;height: 200px;background-color: #38a3a5;border-radius: 50%;");
  client.println("margin: 0 auto;display: flex;align-items: center;justify-content: center;}");
  client.println(".off {width: 200px;height: 200px;background-color: red;border-radius: 50%;");
  client.println("margin: 0 auto;display: flex;align-items: center;justify-content: center;}");
  client.println("span {font-size: 3rem;font-weight: bolder;}");
  client.println("a{text-decoration: none;  }");
  client.println("div , .on , #on {background-color:#38a3a5;color: orangered;}");
  client.println("div , .off , #off {background-color:red;color: whitesmoke;}");
  client.println("</style></head>");
  // Web Page Heading
  client.println("<body><div class='center'>autoAlarm</div>");
  if (!itOn) {
    client.println("<div class=\"on\"><a href=\"/on\"><span id=\"on\">ON</span></a></div>");
  } else {
    client.println("<div class=\"off\"><a href=\"/off\"><span id=\"off\">OFF</span></a></div>");
  }

  client.println("</body></html>");
  client.println();
}

void updateRelay() {
  // In here we will check the HTTP request of the connected client
  //  using the HTTP GET function,
  //  Then turns the LED on / off according to the HTTP request
  if    (http.indexOf("GET /on") >= 0) {
    Serial.println("Turning the relay ON");
    itOn = true;
    digitalWrite(relay, HIGH);
  } else if (http.indexOf("GET /off") >= 0) {
    Serial.println("Turning off relay");
    digitalWrite(relay, LOW);
    itOn = false;
  }
}

void loop() {

  if ( client = server.available() ) {  // Checks if a new client tries to connect to our server
    Serial.println("New Client.");
    String clientData = "";
    while ( client.connected() ) {    // Wait until the client finish sending HTTP request
      if ( client.available() ) {     // If there is a data,
        char c = client.read();      //  read one character
        http += c;            //  then parse it
        Serial.write(c);
        if (c == '\n') {         // If the character is carriage return,
          //  it means end of http request from client
          if (clientData.length() == 0) { //  Now that the clientData is cleared,
            sendResponse();        //    perform the necessary action
            updateRelay();
            updateWebpage();
            break;
          } else {
            clientData = "";       //  First, clear the clientData
          }
        } else if (c != '\r') {      // Or if the character is NOT new line
          clientData += c;        //  store the character to the clientData variable
        }
      }
    }
    http = "";
    client.stop();            // Disconnect the client.
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
