#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <WiFiManager.h>
#include <MQTT.h>

#define NUM_LEDS 40
#define DATA_PIN 5 //D1

#define CLIENT_ID "client3"
#define TOPIC "led/rgb"

String wifiName = "LED";
byte arrData[10];

CRGB leds[NUM_LEDS];

void myDataCb(String& topic, String& data);
void myPublishedCb();
void myDisconnectedCb();
void myConnectedCb();




// create MQTT
MQTT myMqtt(CLIENT_ID, "nektar.l-tek.io", 8192);


const char* ssid     = "L-Tek_FF";
const char* password = "fireflytag";


WiFiClient espClient;

//
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFiManager wifiManager;
  //WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  Serial.println("Connecting to MQTT server");  

  // setup callbacks

  myMqtt.setUserPwd("test","test");
  
  myMqtt.onConnected(myConnectedCb);
  myMqtt.onDisconnected(myDisconnectedCb);
  myMqtt.onPublished(myPublishedCb);
  myMqtt.onData(myDataCb);
  
  Serial.println("connect mqtt...");
  myMqtt.connect();

  Serial.println("subscribe to topic...");
  myMqtt.subscribe(TOPIC);

  delay(10);

  wifiManager.autoConnect(wifiName.c_str());

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,2500);

  arrData[0] = 3; // go to standby
  
}

//
void loop() {
  switch (arrData[0]) {
      case 0: // LED OFF
        LEDsOFF();
        arrData[0] = 3; // go to standby
        break;
      case 1: // LED ON (full power white all led's)
        for(int i = 0; i < NUM_LEDS; i++){
          fill_solid( &(leds[i]), 1 /*number of leds*/, CRGB( 255, 255, 255));
        }
        FastLED.show();
        arrData[0] = 3; // go to standby
        break;
      case 2: // Night mode ON(no blue light)
        for(int i = 0; i < NUM_LEDS; i++){
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
          fill_solid( &(leds[0]), NUM_LEDS /*number of leds*/, CHSV( arrData[2], arrData[3], arrData[4]));
          FastLED.show();
        }else{
          fill_solid( &(leds[0]), NUM_LEDS /*number of leds*/, CRGB( arrData[2], arrData[3], arrData[4]));
          FastLED.show();
        }
        arrData[0] = 3; // go to standby
        break;
      case 13:  // EFFECT fade in fade out
        fill_solid( &(leds[0]), NUM_LEDS /*number of leds*/, CHSV( 160, 255, map(arrData[1],0,100,0,255)));
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


/*
 * 
 */ 
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
    /*for(int i = 0; i < NUM_LEDS; i++){
      fill_solid( &(leds[i]), 1, CRGB( 255, 255, 255));
    }
    FastLED.show(); */
  }else if(data == "0" || data == "OFF"){
    arrData[0] = 0;
    Serial.println("OFF");
    /*for(int i = 0; i < NUM_LEDS; i++){
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
    if(arrData[2] > (NUM_LEDS - arrData[1])){
      arrData[2] = NUM_LEDS - arrData[1];
    }
  }else if(arrData[0] == 11){
    if(arrData[3] > (NUM_LEDS - arrData[2])){
      arrData[3] = NUM_LEDS - arrData[2];
    }
  }    
}

void LEDsOFF(){
  fill_solid( &(leds[0]), NUM_LEDS /*number of leds*/, CRGB( 0, 0, 0));
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
    fill_solid( &(leds[0]), NUM_LEDS , CRGB(255, 255, 255));
    FastLED.show();
    delay(20);
    fill_solid( &(leds[0]), NUM_LEDS , CRGB(0, 0, 0));
    FastLED.show();
    delay(20);
  }
}



