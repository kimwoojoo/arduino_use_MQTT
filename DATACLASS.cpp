#include "DATACLASS.h"
void Data::PinSet()
{
  pinMode(TempPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(RelayPin, OUTPUT);
  pinMode(RelayFanPin, OUTPUT);
}


float Data::getDistance()
{
  return distance;
}
int Data::getTlurbidity()
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

String Data::getState()
{
  return State;
}

String Data::getIp()
{
  return ip;
}

String Data::getServerState()
{
  return ServerState;
}
String Data::getDiviceChip()
{
  return DiviceChip;
}

