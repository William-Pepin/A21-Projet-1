#include <LibRobus.h>
#include <math.h>
#include "CapteurCouleur.h"
#include "Movements.h"
#include "Tests.h"
#include "Distribution.h"
#include "Patient.h"
#include "Suiveur.h"
#include "capteurSonar.h"

#define COULEUR_CHAMBRE (0x03) // TODO Modifier pour la bonne couleur
#define COULEUR_POSTE (0x04)   // TODO Modifier pour la bonne couleur

const int redLED = 39;
const int yellowLED = 41;
const int greenLED = 43;
const int blueLED = 45;
const int greenButton = 47;
const int redButton = 49;

uint8_t couleur = 0;

bool isFollowingALine = true;
bool isInChamber = false;
bool isInPoste = false;
bool isSequenceFinished = false;

int chamberNumber = 0;
int timerRFID = 0;
uint16_t audioTrack = 0;
int RFID = -1;

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
  mainSequence();
}

void mainSequence()
{
  if (!isSequenceFinished)
  {
    if (isFollowingALine)
    {
      Serial.println("Je suis une ligne ");
      AjustementDirection();
      MesureSuiveur();
      TimerUpdate();
      DetectChamber();
      DetectObstacle();
    }
    else if (isInChamber)
    {
      Serial.print("isInChamber");
      MOVEMENTS_Stop();
      //SequenceChamber();
      delay(5000);
      MOVEMENTS_Start();
      delay(2500);
      isInChamber = false;
      isFollowingALine = true;
    }
    else if (isInPoste)
    {
      Serial.print("isInposte");
      //if (chamberNumber > 4)
      //{
      //  isSequenceFinished = true;
      //}
    }
  }
}

void SequenceChamber()
{
  AUDIO_PlayBlocking(0);

  //on met un délais pour savoir si le client appuis dessus sinon on alerte les infirmière
  while (RFID != -1 || timerRFID == 15000)
  {
    delay(50);
    /*
     scan RFID Lire identifie Patient
     */
    currentPatient = Tremblay; // todo combiner le rfid
    timerRFID += 50;
  }

  if (timerRFID < 15000)
  {
    //Appeler infirmière (allumer LED)
    timerRFID == 0;
  }

  //chamberNumber est la chambre dans laquelle le robot est
  if (chamberNumber == 0)
  {

    if (chamberNumber == currentPatient.room_number)
    {
      AUDIO_PlayBlocking(1); // Bonjour Mr Tremblay
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
      AUDIO_PlayBlocking(2); // Bonjour Mr Gagnon
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
      AUDIO_PlayBlocking(3); // Bonjour Mr Roy
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
      AUDIO_PlayBlocking(4); // Bonjour Mr Côté
    }
    else
    {
      //allumé LED
    }
  }

  AUDIO_PlayBlocking(5); //Avez vous besoin d'assitance médicale immédiate
  /*
     Vérifie sur quel bouton le patient à appuyer
   */
  AUDIO_PlayBlocking(6); //Avez-vous eu votre dose de médicament quotidienne
  /*
     Vérifie sur quel bouton le patient à appuyer
   */
  if (currentPatient.daily_drugs_confirmation == false)
  {
    AUDIO_PlayBlocking(7); //Voici vos pillules ?
    /*
       Distribution de pillule
    */
  }
  else
  {
    //Allumé LED
  }

  AUDIO_PlayBlocking(8); //Aimeriez-vous voir une infirmère pour tout autre raison non-urgente
                         /*
     Vérifie sur quel bouton le patient à appuyer
   */

  AUDIO_PlayBlocking(8); //Merci de votre compréhension, bye bye

  //isInChamber = false;
  chamberNumber += 1;
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

  Tremblay.room_number = 1;

  Tremblay.rfid_code[0] = '1';
  Tremblay.rfid_code[1] = '\0';

  Tremblay.daily_drugs.red = 1;
  Tremblay.daily_drugs.bleu = 2;
  Tremblay.daily_drugs.green = 3;

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

  Gagnon.room_number = 2;

  Gagnon.rfid_code[0] = '2';
  Gagnon.rfid_code[1] = '\0';

  Gagnon.daily_drugs.red = 2;
  Gagnon.daily_drugs.bleu = 2;
  Gagnon.daily_drugs.green = 2;

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

  Roy.room_number = 3;

  Roy.rfid_code[0] = '3';
  Roy.rfid_code[1] = '\0';

  Roy.daily_drugs.red = 3;
  Roy.daily_drugs.bleu = 2;
  Roy.daily_drugs.green = 1;

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

  Cote.room_number = 4;

  Cote.rfid_code[0] = '4';
  Cote.rfid_code[1] = '\0';

  Cote.daily_drugs.red = 3;
  Cote.daily_drugs.bleu = 3;
  Cote.daily_drugs.green = 4;

  Cote.daily_drugs_confirmation = false;
}

int gestionBouton()
{
  if (digitalRead(greenButton) == LOW)
  {
    return 1;
  }
  else if (digitalRead(redButton) == LOW)
  {
    return 0;
  }

  return -1;
}

void DetectChamber()
{
  if (StateDirection == 12)
  {
    uint8_t couleur = GetCouleur();
    if (couleur == COULEUR_CHAMBRE)
    {
      isInChamber = true;
      isFollowingALine = false;
    }
    else if (couleur == COULEUR_POSTE)
    {
      if (chamberNumber > 4)
      {
        isSequenceFinished = true;
      }
      isInPoste = true;
      isFollowingALine = false;
    }
  }
}
