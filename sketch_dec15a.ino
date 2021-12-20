#include <BH1750.h>

#include <Wire.h>



#include <stdio.h>
#include <CircularBuffer.h>
#include <LiquidCrystal.h>
#include <dht.h>
#define BUFFER_SIZE 60
#define _TOOLOW                       25
#define _LOW                          50
#define _HIGH                         500
#define _TOOHIGH                      750
#define LEVEL_TOOLOW                  "Trop bas"  
#define LEVEL_LOW                     "Bas"          
#define LEVEL_OPTIMAL                 "Idéal"         
#define LEVEL_HIGH                    "Elevé"        
#define LEVEL_TOOHIGH                 "Trop élevé"  
uint16_t lux        = 250;
int      luxLevel   = 3;
String   luxMessage = LEVEL_OPTIMAL;
CircularBuffer<int, BUFFER_SIZE> temperature_mesure; 
CircularBuffer<int, BUFFER_SIZE> humidity_mesure; 

BH1750 lightMeter(0x23);
dht DHT; 
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
int displayMode = 0;
const int DHT11_PIN = 7;
const int BTN_PIN = A3;
const int BTN2_PIN = A2;
int etatBtn2;
const int led1 = 2;
const int led2 = 3;
static const unsigned long REFRESH_INTERVAL = 6000;
static unsigned long lastRefreshTime = 0;
int temperature_value = 0;
int temperature_average = 0; 
int humidity_value = 0;
int humidity_average = 0;
int etat_led;
unsigned long MS;
unsigned long start;
void setup() { 
pinMode(led1, OUTPUT);
pinMode(led2, OUTPUT);
lcd.begin(16,2);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("    bonjour");
delay(3000);
 displayInfos();
 getAllMesures();
 start=millis();

  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
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
 etatBtn2 = digitalRead(BTN2_PIN);
 if( etatBtn2 == HIGH)
 {
  digitalWrite(led1,HIGH);
 }
 else
 {
  digitalWrite(led1,LOW);
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
     text = "led";
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
    
    break;
   case 4 : 
    text = "temps ecroulé";
    lcd.setCursor(0,0);
    lcd.print(text);
    lcd.setCursor(0,1);
     lcd.print ((MS / 1000) / 60);      
  lcd.print ("' ");
  lcd.print ((MS / 1000) % 60);      
  lcd.print ("'' ");
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
