#include <LibRobus.h>
#include <math.h>
#include <Wire.h>
#include "CapteurCouleur.h"



void setup()
{
  
  BoardInit();
  delay(100);
  ColorCapteurBegin();
 
}

void loop()
{
  struct RGB Couleurs;
  uint8_t Lecture = LectureCouleur(&Couleurs);
  Serial.print(Lecture);
  delay(2000);
}