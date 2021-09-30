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