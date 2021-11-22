
#include <LibRobus.h>

#define BLACK_P  1
#define BLUE_P   2
#define RED_P    3
#define YELLOW_P 4

// testé et les positions sont bonnes
#define DEG_FOR_0_0   0
#define DEG_FOR_90_0  80
#define DEG_FOR_180_0 160

// pas encore testé
#define DEG_FOR_90_1  90
#define DEG_FOR_180_1 180
#define DEG_FOR_0_1   4

#define SERVO_0 0
#define SERVO_ONE 1


void ServoInit(void);

/** Function to distribute X number of Y pills
@param whichPills
Which colors of pills to distribute [BLACK_P, BLUE_P, RED_P, YELLOW_P]
@param howMany
How many pills to distribute [1, 2, 3 or 4]
*/
void ServoDropPills(uint8_t whichPills, uint8_t howMany);
void ResetServo90(void);





void ServoInit(void)
{
  SERVO_Enable(SERVO_0);
  SERVO_Enable(SERVO_ONE);
  ResetServo90();
}

/** Function to distribute X number of Y pills
@param whichPills
Which colors of pills to distribute [BLACK_P, BLUE_P, RED_P, YELLOW_P]
@param howMany
How many pills to distribute [1, 2, 3 or 4]
*/
void ServoDropPills(uint8_t whichPills, uint8_t howMany)
{

  if(whichPills == BLACK_P)
  {
    for (size_t i = howMany; i > 0; i--)
    {
      SERVO_SetAngle(SERVO_0,DEG_FOR_180_0);
      delay(1500);
      SERVO_SetAngle(SERVO_0,DEG_FOR_90_0);
      delay(1500);
    }
  }
  else if(whichPills == BLUE_P)
  {
    for (size_t i = howMany; i > 0; i--)
    {
      SERVO_SetAngle(SERVO_0,DEG_FOR_0_0);
      delay(1500);
      SERVO_SetAngle(SERVO_0,DEG_FOR_90_0);
      delay(1500);
    }
  }
  else if(whichPills == RED_P)
  {
    for (size_t i = howMany; i > 0; i--)
    {
      SERVO_SetAngle(SERVO_ONE,DEG_FOR_180_1);
      delay(1500);
      SERVO_SetAngle(SERVO_ONE,DEG_FOR_90_1);
      delay(1500);
    }
  }
  else if(whichPills == YELLOW_P)
  {
    for (size_t i = howMany; i > 0; i--)
    {
      SERVO_SetAngle(SERVO_ONE,DEG_FOR_0_1);
      delay(1500);
      SERVO_SetAngle(SERVO_ONE,DEG_FOR_90_1);
      delay(1500);
    }
  }
  ResetServo90();
}

void ResetServo90(void)
{
  SERVO_SetAngle(SERVO_0, DEG_FOR_90_0);
  SERVO_SetAngle(SERVO_ONE, DEG_FOR_90_1);
}