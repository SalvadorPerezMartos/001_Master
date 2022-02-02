#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#define LED     4
#define BUTTON  33


const char* ssid = "MOVISTAR_7E6C";              //MOVISTAR_7E6C   WLAN_64
const char* password = "868BE85F3D7F919415E2";     //868BE85F3D7F919415E2  X000138FB9A64
const char* hostname = "ESP32_2";

WebServer server(80);


unsigned long previousMillis = 0;
unsigned long interval = 30000;

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 200);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional


String device = "";
String answer = "";



void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  Serial.print("Conectando a la red IP: ");
  Serial.println(WiFi.localIP());

}




void setAnswer(){
  /*answer = "<DOCTYPE html>\
            <html>\
            <body>\
            <h1>Hola \"" + device + "\"! </h1>\
            </body>\
            </html>";*/
            answer ="estado del dispositivo"+;
}

void handleConnectionRoot(){
  server.send(200, "text/plain", "Ningún dispositivo asociado");
  // server.send(200, "text/plain", "Ningún dispositivo asociado");
}

void handleDevice1(){
  device = "ESP_01";
  Serial.print(device);
  setAnswer();
  server.send(200, "text/plain", answer);

}

void handleDevice2(){
  device = "ESP_02";
  Serial.print(device);
  setAnswer();
  server.send(200, "text/plain", answer);

}

void handleDevice3(){
  device = "ESP_03";
  Serial.print(device);
  setAnswer();
  server.send(200, "text/plain", answer);

}

void handleNotFound(){
  server.send(404, "text/plain", "Not found.");
}

void initServer()
{
   // Ruteo para '/'
   server.on("/", handleConnectionRoot);
 
   // Ruteo para '/deviceX' usando función 
                                            //server.on("device1", HTTP_GET, sendStatus);
   server.on("/device1", handleDevice1);
   server.on("/device2", handleDevice2);
   server.on("/device3", handleDevice3);
   
 
   // Ruteo para URI desconocida
   server.onNotFound(handleNotFound);
 
   // Iniciar servidor
   server.begin();
   Serial.println("HTTP server started");
   Serial.println("Dispositivos conectados: ");
}

void setup() {
  Serial.begin(9600);
  delay(50);

  pinMode(LED, OUTPUT);

  initWiFi();
  initServer();
   digitalWrite(LED,HIGH);
}


void loop() {
  
//unsigned long currentMillis = millis();
// if WiFi is down, try reconnecting

/*
if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
  Serial.print(millis());
  Serial.println("Reconnecting to WiFi...");
  WiFi.disconnect();
  WiFi.reconnect();
  previousMillis = currentMillis;
}
*/

server.handleClient();


}