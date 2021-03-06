/*
 Version 0.1 - March 17 2018
*/ 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>
//INCLUSÃO REF CÓDIGO DO NOSSO PROJETO
#include <Servo.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "03306f0c-0ba6-4b53-add8-70c4397ec5f9" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "Nossa Rede 2016" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "r68888031120r" // TODO: Change to your Wifi network password

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

#define LedVerde D1 
#define LedVermelho D2
#define LedBranco D7
Servo minhaPorta;


uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

 
void turnOn(String deviceId) {
  if (deviceId == "5e9f22552efe8349863b048d") // Device ID of first device
  {  
    digitalWrite(LedVerde, HIGH);
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  } 
  else if (deviceId == "5e9f22a22efe8349863b0507") // Device ID of second device
  { 
    digitalWrite(LedVermelho, HIGH);
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  }
  else if (deviceId == "5e9f22f22efe8349863b05a9") // Device ID of second device
  { 
    digitalWrite(LedBranco, HIGH);
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  }
  else if (deviceId == "5ea9bef72efe8349864ec2d7") // Device ID of second device
  { 
    minhaPorta.write(180);
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  }
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == "5e9f22552efe8349863b048d") // Device ID of first device
   {  
     digitalWrite(LedVerde, LOW);     
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
   }
   else if (deviceId == "5e9f22a22efe8349863b0507") // Device ID of second device
   { 
     digitalWrite(LedVermelho, LOW);
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
  }
  else if (deviceId == "5e9f22f22efe8349863b05a9") // Device ID of second device
   { 
     digitalWrite(LedBranco, LOW);
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
  }
  else if (deviceId == "5ea9bef72efe8349864ec2d7") // Device ID of second device
   { 
     minhaPorta.write(0);
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
  }
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId);    
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch  types
        // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":true}} // https://developers.google.com/actions/smarthome/traits/onoff
        // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":false}}

#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);      
#endif        
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "action.devices.commands.OnOff") { // Switch 
            String value = json ["value"]["on"];
            Serial.println(value); 
            
            if(value == "true") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
    default: break;
  }
}

void setup() {
  Serial.begin(115200);
  
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  //definindo moddo dos pinos
  pinMode(LedVerde, OUTPUT); 
  pinMode(LedVermelho, OUTPUT);
  pinMode(LedBranco, OUTPUT);
  minhaPorta.attach(D1);
  minhaPorta.write(0);

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/"); //"iot.sinric.com", 80

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }   
}
 
