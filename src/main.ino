/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Essentielle pour utiliser RobUS
#include <test.h>

/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
// -> defines...
// L'ensemble des fonctions y ont acces
// Offset
const float pi = 3.1415926535;
const float WHEEL_CIRCONFERENCE = 2 * pi * 1.5;
const float ROBOT_CIRCONFERENCE = 2 * pi * 3.6;

/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */
void maFonction()
{
  // code
}

/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */
// -> Se fait appeler au debut du programme
// -> Se fait appeler seulement un fois
// -> Generalement on y initilise les varibbles globales

void setup()
{
  BoardInit();
  delay(100);
}

/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

void loop()
{
  Acceleration_Sur_Distance(10, .7);
  avancer_distance(80, .7);
  Deceleration_Sur_Distance(10, .7);
}

void correction(float speed)
{
  float leftEncoder = ENCODER_Read(0);
  float rightEncoder = ENCODER_Read(1);
  if (leftEncoder == rightEncoder)
  {
    // Si la roue droite tourne plus vite
    MOTOR_SetSpeed(1, speed);
  }
  if (leftEncoder > rightEncoder + 1)
  {
    // Si la roue gauche tourne plus vite
    MOTOR_SetSpeed(1, speed * 1.123);
  }
  if (leftEncoder < rightEncoder + 1)
  {
    // Si la roue gauche tourne plus vite
    MOTOR_SetSpeed(1, speed - (speed * 0.123));
  }
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
  while (leftEncoder <= encoderDesire)
  {
    leftEncoder = ENCODER_Read(0);
    acceleration = speed * leftEncoder / encoderDesire;
    if (acceleration < 0.25)
      acceleration = 0.25;
    MOTOR_SetSpeed(0, acceleration);
    MOTOR_SetSpeed(1, acceleration);
    correction(acceleration);
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
  while (leftEncoder <= encoderDesire)
  {
    leftEncoder = ENCODER_Read(0);
    acceleration = speed - speed * leftEncoder / encoderDesire;

    Serial.println(acceleration);
    MOTOR_SetSpeed(0, acceleration);
    MOTOR_SetSpeed(1, acceleration);
    correction(acceleration);
  }
}

// en pouce
void avancer_distance(float distance, float speed)
{
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  float tourDeRoue = Tour_De_Roue_Selon_Distance(distance);
  float leftEncoder = ENCODER_Read(0);
  MOTOR_SetSpeed(0, speed);
  MOTOR_SetSpeed(1, speed);

  while ((leftEncoder / 3200) < tourDeRoue)
  {
    leftEncoder = ENCODER_Read(0);
    correction(speed);
  }
}
void accelerate(float speed)
{
  float acceleration;
  const int segment = 100;
  for (size_t i = 0; i < segment; i++)
  {
    acceleration = speed * i / segment;
    MOTOR_SetSpeed(0, acceleration);
    MOTOR_SetSpeed(1, acceleration);
    delay(10);
  }
}

void tourner(float angle, bool direction, float speed)
{
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  float distance = ROBOT_CIRCONFERENCE * (angle / 360);
  float tourDeRoue = distance / WHEEL_CIRCONFERENCE;
  float encoder = 0;
  if (direction)
  {
    Serial.println("droite");
    MOTOR_SetSpeed(0, -speed / 2);
    delay(10);
    MOTOR_SetSpeed(1, speed / 2);
    while ((encoder / 3200) < tourDeRoue)
    {
      encoder = ENCODER_Read(1);
      Serial.println(encoder);
    }
  }
  else
  {
    Serial.println("gauche");
    MOTOR_SetSpeed(0, speed / 2);
    delay(10);
    MOTOR_SetSpeed(1, -(speed / 2));
    while ((encoder / 3200) < tourDeRoue)
    {
      encoder = ENCODER_Read(0);
      Serial.println(encoder);
    }
  }

  stop();
  ENCODER_Reset(0);
}

void stop()
{
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
}