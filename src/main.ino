#include <LibRobus.h>
#include <math.h>
#include <string.h>
#include <Wire.h>

#include "capteurCouleur.h"
#include "Movements.h"
#include "Tests.h"
#include "Distribution.h"
#include "Patient.h"
#include "capteurSonar.h"
#include "../lib/ds3231/ds3231.h"
#include "ServoMoteur.h"

#define COULEUR_CHAMBRE (0x02) // TODO Modifier pour la bonne couleur
#define COULEUR_POSTE (0x05)   // TODO Modifier pour la bonne couleur

const int redButtonLED = 39;
const int yellowLED = 41;
const int blueLED = 43;
const int greenButtonLED = 47;
const int greenButton = 45;
const int redButton = 49;
const int NOMBRE_DE_PATIENTS = 3;

const uint16_t a_Introduction = 11;
const uint16_t a_DixSecondes = 12;
const uint16_t a_RFID = 2;
const uint16_t a_Tremblay = 3;
const uint16_t a_Gagnon = 4;
const uint16_t a_Roy = 5;
const uint16_t a_Cote = 6;
const uint16_t a_Medication = 7;
const uint16_t a_Indication = 8;
const uint16_t a_Demande = 9;
const uint16_t a_Outroduction = 10;

int StateStep = 0;
//0: Cherche infirmière, 1: Ronde normale: Entre dans chambre, 2: Quiz, 3: Retour infirmière
int StepSuiveur = 0;
//0: Suivi de ligne blanche, 1: Arrivée à une pièce, 2: Réentrée sur la ligne, 3: Obstruction, 4: Retour sur ligne, 5: Attente
int Timer = 0;
int StateDirection = 0;
int shouldTurnLeft = 0;
int shouldTurnRight = 0;
int StateAvertissement = 0;
int StateAvertissementOld = 0;
int StateRoom = 0;
struct Patient ListePatient[5];
int PatientChoisi = -1;
//0: Infirmière, 1: Tremblay, 2: Gagnon, 3: Roy
int help = 0;

struct ts t; 


void setup()
{
    BoardInit();
    AudioInit();
    ColorCapteurBegin();
    SERVO_Enable(0);
    SERVO_SetAngle(0, 115);

    pinMode(yellowLED, OUTPUT);
    digitalWrite(yellowLED, LOW);

    pinMode(blueLED, OUTPUT);
    digitalWrite(blueLED, LOW);

    pinMode(yellowLED, OUTPUT);
    digitalWrite(yellowLED, LOW);

    pinMode(greenButtonLED, OUTPUT);
    digitalWrite(greenButtonLED, LOW);

    pinMode(redButtonLED, OUTPUT);
    digitalWrite(redButtonLED, LOW);

    pinMode(greenButton, INPUT_PULLUP);
    pinMode(redButton, INPUT_PULLUP);

    Serial.begin(9600);
    Wire.begin();
    DS3231_init(DS3231_CONTROL_INTCN);

    Serial1.begin(9600);

    data_initialisation(ListePatient);
}


void loop()
{
    MesureSuiveur();
    MesureSonar();
    Interface();
    AjustementDirection();
    TimerUpdate();
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
    shouldTurnRight = 0;

    if (mesureGauche < 670)
    shouldTurnLeft++;
    else
    shouldTurnLeft = 0;

    if (shouldTurnLeft > 4 && shouldTurnRight > 4 && StepSuiveur == 0)
    {
        StepSuiveur = 1;
        shouldTurnLeft = 0;
        shouldTurnRight = 0;
    }

    if (mesure < 40)
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
        if (StepSuiveur == 0)
        {
            if (StateDirection == 0)
            {
                MOTOR_SetSpeed(0, -0.2);
                MOTOR_SetSpeed(1, -0.2);
            }
            else if (StateDirection == 3 || StateDirection == 10)
            {
                MOTOR_SetSpeed(0, -0.2);
                MOTOR_SetSpeed(1, -0.2);
            }
            else if (StateDirection == 2 || StateDirection == 11)
            {
                MOTOR_SetSpeed(0, -0.02);
                MOTOR_SetSpeed(1, -0.2);
            }
            else if (StateDirection == 4 || StateDirection == 8)
            {
                MOTOR_SetSpeed(0, -0.2);
                MOTOR_SetSpeed(1, -0.02);
            }
        }
        else if (StepSuiveur == 1 && StateStep == 0)
        {
            shouldTurnLeft = 0;
            shouldTurnRight = 0;
            MOTOR_SetSpeed(0, 0);
            MOTOR_SetSpeed(1, 0);
            MOVEMENTS_Turn(1, 165, 0.3);
            MOVEMENTS_Forward(13, 0.3);
            MOVEMENTS_Turn(0, 65, 0.3);
            uint8_t couleur = GetCouleur();
            if(couleur == COULEUR_POSTE)
                StateStep = 1;
            MOVEMENTS_Turn(0, 75, 0.3);
            StepSuiveur = 2;
            MOTOR_SetSpeed(0, -0.1);
            MOTOR_SetSpeed(1, 0.1);
        }
        else if (StepSuiveur == 2 && StateDirection == 3)
        {
            MOTOR_SetSpeed(0, -0.2);
            MOTOR_SetSpeed(1, -0.2);
            StepSuiveur = 0;
        }
        else if (StepSuiveur == 3)
        {
            MOTOR_SetSpeed(0, 0);
            MOTOR_SetSpeed(1, 0);
        }
        else if (StepSuiveur == 1 && StateStep == 1 && StateRoom < 3)
        {
            shouldTurnLeft = 0;
            shouldTurnRight = 0;
            MOTOR_SetSpeed(0, 0);
            MOTOR_SetSpeed(1, 0);
            MOVEMENTS_Turn(1, 165, 0.3);
            MOVEMENTS_Forward(9, 0.3);
            MOVEMENTS_Turn(0, 90, 0.3);
            StateRoom++;
            StateStep = 2;
        }
        else if (StepSuiveur == 1 && StateStep == 1 && StateRoom == 3)
        {
            shouldTurnLeft = 0;
            shouldTurnRight = 0;
            MOTOR_SetSpeed(0, 0);
            MOTOR_SetSpeed(1, 0);
            MOVEMENTS_Turn(1, 165, 0.3);
            MOVEMENTS_Forward(9, 0.3);
            MOVEMENTS_Turn(0, 90, 0.3);
            StateRoom = 0;
            StepSuiveur = 5;
        }
        else if (StepSuiveur == 1 && StateStep == 3 && StateRoom < 3)
        {
            shouldTurnLeft = 0;
            shouldTurnRight = 0;
            MOTOR_SetSpeed(0, 0);
            MOTOR_SetSpeed(1, 0);
            MOVEMENTS_Turn(1, 165, 0.3);
            MOVEMENTS_Forward(13, 0.3);
            MOVEMENTS_Turn(0, 140, 0.3);
            StateRoom++;
            StepSuiveur = 2;
            MOTOR_SetSpeed(0, -0.1);
            MOTOR_SetSpeed(1, 0.1);
        }
        else if (StepSuiveur == 1 && StateStep == 3 && StateRoom == 3)
        {
            shouldTurnLeft = 0;
            shouldTurnRight = 0;
            MOTOR_SetSpeed(0, 0);
            MOTOR_SetSpeed(1, 0);
            MOVEMENTS_Turn(1, 165, 0.3);
            MOVEMENTS_Forward(9, 0.3);
            MOVEMENTS_Turn(0, 90, 0.3);
            StateRoom = 0;
            StepSuiveur = 5;
        }
        else if (StepSuiveur == 4)
        {
            MOVEMENTS_Turn(1, 90, 0.3);
            MOVEMENTS_Forward(3, 0.3);
            MOVEMENTS_Turn(0, 140, 0.3);
            StepSuiveur = 2;
            MOTOR_SetSpeed(0, -0.1);
            MOTOR_SetSpeed(1, 0.1);
        }
        else if (StepSuiveur == 5 && t.min % 5 == 0)
        {
            StepSuiveur = 4;
            StateStep = 1;
        }


}

void MesureSonar()
{
  if (Timer % 100 == 0 && (StepSuiveur == 0 || StepSuiveur == 3))
  {
    float Distance = SONAR_GetRange(0);
    //Serial.println(Distance);
    if (Distance < 10)
    {
        StateAvertissementOld = StateAvertissement;
        StateAvertissement = 1;
        StepSuiveur = 3;
    }
    else 
    {
        StateAvertissementOld = StateAvertissement;
        StateAvertissement = 0;
        if(StateAvertissement == 0 && StateAvertissementOld == 1)
        {
            digitalWrite(greenButtonLED, LOW);
            digitalWrite(redButtonLED, LOW);
            StepSuiveur = 2;
            MOTOR_SetSpeed(0, -0.2);
            MOTOR_SetSpeed(1, -0.02);
        }
    }
    
  }
}

void Interface()
{
    if(StepSuiveur == 3)
    {
        if(StateAvertissement == 1 && StateAvertissementOld == 0)
        {
            AUDIO_Play(a_Outroduction);
        }
        if(Timer % 100 == 0 && Timer % 200 != 0)
        {
            digitalWrite(greenButtonLED, HIGH);
            digitalWrite(redButtonLED, HIGH);
        }
        else if(Timer % 200 == 0)
        {
            digitalWrite(greenButtonLED, LOW);
            digitalWrite(redButtonLED, LOW);
        }  
    }
    else if(StateStep == 2)
    {
        int answer = 0;
        AUDIO_Play(a_Introduction);
        delay(5000);
        digitalWrite(greenButtonLED, HIGH);
        digitalWrite(redButtonLED, HIGH);

        answer = buttonResponse();

        digitalWrite(greenButtonLED, LOW);
        digitalWrite(redButtonLED, LOW);
        if(answer == 2)
        {
            AUDIO_PlayBlocking(a_Outroduction);
            StateStep = 3;
        }
        else if(answer == 1)
        {
            AUDIO_PlayBlocking(a_DixSecondes);
            AUDIO_Play(a_RFID);
            char RFID[50] = "";
            delay(200);
            Serial.println("Avant Rfid");
            CheckRFID(RFID);
            Serial.println("Apres Rfid");
            int str = 1;
            for(int i = 0; i < NOMBRE_DE_PATIENTS; i++)
            {
                str = strcmp(ListePatient[i].rfid_code, RFID);
                if(str == 0)
                {
                    PatientChoisi = i;
                    break;
                }
            }
            Serial.println(PatientChoisi);
            Serial.println(StateRoom);
            if(PatientChoisi == 0)
            {
                if(ListePatient[PatientChoisi].room_number == StateRoom)
                {
                    delay(200);
                    AUDIO_PlayBlocking(a_Tremblay);
                    delay(3500);
                }
                else
                {
                    StateStep = 3;
                }
            }
            else if(PatientChoisi == 1)
            {
                if(ListePatient[PatientChoisi].room_number == StateRoom)
                {
                    delay(200);
                    AUDIO_PlayBlocking(a_Gagnon);
                    delay(3500);
                }
                else
                {
                    StateStep = 3;
                }
            }
            else if(PatientChoisi == 2)
            {
                if(ListePatient[PatientChoisi].room_number == StateRoom)
                {
                    delay(200);
                    AUDIO_PlayBlocking(a_Roy);
                    delay(3500);
                }
                else
                {
                    StateStep = 3;
                }   
            }
            else
            {
                StateStep = 3;
            }
            if(StateStep == 2)
            {
                AUDIO_PlayBlocking(a_Medication);

                digitalWrite(greenButtonLED, HIGH);
                digitalWrite(redButtonLED, HIGH);

                answer = buttonResponse();

                digitalWrite(greenButtonLED, LOW);
                digitalWrite(redButtonLED, LOW);

                if(ListePatient[PatientChoisi].distributed == false && answer == 2)
                {
                    ServoDropPills(RED_P, ListePatient[PatientChoisi].dailydrugs.red);
                    ServoDropPills(BLACK_P, ListePatient[PatientChoisi].dailydrugs.green);
                    ServoDropPills(BLUE_P, ListePatient[PatientChoisi].dailydrugs.blue);
                    ServoDropPills(YELLOW_P, ListePatient[PatientChoisi].dailydrugs.yellow);
                    AUDIO_PlayBlocking(a_Indication);
                }
                else
                {
                    StateStep = 1;
                }
            }
            if(StateStep == 2)
            {
                AUDIO_PlayBlocking(a_Demande);

                digitalWrite(greenButtonLED, HIGH);
                digitalWrite(redButtonLED, HIGH);

                answer = buttonResponse();

                digitalWrite(greenButtonLED, LOW);
                digitalWrite(redButtonLED, LOW);

                if(answer == 2)
                {
                    AUDIO_PlayBlocking(a_Outroduction);
                    StateStep = 1;
                }
                else
                {
                    AUDIO_PlayBlocking(a_Outroduction);
                    StateStep = 3;
                }
            }
        }
        else
        {
            StateStep = 3;
        }
        StepSuiveur = 4;
    }
}

void CheckRFID(char RFID_ID[50])
{
    int boucle = 1;
    int incoming = 0;
    char crecu = '0';
    int i = 0;
    while (boucle == 1)
    {
        if (Serial1.available())
        {
            crecu = Serial1.read(); // lit le ID-12
            switch (crecu)
            {
            case 0x02:
                // START OF TRANSMIT
                AX_BuzzerON();
                i = 0;
                incoming = 1;
                break;
            case 0x03:
                // END OF TRANSMIT
                AX_BuzzerOFF();
                RFID_ID[11] = '\0';
                incoming = 0;
                // Affiche le code recu sans valider le checksum
                for (i = 0; i < 10; i++)
                    Serial.print(RFID_ID[i]);
                Serial.println("");
                boucle = 0;
                break;
            default:
                if (incoming)
                    RFID_ID[i++] = crecu;
                break;
            }
        }
    }
}

void TimerUpdate()
{
    if (Timer % 500 == 0)
    {
        DS3231_get(&t);
    }
    if (Timer == 10000)
    {
        Timer = 0;
    }
    delay(5);
    Timer += 5;
}

void data_initialisation(struct Patient Liste[5])
{

    strcpy(Liste[0].first_name, "Liam");
    strcpy(Liste[0].last_name, "Tremblay");
    Liste[0].room_number = 1;
    strcpy(Liste[0].rfid_code, "0E008E6F32D"); 
    Liste[0].dailydrugs.red = 1;
    Liste[0].dailydrugs.green = 2;
    Liste[0].dailydrugs.blue = 1;
    Liste[0].dailydrugs.yellow = 3;
    Liste[0].distributed = false;


    strcpy(Liste[1].first_name, "Thomas");
    strcpy(Liste[1].last_name, "Gagnon");
    Liste[1].room_number = 2;
    strcpy(Liste[1].rfid_code, "0E008E637F9"); //0E008E637F
    Liste[1].dailydrugs.red = 0;
    Liste[1].dailydrugs.green = 0;
    Liste[1].dailydrugs.blue = 0;
    Liste[1].dailydrugs.yellow = 0;
    Liste[1].distributed = false;
 

    strcpy(Liste[2].first_name, "Olivia");
    strcpy(Liste[2].last_name, "Roy");
    Liste[2].room_number = 3;
    strcpy(Liste[2].rfid_code, "0F027D729D9"); //0F027D729D
    Liste[2].dailydrugs.red = 0;
    Liste[2].dailydrugs.green = 0;
    Liste[2].dailydrugs.blue = 0;
    Liste[2].dailydrugs.yellow = 0;
    Liste[2].distributed = false;


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