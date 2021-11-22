#include <LibRobus.h>
#include <math.h>
#include "CapteurCouleur.h"

const int redLED = 39;
const int yellowLED = 41;
const int greenLED = 43;
const int blueLED = 45;
const int greenButton = 45;
const int redButton = 49;

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

bool isInChamber = true;
int chamberNumber = 0;
int timerRFID = 0;
uint16_t audioTrack = 0;
int RFID = -1;
int buttonBuffer = -2;



struct Drugs
{
  short red;
  short blue;
  short black;
  short yellow;
};

struct Patient
{
  char first_name[50];
  char last_name[50];
  short room_number;
  char rfid_code[50];
  struct Drugs daily_drugs;
  bool daily_drugs_confirmation;
};
struct Patient Tremblay;
struct Patient Gagnon;
struct Patient Cote;
struct Patient Roy;
struct Patient currentPatient;

void setup()
{
  BoardInit();
  ColorCapteurBegin();
  SERVO_Enable(0);
  SERVO_SetAngle(0, 115);
  delay(100);
  pinMode(yellowLED, OUTPUT);
  digitalWrite(yellowLED, LOW);
  pinMode(blueLED, OUTPUT);
  digitalWrite(blueLED, LOW);
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, LOW);
  data_initialisation();
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(redButton, INPUT_PULLUP);
}

void loop()
{
  if (isInChamber == true)
  {
    //AUDIO_PlayBlocking(0); // Allo je suis bla bla bla avez besoin d'aide
    //AUDIO_PlayBlocking(9); // si vous répondez pas dans 10 sec infirmière
    Serial.println("Fini dire 10 sec");
    buttonBuffer = buttonResponse();
    if (buttonBuffer == 2 || buttonBuffer == -1) // à changer parce qu'au sinon le client peut pas cliqué tant que pas fini parler
        {
          Serial.println("Besoin d'urgence");
          //Allumé LED caller infirmière
        }

        Serial.println("pas prob");
        
    delay(3000);
      
    //AUDIO_PlayBlocking(10); // Veuillez scanner votre carte RFID
    Serial.println("scanner RFID");
    //on met un délais pour savoir si le client SCAN sinon on alerte les infirmière
    while (RFID == -1 && timerRFID <= 100)
    {
      delay(50);
      /*
        scan RFID Lire identifie Patient
      */
      currentPatient = Tremblay; // todo combiner le rfid
      timerRFID += 50;
    }

    if (timerRFID >= 10000)
    {
      //Allumer LED (appeller infirmière)
    }
    timerRFID = 0;

    //chamberNumber est la chambre dans laquelle le robot est
    if (chamberNumber == 0)
    {

      if (chamberNumber == currentPatient.room_number)
      {
        //AUDIO_PlayBlocking(1); // Bonjour Mr Tremblay
        Serial.println("1 audio temblay");
        delay(3000);
      }
      else
      {
        //allumé LED
      }
    }
    else if (chamberNumber == 1)
    {
      if (chamberNumber == currentPatient.room_number)
      {
        //AUDIO_PlayBlocking(2); // Bonjour Mr Gagnon
      }
      else
      {
        //allumé LED
      }
    }
    else if (chamberNumber == 2)
    {
      if (chamberNumber == currentPatient.room_number)
      {
        //AUDIO_PlayBlocking(3); // Bonjour Mr Roy
      }
      else
      {
        //allumé LED
      }
    }
    else if (chamberNumber == 3)
    {
      if (chamberNumber == currentPatient.room_number)
      {
       // AUDIO_PlayBlocking(4); // Bonjour Mr Côté
      }
      else
      {
        //allumé LED
      }
    }

    //AUDIO_PlayBlocking(1); // Médication quotidienne ?
    if (buttonResponse() == 2 && currentPatient.daily_drugs_confirmation == false)
        {
          Serial.println("3 audio distri pillule");
          delay(3000);
          //distribue pillule
          //AUDIO_PlayBlocking(2); //Voici vos pillules ?
        }else
        {
          Serial.println("3 audio pas distri pillule");
          delay(3000);
          //Allumé LED (infirmière)
        }

    //AUDIO_PlayBlocking(8); //Aimeriez-vous voir une infirmère pour tout autre raison non-urgente
    buttonBuffer = buttonResponse();
    if (buttonBuffer == 1 || buttonBuffer == -1)
        {
          Serial.println("4 audio aide medi pas importante");
          delay(3000);
          //Allumé LED (infirmière)
        }

    //AUDIO_PlayBlocking(8); //Merci de votre compréhension, bye bye
    Serial.println("finito pepito");
    isInChamber = false;
    chamberNumber += 1;
    buttonBuffer = -2; 
  }

  // while (!(LineFollowerLeftDone && LineFollowerRightDone))
  // {
  //   AjustementDirection();
  //   MesureSuiveur();
  //   MesureSonar();
  //   TimerUpdate();
  //   Serial.println(StateQuilleTombee);
  // }
  // CouleurSequence();
  // StateQuilleTombee = 0;
  // LineFollowerLeftDone = false;
  // LineFollowerRightDone = false;
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
  MOVEMENTS_Forward(9.8, 0.4);
  CAGE_Close();

  if (couleur == BLUE)
  {
    // Bleu
    digitalWrite(blueLED, HIGH);
    MOVEMENTS_Turn(0, 90, 0.4);
    MOVEMENTS_Forward(16, 0.4);
    MOVEMENTS_Turn(1, 90, 0.4);
    MOVEMENTS_Forward(90, 0.8);
    CAGE_Open();
    delay(1000);
    MOTOR_SetSpeed(0, -.4);
    MOTOR_SetSpeed(1, -.4);
    delay(1000);
    MOTOR_SetSpeed(0, 0);
    MOTOR_SetSpeed(1, 0);
    CAGE_Close();
    digitalWrite(blueLED, LOW);
    MOVEMENTS_Turn(1, 180, 0.4);
    MOVEMENTS_Forward(100, .8);
    MOVEMENTS_Turn(1, 160, 0.4);
    MOTOR_SetSpeed(0, -.3);
    MOTOR_SetSpeed(1, -.2);
  }
  else if (couleur == RED)
  {
    // Rouge
    digitalWrite(redLED, HIGH);
    MOVEMENTS_Forward(90, 0.8);
    CAGE_Open();
    delay(1000);
    MOTOR_SetSpeed(0, -.4);
    MOTOR_SetSpeed(1, -.4);
    delay(1000);
    MOTOR_SetSpeed(0, 0);
    MOTOR_SetSpeed(1, 0);
    CAGE_Close();
    digitalWrite(redLED, LOW);
    MOVEMENTS_Turn(1, 180, 0.4);
    MOVEMENTS_Forward(123, .8);
    MOVEMENTS_Turn(1, 25, 0.3);
    MOTOR_SetSpeed(0, -.2);
    MOTOR_SetSpeed(1, -.3);
  }
  else
  {
    digitalWrite(yellowLED, HIGH);
    // Jaune
    MOVEMENTS_Turn(1, 90, 0.4);
    MOVEMENTS_Forward(19, 0.4);
    MOVEMENTS_Turn(0, 90, 0.4);
    MOVEMENTS_Forward(90, 0.8);
    CAGE_Open();
    delay(1000);
    MOTOR_SetSpeed(0, -.4);
    MOTOR_SetSpeed(1, -.4);
    delay(1000);
    MOTOR_SetSpeed(0, 0);
    MOTOR_SetSpeed(1, 0);
    CAGE_Close();
    digitalWrite(yellowLED, LOW);
    MOVEMENTS_Turn(1, 180, 0.4);
    MOVEMENTS_Forward(120, .8);
    MOVEMENTS_Turn(1, 90, 0.4);
    MOTOR_SetSpeed(0, .25);
    MOTOR_SetSpeed(1, -.2);
  }
}

void CAGE_Open()
{
  SERVO_SetAngle(0, 30);
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
      digitalWrite(blueLED, HIGH);
      digitalWrite(redLED, HIGH);
      digitalWrite(yellowLED, HIGH);
      float Distance = SONAR_GetRange(0);
      if (Distance < 35)
      {
        digitalWrite(blueLED, LOW);
        digitalWrite(redLED, LOW);
        digitalWrite(yellowLED, LOW);
        digitalWrite(greenLED, HIGH);
        delay(200);
        MOTOR_SetSpeed(0, 0);
        MOTOR_SetSpeed(1, 0);
        MOVEMENTS_Turn(0, 110, 0.4);
        MOVEMENTS_Forward(Distance - 5, 0.5);
        MOVEMENTS_Turn(1, 180, 0.4);
        MOVEMENTS_Forward(Distance - 5, 0.5);
        MOTOR_SetSpeed(0, -0.15);
        MOTOR_SetSpeed(1, 0.15);
        digitalWrite(greenLED, LOW);
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
  if (mesureGauche < 670)
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
    delay(65);
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

void data_initialisation()
{
  Tremblay.last_name[0] = 'T';
  Tremblay.last_name[1] = 'r';
  Tremblay.last_name[2] = 'e';
  Tremblay.last_name[3] = 'm';
  Tremblay.last_name[4] = 'b';
  Tremblay.last_name[5] = 'l';
  Tremblay.last_name[6] = 'a';
  Tremblay.last_name[7] = 'y';
  Tremblay.last_name[8] = '\0';

  Tremblay.first_name[0] = 'L';
  Tremblay.first_name[1] = 'i';
  Tremblay.first_name[2] = 'a';
  Tremblay.first_name[3] = 'm';
  Tremblay.first_name[4] = '\0';

  Tremblay.room_number = 0;

  Tremblay.rfid_code[0] = '1';
  Tremblay.rfid_code[1] = '\0';

  Tremblay.daily_drugs.red = 1;
  Tremblay.daily_drugs.blue = 2;
  Tremblay.daily_drugs.black = 3;

  Tremblay.daily_drugs_confirmation = false;

  Gagnon.last_name[0] = 'G';
  Gagnon.last_name[1] = 'a';
  Gagnon.last_name[2] = 'g';
  Gagnon.last_name[3] = 'n';
  Gagnon.last_name[4] = 'o';
  Gagnon.last_name[5] = 'n';
  Gagnon.last_name[6] = '\0';

  Gagnon.first_name[0] = 'T';
  Gagnon.first_name[1] = 'h';
  Gagnon.first_name[2] = 'o';
  Gagnon.first_name[2] = 'm';
  Gagnon.first_name[3] = 'a';
  Gagnon.first_name[4] = 's';
  Gagnon.first_name[5] = '\0';

  Gagnon.room_number = 1;

  Gagnon.rfid_code[0] = '2';
  Gagnon.rfid_code[1] = '\0';

  Gagnon.daily_drugs.red = 2;
  Gagnon.daily_drugs.blue = 2;
  Gagnon.daily_drugs.black = 2;

  Gagnon.daily_drugs_confirmation = false;

  Roy.last_name[0] = 'R';
  Roy.last_name[1] = 'o';
  Roy.last_name[2] = 'y';
  Roy.last_name[3] = '\0';

  Roy.first_name[0] = 'O';
  Roy.first_name[1] = 'l';
  Roy.first_name[2] = 'i';
  Roy.first_name[2] = 'v';
  Roy.first_name[3] = 'i';
  Roy.first_name[4] = 'a';
  Roy.first_name[5] = '\0';

  Roy.room_number = 2;

  Roy.rfid_code[0] = '3';
  Roy.rfid_code[1] = '\0';

  Roy.daily_drugs.red = 3;
  Roy.daily_drugs.blue = 2;
  Roy.daily_drugs.black = 1;

  Roy.daily_drugs_confirmation = false;

  Cote.last_name[0] = 'C';
  Cote.last_name[1] = 'o';
  Cote.last_name[2] = 't';
  Cote.last_name[3] = 't';
  Cote.last_name[4] = '\0';

  Cote.first_name[0] = 'A';
  Cote.first_name[1] = 'l';
  Cote.first_name[2] = 'i';
  Cote.first_name[2] = 'c';
  Cote.first_name[3] = 'e';
  Cote.first_name[4] = '\0';

  Cote.room_number = 3;

  Cote.rfid_code[0] = '4';
  Cote.rfid_code[1] = '\0';

  Cote.daily_drugs.red = 3;
  Cote.daily_drugs.blue = 3;
  Cote.daily_drugs.yellow = 4;

  Cote.daily_drugs_confirmation = false;
}

int witchButton()
{

  if (digitalRead(greenButton) == LOW)
  {
    return 1;
  }
  else if (digitalRead(redButton) == LOW)
  {
    return 2;
  }
  return -1;
}

int buttonResponse(){
  int buttonState = -1;
  int timerButton = 0;
   while (buttonState == -1 && timerButton <= 10000)
    {
      delay(50);
      buttonState = witchButton();
      timerButton += 50;
    }

    return buttonState;
}