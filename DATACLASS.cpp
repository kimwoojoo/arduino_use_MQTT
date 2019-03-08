#include "DATACLASS.h"
void Data::PinSet()
{
  pinMode(TempPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(RelayPin, OUTPUT);
  pinMode(RelayFanPin, OUTPUT);
}

int Data::getWaterLiters()
{
  return WaterLiters;
}
float Data::getWaterCm()
{
  return WaterCm;
}
float Data::getWaterquality()
{
  return Waterquality;
}
float Data::getDistance()
{
  return distance;
}
float Data::getTlurbidity()
{
  return tlurbidity;
}
float Data::getVolt()
{
  return Volt;
}
float Data::getTemperature()
{
  return temperature;
}

String Data::getFANState()
{
  return FANState;
}

String Data::getIp()
{
  return ip;
}

String Data::getMotorState()
{
  return MotorState;
}
String Data::getDiviceChip()
{
  return DiviceChip;
}

