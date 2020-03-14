/*
 * This project is part of the project on "Remote Agricultural Parameters Analysis and Control"
 * This code is creates a local server allowing the user to control the agricultural parameters
 * This is to be integrated with the agricultural parameters analysis to automate the entire process
 */


//Include libraries

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>

//Enter WiFi credentials
const char* ssid = "WIFI";
const char* password = "password";

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

String page = "";

//Pin 13 is connected to a peristaltic pump to control water flow through a relay
#define irrigate 13
//Pin 12 is connected to a vapourizer to control humidity through a relay
#define humidity 12

void setup(void){
 //the HTML of the web page
 page = "<style>body{background-color:#5585d1;color:#ffffff;}h1{color:#ffffff;text-align:center;font-size:45px;}button{background-color:#264dcc;border-color:#ffffff;margin-left:15%;border-radius: 10px;color:#ffffff; width:20%;margin-right:10%;}button:hover{background-color:#f74f25;}li{font-size:20px;}</style><h1>Remote agricultural parameters control</h1><p><ul><li>The project Remote agricultural parameters control is a continuation of our previous project, where we developed a Wireless Sensor Network [WSN] using soil moisture, humidity and temperature sensors, NodeMCU and RaspberryPi.</li><li>One of the improvements of the previous project that we had mentioned was incorporating control of the parameters of the agricultural area.</li><li>Controlling the irrigation of the land and controlling the humidity content of a greenhouse set-up for instance. Thus, this project incorporates control of water flow for irrigation and humidity control. </li><li>The working of the project is as follows: <br><h3><ul><li>Pressing the Irrigate button will turn on the water flow for 10 seconds</li><li>Pressing the Humidity button will turn on the vaporizer for 20 seconds</li></ul></h3></li></ul></p><p><a href=\"Irrigate\"><button><h3>Irrigate</h3></button></a>&nbsp;<a href=\"Humidity\"><button><h3>Humidity</h3></button></a></p>";
 //The relays connected to vapourizer and pump are initially off
 pinMode(humidity, OUTPUT);
 digitalWrite(humidity, HIGH);
 pinMode(irrigate,OUTPUT);
 digitalWrite(irrigate,HIGH);

 delay(1000);

 //Connect to the web-server
 Serial.begin(115200);
 WiFi.begin(ssid, password); //begin WiFi connection
 Serial.println("");

 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
 }

 Serial.println("");
 Serial.print("Connected to ");
 Serial.println(ssid);
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());

 server.on("/", [](){
    server.send(200, "text/html", page);
 });

 //If user clicks on irrigate button, turn on the pump for 10 seconds
 server.on("/Irrigate", [](){
    server.send(200, "text/html", page);
    digitalWrite(irrigate,LOW);
    delay(10000);
    digitalWrite(irrigate,HIGH);
 });

//If user clicks on humidity button, turn on vapourizer for 20 seconds
 server.on("/Humidity", [](){
    server.send(200, "text/html", page);
    digitalWrite(humidity,LOW);
    delay(20000);
    digitalWrite(humidity,HIGH);
 });

 server.begin();
 webSocket.begin();
 webSocket.onEvent(webSocketEvent);

 Serial.println("Web server started!");
}

void loop(void){
  //Run web socket on loop
  
  webSocket.loop();
  server.handleClient();
  if (Serial.available() > 0){
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if (type == WStype_TEXT){
   for(int i = 0; i < length; i++) Serial.print((char) payload[i]);
   Serial.println();
  }
}
