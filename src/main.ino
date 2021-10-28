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
  if(Lecture == RED)
  {
    Serial.print("RED\n");
  }
  if(Lecture == GREEN)
  {
    Serial.print("GREEN\n");
  }
  if(Lecture == BLUE)
  {
    Serial.print("BLUE\n");
  }
  if(Lecture == YELLOW)
  {
    Serial.print("YELLOW\n");
  }
  delay(1000);
}