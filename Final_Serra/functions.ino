/*
 * see here for weather codes
 * https://openweathermap.org/weather-conditions
 */

void WeatherRequest(){
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    //shifting array elements 
    for(i=0;i<NUM_MEASURES-1;i++){
      T_last24H_OWM[i]=T_last24H_OWM[i+1];
      H_last24H_OWM[i]=H_last24H_OWM[i+1];
      XaxisPlotTimes[i]=XaxisPlotTimes[i+1];

      if (TH_Sensor){
        T_last24H_REAL[i]=T_last24H_REAL[i+1];
        H_last24H_REAL[i]=H_last24H_REAL[i+1];
      }
    }
    
    HTTPClient http;  //Declare an object of class HTTPClient
    DynamicJsonDocument jsonBuffer(1024);
 
    // specify request destination
    http.begin(wifiClient,OpenWeatherLink);
 
    int httpCode = -1;
    byte tmp=0;
    while (tmp<20 && httpCode<0) { // check the returning code
      tmp+=1;
      httpCode = http.GET();  // send the request, httpCode will be negative on error
      if (httpCode<0)
        continue;
      String payload = http.getString();   //Get the request response payload
      // Parse JSON object
      DeserializationError error = deserializeJson(jsonBuffer, payload);
      if (error) {
        bot.sendMessage("218443255", "Errore nella deserializzazione del payload", "");
        return;
      }
    }

    //OPENWEATHERMAP DATA
    T_last24H_OWM[NUM_MEASURES-1] = (tmp==20) ? 0 : (float)(jsonBuffer["main"]["temp"]) - 273.15;        // get temperature
    H_last24H_OWM[NUM_MEASURES-1] = (tmp==20) ? 0 : jsonBuffer["main"]["humidity"];                  // get humidity
    if (tmp!=20){
      wind = jsonBuffer["wind"]["speed"];                   // get wind speed
      weather = String(jsonBuffer["weather"][0]["main"]);      //get weather info
      if ((int)jsonBuffer["weather"][0]["id"]<700) 
        no_water = true;
      else
        no_water = false;
    }
    
    // SENSOR DHT11 DATA 
    if (TH_Sensor){
      float temp_tmp = dht.readTemperature();
      float hum_tmp = dht.readHumidity();
      T_last24H_REAL[NUM_MEASURES-1] = (isnan(temp_tmp)) ? 0 : temp_tmp; 
      H_last24H_REAL[NUM_MEASURES-1] = (isnan(hum_tmp)) ? 0 : hum_tmp;
    }

    //update datetime
    XaxisPlotTimes[NUM_MEASURES-1]=timeClient.getEpochTime();
 
    if (tmp==20){
      bot.sendMessage("218443255","Errore http.get", "");
      bot.sendMessage("218443255",http.errorToString(httpCode).c_str(), "");
    }
    http.end();   //Close connection
    
    DataPlotOrganizer(); //use new data to update T and H matrix 
  }
}

void StartIrrigation(int pump){
  digitalWrite(relayGPIOs[pump], HIGH);
  delay(Del*1000);  // sec*1000=ms
  digitalWrite(relayGPIOs[pump], LOW);
  //update history
  
  UpdateState[pump]="Ultima irrigazione " + getFullFormattedTime(timeClient.getEpochTime());
  for(int k=18;k>=0;k--){    //shift all the values 
    for(byte j=0;j<2;j++)
      history[k+1][j]=history[k][j];
  }
  history[0][0]=pump+1;
  history[0][1]=timeClient.getEpochTime();
}
