#include <LibRobus.h>
#include <math.h>
#include "CapteurCouleur.h"

int StateSignalSonore = 0;
int StateQuilleTombee = 0;
int StateCouleur = 0; //noir=0, blanc=1, rouge=2, etc.
int StateDirection = 0;
int Timer = 0;
int shouldTurnRight = 0;
int shouldTurnLeft = 0;
bool LineFollowerRightDone = false;
bool LineFollowerLeftDone = false;
const double WHEEL_CIRCONFERENCE = 2 * PI * 1.5;
const double ROBOT_CIRCONFERENCE_RIGHT = 2 * PI * 3.75;
const double ROBOT_CIRCONFERENCE_LEFT = 2 * PI * 3.85;
const double MIN_SPEED = 0.07;
uint8_t couleur = 0;
;

void setup()
{
  BoardInit();
  ColorCapteurBegin();
  SERVO_Enable(0);
  SERVO_SetAngle(0, 115);
  delay(100);
}

void loop()
{
  while (!(LineFollowerLeftDone && LineFollowerRightDone))
  {
    AjustementDirection();
    MesureSuiveur();
    MesureSonar();
    TimerUpdate();
    Serial.println(StateQuilleTombee);
  }
  CouleurSequence();
  delay(1000);
  SERVO_Disable(0);
  StateQuilleTombee = 0;
  LineFollowerLeftDone = false;
  LineFollowerRightDone = false;
}
void TesterValeur()
{
  int mesureGauche = analogRead(A7);
  int mesureDroite = analogRead(A5);
  Serial.print("Gauche : ");
  Serial.println(mesureGauche);
  Serial.print("Droite : ");
  Serial.println(mesureDroite);
  delay(1000);
}

void TesterCouleur()
{
  uint8_t couleur = GetCouleur();
  Serial.println(couleur);
  bool isBlue = couleur == BLUE;
  bool isRed = couleur == RED;
  bool isYellow = couleur == YELLOW;
  Serial.println(isBlue);
  Serial.println(isRed);
  Serial.println(isYellow);
  delay(1000);
}

void CouleurSequence()
{
  MOVEMENTS_Turn(0, 180, 0.4);
  MOVEMENTS_Forward(10, 0.4);
  uint8_t couleur = GetCouleur();
  Serial.println(couleur);

  CAGE_Open();
  MOVEMENTS_Forward(10.8, 0.4);
  CAGE_Close();

  if (couleur == BLUE)
  {
    // Bleu
    MOVEMENTS_Turn(0, 90, 0.4);
    MOVEMENTS_Forward(16, 0.4);
    MOVEMENTS_Turn(1, 90, 0.4);
    MOVEMENTS_Forward(90, 0.8);
    CAGE_Open();
  }
  else if (couleur == RED)
  {
    // Rouge
    MOVEMENTS_Forward(90, 0.8);
    CAGE_Open();
  }
  else
  {
    // Jaune
    MOVEMENTS_Turn(1, 90, 0.4);
    MOVEMENTS_Forward(20, 0.4);
    MOVEMENTS_Turn(0, 90, 0.4);
    MOVEMENTS_Forward(90, 0.8);
    CAGE_Open();
  }
  MOTOR_SetSpeed(0, -.4);
  MOTOR_SetSpeed(1, -.4);
  delay(1000);
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
  CAGE_Close();
  MOVEMENTS_Turn(1, 180, 0.4);
  MOVEMENTS_Forward(120, .8);
}

void CAGE_Open()
{
  SERVO_SetAngle(0, 15);
}

void CAGE_Close()
{
  SERVO_SetAngle(0, 115);
}

void MesureSonar()
{
  if (Timer == 100)
  {
    if (StateSignalSonore > 10)
    {
      float Distance = SONAR_GetRange(0);
      if (Distance < 35)
      {
        delay(200);
        MOTOR_SetSpeed(0, 0);
        MOTOR_SetSpeed(1, 0);
        MOVEMENTS_Turn(0, 100, 0.5);
        MOVEMENTS_Forward(Distance - 5, 0.7);
        MOVEMENTS_Turn(1, 180, 0.5);
        MOVEMENTS_Forward(Distance - 5, 0.7);
        MOTOR_SetSpeed(0, -0.15);
        MOTOR_SetSpeed(1, 0.15);
        StateSignalSonore = 0;
        StateQuilleTombee = 1;
        StateDirection = 0;
      }
    }
    else
    {
      StateSignalSonore = 0;
    }
  }
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

  if (mesureDroite < 850)
    shouldTurnRight++;
  else
  {
    shouldTurnRight = 0;
  }
  if (mesureGauche < 650)
    shouldTurnLeft++;
  else
  {
    shouldTurnLeft = 0;
  }

  if (shouldTurnLeft > 4 && StateQuilleTombee == 7)
  {
    MOTOR_SetSpeed(1, 0);
    StateDirection = -1;
    LineFollowerLeftDone = true;
  }

  if (shouldTurnRight > 4 && StateQuilleTombee == 7)
  {
    delay(35);
    MOTOR_SetSpeed(0, 0);
    StateDirection = -1;
    LineFollowerRightDone = true;
  }

  else if (shouldTurnRight > 4 && StateQuilleTombee == 2)
    StateQuilleTombee = 3;
  else if (shouldTurnLeft > 4 && StateQuilleTombee == 2)
    StateQuilleTombee = 4;
  else if (mesure < 40)
  {
    StateDirection = 1;
  }
  else if (mesure >= 40 && mesure < 105)
  {
    StateDirection = 5;
    StateSignalSonore++;
  }
  else if (mesure >= 105 && mesure < 180)
  {
    StateDirection = 4;
  }
  else if (mesure >= 180 && mesure < 245)
  {
    StateDirection = 8;
    StateSignalSonore++;
  }
  else if (mesure >= 245 && mesure < 300)
  {
    StateDirection = 3;
  }
  else if (mesure >= 300 && mesure < 375)
  {
    StateDirection = 10;
    StateSignalSonore++;
  }
  else if (mesure >= 375 && mesure < 440)
  {
    StateDirection = 7;
  }
  else if (mesure >= 440 && mesure < 500)
  {
    StateDirection = 13;
    StateSignalSonore++;
  }
  else if (mesure >= 500 && mesure < 575)
  {
    StateDirection = 2;
  }
  else if (mesure >= 575 && mesure < 640)
  {
    StateDirection = 11;
    StateSignalSonore++;
  }
  else if (mesure >= 640 && mesure < 700)
  {
    StateDirection = 9;
  }
  else if (mesure >= 700 && mesure < 775)
  {
    StateDirection = 15;
    StateSignalSonore++;
  }
  else if (mesure >= 775 && mesure < 840)
  {
    StateDirection = 6;
  }
  else if (mesure >= 840 && mesure < 900)
  {
    StateDirection = 14;
    StateSignalSonore++;
  }
  else if (mesure >= 900 && mesure < 975)
  {
    StateDirection = 12;
  }
  else if (mesure >= 975)
  {
    StateDirection = 16;
    StateSignalSonore++;
  }
}

void AjustementDirection()
{
  if (Timer % 10 == 0)
  {
    if (StateDirection == 0)
    {
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, -0.4);
    }
    else if (StateDirection == 3 && StateQuilleTombee == 1)
    {
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, -0.4);
      StateQuilleTombee = 2;
      shouldTurnLeft = 0;
      shouldTurnRight = 0;
    }
    else if (StateDirection == 3 && StateQuilleTombee == 6)
    {
      MOTOR_SetSpeed(0, -0.2);
      MOTOR_SetSpeed(1, -0.2);
      StateQuilleTombee = 7;
    }
    else if ((StateDirection == 3 || StateDirection == 10) && (StateQuilleTombee == 0 || StateQuilleTombee == 2))
    {
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, -0.4);
    }
    else if ((StateDirection == 2 || StateDirection == 11) && (StateQuilleTombee == 0 || StateQuilleTombee == 2))
    {
      MOTOR_SetSpeed(0, -0.1);
      MOTOR_SetSpeed(1, -0.4);
    }
    else if ((StateDirection == 4 || StateDirection == 8) && (StateQuilleTombee == 0 || StateQuilleTombee == 2))
    {
      MOTOR_SetSpeed(0, -0.4);
      MOTOR_SetSpeed(1, -0.1);
    }
    else if ((StateDirection == 3 || StateDirection == 10) && StateQuilleTombee == 7)
    {
      MOTOR_SetSpeed(0, -0.2);
      MOTOR_SetSpeed(1, -0.2);
    }
    else if ((StateDirection == 2 || StateDirection == 11) && StateQuilleTombee == 7)
    {
      MOTOR_SetSpeed(0, -0.0);
      MOTOR_SetSpeed(1, -0.2);
    }
    else if ((StateDirection == 4 || StateDirection == 8) && StateQuilleTombee == 7)
    {
      MOTOR_SetSpeed(0, -0.2);
      MOTOR_SetSpeed(1, -0.0);
    }
    else if (StateQuilleTombee == 3)
    {
      MOTOR_SetSpeed(0, 0.15);
      MOTOR_SetSpeed(1, -0.15);
      StateQuilleTombee = 6;
      delay(400);
    }
    else if (StateQuilleTombee == 4)
    {
      MOTOR_SetSpeed(0, -0.15);
      MOTOR_SetSpeed(1, 0.15);
      StateQuilleTombee = 6;
      delay(400);
    }
    //else if (StateQuilleTombee == 5)
    //{
    //  MOTOR_SetSpeed(0, 0);
    //  MOTOR_SetSpeed(1, 0);
    //}
  }
}

void TimerUpdate()
{
  if (Timer == 100)
  {
    Timer = 0;
  }
  delay(5);
  Timer += 5;
}

/** Function to accelerate exponentially to a certain distance

@param distance, distance travelled in inches

@param speed, represents direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_Accelerate(double distance, double speed)
{
  double acceleration;

  MOVEMENTS_ResetEncoder();
  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance);
  int32_t currentEncoder = MOVEMENTS_ReadEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadEncoder();
    acceleration = MOVEMENTS_CalculateAcceleration(speed, currentEncoder, targetEncoder);

    MOVEMENTS_Forward(acceleration);
  }
  MOVEMENTS_ResetEncoder();
}

/** Function to deccelerate exponentially to a certain distance

@param distance, distance travelled in inches

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_Deccelerate(double distance, double speed)
{
  double decceleration;

  MOVEMENTS_ResetEncoder();
  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance);
  int32_t currentEncoder = MOVEMENTS_ReadEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadEncoder();
    decceleration = MOVEMENTS_CalculateDecceleration(speed, currentEncoder, targetEncoder);

    MOVEMENTS_Forward(decceleration);
  }
  MOVEMENTS_Stop();
  MOVEMENTS_ResetEncoder();
}

/** Function to go accelerate until reached speed,
 * then deccelerate at the desired distance. 

@param distance, distance travelled in inches

@param speed, represents direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_Forward(double distance, double speed)
{
  MOVEMENTS_Accelerate(distance * .10, speed);

  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance * .70);
  int32_t currentEncoder = MOVEMENTS_ReadEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadEncoder();
    MOVEMENTS_Forward(speed);
  }

  MOVEMENTS_Deccelerate(distance * .20, speed);
}

/** Function to accelerate exponentially to a certain distance while turning

@param direction, 0 left, 1 right

@param distance, distance travelled in inches

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_AccelerateTurn(bool direction, double distance, double speed)
{
  double acceleration;

  MOVEMENTS_ResetEncoder();
  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance);
  int32_t currentEncoder = MOVEMENTS_ReadAbsEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadAbsEncoder();
    acceleration = MOVEMENTS_CalculateAcceleration(speed, currentEncoder, targetEncoder);

    if (direction)
      MOVEMENTS_TurnRight(acceleration);
    else
      MOVEMENTS_TurnLeft(acceleration);
  }
  MOVEMENTS_ResetEncoder();
}

/** Function to deccelerate exponentially to a certain distance while turning

@param direction, 0 left, 1 right

@param distance, distance travelled in inches

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_DeccelerateTurn(bool direction, double distance, double speed)
{
  double decceleration;

  MOVEMENTS_ResetEncoder();
  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance);
  int32_t currentEncoder = MOVEMENTS_ReadAbsEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadAbsEncoder();
    decceleration = MOVEMENTS_CalculateDecceleration(speed, currentEncoder, targetEncoder);

    if (direction)
      MOVEMENTS_TurnRight(decceleration);
    else
      MOVEMENTS_TurnLeft(decceleration);
  }
  MOVEMENTS_Stop();
  MOVEMENTS_ResetEncoder();
}

/** Function to turn smoothly to a certain angle

@param direction, 0 left, 1 right

@param angle, angle in degrees

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_Turn(bool direction, double angle, double speed)
{

  double distance = MOVEMENTS_DistanceForAngle(direction, angle);

  MOVEMENTS_AccelerateTurn(direction, distance * .20, speed);

  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance * .60);
  int32_t currentEncoder = MOVEMENTS_ReadAbsEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadAbsEncoder();
    if (direction)
      MOVEMENTS_TurnRight(speed);
    else
      MOVEMENTS_TurnLeft(speed);
  }
  MOVEMENTS_DeccelerateTurn(direction, distance * .20, speed);
}

/** Function to correct the right wheel based on the left wheel

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

*/
double MOVEMENTS_SyncRightWheel(double speed)
{
  const double factor = 1.05;

  double leftEncoder = abs(ENCODER_Read(0));
  double rightEncoder = abs(ENCODER_Read(1));
  if (leftEncoder == rightEncoder)
    return speed;
  else if (leftEncoder > rightEncoder + 1)
    return speed * factor;
  else if (leftEncoder < rightEncoder + 1)
    return (speed - (speed * (factor - 1)));
}

/** Function to get the number of encoder ticks needed to reach a certain distance

@param distance, distance travelled in inches

*/
double MOVEMENTS_EncoderForDistance(double distance)
{
  return (distance / WHEEL_CIRCONFERENCE) * 3200;
}

/** Function to get the number of distance to travel to reach a certain angle

@param direction, 0 left, 1 right

@param angle, angle in degrees

*/
double MOVEMENTS_DistanceForAngle(bool direction, double angle)
{

  if (direction)
    return ROBOT_CIRCONFERENCE_RIGHT * (angle / 360);
  return ROBOT_CIRCONFERENCE_LEFT * (angle / 360);
}

/** Function to calculate the acceleration based on the current encoder tick and the target encoder tick

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

@param currentEncoder, encoder tick at the instant

@param targetEncoder, encoder tick to reach

*/
double MOVEMENTS_CalculateAcceleration(double speed, int32_t currentEncoder, int32_t targetEncoder)
{
  double acceleration = speed * currentEncoder / targetEncoder;

  if (acceleration < MIN_SPEED)
    acceleration = MIN_SPEED;

  return acceleration;
}

/** Function to calculate the decceleration based on the current encoder tick and the target encoder tick

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

@param currentEncoder, encoder tick at the instant

@param targetEncoder, encoder tick to reach

*/
double MOVEMENTS_CalculateDecceleration(double speed, int32_t currentEncoder, int32_t targetEncoder)
{
  double decceleration = speed - speed * currentEncoder / targetEncoder;

  if (decceleration < MIN_SPEED)
    decceleration = MIN_SPEED;

  return decceleration;
}

/** Function to read the main encoder */
int32_t MOVEMENTS_ReadEncoder()
{
  return ENCODER_Read(0);
}

/** Function to read the absolute value of the main encoder */
int32_t MOVEMENTS_ReadAbsEncoder()
{
  return abs(ENCODER_Read(0));
}

/** Function to read the main encoder and the slave encoder */
int32_t MOVEMENTS_ResetEncoder()
{
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
}

/** Function to turn both wheel at the same time

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

*/
void MOVEMENTS_Forward(double speed)
{
  MOTOR_SetSpeed(0, speed);
  MOTOR_SetSpeed(1, MOVEMENTS_SyncRightWheel(speed));
}

/** Function to turn left using both wheel

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

*/
void MOVEMENTS_TurnLeft(double speed)
{
  MOTOR_SetSpeed(0, -speed);
  MOTOR_SetSpeed(1, MOVEMENTS_SyncRightWheel(speed));
}

/** Function to turn right using both wheel

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_TurnRight(double speed)
{
  MOTOR_SetSpeed(0, speed);
  MOTOR_SetSpeed(1, -MOVEMENTS_SyncRightWheel(speed));
}

/** Function to stop both wheel at the same time */
void MOVEMENTS_Stop()
{
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
}

bool isLineFollowingDone()
{
  return LineFollowerRightDone && LineFollowerLeftDone;
}

uint8_t GetCouleur()
{
  struct RGB Couleurs;
  uint8_t Lecture = LectureCouleur(&Couleurs);
  Serial.print(Lecture);
  return Lecture;
}
