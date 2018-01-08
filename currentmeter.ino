// Monitoraggio produzione da impianto fotovoltaico
// (C) 2016 - Roberto Giungato

// componenti:
// pinza amperometrica
// termometro digitale
// invio dati a Cloud Particle

// This #include statement was automatically added by the Particle IDE.
#include "EmonLib/EmonLib.h"
#include "OneWire/OneWire.h"
#include "spark-dallas-temperature/spark-dallas-temperature.h"

double tempC = 0.0;
int tempSensorPin = D5;
int redPin = D2;
double Irms, VI;
float realPower;
String sIrms, stempC, srealPower;

OneWire oneWire(tempSensorPin);
DallasTemperature sensors(&oneWire);

EnergyMonitor emon1;                   // Create an instance of Emonlib


// double VI;
// String stempC;
// String srealPower;
// int red = D2;

void setup()
{  
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  digitalWrite(redPin, LOW);
  sensors.begin();

  emon1.voltage(2, 234.26, 1.7);
  emon1.current(1, 30);             // Current: input pin, calibration. it was 111.1 for SCT-013-030
}

void loop()
{
  Irms = emon1.calcIrms(1480);  // Calculate Irms only   era 1480
  emon1.calcVI(20,2000);
  realPower = emon1.realPower;
  sIrms = String(Irms*100);
  srealPower = String(realPower);

  Serial.print(Irms*220.0);	       // Apparent power
  Serial.print(" ");
  Serial.println(Irms);	// Irms
  
  
  Particle.publish("IRMS",sIrms,60,PRIVATE);
  Particle.publish("RealPower", srealPower,60, PRIVATE);
  delay(4000);
  flashLed(redPin);
  
  sensors.requestTemperatures();
  double tempCheck = sensors.getTempCByIndex(0);
  
  if ( tempCheck > -50 && tempCheck < 50) // might want to guard against a spurious high value as well
  {
    tempC = tempCheck;
  }
  stempC = String(tempC-3);
  Particle.publish("Temp",stempC, 60, PRIVATE);
  delay(4000);

}

void flashLed(int pin)
{
  int ledState = digitalRead(pin);
  if (ledState == LOW) {
      digitalWrite(pin, HIGH);
      delay(100);
      digitalWrite(redPin, LOW);
  } else {
      digitalWrite(pin, LOW);
      delay(100);
      digitalWrite(redPin, HIGH);  
  }
}
