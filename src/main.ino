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

/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
// -> defines...
// L'ensemble des fonctions y ont acces
// Offset
const float pi = 3.1415926535;
const float WHEEL_CIRCONFERENCE = 2 * pi * 1.5;
const float SPEED = .6;
const bool REFERENCE_WHEEEL = 0;
const bool SLAVE_WHEEL = 1;
const float A_SPEED_RIGHT_WHEELOFFSET = 0;
const float B_SPEED_RIGHT_WHEELOFFSET = 0.0267;
const float B_SPEED_RIGHT_WHEELOFFSET_BASED_ON_SPEED = 0.0445 * SPEED;
char RobotId;

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
  RobotId = 'B';
  delay(100);
}

/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

void loop()
{
  avancer_distance(88);
  tourner(90, 1);
  avancer_distance(22);
  tourner(90, 0);
  avancer_distance(22);
  tourner(90, 0);
  avancer_distance(22);
  tourner(90, 1);
  exit(0);
}

// en pouce
void avancer_distance(float distance)
{
  float tourDeRoue = distance / WHEEL_CIRCONFERENCE;
  avancer();
  float encoder = ENCODER_Read(0);
  while ((encoder / 3200) < tourDeRoue)
  {
    encoder = ENCODER_Read(0);
    Serial.println(encoder);
  }
  stop();
  ENCODER_Reset(0);
}

void tourner(float angle, bool direction)
{
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  float distance = ROBOT_CIRCONFERENCE * (angle / 360);
  float tourDeRoue = distance / WHEEL_CIRCONFERENCE;
  float encoder = 0;
  if (direction)
  {
    Serial.println("droite");
    MOTOR_SetSpeed(0, -SPEED / 2);
    delay(10);
    MOTOR_SetSpeed(1, SPEED / 2);
    while ((encoder / 3200) < tourDeRoue)
    {
      encoder = ENCODER_Read(1);
      Serial.println(encoder);
    }
  }
  else
  {
    Serial.println("gauche");
    MOTOR_SetSpeed(0, SPEED / 2);
    delay(10);
    MOTOR_SetSpeed(1, -(SPEED / 2));
    while ((encoder / 3200) < tourDeRoue)
    {
      encoder = ENCODER_Read(0);
      Serial.println(encoder);
    }
  }

  stop();
  ENCODER_Reset(0);
}

void avancer()
{
  if (RobotId == 'B')
  {
    MOTOR_SetSpeed(0, SPEED);
    delay(10);
    MOTOR_SetSpeed(1, SPEED + B_SPEED_RIGHT_WHEELOFFSET_BASED_ON_SPEED);
  }
}

void stop()
{
  for (size_t i = 5; i > 0; i--)
  {
    float speed = i / 10;
    MOTOR_SetSpeed(0, speed);
    MOTOR_SetSpeed(1, speed + B_SPEED_RIGHT_WHEELOFFSET);
    delay(100);
  }
}