#include "cactus_io_DHT22.h"
#include <LiquidCrystal.h>
#define DHT22_PIN 2
int botao_ajuste_temp = 3;
const int MOSFET_PIN = 11;
unsigned long last_execution = 0;
float humid, temp;
unsigned int execution_idle_time = 60000;
float goal_temp = 18; 
 

DHT22 dht(DHT22_PIN);
const int rs = A5 , en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
 
void setup() 
{
  pinMode(botao_ajuste_temp, INPUT_PULLUP);
  attachInterrupt(1, set_temp, RISING); 
  dht.begin();
  pinMode(MOSFET_PIN, OUTPUT);
  lcd.begin(16, 2);
}

void set_temp(){
 static unsigned long last_interrupt_time = 0;
 unsigned long interrupt_time = millis();
 // If interrupts come faster than 500ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 500) {
    if (goal_temp >= 25){
      goal_temp = 16;
    }else{
      goal_temp += 1;
    } 
  }
  last_interrupt_time = interrupt_time;
  
}
 
void get_temp_and_humidity() 
{
  // Reading temperature or humidity takes about 250 milliseconds!
  dht.readHumidity();
  dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(dht.humidity) || isnan(dht.temperature_C)) {
    lcd.setCursor(0, 1);
    lcd.print("Err sensor DHT");    
  }
}

void loop(void) {//if millis overflow he will be minor than last_execution
  if((millis() > last_execution + execution_idle_time) or (millis() < last_execution)){
  get_temp_and_humidity();
  if (dht.temperature_C > (goal_temp)){
    digitalWrite(MOSFET_PIN, HIGH);
  }else if(dht.temperature_C < goal_temp){
    digitalWrite(MOSFET_PIN, LOW); 
  }
  last_execution = millis();
 }
  lcd.setCursor(0, 0);
  lcd.print((String)"Temp Alvo:"+goal_temp+" C");
  lcd.setCursor(0, 1);
  lcd.print((String)"T:" + dht.temperature_C + "C" + (String)" U:" + dht.humidity + "%%");
}
