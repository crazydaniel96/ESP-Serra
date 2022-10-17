String relayState(int numRelay){
  if(PumpState[numRelay-1]==1)
    return "checked";
  else
    return "";
}

String inhibit_state(int numRelay){
  if(water_inib[numRelay-1]==true)
    return "checked";
  else
    return "";
}

// Replaces placeholder
String processor(const String& var){
  if(var == "WEATHERICON"){
    String icon;
    if (weather=="Thunderstorm")
      icon="<i class='fas fa-cloud-showers-heavy'></i>";
    else if (weather=="Drizzle")
      icon="<i class='fas fa-cloud-sun-rain'></i>";
    else if (weather=="Rain")
      icon="<i class='fas fa-cloud-showers-heavy'></i>";
    else if (weather=="Snow")
      icon="<i class='far fa-snowflake'></i>";
    else if (weather=="Atmosphere")
      icon="<i class='fas fa-smog'></i>";
    else if (weather=="Clear")
      icon="<i class='fas fa-sun'></i>";
    else if (weather=="Clouds")
    icon="<i class='fas fa-cloud-sun'></i>";
    return icon;
  }
  
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="<div class='row'>";
    for(int i=1; i<=NUM_PUMP; i++){

      //place slider timer
      buttons+="<div class='col-lg-6'>"
      "<div class='card'>"
      "<h3 class='card-header'>Pompa #" + String(i) + " - GPIO " + relayGPIOs[i-1] + "</h3>"
      "<div class='card-body'>"
      "<h3><output  id=\"output"+String(i)+"\">"+timer[i-1]+"</output> Ore</h3><p><input type=\"range\" onchange=\"updateSliderTimer(this)\" id=\"timerSlider" + String(i) +"\" min=\"1\" max='" +String(Max_time)+ "' value=\""+timer[i-1]+"\" step=\"1\" oninput=\"output"+String(i)+".value = timerSlider" + String(i)+".value  \" class=\"slider2\"></p>";
      //place button to enable pump
      buttons+= "<label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(i) + "\" "+ relayState(i) +"><span class=\"slider\"></span></label>";
      //checkbox to prevent pump on during raining
      buttons+= "<h5><input type='checkbox' onchange='toggleCheckboxInhibit(this)' id=inhibit"+ String(i) +" "+ inhibit_state(i) +"><label for=inhibit"+ String(i) +">No acqua se piove</label></h5>";
      //add last time irrigation
      buttons+= "<hr><h5>" + UpdateState[i-1] + "</h5>"
      "</div></div></div>";
    }
    buttons+="</div>";
    return buttons;
  }
  if(var == "OTHERINFOPLACEHOLDER"){
    String infos ="";
      
      infos+="<div class='row'><div class='col-lg-6'><br>"
      "<h4>Temperatura OWM: "+ String(T_last24H_OWM[NUM_MEASURES-1]) +
      " °C<br>Temperatura reale: "+ String(T_last24H_REAL[NUM_MEASURES-1]) +
      " °C<hr>Umidità OWM: "+String(H_last24H_OWM[NUM_MEASURES-1])+
      " &#37<br>Umidità reale: "+String(H_last24H_REAL[NUM_MEASURES-1])+
      " &#37<hr>Velocità vento: " + String(wind)+
      " m/s ("+String(wind*3.6)+" km/h)<hr>Meteo: " + weather + "</h4></div>"
      "<div class='col-lg-6'>"
      "<table class='table'><thead class='table-dark'><tr><th>Pompa</th><th>data</th></thead></tr><tbody>";
      
      for(byte i=0;i<20;i++){
        if (history[i][1]!=0)
          infos+="<tr><td>Pompa "+ String(history[i][0]) +"</td><td> " + getFullFormattedTime((time_t)history[i][1]) +" </td></tr>";
      }

      infos+="</tbody></table></div></div>";
    return infos;
  }
  if(var == "GRAPHPLACEHOLDER"){
    String graph=""; 
    graph+= "<script>"
"      Highcharts.chart('Temperature',{"  
"      chart: {type: 'areaspline'},"
"      title: {text: 'Temperatura nel tempo'},"
"      yAxis: {title: {text: '°C'}},"
"      xAxis: {"
"            type: 'datetime'   "
"      },"
"      plotOptions: {"
"         series: {marker: {enabled: true}}"
"      },"
"      series: [{"
"          name: 'Temperatura OWM',"
"          data: ["+T_24H_OWM+"]"
"      }";
    if (TH_Sensor){
      graph+=",{" 
"          name: 'Temperatura reale',"
"          data: ["+T_24H_REAL+"]"
"      }";
    }
    graph+="]});";
    graph+= "Highcharts.chart('Humidity',{"
"      chart: {type: 'areaspline'},"
"      title: {text: 'Umidità nel tempo'},"
"      yAxis: {title: {text: '%%'}},"
"      xAxis: {"
"           type: 'datetime'   "
"      },"
"      plotOptions: {"
"         series: {marker: {enabled: true}}"
"      },"
"      series: [{"
"          name: 'Umidità OWM',"
"          data: ["+H_24H_OWM+"]"
"      }";
    if (TH_Sensor){
      graph+=",{" 
"          name: 'Umidità reale',"
"          data: ["+H_24H_REAL+"]"
"      }";
    }
    graph+="]});</script>";
    return graph;
  }
  if (var == "DELAYPLACEHOLDER"){
    return String(Del);
  }
  if (var == "MAXTIMEPLACEHOLDER"){
    return String(Max_time);
  }
  if (var == "MANUALPLACEHOLDER"){
    String manual_pump="";
    for(int i=1; i<=NUM_PUMP; i++){
      manual_pump+="<div class='input-group mb-3'>";
      manual_pump+="<span class='input-group-text'>Attiva pompa "+String(i)+"</span>"
        "<button onclick='ManualPumpON("+ String(i-1) +")' class='btn btn-outline-secondary' type='button' id='button-addon'> ON </button></div>";
    }
    return manual_pump;
  }
  if (var == "THSENSORPLACEHOLDER"){
    return (TH_Sensor==true) ? "<option value='1' selected>Si</option><option value='0'>No</option>" : "<option value='1'>Si</option><option value='0' selected>No</option>";
  }  
  return String();
}


String getFullFormattedTime(time_t epochTime) {
   struct tm * ti = gmtime (&epochTime);
   
   uint16_t year = ti->tm_year + 1900;
   String yearStr = String(year);

   uint8_t month = ti->tm_mon + 1;
   String monthStr = month < 10 ? "0" + String(month) : String(month);

   uint8_t day = ti->tm_mday;
   String dayStr = day < 10 ? "0" + String(day) : String(day);

   uint8_t hours = ti->tm_hour;
   String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

   uint8_t minutes = ti->tm_min;
   String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

   //uint8_t seconds = ti->tm_sec;
   //String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);
   
   return dayStr + "-" + monthStr + "-" + yearStr + " " + hoursStr + ":" + minuteStr;
}

void DataPlotOrganizer(){
  T_24H_OWM="";
  H_24H_OWM="";
  T_24H_REAL="";
  H_24H_REAL="";
  for (i=0; i<NUM_MEASURES;i++){
    if (XaxisPlotTimes[i]!=0){
      T_24H_OWM+="["+ String(XaxisPlotTimes[i]) + "000," + String(T_last24H_OWM[i])+ "]";  
      H_24H_OWM+="["+ String(XaxisPlotTimes[i]) + "000," + String(H_last24H_OWM[i])+ "]";
      T_24H_REAL+="["+ String(XaxisPlotTimes[i]) + "000," + String(T_last24H_REAL[i])+ "]";
      H_24H_REAL+="["+ String(XaxisPlotTimes[i]) + "000," + String(H_last24H_REAL[i])+ "]";
    
      if (i!=NUM_MEASURES-1){
        T_24H_OWM+=",";
        H_24H_OWM+=",";
        T_24H_REAL+=",";
        H_24H_REAL+=",";
      } 
    }
  }
}
