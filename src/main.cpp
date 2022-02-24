#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <string>
#include "DHTesp.h"

#define dhtPin 4  

const char* ssid = "Tenda_877700";
const char* password = "12345678";
const char* hostname = "ESP32_Server";

WebServer server(80);

float temperature = 20.0;
float humidity = 65.0;

unsigned long previousMillis = 0;
unsigned long interval = 30000;
unsigned long getDataTimer = 0;

DHTesp dht;
bool isOn = true ;
bool tempMax = false;
bool tempMin = false;
bool humMax = false;
bool humMin = false;

// Set your Static IP address
IPAddress local_IP(192, 168, 2, 200);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional


String device = "";
String answer = "";
String status = "02";

struct Button {
  const uint8_t PIN;
  bool pressed;
};

Button button1 = {34, false};
Button button2 = {35, false};
Button button3 = {33, false};


// Convierte un float en una cadena.
// n -> número a convertir.
// l -> longitud total de la cadena, por defecto 8.
// d -> decimales, por defecto 2.
// z -> si rellenamos con ceros a la izq, por defecto true
String floatToString( float n, int l, int d, boolean z){
 char c[l+1];
 String s;

 dtostrf(n,l,d,c);
 s=String(c);

 if(z){
 s.replace(" ","0");
 }

 return s;
}


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

void checkWifi(){

  // reconectar
  unsigned long currentMillis = millis();

  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}

void IRAM_ATTR isr1() {
  button1.pressed = true;
}

void IRAM_ATTR isr2() {
  button2.pressed = true;
}

void IRAM_ATTR isr3() {
  button3.pressed = true;
}



void setAnswer(){
  answer ="Estado " + status;
}

void handleConnectionRoot(){
  server.send(200, "text/plain", "Ningún dispositivo asociado");
}

void handleDevice1(){
  device = "ESP_01";
  Serial.println(device);
  setAnswer();
  server.send(200, "text/plain", answer);

}

void handleDevice2_Status(){
  device = "Raspberry Pi Estado";
  Serial.println(device);
  answer = status;
  server.send(200, "text/plain", answer);

}

void handleDevice2_Temp(){
  device = "Raspberry Pi Temperatura";
  Serial.println(device);
  answer = (String)temperature;
  server.send(200, "text/plain", answer);
}

void handleDevice2_Hum(){
  device = "Raspberry Pi Humedad";
  Serial.println(device);
  answer = floatToString(humidity, 5, 2, true);
  server.send(200, "text/plain", answer);
}

void handleDevice2_SistOn(){
  device = "Raspberry Pi Sist On";
  Serial.println(device);
  status = "02";
  Serial.println("Sistema encendido. Estado " + status);
  isOn=true;
  setAnswer();
  server.send(200, "text/plain", answer + ". Sistema encendido.");

}
void handleDevice2_SistOff(){
  device = "Raspberry Pi Sist Off";
  Serial.println(device);
  status = "02";
  Serial.println("Sistema apagado. Estado " + status);
  isOn=false;
  setAnswer();
  server.send(200, "text/plain", answer + ". Sistema apagado.");

}

void handleDevice2_Status1(){
  device = "Raspberry Pi Estado 1";
  Serial.println(device);
  if(isOn==false){
    status="01";
    setAnswer();
    server.send(200, "text/plain", answer);
  } else{
    answer = "El estado no ha cambiado porque el sistema esta en modo automatico.";
    server.send(200, "text/plain", answer);
  }
}

void handleDevice2_Status2(){
  device = "Raspberry Pi Estado 1";
  Serial.println(device);
  if(isOn==false){
    status="02";
    setAnswer();
    server.send(200, "text/plain", answer);
  } else{
    answer = "El estado no ha cambiado porque el sistema esta en modo automatico.";
    server.send(200, "text/plain", answer);
  }

}
void handleDevice2_Status3(){
  device = "Raspberry Pi Estado 1";
  Serial.println(device);
  if(isOn==false){
    status="03";
    setAnswer();
    server.send(200, "text/plain", answer);
  } else{
    answer = "El estado no ha cambiado porque el sistema esta en modo automatico.";
    server.send(200, "text/plain", answer);
  }

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
   server.on("/device2/status", handleDevice2_Status);
   server.on("/device2/temp", handleDevice2_Temp);
    server.on("/device2/hum", handleDevice2_Hum);
    //Hacer POST del bot
    server.on("/device2/sistOn", handleDevice2_SistOn);
    server.on("/device2/sistOff", handleDevice2_SistOff);
    server.on("/device2/status1", handleDevice2_Status1);
    server.on("/device2/status2", handleDevice2_Status2);
    server.on("/device2/status3", handleDevice2_Status3);
   
 
   // Ruteo para URI desconocida
   server.onNotFound(handleNotFound);
 
   // Iniciar servidor
   server.begin();
   Serial.println("HTTP server started");
   Serial.println("Dispositivos conectados: ");
}


void checkHum(){
  if(humidity > 75.0){
    humMax=true;
    humMin=false;
    status = "03";
  } else if (humidity < 55.0){
    humMax=false;
    humMin=true;
    status = "01";
  } else {
    status = "02";
  }
}

void checkTemp(){

  if (isOn==true){
    if(temperature > 25.0){
    tempMax=true;
    tempMin=false;
    status = "03";
    } else if (temperature < 15.0){
    tempMax=false;
    tempMin=true;
    status = "01";
   } else {
    status = "02";
    checkHum();
    }
  }

}


void checkSensorDHT(){

   if (millis() - getDataTimer >= 10000){ //10 segundos
    temperature = dht.getTemperature();
    humidity = dht.getHumidity();
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.println("Estado " + status);
    Serial.println("");

    getDataTimer = millis();
  }
  checkTemp();
}


void initGPIO(){
  pinMode(button1.PIN, INPUT_PULLDOWN);
  pinMode(button2.PIN, INPUT_PULLDOWN);
  pinMode(button3.PIN, INPUT_PULLDOWN);
  attachInterrupt(button1.PIN, isr1, FALLING);
  attachInterrupt(button2.PIN, isr2, FALLING);
  attachInterrupt(button3.PIN, isr3, FALLING);
}

void checkButton(){

  if (button1.pressed) {
    if(isOn==false){
      if (status == "01"){
        status = "02";
      }else{
        status = "03";
      }
    }

    Serial.println("Btn arriba. Estado " + status);
    delay(750);
    button1.pressed = false;
  }

  if (button2.pressed) {
    if(isOn==false){
      if (status == "03"){
        status = "02";
      }else{
        status = "01";
      }
    }

    Serial.println("Btn abajo. Estado " + status);

    delay(750);
    button2.pressed = false;
  }

  if (button3.pressed) {
      
      if (isOn == false){
        status = "02";
        Serial.println("Sistema encendido. Estado " + status);
        isOn=true;
      } else if (isOn == true) {
        status = "02";
        Serial.println("Sistema apagado. Estado " + status);
        isOn=false;
      }
      delay(750);
      button3.pressed = false;
  }
}

void setup() {
  Serial.begin(9600);
  delay(50);

  dht.setup(dhtPin, DHTesp::DHT22); 
  initGPIO();
  initWiFi();
  initServer();
}


void loop() {

  checkWifi();
  checkButton();
  checkSensorDHT();
  server.handleClient();
}