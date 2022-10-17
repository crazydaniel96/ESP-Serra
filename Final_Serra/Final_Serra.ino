#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"
#include "NTPClient.h"
#include "WiFiUdp.h"
#include "ArduinoJson.h"
#include "ESP8266HTTPClient.h"
#include "DHT.h"
#include "EEPROM.h"
#include "ArduinoOTA.h"
#include "UniversalTelegramBot.h"
#include "WiFiClientSecure.h"
#include "webPage.h"

// Set number of relays
#define NUM_MEASURES 12
#define DHTTYPE DHT11
#define DHTPIN 2

int Del=5;  //del is delay set by user in secs
int Max_time=96;
int manual=-1;
byte i=0;

// modify them all if serra change
#define NUM_PUMP  1
bool TH_Sensor=0; //change it if humidity sensor is connected or not  
int relayGPIOs[NUM_PUMP] = {5}; //{5,4,0}
int timer[NUM_PUMP] = {10};   //as hours
int PumpState[NUM_PUMP] = {0}; 
String UpdateState[NUM_PUMP]={""};
bool water_inib[NUM_PUMP] = {false};
//network credentials
//const char* ssid = ""; 
//const char* password = "";

//weather info
const char* OpenWeatherLink = "";

// Telegram BOT 
#define BOT_TOKEN ""
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

//irrigation history
long history[20][2];

//weather variables
int currentHour = 0, hours = 0;
byte intervalWeather = 0;
float wind;
bool no_water = false;
String weather = "";

WiFiClient wifiClient;

//variables for temporal graph
//OWM
float T_last24H_OWM[NUM_MEASURES]= {0};    
int H_last24H_OWM[NUM_MEASURES]= {0};
String T_24H_OWM="";                      //string measurements used for highcharts
String H_24H_OWM="";
//REAL
float T_last24H_REAL[NUM_MEASURES]= {0};   
int H_last24H_REAL[NUM_MEASURES]= {0};
String T_24H_REAL="";                     //string measurements used for highcharts
String H_24H_REAL="";
//TIME ARRAY
long XaxisPlotTimes[NUM_MEASURES]={0};   //time

DHT dht(DHTPIN, DHTTYPE);
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200);  //utcOffsetInSeconds = 3600 when solar time zone (italy)

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void setup(){
  
  ArduinoOTA.begin();
  // setup initial pump state
  for(i=0; i<NUM_PUMP; i++){
    pinMode(relayGPIOs[i], OUTPUT);
    digitalWrite(relayGPIOs[i], LOW);
  }
  for (i=0;i<20;i++)   //init matrix
    for(byte j=0;j<3;j++)
      history[i][j]=0;
  
  EEPROM.begin(512);
  dht.begin();

  //retrieve values from EEPROM
  if (Del!=EEPROM.read(100)){
    Del=EEPROM.read(100);
  }
  if (Max_time!=EEPROM.read(99)){
    Max_time=EEPROM.read(99);
  }
  for(i=0; i<NUM_PUMP; i++){
    if (EEPROM.read(i*3+1)!=timer[i]&&EEPROM.read(i*3+1)!=0)
      timer[i] = EEPROM.read(i*3+1);   //expressed as hours; read from EEPROM memory the saved state.
    if (EEPROM.read(i*3)!=PumpState[i])
      PumpState[i] = EEPROM.read(i*3); //read from EEPROM memory the saved state. for each pump are saved timer and on/off values, so each pump takes up 2 slot
    if (EEPROM.read(i*3+2)!=water_inib[i])
      water_inib[i] = EEPROM.read(i*3+2);  //read if inhibit during raining
  }
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  // Print ESP Local IP Address

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  
   
  // Send a GET request to <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam("relay") & request->hasParam("state")) {
      inputMessage = request->getParam("relay")->value(); //pump
      inputMessage2 = request->getParam("state")->value(); //state
      PumpState[inputMessage.toInt()-1]=inputMessage2.toInt(); //update pump state
      EEPROM.write((inputMessage.toInt()-1)*3, inputMessage2.toInt()); //update memory with state (addr,value)
      EEPROM.commit();
    }
    else if (request->hasParam("relay") & request->hasParam("timer")) {
      inputMessage = request->getParam("relay")->value(); //pump 
      inputMessage2 = request->getParam("timer")->value(); //timer
      timer[inputMessage.toInt()-1] = inputMessage2.toInt();
      EEPROM.write((inputMessage.toInt()-1)*3+1, inputMessage2.toInt()); //update memory with state (addr,value)
      EEPROM.commit();
    }
    else if (request->hasParam("relay") & request->hasParam("manual")) {
      inputMessage = request->getParam("relay")->value(); //pump 
      manual=inputMessage.toInt();
    }
    else if (request->hasParam("relay") & request->hasParam("inhibit")) {
      inputMessage = request->getParam("relay")->value(); //pump 
      inputMessage2 = request->getParam("inhibit")->value(); //inhibition when raining
      water_inib[inputMessage.toInt()-1] = inputMessage2.toInt();
      EEPROM.write((inputMessage.toInt()-1)*3+2, inputMessage2.toInt()); //update memory with state (addr,value)
      EEPROM.commit();
    }
    else if (request->hasParam("delay")) {
      inputMessage = request->getParam("delay")->value(); //pump delay in seconds
      Del=inputMessage.toInt();
      EEPROM.write(100, inputMessage.toInt()); //update memory with state (addr,value); slot 0,..,98 are reserved for pumps' values
      EEPROM.commit();
    }
    else if (request->hasParam("Max_time")) {
      inputMessage = request->getParam("Max_time")->value(); //Max_time in seconds
      Max_time=inputMessage.toInt();
      EEPROM.write(99, inputMessage.toInt()); //update memory with state (addr,value); slot 0,..,98 are reserved for pumps' values
      EEPROM.commit();
    }
    
    request->send(200, "text/plain", "OK");
  });
 
  // Start server
  server.begin();

  //keep first time weather info
  timeClient.update();
  WeatherRequest();

  configTime(0, 0, "pool.ntp.org");
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  // send bot message
  bot.sendMessage("218443255", "Setup effettuato con successo", "");

}
  
void loop() {
  ArduinoOTA.handle();
  timeClient.update();
  
  hours=timeClient.getEpochTime()/3600;      
  
  if (currentHour!=hours){
    currentHour=hours;
    intervalWeather+=1;
    //update weather info every 3 hours (OWM update weather every 3 hours, so sensor reading is synched)
    if (intervalWeather==3){
      WeatherRequest();
      intervalWeather=0;
    }
    //update pump
    for(i=0; i<NUM_PUMP; i++){
      if (hours%timer[i]==0 && PumpState[i]==1){    //pump on only if weather is good (not raining)
        if ( !no_water | !water_inib[i]){
          StartIrrigation(i);
          bot.sendMessage("218443255", "Azionata pompa "+String(i+1), "");
        }
        else
          bot.sendMessage("218443255", "Pompa "+String(i+1)+" non azionata causa pioggia", "");
      }
    }
  }
  if (manual>=0){
    StartIrrigation(manual);
    manual=-1;
  }
  delay(5000);  //if lower than 5000 could heat up
}
