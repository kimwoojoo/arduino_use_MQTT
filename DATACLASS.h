#ifndef DATACLASS_h
#define DATACLASS_h
#include "ESPHEADER.h"
const int TempPin = 5;

class Data{
  public :
  float distance;
  float tlurbidity;
  float Volt;
  float temperature;
  float Waterquality;
  int trigPin;
  int echoPin;
  int RelayPin;
  int TurbidityPin;
  int OledSDAPin;
  int OledSCKPin;
  int RelayFanPin;
  String DiviceChip;
  String ip;
  String FANState;
  String MotorState;
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
     RelayFanPin = 4;
  }
  ~Data()
  {
  }
  
  void PinSet();
  float getDistance();
  float getTlurbidity();
  float getVolt();
  float getWaterquality();
  float getTemperature();
  String getFANState();
  String getIp();
  String getMotorState();
  String getDiviceChip();
};
#endif
