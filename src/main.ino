#include <LibRobus.h>
#include <Wire.h>
#include <ds3231.h>
 
struct ts t; 


void setup()
{
  BoardInit();
  Serial.begin(9600);
  Wire.begin();
}

void loop()
{
  AUDIO_PlayBlocking(track);
}
