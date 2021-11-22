#include <LibRobus.h>
#include <math.h>
#include "ServoMoteur.h"



void setup()
{
  BoardInit();
  delay(100);
  ServoInit();
}

void loop()
{
  ServoDropPills(BLACK_P, 2);
  ServoDropPills(BLUE_P, 3);
  ServoDropPills(RED_P, 1);
}




