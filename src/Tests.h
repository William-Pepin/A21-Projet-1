#include <LibRobus.h>

void TesterMoteur(float offset)
{
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    MOTOR_SetSpeed(0, .6);
    MOTOR_SetSpeed(1, .6 + offset);
    delay(1000);
    float left = ENCODER_ReadReset(0);
    float right = ENCODER_ReadReset(1);
    Serial.print("Gauche ");
    Serial.println(left);
    Serial.print("Droite ");
    Serial.println(right);
    MOTOR_SetSpeed(0, 0);
    MOTOR_SetSpeed(1, 0);
    delay(1000);
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