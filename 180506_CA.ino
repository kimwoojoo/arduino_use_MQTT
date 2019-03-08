#include "ESPHEADER.h"
#include <pthread.h>

uint64_t chipID; 
//ESP32에서 센싱된 데이터들을 위한 값 
Data DataSet;
SSD1306Wire  display(0x3c, DataSet.OledSDAPin, DataSet.OledSCKPin);
pthread_cond_t UPloadCond, DisplayCond; 
pthread_mutex_t mutex,mutex2;
OneWire ds(TempPin);
AnalogPHMeter pHSensor(A0);
unsigned int pHCalibrationValueAddress = 0;


const char* ssid = "KimCGAC";
const char* password = "cgac5336";

//const char* ssid = "esp32";
//const char* password = "dnwndnwndnwn";
const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDpzCCAo+gAwIBAgIJANgOMi/sVJVVMA0GCSqGSIb3DQEBDQUAMGoxFzAVBgNV\n"\
"BAMMDkFuIE1RVFQgYnJva2VyMRYwFAYDVQQKDA1Pd25UcmFja3Mub3JnMRQwEgYD\n"\
"VQQLDAtnZW5lcmF0ZS1DQTEhMB8GCSqGSIb3DQEJARYSbm9ib2R5QGV4YW1wbGUu\n"\
"bmV0MB4XDTE4MDUwNTIwNTEzM1oXDTMyMDUwMTIwNTEzM1owajEXMBUGA1UEAwwO\n"\
"QW4gTVFUVCBicm9rZXIxFjAUBgNVBAoMDU93blRyYWNrcy5vcmcxFDASBgNVBAsM\n"\
"C2dlbmVyYXRlLUNBMSEwHwYJKoZIhvcNAQkBFhJub2JvZHlAZXhhbXBsZS5uZXQw\n"\
"ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCi86FmqBJ8Je9u4WC7nU4O\n"\
"8OX+3ihOOOmhzXJNs/Q3UtsQLWKeRQemfL5cW3+wr6BNRtQZgKT7RFDzGPBmdbmh\n"\
"vtX/Pbtbl23uUIVnUHc6nhPvSR0wkPXw25J87QVZnm+DX9lIb1UrblWNOq6/MOur\n"\
"mLxeDFruBCMAooNoTVZQJOj3lrtwCkAggilA3GO+x6CI6U2gEetQsa6HZ6kwVOJd\n"\
"XNBtYKeD7MJu9HsJ2sRw6Kvo7VIDjczXz8ck2A1HnU/uTX0l0w8+KUeIIVnqizOx\n"\
"hw5yDjTdZUSc1FvfPqzR1qbIpyIYSSWDFdS/1pCRLp9FTgI1bWLfKrY2h22J9hSf\n"\
"AgMBAAGjUDBOMB0GA1UdDgQWBBTJ2Cpql6Sz/kFl+yBU3wSaYORZRjAfBgNVHSME\n"\
"GDAWgBTJ2Cpql6Sz/kFl+yBU3wSaYORZRjAMBgNVHRMEBTADAQH/MA0GCSqGSIb3\n"\
"DQEBDQUAA4IBAQB2CJPRG7X0B+ftMlHNzHQ4w0PP06luYyRMFZd17m8lsEdFifdA\n"\
"b1JVPwZNt1k8gOvxJ2/yVe9IqCsTQDrYK1Yvf72oabM6bob6L4u0OGfrptuJs4vZ\n"\
"6XvpbbdBpxFsko3yL2ajaeK6Kwcbq38cNQejC4plryrup71UO7QuSqYXRkfGM1Er\n"\
"p3aBkl/eC3u/XP7IxKsC98xS0XagRDmgK1mr5nPtTHO/eogmwgHounZBwuZ3lL+f\n"\
"RgcwHEsVRuT+JhKxojNOBQDzE+Lpuw1gSYZacf3UQGQ1GpMfE1/XrJoTmEffUaqs\n"\
"rsYFA+tWpX03t4fDAoGDFDnKzr50Er9JU22l\n"\
"-----END CERTIFICATE-----\n";                

HTTPClient http; //get으로 Data가져오기
AsyncWebServer server(80); 
Adafruit_INA219 Sensor219; //전류전압센서
String MSGPrint; //보내줄 MSG 저장
#define MQTT_HOST IPAddress(?,?,?,?)
#define MQTT_PORT ?

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;


void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  /*
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    GetState();
    request->send(200, "HTTP/1.1 200 OK\nContent-type:text/html\n<meta charset=\"utf-8\">", HeadScript() +
    "<body>" +"칩 아이디 "+ DataSet.getDiviceChip() + "아이피 정보 " + DataSet.getIp() + "모터 상태 " + DataSet.getMotorState() + AutoSubmit() + formPrint() + "</body>");
  });
  */
/*
   server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(200, "HTTP/1.1 200 OK\nContent-type:text/html\n<meta charset=\"utf-8\">", HeadScript() +
    "<body>" +" "+ DataSet.getDiviceChip() + "아이피 정보 " + DataSet.getIp() + "모터 상태 " + DataSet.getMotorState() + AutoSubmit() + formPrint() + "</body>");
  });*/

}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        pthread_t UPload, Display;
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&UPloadCond, NULL);
        pthread_cond_init(&DisplayCond, NULL);
        pthread_create(&UPload, NULL, UPloadFunction, NULL);
        pthread_create(&Display, NULL, DisplayFunction, NULL);
        pthread_join(UPload, NULL);
        pthread_join(Display, NULL);
        pthread_cond_destroy(&UPload);
        pthread_cond_destroy(&Display);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}
char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}
void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe("esp32", 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);
  Serial.println(string2char(DataSet.ip));
  uint16_t packetIdPub2 = mqttClient.publish("esp32", 2, true, string2char(MSGPrint));
  Serial.print("Publishing at QoS 2, packetId: ");
  Serial.println(packetIdPub2);
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}
void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
void FANSwitch(int STATE)
{
  digitalWrite(DataSet.RelayFanPin, STATE);
}

void MOTORSwitch(int STATE)
{
  digitalWrite(DataSet.RelayPin, STATE);
}
void StateSwitch(String MQTTMESSAGE)
{
  //MQTTMESSAGE.length();
  
  if(MQTTMESSAGE.substring(12,17).compareTo("FANON") == 0)
  {
    DataSet.FANState = "1";
    FANSwitch(1);
    Serial.println(MQTTMESSAGE.substring(12,17));
  }
    
  else if(MQTTMESSAGE.substring(12,18).compareTo("FANOFF") == 0)
  {
    DataSet.FANState = "0";
    FANSwitch(0);
    Serial.println(MQTTMESSAGE.substring(12,18));
  }
  else if(MQTTMESSAGE.substring(12,19).compareTo("MOTORON") == 0)
  {
    DataSet.MotorState = "1";
    MOTORSwitch(1);
    Serial.println(MQTTMESSAGE.substring(12,19));
  }
  else if(MQTTMESSAGE.substring(12,20).compareTo("MOTOROFF") == 0)
  {
    DataSet.MotorState = "0";
    MOTORSwitch(0);
    Serial.println(MQTTMESSAGE.substring(12,20));
  }

}




//MQTT MESSAGE 받는것 ^^
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print(" payload: ");
  String payLoadTemp;

  payLoadTemp += payload;

  payLoadTemp.trim();

  //esp32 chipid랑 서버에서 보낸 message 속의 chipid가 같으면 동작 

  if(payLoadTemp.substring(0, 12).compareTo(DataSet.getDiviceChip()) == 0)
  {
    StateSwitch(payLoadTemp);
  }

  Serial.println(payLoadTemp);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

#define DEMO_DURATION 3000
#define Depth 30 //깊이
#define Height 24 //세로
#define Width 24// 가로
#define Calc (Depth * Height * Width)/10000
int demoMode = 0;
int counter = 1;
//세로 17
//높이 22
//가로 30
//sda a0, scl a1 Sensor219.begin();
typedef void (*Demo)(void);
/*
void setup()
{
  Serial.begin(115200);
  DataSet.PinSet();
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Server IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("\n\n");
  //DataSet.ip = SplitIp(clientData);
  //Sensor219.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setContrast(255);
  chipID=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  unsigned long long1 = (unsigned long)((chipID & 0xFFFF00000000) >> 32 );
  unsigned long long2 = (unsigned long)((chipID & 0x0000FFFF0000) >> 16 );
  unsigned long long3 = (unsigned long)((chipID & 0x00000000FFFF));
  DataSet.DiviceChip = String(long1, HEX) + String(long2, HEX) + String(long3, HEX);
  DataSet.DiviceChip.toUpperCase(); 
  server.on("", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    GetState();
    request->send(200, "HTTP/1.1 200 OK\nContent-type:text/html\n<meta charset=\"utf-8\">", HeadScript() +
    "<body>" +"칩 아이디 "+ DataSet.getDiviceChip() + "아이피 정보 " + DataSet.getIp() + "모터 상태 " + DataSet.getMotorState() + AutoSubmit() + formPrint() + DATAPrint() + "</body>");
  });

  display.clear();
  server.begin();
}
*/

void Send_MQTTMSG()
{
  MSGPrint="{\"Distance\":";
  MSGPrint += "";
  MSGPrint +=DataSet.getWaterCm();
  MSGPrint +=",";
  MSGPrint +="\"Temp\":";
  MSGPrint +=DataSet.getTemperature();
  MSGPrint +=",";
  MSGPrint +="\"Turbidity\":";
  MSGPrint +=DataSet.getVolt();
  MSGPrint +=",";
  MSGPrint +="\"Waterquality\":";
  MSGPrint +=DataSet.getWaterquality();
  MSGPrint +=",";
  MSGPrint +="\"chipid\":\"";
  MSGPrint +=DataSet.getDiviceChip();
  MSGPrint +="\",";
  MSGPrint +="\"MOTORState\":\"";
  MSGPrint +=DataSet.getMotorState();
  MSGPrint +="\",";
  MSGPrint +="\"FANState\":\"";
  MSGPrint +=DataSet.getFANState();
  MSGPrint +="\",";
  MSGPrint +="\"IPAdress\":\"";
  MSGPrint += DataSet.getIp();
  MSGPrint +="\"}";
  mqttClient.publish("esp32", 2, true, string2char(MSGPrint));
}
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  DataSet.PinSet();
  chipID=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  unsigned long long1 = (unsigned long)((chipID & 0xFFFF00000000) >> 32 );
  unsigned long long2 = (unsigned long)((chipID & 0x0000FFFF0000) >> 16 );
  unsigned long long3 = (unsigned long)((chipID & 0x00000000FFFF));
  DataSet.DiviceChip = String(long1, HEX) + String(long2, HEX) + String(long3, HEX);
  DataSet.DiviceChip.toUpperCase(); 
  
  struct PHCalibrationValue pHCalibrationValue;
  EEPROM.get(pHCalibrationValueAddress, pHCalibrationValue);
  pHSensor.initialize(pHCalibrationValue);
  //GetState();
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  WiFi.onEvent(WiFiEvent);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  connectToWifi();
  adc1_config_width(ADC_WIDTH_12Bit);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setContrast(255);
  display.clear();
  DataSet.MotorState ="0";
  DataSet.FANState = "0"; 


  
}
void GetState()
{
  http.begin("http://api.ipify.org/?ip=1");
  http.GET();
  DataSet.ip = http.getString();  
  http.end();
  /*
  http.begin("http://115.68.228.55/GetEspState.php/?chipid="+DataSet.getDiviceChip());
  http.GET();
  DataSet.MotorState = http.getString();
  http.end();*/
}
void CurrentSensorPrint()
{
 float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;

  shuntvoltage = Sensor219.getShuntVoltage_mV(); //Shunt 전압측정
  busvoltage = Sensor219.getBusVoltage_V(); //Bus 전압측정 
  current_mA = Sensor219.getCurrent_mA();   //전류 측정
  loadvoltage = busvoltage + (shuntvoltage / 1000); //Load 전압계산
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");   //Bus 전압 출력
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV"); //Shunt 전압 출력
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");   //Load 전압 출력
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");   //전류 출력
  Serial.println("");
}

String ControlPrint()
{
  String HTMLPrint = "<script type=\"text/javascript\">";
  HTMLPrint += "function BtnSet(S){";
  HTMLPrint +="if(S==\"PumpOn\"){";
  HTMLPrint +="document.getElementById(\"PumpState\").value=\"1\";}";
  HTMLPrint +="else {document.getElementById(\"PumpState\").value=\"0\";}";
  HTMLPrint +="}";
  HTMLPrint +="</script>";    
  return HTMLPrint;
}
String AutoSubmit()
{
   String SubmitUpDate = "<form name = \"AutoSubmitForm1\" action = \"http://115.68.228.55/EspUp.php\" method = \"post\">";
   SubmitUpDate += "<input  type=\"hidden\" name = \"chipid\" value =\"";
   SubmitUpDate += DataSet.getDiviceChip();
   SubmitUpDate +="\">";
   SubmitUpDate +="<input id=\"PumpState\" type=\"hidden\" name=\"State\" value = \"";
   SubmitUpDate += "1";
   SubmitUpDate +="\">";
   SubmitUpDate += "<input  type=\"hidden\" name = \"IPA\" value =\"";
   SubmitUpDate += WiFi.localIP().toString();
   SubmitUpDate +="\">";
   SubmitUpDate +="<input type=\"submit\" value=\"PumpOn\">";
   SubmitUpDate +="</form>"; 
   SubmitUpDate += "<form name = \"AutoSubmitForm2\" action = \"http://115.68.228.55/EspUp.php\" method = \"post\">";
   SubmitUpDate += "<input  type=\"hidden\" name = \"chipid\" value =\"";
   SubmitUpDate += DataSet.getDiviceChip();
   SubmitUpDate +="\">";
   SubmitUpDate +="<input id=\"PumpState\" type=\"hidden\" name=\"State\" value = \"";
   SubmitUpDate += "0";
   SubmitUpDate +="\">";
   SubmitUpDate += "<input  type=\"hidden\" name = \"IPA\" value =\"";
   SubmitUpDate += WiFi.localIP().toString();
   SubmitUpDate +="\">";
   SubmitUpDate +="<input type=\"submit\" value=\"PumpOFF\">";
   SubmitUpDate +="</form>"; 
   return SubmitUpDate;
}

String HeadScript()
{
  String HTMLPrint = "<head><meta charset=\"UTF-8\" />" ;
  HTMLPrint += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>";
  HTMLPrint += "<script type=\"text/javascript\">";
  
  /*
  HTMLPrint += "function BtnSet(S){";
  HTMLPrint +="if(S==\"PumpOn\"){";
  HTMLPrint +="document.getElementById(\"PumpState\").value=\"1\";";
  HTMLPrint +="this.document.getElementById(\"AutoSubmitForm\").submit();}";
  HTMLPrint +="else {document.getElementById(\"PumpState\").value=\"0\";";
  HTMLPrint +="this.document.getElementById(\"AutoSubmitForm\").submit();}";
  HTMLPrint +="}";
  */
 
  HTMLPrint += "$(document).ready(function(){ ";
  HTMLPrint += "$(\"#HideDiv\").hide();";
  HTMLPrint += "var iTime = 10;";
  HTMLPrint += "var h;";
  HTMLPrint += "var m;";
  HTMLPrint += " setInterval(function() {";
  HTMLPrint += "if(iTime == 0){";
  HTMLPrint += "iTime = 5;";
  HTMLPrint += "refresh();";
  HTMLPrint += "}";
  HTMLPrint += "iTime--;";
  HTMLPrint += "h = parseInt(iTime/60);";
  HTMLPrint += "m = iTime%60;";
  HTMLPrint += "if(m < 10){";
  HTMLPrint += "m = \"0\"+m;";
  HTMLPrint += "}";
  HTMLPrint += " },1000);";
  HTMLPrint += "});";
  HTMLPrint += "function refresh(){";
  HTMLPrint += "document.DS.submit();";
  //HTMLPrint += "window.location.reload();";
  HTMLPrint += "}";
  HTMLPrint +="</script>"; 
  HTMLPrint +="</head>";
  return HTMLPrint;
}
String formPrint()
{
  //부분 숨기기
  String HTMLPrint = "<div id=HideDiv>";
  HTMLPrint +="<form name = \"DS\" action = \"http://115.68.228.55/EspEx.php\" method = \"post\">";
  HTMLPrint +="<input type = \"text\" name = \"Dis\" value = \"";
  HTMLPrint +=DataSet.getDistance();
  HTMLPrint +="\">";
  HTMLPrint +="<input type = \"text\" name = \"Temp\" value = \"";
  HTMLPrint +=DataSet.getTemperature();
  HTMLPrint +="\">";
  HTMLPrint +="<input type = \"text\" name = \"Turbidity\" value = \"";
  HTMLPrint +=DataSet.getVolt();
  HTMLPrint +="\">";
  HTMLPrint +="<input type = \"text\" name = \"chipid\" value = \"";
  HTMLPrint +=DataSet.getDiviceChip();
  HTMLPrint +="\">";
  HTMLPrint +="<input type = \"text\" name = \"State\" value = \"";
  HTMLPrint +=DataSet.getFANState();
  HTMLPrint +="\">";
  HTMLPrint +="<input type = \"text\" name = \"State\" value = \"";
  HTMLPrint +=DataSet.getMotorState();
  HTMLPrint +="\">";
  HTMLPrint +="<input type = \"text\" name = \"IPA\" value = \"";
  HTMLPrint += DataSet.getIp();
  HTMLPrint +="\">";
  HTMLPrint +="<input type=\"submit\" value=\"Submit\">" ;
  HTMLPrint +="</form>";
  HTMLPrint += "</div>";
  return HTMLPrint;
}
//Demo demos[] = {drawFontFaceDemo, drawTextFlowDemo, drawTextAlignmentDemo, drawRectDemo, drawCircleDemo, drawProgressBarDemo, drawImageDemo};
Demo demos[] = {drawFontFaceDemo, drawTextAlignmentDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;
void loop()
{

//DataSet.MotorState 모터 동작 상태 0이면 OFF
//DataSet.State 수위 상태 0 이면 저수위
//서버 동작 신호가 1이고 저수위 일경우 동작
  
  //if (DataSet.MotorState.equals("1")) 
 // {
  //    digitalWrite(DataSet.RelayPin, HIGH);               // GET /H turns the Motor on
 // }
  //if ( DataSet.State.equals("1")) 
  //{
  //  digitalWrite(DataSet.RelayPin, HIGH);               // GET /H turns the Motor on
 // }
  //if (DataSet.MotorState.equals("0")) 
  //{
  //    digitalWrite(DataSet.RelayPin, LOW);               // GET /H turns the Motor on
 // }
  //else if(DataSet.State.equals("0"))
  //{
  //  digitalWrite(DataSet.RelayPin, LOW);               // GET /H turns the Motor on
 // }

}

/*
String SplitIp(String HostIp)
{
  String TempHostIp;
  String ReturnHostIp;
  int FirstIndex;
  int LastIndex;
  TempHostIp = HostIp;
  TempHostIp.trim();
  FirstIndex = TempHostIp.indexOf("Host:");
  FirstIndex += 6;
  LastIndex = TempHostIp.indexOf("Connection");
  ReturnHostIp = TempHostIp.substring(FirstIndex, LastIndex);
  return ReturnHostIp;
}
*/

float getDistance()
{
  float distance;
  float hWaterCm; 
  digitalWrite(DataSet.trigPin, LOW);
  delayMicroseconds(20);
  digitalWrite(DataSet.trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(DataSet.trigPin, LOW);
  distance=pulseIn(DataSet.echoPin, HIGH)/58.2;
  //hWaterCm =  Height - distance + 10;
  //return hWaterCm;
  return distance;
}


float getTemp()
{                                   //온도 측정 후 반환하는 함수
 byte data[12];
 byte addr[8];
 //return -1000인 경우 오류가 생겼다는 이야기.
 //-1000 이거나 온도가 100도 이상 돼었을때 값 리턴 시켜주지 않는 방법을 이용
 if ( !ds.search(addr)) 
 {
   ds.reset_search();
   return -1000;
 }
 
 if ( OneWire::crc8( addr, 7) != addr[7]) 
 {
   Serial.println("CRC is not valid!");
   return -1000;
 }
 
 if ( addr[0] != 0x10 && addr[0] != 0x28) 
 {
   Serial.print("Device is not recognized");
   return -1000;
 }
 
 ds.reset();
 ds.select(addr);
 ds.write(0x44,1);                                   
 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); 
 
 for (int i = 0; i < 9; i++) { 
  data[i] = ds.read();                                                          
 }
 
 ds.reset_search(); 
 byte MSB = data[1];
 byte LSB = data[0];
 float tempRead = ((MSB << 8) | LSB); 
 float TemperatureSum = tempRead / 16; 
 if(!(TemperatureSum >100))
 return TemperatureSum;   
                                                                 
}


/////////////////////////////////////////////////////////
//                     PthreadStart                    //
/////////////////////////////////////////////////////////
void *UPloadFunction(void *arg)
{
  int liters;
  float WaterCm;
  while(1)
  {
    adc1_config_channel_atten(ADC1_CHANNEL_5,ADC_ATTEN_11db); //채널 5번은 gpio 33
    float Voltage;
    //CurrentSensorPrint();
    float TempRaTure = getTemp();
    //float Tempdistance = getDistance();
    DataSet.distance = getDistance(); //물과의 거리

    
    //물높이는 세로 - 거리 + 10(초음파 센서 측정값 mm)
    WaterCm = Height - DataSet.distance + 10; //물의 높이
    DataSet.WaterCm = WaterCm;
    liters = floor(Calc * WaterCm);
    DataSet.WaterLiters = liters;
    Serial.println(DataSet.getWaterCm());
    Serial.println(DataSet.getWaterLiters());
    int sensorValue = adc1_get_voltage(ADC1_CHANNEL_5);
    Voltage = sensorValue * ( 5.0 / 1024.0) ; 
    DataSet.Volt = Voltage;
    DataSet.Waterquality = pHSensor.singleReading().getpH();
    if(!(TempRaTure < -50 || TempRaTure > 100))
    {
      DataSet.temperature = TempRaTure;
    }
    //물 높이(liter) 재기 
    //DataSet.distance = floor(Calc * Tempdistance * 10);
    //DataSet.distance = getDistance();
    Send_MQTTMSG();
    delay(10000);
  }
  

}

void *DisplayFunction(void *arg)
{
  while(1)
  {
    display.clear();
    demos[demoMode]();
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(10, 128, String(millis()));
    // write the buffer to the display
    display.display();
    if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
    }
  }
  delay(1000);
}
/////////////////////////////////////////////////////////
//                     PthreadEnd                      //
/////////////////////////////////////////////////////////





void drawFontFaceDemo() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "IP:"+WiFi.localIP().toString() );
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 5, DataSet.getIp());
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 25, "ChipID:"+ DataSet.getDiviceChip());
    display.setFont(ArialMT_Plain_10);
    if(DataSet.getMotorState().compareTo("1")==0)
    {
      display.drawString(0, 35,"MOTOR STATE : ON");
    }
    else
    {
      display.drawString(0, 35,"MOTOR STATE : OFF");
    }
    display.setFont(ArialMT_Plain_10);
    if(DataSet.getFANState().compareTo("1")==0)
    {
      display.drawString(0, 45,"FAN STATE : ON");
    }
    else
    {
      display.drawString(0, 45,"FAN STATE : OFF");
    }

}
/*
void drawTextFlowDemo() {
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawStringMaxWidth(0, 0, 128,"HI" );
}
*/
void drawTextAlignmentDemo() {
    // Text alignment demo
  display.setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0 ,"Water Distance : "+String(DataSet.getDistance()));
  display.drawString(0, 10 ,"Water Liters : "+String(DataSet.getWaterLiters()));
  display.drawString(0, 20 ,"Water Cm: " + String(DataSet.getWaterCm()));
  // The coordinates define the center of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 30, "Temperature : "+String(DataSet.getTemperature()));

  // The coordinates define the right end of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 40, "Tudibity : "+String(DataSet.getVolt()));
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 50, "Water Quality : " +String(DataSet.getWaterquality()));
}
/*
void drawRectDemo() {
      // Draw a pixel at given position
    for (int i = 0; i < 10; i++) {
      display.setPixel(i, i);
      display.setPixel(10 - i, i);
    }
    display.drawRect(12, 12, 20, 20);

    // Fill the rectangle
    display.fillRect(14, 14, 17, 17);

    // Draw a line horizontally
    display.drawHorizontalLine(0, 40, 20);

    // Draw a line horizontally
    display.drawVerticalLine(40, 0, 20);
}

void drawCircleDemo() {
  for (int i=1; i < 8; i++) {
    display.setColor(WHITE);
    display.drawCircle(32, 32, i*3);
    if (i % 2 == 0) {
      display.setColor(BLACK);
    }
    display.fillCircle(96, 32, 32 - i* 3);
  }
}

void drawProgressBarDemo() {
  int progress = (counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}

void drawImageDemo() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}
*/


