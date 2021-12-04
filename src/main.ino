#include <LibRobus.h>
#include <Wire.h>
#include <ds3231.h>
 
struct ts t; 

int Q1=0;

void setup()
{
  BoardInit();
  Serial.begin(9600);
  Wire.begin();
  DS3231_init(DS3231_CONTROL_INTCN);
  t.hour=17; 
  t.min=26;
  t.sec=0;
  t.mday=04;
  t.mon=12;
  t.year=2021;
  while(Q1==0){
    if(ROBUS_IsBumper(2)){
      DS3231_set(t); 
      Q1=1;
    }
  }
}

void loop()
{
  DS3231_get(&t);
  Serial.print(t.hour);
  Serial.print("h");
  Serial.print(t.min);
  Serial.print(" ");
  Serial.print(t.sec);
  Serial.println("s");
  delay(500);
}
