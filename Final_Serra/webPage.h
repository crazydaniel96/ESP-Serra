const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1" charset="utf-8">
  <title>Serra autonoma</title>
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <script src="https://code.highcharts.com/modules/series-label.js"></script>
  <script src="https://kit.fontawesome.com/b5a5b5b989.js" crossorigin="anonymous"></script>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-KyZXEAg3QhqLMpG8r+8fhAXLRk2vvoC2f3B09zVXn8CA5QIVfZOJ3BCsw2P0p/We" crossorigin="anonymous">
  <style>
    * {box-sizing: border-box;}
    html {font-family: Arial; display: inline-block; text-align: center;}
    h1 {font-size: 3.0rem;}
    h2 {font-size: 2.3rem;}
    h3 {font-size: 2.0rem; margin-bottom: 0px;}
    h4 {font-size: 1em;}
    h5 {font-size: 0.83em;}
    p {font-size: 3.0rem;margin-top: 0px;}
    body {max-width: 800px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    .slider2 { -webkit-appearance: none; margin: 0px; width: 300px; height: 20px; background: #ccc; outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider2::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 30px; height: 30px; background: #2f4468; cursor: pointer;}
    .slider2::-moz-range-thumb { width: 30px; height: 30px; background: #2f4468; cursor: pointer; }   
    .tab {  overflow: hidden;  border: 1px solid #ccc;  background-color: #f1f1f1;}
    .tab button {  background-color: inherit;  float: left;  border: none;  outline: none;  cursor: pointer;  padding: 14px 16px;  transition: 0.3s;  font-size: 17px;}
    .tab button:hover {  background-color: #ddd;}
    .tab button.active {  background-color: #ccc;}
    .tabcontent {display: none;  animation: fadeEffect 1s;  padding: 6px 12px;  border: 1px solid #ccc;  border-top: none;}
    //.btn {display: block; background-color:#e3e6e3;color: black;padding: 12px;margin: 20px auto;border: none;border-radius: 3px;cursor: pointer;font-size: 17px;}
    .card{margin-top: 0.5rem;}
    @keyframes fadeEffect {  from {opacity: 0;}  to {opacity: 1;}}
    @media (max-width: 700px) {.row {flex-direction: column-reverse;}}
  </style>
</head>
<body>
  
  <div class="tab">
    <button class="tablinks" onclick="openTab(event, 'PumpControl')" id="defaultOpen">Controllo pompe</button>
    <button class="tablinks" onclick="openTab(event, 'Info')">Informazioni %WEATHERICON%</button>
    <button class="tablinks" onclick="openTab(event, 'Graphs')">Grafici</button>
    <button class="tablinks" onclick="openTab(event, 'Service')">Manutenzione</button>
  </div>
  
  <div id="PumpControl" class="tabcontent">
    %BUTTONPLACEHOLDER%
  </div>
  
  <div id="Info" class="tabcontent">
    %OTHERINFOPLACEHOLDER% 
  </div>

  <div id="Graphs" class="tabcontent">
    <div id="Temperature"></div>
    <div id="Humidity"></div>
  </div>
  
  <div id="Service" class="tabcontent">
    <div class="row">
      <div class="col-lg-6">
        <p style="font-size: 1.5rem">Attivazione pompe</p>
        %MANUALPLACEHOLDER%
      </div>
      <div class="col-lg-6">
        <p style="font-size: 1.5rem">Parametri irrigazione</p>
        <div class="input-group mb-3">
          <span class="input-group-text">Delay pompe</span>
          <input type="text" class="form-control" id="DelayInput" aria-describedby="button-addon" value=%DELAYPLACEHOLDER%>
          <button onclick="UpdateDelay()" class="btn btn-outline-secondary" type="button" id="button-addon"> Aggiorna </button>
        </div>

        <div class="input-group mb-3">
          <span class="input-group-text">Tempo massimo</span>
          <input type="text" class="form-control" id="Max_time" aria-describedby="button-addon2" value=%MAXTIMEPLACEHOLDER%>
          <button onclick="Update_Max_time()" class="btn btn-outline-secondary" type="button" id="button-addo2"> Aggiorna </button>
        </div>

        <br><p style="font-size: 1.5rem">Struttura serra</p>
        <div class="input-group mb-3">
          <label class="input-group-text" for="inputGroupSelect01">Sensore DHT11</label>
          <select class="form-select" id="inputGroupSelect01">
            %THSENSORPLACEHOLDER%
          </select>
          <button onclick="" class="btn btn-outline-secondary" type="button" id="button-addo3"> Aggiorna </button>
        </div>
                
      </div>
    </div>  
  </div>
  %GRAPHPLACEHOLDER%
  
<script>
  function UpdateDelay() {
    var delayValue = document.getElementById("DelayInput").value;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?delay="+delayValue, true);
    xhr.send();
  }

  function Update_Max_time() {
    var Max_time = document.getElementById("Max_time").value;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?Max_time="+Max_time, true);
    xhr.send();
  }
  
  function ManualPumpON(pump) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?relay="+pump+"&manual", true);
    xhr.send();
  }
  
  function toggleCheckbox(element) {
    var xhr = new XMLHttpRequest();
    if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
    else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
    xhr.send();
  }

  function toggleCheckboxInhibit(element) {
    index=element.id.slice(-1);
    var xhr = new XMLHttpRequest();
    if(element.checked){ xhr.open("GET", "/update?relay="+index+"&inhibit=1", true); }
    else { xhr.open("GET", "/update?relay="+index+"&inhibit=0", true); }
    xhr.send();
  }
  
  function updateSliderTimer(element) {
    index=element.id.slice(-1);
    var sliderValue = document.getElementById("timerSlider"+index).value;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?relay="+index+"&timer="+sliderValue, true);
    xhr.send();
  }  
  
  function openTab(evt, TabName) {
    var i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("tabcontent");
    for (i = 0; i < tabcontent.length; i++) {
      tabcontent[i].style.display = "none";
    }
    tablinks = document.getElementsByClassName("tablinks");
    for (i = 0; i < tablinks.length; i++) {
      tablinks[i].className = tablinks[i].className.replace(" active", "");
    }
    document.getElementById(TabName).style.display = "block";
    evt.currentTarget.className += " active";
  }
</script>
<script>document.getElementById("defaultOpen").click();</script>
</body>
</html>
)rawliteral";
