//
//  PGM:  SENSOR01
//
//  Ver:  1.1.1
//
//  Date: 2016-01-22
//
//  Requires:
//
//  Hardware: Uses the BME280 by Embeded Adventures Temp and Humid sensor.
//
//  This sketch takes a sample of Temperature, Humidity and Pressure every 5 minute. This data is 
//  written to the MySQL database and ThingSpeak.
#include "Timer.h"
#include <BME280_MOD-1022.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Ethernet.h>
#include <SPI.h>
#include "ethernetSettings.h"

#define THING_SPEAK          "POST /update HTTP/1.1\n\rHost: api.thingspeak.com\n\rConnection:close\n\rX-THINGSPEAKAPIKEY: UMBGKCJ3ZULDV9Y2\n\rContent-type: application/x-www-form-urlencoded\n\rContent-Length: "
#define CLOSE_STRING         "Host: 10.1.1.25 Connection: close"
#define MYSQL_OPEN_STRING    "Get /add2.php?f0="
#define ONE_MINUTE           60000
#define FIVE_MINUTE          300000
#define SIXTY_MINUTE         3600000
#define SENSORID             "SENSOR01"
LiquidCrystal_I2C lcd(0x27, 16, 2);// Set the LCD address to 0x27 for a 16 chars and 2 line display

//Setup Sensor Interface
float temp, humidity, pressure;

String getString, getIOTString, sensorID;

Timer t;

void setup()
{
  Serial.begin(9600);
  
  Ethernet.begin(mac, ip);
      
  int fiveMinutePause = t.every(FIVE_MINUTE, timeIsUp);
  Serial.print("5 minute tick started id= ");
  Serial.println(fiveMinutePause);
 
  // initialize the LCD
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(SENSORID);
  lcd.setCursor(0,1);
  lcd.print("-Initilizing...");
  
  delay(1000);

  //lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("IP=");
  lcd.print(Ethernet.localIP());
  lcd.print("   ");
  lcd.print(9,0);
  lcd.print("BME280");
  delay (5000);//wait a bit for everything to settle

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
  Serial.println("Setup complete, starting timer loop");

  //Send the first data point before timmer is started.
  delay(6000); //Wait a bit so IP data can be read
  doIT();
}
  
void loop()
{
  t.update();
}

void doIT() {
  // Need to turn on 1x oversampling, default is os_skipped, which means it doesn't measure anything
  BME280.writeOversamplingPressure(os1x);  // 1x over sampling (ie, just one sample)
  BME280.writeOversamplingTemperature(os1x);
  BME280.writeOversamplingHumidity(os1x);
  
  // example of a forced sample.  After taking the measurement the chip goes back to sleep
  BME280.writeMode(smForced);
  BME280.readCompensationParams();
  BME280.readMeasurements();
  temp      = BME280.getTemperature();
  humidity  = BME280.getHumidity();
  pressure  = BME280.getPressure();
  
  Serial.print("Sensors readings: ");
  Serial.print((temp*1.8)+32);
  Serial.print(" | ");
  Serial.print(humidity);
  Serial.print(" | ");
  Serial.println(pressure);
  //Print to LCD Display
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print((temp*1.8)+32);
  lcd.print("F");
  lcd.setCursor(10,0);
  lcd.print(humidity);
  lcd.print("%");
  lcd.setCursor(1,1);
  lcd.print(pressure);
  lcd.print("Mb");
  
  //make the MySQL string
  getString = MYSQL_OPEN_STRING;
  getString += SENSORID;
  getString += "&f1=";
  getString += temp;
  getString += "&f2=";
  getString += humidity;
  getString += "&f3=";
  getString += pressure;
  getString += "&f4=0v0";
  Serial.print("sqlString: ");
  Serial.println(getString);
  Serial.println();
  //Send data to the MySQL server
  if (client.connect(serverLH, 80))  {
    client.println (getString);
    client.println(CLOSE_STRING);
    client.println();
    client.println();
    client.stop();
    client.stop();
  }
  else  {
    Serial.println("Transmission error with MySQL server");
    client.stop();
  }
  //make the IOT string
  getIOTString = "field1=";
  getIOTString += SENSORID;
  getIOTString += "&field2=";
  getIOTString += ((temp*1.8)+32);
  getIOTString += "&field3=";
  getIOTString += humidity;
  getIOTString += "&field4=";
  getIOTString += pressure;
  Serial.print("iotString: ");
  Serial.println(getIOTString);
  //Send the data to ThingSpeak
  if (client.connect(serverTS, 80))  { 
//      client.print("POST /update HTTP/1.1\n");
//      client.print("Host: api.thingspeak.com\n");
//      client.print("Connection:close\n");
//      client.print("X-THINGSPEAKAPIKEY: UMBGKCJ3ZULDV9Y2\n");
//      client.print("Content-type: application/x-www-form-urlencoded\n");
//      client.print("Content-Length: ");
      client.print(THING_SPEAK);
      client.print(getIOTString.length());
      client.print("\n\n");
      client.print(getIOTString);
      client.stop();
  }
  else  {
      Serial.println("Transmission error with ThingSpeak server");
      client.stop();
  }
}
void timeIsUp()
{
  Serial.println("5 minutes have elapsed, getting and sending data");
  doIT();
}

