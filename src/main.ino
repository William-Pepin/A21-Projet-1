#include <LibRobus.h>
#include <math.h>
#include <string.h>

#include "capteurCouleur.h"
#include "Movements.h"
#include "Tests.h"
#include "Distribution.h"
#include "Patient.h"
#include "capteurSonar.h"
#include "../lib/ds3231/ds3231.h"

#define COULEUR_CHAMBRE (0x02) // TODO Modifier pour la bonne couleur
#define COULEUR_POSTE (0x05)   // TODO Modifier pour la bonne couleur

const int redButtonLED = 39;
const int yellowLED = 41;
const int blueLED = 43;
const int greenButtonLED = 47;
const int greenButton = 45;
const int redButton = 49;
const int NOMBRE_DE_PATIENTS = 3;

const uint16_t a_Introduction = 1;
const uint16_t a_DixSecondes = 2;
const uint16_t a_RFID = 3;
const uint16_t a_Tremblay = 4;
const uint16_t a_Gagnon = 5;
const uint16_t a_Roy = 6;
const uint16_t a_Cote = 7;
const uint16_t a_Medication = 8;
const uint16_t a_Indication = 9;
const uint16_t a_Obstruction = 10;

int StateStep = 0;
//0: Cherche infirmière, 1: Ronde normale: Entre dans chambre, 2: Quiz,
int StepSuiveur = 0;
//0: Suivi de ligne blanche, 1: Arrivée à une pièce, 2: Réentrée sur la ligne, 3: Obstruction
int Timer = 0;
int StateDirection = 0;
int shouldTurnLeft = 0;
int shouldTurnRight = 0;
int StateAvertissement = 0;
int StateAvertissementOld = 0;
int StateRoom = 0;
struct Patient ListePatient[5];
int PatientChoisi;


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
            StateStep = 2;
        }
}

void MesureSonar()
{
  if (Timer % 100 == 0 && (StepSuiveur == 0 || StepSuiveur == 3))
  {
    float Distance = SONAR_GetRange(0);
    Serial.println(Distance);
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
            AUDIO_Play(a_Obstruction);
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
        delay(200);
        digitalWrite(greenButtonLED, HIGH);
        digitalWrite(redButtonLED, HIGH);
        while(true)
        {
            if(digitalRead(redButton) == LOW)
            {
                answer = 0;
                break;
            }
            if(digitalRead(greenButton) == LOW)
            {
                answer = 1;
                break;
            }
        }
        digitalWrite(greenButtonLED, LOW);
        digitalWrite(redButtonLED, LOW);
        if(answer == 0)
        {
            AUDIO_PlayBlocking(a_Obstruction);
            delay(200);
            while(true)
            {

            }
        }
        else
        {
            AUDIO_PlayBlocking(a_DixSecondes);
            AUDIO_Play(a_RFID);
            char RFID[50] = "";
            delay(200);
            CheckRFID(RFID);
            for(int i = 0; i < NOMBRE_DE_PATIENTS; i++)
            {
                if(strcmp(ListePatient[i].rfid_code, RFID) == 0)
                {
                    PatientChoisi = i;
                    break;
                }
            }
            if(PatientChoisi == 0)
            {
                AUDIO_Play(a_Tremblay);
                delay(200);
            }
            else if(PatientChoisi == 1)
            {
                AUDIO_Play(a_Gagnon);
                delay(200);
            }
            else if(PatientChoisi == 2)
            {
                AUDIO_PlayBlocking(a_Roy);
                delay(200);
            }
        }

    }
}

void CheckRFID(char RFID_ID[50])
{

    char crecu, i, incoming = 0;
    while (1)
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
                incoming = 0;
                // Affiche le code recu sans valider le checksum
                for (i = 0; i < 10; i++)
                    Serial.print(RFID_ID[i]);
                Serial.println("");
                break;
            default:
                if (incoming)
                    RFID_ID[i++];
                break;
            }
        }
    }
}

void TimerUpdate()
{
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
    strcpy(Liste[0].rfid_code, "**CODE"); //TODO ajouter code
    Liste[0].morning.red = 0;
    Liste[0].morning.green = 0;
    Liste[0].morning.blue = 0;
    Liste[0].morning.yellow = 0;
    Liste[0].noon.red = 0;
    Liste[0].noon.green = 0;
    Liste[0].noon.blue = 0;
    Liste[0].noon.yellow = 0;
    Liste[0].souper.red = 0;
    Liste[0].souper.green = 0;
    Liste[0].souper.blue = 0;
    Liste[0].souper.yellow = 0;
    Liste[0].night.red = 0;
    Liste[0].night.green = 0;
    Liste[0].night.blue = 0;
    Liste[0].night.yellow = 0;

    strcpy(Liste[1].first_name, "Thomas");
    strcpy(Liste[1].last_name, "Gagnon");
    Liste[1].room_number = 2;
    strcpy(Liste[1].rfid_code, "**CODE"); //TODO ajouter code
    Liste[1].morning.red = 0;
    Liste[1].morning.green = 0;
    Liste[1].morning.blue = 0;
    Liste[1].morning.yellow = 0;
    Liste[1].noon.red = 0;
    Liste[1].noon.green = 0;
    Liste[1].noon.blue = 0;
    Liste[1].noon.yellow = 0;
    Liste[1].souper.red = 0;
    Liste[1].souper.green = 0;
    Liste[1].souper.blue = 0;
    Liste[1].souper.yellow = 0;
    Liste[1].night.red = 0;
    Liste[1].night.green = 0;
    Liste[1].night.blue = 0;
    Liste[1].night.yellow = 0;

    strcpy(Liste[2].first_name, "Olivia");
    strcpy(Liste[2].last_name, "Roy");
    Liste[2].room_number = 3;
    strcpy(Liste[2].rfid_code, "**CODE"); //TODO ajouter code
    Liste[2].morning.red = 0;
    Liste[2].morning.green = 0;
    Liste[2].morning.blue = 0;
    Liste[2].morning.yellow = 0;
    Liste[2].noon.red = 0;
    Liste[2].noon.green = 0;
    Liste[2].noon.blue = 0;
    Liste[2].noon.yellow = 0;
    Liste[2].souper.red = 0;
    Liste[2].souper.green = 0;
    Liste[2].souper.blue = 0;
    Liste[2].souper.yellow = 0;
    Liste[2].night.red = 0;
    Liste[2].night.green = 0;
    Liste[2].night.blue = 0;
    Liste[2].night.yellow = 0;
}
