#include <LibRobus.h>
#include <math.h>

const double WHEEL_CIRCONFERENCE = 2 * PI * 1.5;
const double ROBOT_CIRCONFERENCE = 2 * PI * 3.667;

void setup()
{
  BoardInit();
  delay(100);
}

void loop()
{
  Acceleration_Sur_Distance(10, .7);
  avancer_distance(96, .7);
  Deceleration_Sur_Distance(10, .7);
  delay(100);
  tourner(89, 0, 0.3);
  delay(100);
  Acceleration_Sur_Distance(10, .7);
  avancer_distance(70, .7);
  Deceleration_Sur_Distance(10, .7);
  delay(100);
  tourner(89, 0, 0.3);
  delay(100);
  Acceleration_Sur_Distance(10, .7);
  avancer_distance(96, .7);
  Deceleration_Sur_Distance(10, .7);
  delay(100);
  tourner(89, 0, 0.3);
  delay(100);
  Acceleration_Sur_Distance(10, .7);
  avancer_distance(70, .7);
  Deceleration_Sur_Distance(10, .7);
  delay(100);
  tourner(89, 0, 0.3);
  delay(100);
}

double MOVEMENTS_RightWheelCorrection(double speed)
{
  const double factor = 1.123;

  float leftEncoder = abs(ENCODER_Read(0));
  float rightEncoder = abs(ENCODER_Read(1));
  if (leftEncoder == rightEncoder)
    return speed;
  else if (leftEncoder > rightEncoder + 1)
    return speed * factor;
  else if (leftEncoder < rightEncoder + 1)
    return speed - (speed * factor);
}

void MOVEMENTS_LeftWheel(double speed)
{
}
void MOVEMENTS_RightWheel(double speed)
{
}

float Tour_De_Roue_Selon_Distance(float distance)
{
  return distance / WHEEL_CIRCONFERENCE;
}

// c la bonne
void Acceleration_Sur_Distance(float distance, float speed)
{
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  float acceleration;
  float tourDeRoue = Tour_De_Roue_Selon_Distance(distance);
  float encoderDesire = tourDeRoue * 3200;
  const int segment = tourDeRoue * 8;
  float leftEncoder = ENCODER_Read(0);
  while (leftEncoder < encoderDesire)
  {
    leftEncoder = ENCODER_Read(0);
    acceleration = speed * leftEncoder / encoderDesire;
    if (acceleration < 0.07)
      acceleration = 0.07;
    MOTOR_SetSpeed(0, acceleration);
    MOTOR_SetSpeed(1, acceleration);
    MOVEMENTS_RightWheelCorrection(acceleration);
  }
}

// c la bonne
void Deceleration_Sur_Distance(float distance, float speed)
{
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  float acceleration;
  float tourDeRoue = Tour_De_Roue_Selon_Distance(distance);
  float encoderDesire = tourDeRoue * 3200;
  const int segment = tourDeRoue * 8;
  float leftEncoder = ENCODER_Read(0);
  while (leftEncoder < encoderDesire)
  {
    leftEncoder = ENCODER_Read(0);
    acceleration = speed - speed * leftEncoder / encoderDesire;
    if (acceleration < 0.07)
      acceleration = 0.07;

    MOTOR_SetSpeed(0, acceleration);
    MOTOR_SetSpeed(1, acceleration);
    MOVEMENTS_RightWheelCorrection(acceleration);
  }
  stop();
}

// en pouce
void avancer_distance(float distance, float speed)
{
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  float tourDeRoue = Tour_De_Roue_Selon_Distance(distance);
  float leftEncoder = ENCODER_Read(0);
  float encoderDesire = tourDeRoue * 3200;
  MOTOR_SetSpeed(0, speed);
  MOTOR_SetSpeed(1, speed);

  while (leftEncoder < encoderDesire)
  {
    leftEncoder = ENCODER_Read(0);
    MOVEMENTS_RightWheelCorrection(speed);
  }
}

void tourner(float angle, bool direction, float speed)
{
  float distance = ROBOT_CIRCONFERENCE * (angle / 360);
  float tourDeRoue = Tour_De_Roue_Selon_Distance(distance * .40);
  float encoder = 0;
  float encoderDesire = tourDeRoue * 3200;
  Acceleration_Sur_Distance_Tourner(distance * .30, direction, speed);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  if (direction)
  {
    encoder = ENCODER_Read(0);
    MOTOR_SetSpeed(0, speed);
    MOTOR_SetSpeed(1, -speed);
    while (encoder < encoderDesire)
    {
      encoder = ENCODER_Read(0);
      MOVEMENTS_RightWheelCorrection(-speed);
    }
  }
  else
  {
    encoder = ENCODER_Read(1);
    MOTOR_SetSpeed(0, -speed);
    MOTOR_SetSpeed(1, speed);
    while (encoder < encoderDesire)
    {
      encoder = ENCODER_Read(1);
      MOVEMENTS_RightWheelCorrection(speed);
    }
  }
  Decceleration_Sur_Distance_Tourner(distance * .30, direction, speed);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
}

void Acceleration_Sur_Distance_Tourner(float distance, float direction, float speed)
{
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  float acceleration;
  float tourDeRoue = Tour_De_Roue_Selon_Distance(distance);
  float encoderDesire = tourDeRoue * 3200;
  float encoder = 0;
  if (direction)
  {
    while (encoder < encoderDesire)
    {
      encoder = ENCODER_Read(0);
      acceleration = speed * encoder / encoderDesire;
      if (acceleration < 0.07)
        acceleration = 0.07;
      MOTOR_SetSpeed(0, acceleration);
      MOTOR_SetSpeed(1, -acceleration);
      MOVEMENTS_RightWheelCorrection(-acceleration);
    }
  }
  else
  {
    while (encoder < encoderDesire)
    {
      encoder = abs(ENCODER_Read(0));
      acceleration = speed * encoder / encoderDesire;
      if (acceleration < 0.07)
        acceleration = 0.07;
      MOTOR_SetSpeed(0, -acceleration);
      MOTOR_SetSpeed(1, acceleration);
      MOVEMENTS_RightWheelCorrection(acceleration);
    }
  }
}

void Decceleration_Sur_Distance_Tourner(float distance, float direction, float speed)
{
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  float acceleration;
  float tourDeRoue = Tour_De_Roue_Selon_Distance(distance);
  float encoderDesire = tourDeRoue * 3200;
  const int segment = tourDeRoue * 8;
  float encoder = 0;
  if (direction)
  {
    while (encoder < encoderDesire)
    {
      encoder = ENCODER_Read(0);
      acceleration = speed - speed * encoder / encoderDesire;
      if (acceleration < 0.15)
        acceleration = 0.15;
      MOTOR_SetSpeed(0, acceleration);
      MOTOR_SetSpeed(1, -acceleration);
      MOVEMENTS_RightWheelCorrection(-acceleration);
    }
  }
  else
  {
    while (encoder < encoderDesire)
    {
      encoder = abs(ENCODER_Read(0));
      acceleration = speed - speed * encoder / encoderDesire;
      if (acceleration < 0.15)
        acceleration = 0.15;
      MOTOR_SetSpeed(0, -acceleration);
      MOTOR_SetSpeed(1, acceleration);
      MOVEMENTS_RightWheelCorrection(acceleration);
    }
  }
  stop();
}

void stop()
{
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
}