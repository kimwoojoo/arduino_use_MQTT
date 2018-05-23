#ifndef DATACLASS_h
#define DATACLASS_h
#include "ESPHEADER.h"
const int TempPin = 5;

class Data{
  public :
  float distance;
  int tlurbidity;
  float Volt;
  float temperature;
  int trigPin;
  int echoPin;
  int RelayPin;
  int TurbidityPin;
  int OledSDAPin;
  int OledSCKPin;
  int RelayFanPin;
  String DiviceChip;
  String ip;
  String State;
  String ServerState;
  int VoltageSDAPin;
  int VoltageSCLPin;
  public :
  Data()
  {
     trigPin=26;
     echoPin=27;
     RelayPin = 17;
     TurbidityPin = 32;
     OledSDAPin = 21;
     OledSCKPin = 22;
     VoltageSDAPin = 23;
     VoltageSCLPin = 19;
     RelayFanPin = 16;
  }
  ~Data()
  {
  }
  
  void PinSet();
  float getDistance();
  int getTlurbidity();
  float getVolt();
  float getTemperature();
  String getState();
  String getIp();
  String getServerState();
  String getDiviceChip();
};
#endif
