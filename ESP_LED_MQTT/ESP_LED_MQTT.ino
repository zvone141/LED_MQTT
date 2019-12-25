#define CUSTOM_CONFIG  //if you want to use default config comment this line

#ifdef CUSTOM_CONFIG
#include "custom_config.h"
#else
#include "config.h"
#endif

#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <MQTT.h>
#include "ESP_EEPROM.h"


byte arrData[10];
byte espMAC[6];

uint16_t curLimit = 13000;
uint16_t ledLen = 50;

char clientID[17];
char response_topic[22];
char status_topic[24];

CRGB leds[NUM_LEDS];

void myDataCb(String& topic, String& data);
void myPublishedCb();
void myDisconnectedCb();
void myConnectedCb();


// init MQTT
MQTT myMqtt(MQTT_CLIENT_ID, MQTT_SERVER, MQTT_PORT);

//WiFiClient espClient;

//custom parameters for WiFiManager
bool shouldSaveConfig = false;
char ledLenStr[5];
WiFiManagerParameter custom_led_len("ledLen", "Nr of leds to control", ledLenStr, 5);

//
void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.macAddress(espMAC);
  sprintf(clientID, "%s%02x%02x%02x%02x%02x%02x\0", MQTT_CLIENT_ID, espMAC[0],espMAC[1],espMAC[2],espMAC[3],espMAC[4],espMAC[5]);
  sprintf(response_topic, "%s%02x%02x%02x%02x%02x%02x\0", RESPONSE_TOPIC, espMAC[0],espMAC[1],espMAC[2],espMAC[3],espMAC[4],espMAC[5]);
  sprintf(status_topic, "%s%02x%02x%02x%02x%02x%02x\0", STATUS_TOPIC, espMAC[0],espMAC[1],espMAC[2],espMAC[3],espMAC[4],espMAC[5]);

  myMqtt.setClientId(clientID);
  Serial.print("clientID: ");
  Serial.println(clientID);
  Serial.println(response_topic);
  Serial.println(status_topic);


  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_led_len);
  wifiManager.setTimeout(600); // ESP closes AP after 10mins and continues operation
  wifiManager.setConnectTimeout(20); // this parameter is useful for when device can not get IP from router, if for instance all IPs are taken
  while(!wifiManager.autoConnect(clientID, AP_PASS)) {
    //Serial.println("Failed to connect and hit timeout!");
    delay(3000);
  }

  if(shouldSaveConfig){
    //strcpy(ledLenStr, custom_led_len.getValue());
    saveToEEPROM(custom_led_len.getValue(), ledLenStr, nrLedsLen, nrLedsAddr);
    commitChangesToEEPROM();
    shouldSaveConfig = false;
  }
  /*WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  */

  Serial.println("Connecting to MQTT server");  

  // setup callbacks

  myMqtt.setUserPwd(MQTT_USER,MQTT_PASS);
  
  myMqtt.onConnected(myConnectedCb);
  myMqtt.onDisconnected(myDisconnectedCb);
  myMqtt.onPublished(myPublishedCb);
  myMqtt.onData(myDataCb);
  
  Serial.println("connecting mqtt server...");
  myMqtt.connect();

  Serial.println("subscribe to topic...");
  myMqtt.subscribe(CMD_TOPIC);

  //myMqtt.publish(status_topic, "led strip connected");

  delay(10);
  memcpy(ledLenStr, readFromEEPROM(nrLedsLen,nrLedsAddr),nrLedsLen);
  ledLen = atoi(ledLenStr);
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, ledLen);
  
  // calculate current limit
  curLimit = ledLen * SINGLE_LED_CURRENT;
  Serial.print("MAX current:");
  Serial.println(curLimit);
  if(curLimit > MAX_CUR){
    curLimit = MAX_CUR;
  }
  Serial.print("MAX current:");
  Serial.println(curLimit);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,curLimit);

  arrData[0] = 3; // go to standby
  
}

//
void loop() {
  if(digitalRead(BUTTON_PIN) == LOW){
    WiFi.disconnect(true);
    WiFiManager wifiManager;
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.addParameter(&custom_led_len);
    wifiManager.setTimeout(600); // ESP closes AP after 10mins and continues operation
    wifiManager.setConnectTimeout(20); // this parameter is useful for when device can not get IP from router, if for instance all IPs are taken
    wifiManager.startConfigPortal(clientID, AP_PASS);

    if(shouldSaveConfig){
      //strcpy(ledLenStr, custom_led_len.getValue());
      saveToEEPROM(custom_led_len.getValue(), ledLenStr, nrLedsLen, nrLedsAddr);
      commitChangesToEEPROM();
      memcpy(ledLenStr, readFromEEPROM(nrLedsLen,nrLedsAddr),nrLedsLen);
      ledLen = atoi(ledLenStr);
      ESP.restart();
      //shouldSaveConfig = false;
    }
  }
  handle_LEDs();
}


/*
 * 
 */ 
void handle_LEDs(){
  switch (arrData[0]) {
      case 0: // LED OFF
        LEDsOFF();
        arrData[0] = 3; // go to standby
        break;
      case 1: // LED ON (full power white all led's)
        for(int i = 0; i < ledLen; i++){
          fill_solid( &(leds[i]), 1 /*number of leds*/, CRGB( 255, 255, 255));
        }
        FastLED.show();
        arrData[0] = 3; // go to standby
        break;
      case 2: // Night mode ON(no blue light)
        for(int i = 0; i < ledLen; i++){
          fill_solid( &(leds[i]), 1 /*number of leds*/, CRGB( 255, 255, 0));
        }
        FastLED.show();
        arrData[0] = 3; // go to standby
        break;
      case 3: // standby
        delay(100);
        break;
      case 4: // STROBE
        strobe();
        break;
      case 9: // SHOW LED's
        FastLED.show();
        arrData[0] = 3; // go to standby
        break;
      case 10: // set one LED
        if(arrData[1] == 0){
          leds[arrData[2]].setHSV( arrData[3], arrData[4], arrData[5]);
        }else{
          leds[arrData[2]].setRGB( arrData[3], arrData[4], arrData[5]);
        }
        FastLED.show();
        arrData[0] = 3; // go to standby
        break;
      case 11: //set multiple LED's
        if(arrData[1] == 0){
          fill_solid( &(leds[arrData[2]]), arrData[3] /*number of leds*/, CHSV( arrData[4], arrData[5], arrData[6]));
        }else{
          fill_solid( &(leds[arrData[2]]), arrData[3] /*number of leds*/, CRGB( arrData[4], arrData[5], arrData[6]));
        }
        FastLED.show();
        arrData[0] = 3; // go to standby
        break;
      case 12: //set all LED's to whatever color
        if(arrData[1] == 0){
          fill_solid( &(leds[0]), ledLen /*number of leds*/, CHSV( arrData[2], arrData[3], arrData[4]));
          FastLED.show();
        }else{
          fill_solid( &(leds[0]), ledLen /*number of leds*/, CRGB( arrData[2], arrData[3], arrData[4]));
          FastLED.show();
        }
        arrData[0] = 3; // go to standby
        break;
      case 13:  // EFFECT fade in fade out
        fill_solid( &(leds[0]), ledLen /*number of leds*/, CHSV( 160, 255, map(arrData[1],0,100,0,255)));
        FastLED.show();
        arrData[0] = 3; // go to standby
        break;
      case 50:  // EFFECT fade in fade out
        effect50();
        break;
      default:
        // statements
        break;
  }
}
//callback of WiFiManager AP when custom parameters have changed
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void myConnectedCb()
{
  Serial.println("connected to MQTT server");
}

void myDisconnectedCb()
{
  Serial.println("disconnected. try to reconnect...");
  delay(500);
  myMqtt.connect();
}

void myPublishedCb()
{
  //Serial.println("published.");
}

void myDataCb(String& topic, String& data)
{

  if(data == "1" || data == "ON"){
    arrData[0] = 1;
    Serial.println("ON");
    /*for(int i = 0; i < ledLen; i++){
      fill_solid( &(leds[i]), 1, CRGB( 255, 255, 255));
    }
    FastLED.show(); */
  }else if(data == "0" || data == "OFF"){
    arrData[0] = 0;
    Serial.println("OFF");
    /*for(int i = 0; i < ledLen; i++){
      fill_solid( &(leds[i]), 1, CRGB( 0, 0, 0));
    }
    FastLED.show();   */ 
    
  }else if(data == "9" || data == "SHOW"){
    arrData[0] = 9;
    Serial.println("SHOW");
    //FastLED.show();
    
  }else if(data == "2" || data == "NIGHTON"){
    arrData[0] = 2;
    Serial.println("NIGHTON");
    //Night mode ON;
  }else if(data == "4" || data == "STROBE"){
    arrData[0] = 4;
    Serial.println("STROBE");
    //STROBE;
  }else{
    parser(data);  
  }

}

void parser(String data){
  int index = 0;
  String alreadyParsed ;
  bool firstTime = 0;
  
  for(int i=0; i<10; i++){
    if(firstTime == 0){
      arrData[index] = (data.substring(alreadyParsed.length(), data.indexOf(',',alreadyParsed.length()+1))).toInt();
      firstTime = 1;
    }else{
      arrData[index] = (data.substring(alreadyParsed.length()+1, data.indexOf(',',alreadyParsed.length()+1))).toInt();
    }
    
    alreadyParsed = alreadyParsed + data.substring(alreadyParsed.length(), data.indexOf(',',alreadyParsed.length()+1));
    index++;
  }


  if(arrData[0] == 50){
    LEDsOFF();
    if(arrData[2] > (ledLen - arrData[1])){
      arrData[2] = ledLen - arrData[1];
    }
  }else if(arrData[0] == 11){
    if(arrData[3] > (ledLen - arrData[2])){
      arrData[3] = ledLen - arrData[2];
    }
  }    
}

void LEDsOFF(){
  fill_solid( &(leds[0]), ledLen /*number of leds*/, CRGB( 0, 0, 0));
  FastLED.show();
}

void effect50(){

  LEDsOFF();

  
  while(arrData[0] == 50){
    for(byte v=0; v<arrData[6]-5; v+=5){
      fill_solid( &(leds[arrData[1]]), arrData[2] , CHSV(arrData[4], arrData[5],  v));
      FastLED.show();
      delay(arrData[3]); 
    }

    Serial.println(arrData[2]);

    for(byte v=arrData[6]; v> 6; v-=5){
      fill_solid( &(leds[arrData[1]]), arrData[2] , CHSV(arrData[4], arrData[5], v));
      FastLED.show();
      delay(arrData[3]);
    }
  }

  LEDsOFF();
}

void strobe(){
  while(arrData[0] == 4){
    fill_solid( &(leds[0]), ledLen , CRGB(255, 255, 255));
    FastLED.show();
    delay(20);
    fill_solid( &(leds[0]), ledLen , CRGB(0, 0, 0));
    FastLED.show();
    delay(20);
  }
}



