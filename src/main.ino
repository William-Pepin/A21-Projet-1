#include <LibRobus.h>
#include <math.h>

bool StateSignalSonore = false;
bool StateQuilleTombee = false;
bool StateCrossroad = false;
int StateCouleur = 0; //noir=0, blanc=1, rouge=2, etc.
float AnglePIDGauche = 20;
float AnglePIDDroit = 340;
int StateDirection = 0;
int Timer = 0;
int shouldTurnRight = 0;
int shouldTurnLeft = 0;

bool isQuilleTomber = 0; // A moduler avec la quille

void setup()
{
  BoardInit();
  delay(100);
}

void loop()
{
  //AjustementDirection();
  MesureSuiveur();
  TimerUpdate();
  Serial.println(StateDirection);
}

void MesureSuiveur()
{
  /*
        StateDirection:                V1 (rouge)   |   V2 (jaune)   |   V3 (rouge)   |   V4 (bleu)   |   Mesure Analogique:
        1                                                                                                    8
        2                                    X                                                               540
        3                                                    X                                               273
        4                                                                    X                               145
        5                                                                                    X               72
        6                                    X               X                                               807
        7                                                    X               X                               410
        8                                                                    X               X               216
        9                                    X                               X                               677
        10                                                   X                               X               341
        11                                   X                                               X               606
        12                                   X               X               X                               942
        13                                                   X               X               X               477
        14                                   X                               X               X               742
        15                                   X               X                               X               872
        16                                   X               X               X               X               1008

        Mesures triées: 8; 72; 145; 216; 273; 341; 410; 477; 540; 606; 677; 742; 807; 872; 942; 1008
        */
  int mesure = analogRead(A6);
  int mesureGauche = analogRead(A7);
  int mesureDroite = analogRead(A5);

  if (mesureDroite < 300)
    shouldTurnRight++;
  else if (mesureGauche < 600)
    shouldTurnLeft++;
  else
  {
    shouldTurnRight = 0;
    shouldTurnLeft = 0;
  }

  if (shouldTurnRight > 4 && isQuilleTomber)
    StateDirection = 17;
  else if (shouldTurnLeft > 4 && isQuilleTomber)
    StateDirection = 18;
  else if (mesure < 40)
  {
    StateDirection = 1;
  }
  else if (mesure >= 40 && mesure < 105)
  {
    StateDirection = 5;
  }
  else if (mesure >= 105 && mesure < 180)
  {
    StateDirection = 4;
  }
  else if (mesure >= 180 && mesure < 245)
  {
    StateDirection = 8;
  }
  else if (mesure >= 245 && mesure < 300)
  {
    StateDirection = 3;
  }
  else if (mesure >= 300 && mesure < 375)
  {
    StateDirection = 10;
  }
  else if (mesure >= 375 && mesure < 440)
  {
    StateDirection = 7;
  }
  else if (mesure >= 440 && mesure < 500)
  {
    StateDirection = 13;
  }
  else if (mesure >= 500 && mesure < 575)
  {
    StateDirection = 2;
  }
  else if (mesure >= 575 && mesure < 640)
  {
    StateDirection = 11;
  }
  else if (mesure >= 640 && mesure < 700)
  {
    StateDirection = 9;
  }
  else if (mesure >= 700 && mesure < 775)
  {
    StateDirection = 15;
  }
  else if (mesure >= 775 && mesure < 840)
  {
    StateDirection = 6;
  }
  else if (mesure >= 840 && mesure < 900)
  {
    StateDirection = 14;
  }
  else if (mesure >= 900 && mesure < 975)
  {
    StateDirection = 12;
  }
  else if (mesure >= 975)
  {
    StateDirection = 16;
  }
}

void AjustementDirection()
{
  if (Timer == 10)
  {
    if (StateDirection == 0)
    {
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, -0.4);
    }

    if (StateDirection == 3)
    {
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, -0.4);
    }
    if (StateDirection == 2)
    {
      MOTOR_SetSpeed(0, -0.1);
      MOTOR_SetSpeed(1, -0.4);
    }
    if (StateDirection == 4)
    {
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, -0.1);
    }
    if (StateDirection == 17)
    {
      MOTOR_SetSpeed(0, 0);
      MOTOR_SetSpeed(1, -0.4);
      delay(500);
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, -0.4);
    }
    if (StateDirection == 18)
    {
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, 0);
      delay(500);
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, -0.4);
    }
  }
}

void TimerUpdate()
{
  if (Timer == 10)
  {
    Timer = 0;
  }
  delay(5);
  Timer += 5;
}