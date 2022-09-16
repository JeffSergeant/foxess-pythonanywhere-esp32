
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <Wire.h>
#include "heltec.h"
#include <HTTPClient.h>

/**********************************************  WIFI Client 注意编译时要设置此值 *********************************
 * wifi client
 */
const char* ssid = "xxxxxx"; //replace "xxxxxx" with your WIFI's ssid
const char* password = "xxxxxx"; //replace "xxxxxx" with your WIFI's password

#define USE_STATIC_IP false

/*******************************************************************
 * OLED Arguments
 */

#define OLED_UPDATE_INTERVAL 500


/*********************************************************************
 * setup wifi
 */
void setupWIFI()
{
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Connecting...");
  Heltec.display->drawString(0, 10, String(ssid));
  Heltec.display->display();

  WiFi.disconnect(true);
  delay(1000);

  WiFi.mode(WIFI_STA);

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);   


  WiFi.begin(ssid, password);

  byte count = 0;

  while(WiFi.status() != WL_CONNECTED && count < 10)
  {
    count ++;
    delay(500);
    Serial.print(".");
  }

  Heltec.display->clear();
  if(WiFi.status() == WL_CONNECTED)
    Heltec.display->drawString(0, 0, "Connected");
  else
    Heltec.display->drawString(0, 0, "Connection Failed");
  Heltec.display->display();
}

/******************************************************
 * arduino setup
 */
void setup()
{
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  pinMode(25, OUTPUT);
  digitalWrite(25,HIGH);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Initialize...");

  setupWIFI();

  Heltec.display->setFont(ArialMT_Plain_24);


}

/******************************************************
 * arduino loop
 */

int battery = 0;
int net = 0;
void loop()
{
  unsigned long ms = millis();
  Heltec.display->clear();
  if(ms % 30000 == 0)
  {
    if(WiFi.status() == WL_CONNECTED){
  
      battery = doHTTPSoC();
     
      net = doHTTPnet();
      
    }
  }
  drawBattery(battery);
  if(battery<25){
    if(ms % 10000 <= 250){
         Heltec.display->setColor(BLACK);
         drawBattery(100); 
         Heltec.display->setColor(WHITE);
    }
  }
  drawNet(net);
  Heltec.display->display();
}


int doHTTPSoC()
{
      HTTPClient http;

      Serial.print("[HTTP] begin...\n");
     
      http.begin("http://MY_PYTHONANYWHERE_URL.pythonanywhere.com/soc"); //HTTP
      http.setTimeout(10000);
      int httpCode = http.GET();
      if(httpCode > 0) {
        
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);

          if(httpCode == HTTP_CODE_OK) {
              String payload = http.getString();
             
              Serial.println(payload);       
             
              //Heltec.display->drawString(0, 0, "Battery   %");
              //Heltec.display->drawString(75, 0, String(payload));
              http.end();
              return payload.toInt();
              
          }
      } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          Heltec.display->drawString(0, 0, "HTTP FAILED");
          http.end();
          
      }
      return 0;

}

int doHTTPnet()
{
      HTTPClient http;

      Serial.print("[HTTP] begin...\n");
     
      http.begin("http://MY_PYTHONANYWHERE_URL.pythonanywhere.com/net"); //HTTP
      http.setTimeout(10000);
      int httpCode = http.GET();
      if(httpCode > 0) {
    
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);

          if(httpCode == HTTP_CODE_OK) {
              String payload = http.getString();
             
              Serial.println(payload);       
              http.end();
              return(payload.toInt());
              
          }
      } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          Heltec.display->drawString(0, 0, "HTTP FAILED");
      }
     
      http.end();
      return 0;
}

void drawBattery(int charge){

  
  Heltec.display->drawLine(12,32, 114, 32);
  Heltec.display->drawLine(12,58, 114, 58);
  Heltec.display->drawLine(12,32, 12,58);
  Heltec.display->drawLine(114,32, 114, 58);
  Heltec.display->drawLine(115,35, 115, 55);
  Heltec.display->drawLine(116,36, 116, 54);
  Heltec.display->drawLine(117,37, 117, 53);

  for (int i = 0; i < charge;i++) {   
    Heltec.display->drawLine(14+i,34, 14+i,56);
  }
 }

void drawNet(int charge){

  //axis
  Heltec.display->drawLine(15,24,113,24);

  //center line
  Heltec.display->drawLine(64,1,64,28);

 //minor marks
  for (int i = 0; i <= 100;i+=25) {
    Heltec.display->drawLine(14+i,22, 14+i,24);
  }
  

  for (int i = 50; i < charge;i++) {
    Heltec.display->drawLine(14+i,5, 14+i,20);
  }
  for (int i = 50; i > charge;i--) {
    Heltec.display->drawLine(14+i,5, 14+i,20);
  }


  
}
