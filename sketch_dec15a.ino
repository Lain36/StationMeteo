#include <BH1750.h>
#include <Wire.h>
#include <stdio.h>
#include <CircularBuffer.h>
#include <LiquidCrystal.h>
#include <dht.h>
#define BUFFER_SIZE 60
CircularBuffer<int, BUFFER_SIZE> temperature_mesure; 
CircularBuffer<int, BUFFER_SIZE> humidity_mesure; 
CircularBuffer<int, BUFFER_SIZE> lux_mesure;
BH1750 lightMeter(0x5c);
dht DHT; 
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
int displayMode = 0;
const int DHT11_PIN = 7;
const int BTN_PIN = A3;
const int BTN2_PIN = A2;
int etatBtn2;
const int led1 = 2;
const int led2 = 3;

const int led3 = 5;
const int led4 = 9;
static const unsigned long REFRESH_INTERVAL = 6000;
static unsigned long lastRefreshTime = 0;
int temperature_value = 0;
int temperature_average = 0; 
int humidity_value = 0;
int humidity_average = 0;
int lux_value = 0;
int lux_average = 0;
int etat_led;
int PIR = 0;
int detect = A1;
unsigned long MS;
unsigned long start;

void setup() 
{ 
 Wire.begin();
 Serial.begin(9600);
 if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
  lcd.clear();
  lcd.setCursor(0,0);
    lcd.print(F("BH1750 Advanced begin"));
    delay(2000);
  } else {
    lcd.setCursor(0,1);
    lcd.print(F("Error initialising BH1750"));
    delay(2000);
  }
 displayInfos();
 getAllMesures();
 start=millis();
pinMode(led1, OUTPUT);
pinMode(led2, OUTPUT);
pinMode(led3, OUTPUT);
pinMode(led4, OUTPUT);
lcd.begin(16,2);


  
}
bool getAllMesures() 
{
  if (millis () - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime = millis();
    mesureHumidity ();
     mesureTemperature();
    return true;
  }
  else 
  {
    return false;
  }
}

void loop() 
{
  digitalWrite(led3, HIGH);
  delay(3000);
  digitalWrite(led4, HIGH);
bool displayNewMesures = getAllMesures();
bool buttonClicked = digitalRead(BTN_PIN);
if (buttonClicked)
{
  displayMode = (displayMode +1 ) % 5;
}
 if (displayNewMesures || buttonClicked)
 {
  displayInfos();
  delay(200);
 }

 
 float lux = lightMeter.readLightLevel();
 if(lux <= 2 )
 {
  digitalWrite(led1,HIGH);
 }
 else 
 { 
  digitalWrite(led1,LOW);
 }
 PIR = analogRead(detect);
 Serial.println(PIR);
  if ( PIR > 300)
  {
    digitalWrite(led2, HIGH);
    delay(700);
    digitalWrite(led2, LOW);
    delay(700);
  }
  else 
  {
     digitalWrite(led2, LOW);
  }
  if (lux <= 2 && PIR > 300)
  {
    digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
     delay(700);
    digitalWrite(led3, HIGH);
     digitalWrite(led4, LOW);
     delay(700);
     digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
     delay(700);
     digitalWrite(led3, HIGH);
     digitalWrite(led4, LOW);
     delay(700);
     digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
     delay(700);
     digitalWrite(led3, HIGH);
     digitalWrite(led4, LOW);
     delay(700);
     digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
     delay(700);
     digitalWrite(led3, HIGH);
     digitalWrite(led4, LOW);
     delay(700);
     digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
     delay(700);
  }
}
void displayInfos()
{
  int value, average;
  String text, unit;
  switch(displayMode)
  {
    case 0: 
    value = temperature_value;
    average = temperature_average; 
    text = "temp :";
    unit = " c";
      lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);
    lcd.print(value);
    lcd.print(unit);
    
    lcd.setCursor(0,1);
   lcd.print("moy : ");
   lcd.print(average);
   lcd.print(unit); 
    break;
     case 1: 
    value = humidity_value;
    average = humidity_average;
    text = "hum :";
    unit = " %";
      lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);
    lcd.print(value);
    lcd.print(unit);
    
    lcd.setCursor(0,1);
   lcd.print("moy : ");
   lcd.print(average);
   lcd.print(unit); 
    break;
     case 2: 
     text = "relay";
       lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);
    lcd.setCursor(0,1);
    lcd.print("etat : ");
    etat_led = digitalRead(led1);
     if (etat_led == HIGH)
  {
    lcd.print("on");
  }
  else if (etat_led == LOW)
  {
    lcd.print("off");
  }
    break;
    case 3 : 
    text = "Light: ";
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);  
    float lux = lightMeter.readLightLevel();
    lcd.print(lux);
    lcd.println(" lx");
  
    break;
    case 4 : 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("fin");
    break;
    
  }
}
void mesureTemperature()
{
  int values = DHT.read11(DHT11_PIN);
  temperature_value = round(DHT.temperature);
  temperature_mesure.push(temperature_value);
  temperature_average = getAverageBuffer(temperature_mesure);
 
}
void mesureHumidity ()
{
  int values = DHT.read11(DHT11_PIN);
  humidity_value = round(DHT.humidity);
  humidity_mesure.push(humidity_value);
  humidity_average = getAverageBuffer(humidity_mesure);
  
}
void mesureLux ()
{
   float lux = lightMeter.readLightLevel();
 
}
void printBuffer(CircularBuffer<int, BUFFER_SIZE> &circular_buffer)
{
  Serial.print("[");
  for (int i=0; i<circular_buffer.size() -1; i++)
  {
    Serial.print(circular_buffer[i]);
    Serial.print(";");  
  }
  Serial.println("]"); 
}

int getAverageBuffer(CircularBuffer<int,BUFFER_SIZE> &circular_buffer)
{
  int sum = 0;
  for ( int i=0; i< circular_buffer.size() -1; i++)
  {
    sum += circular_buffer[i];
  }
  float average = (float)sum / (circular_buffer.size() -1);
  return round(average);
}
