
#include <LibRobus.h>
#include <math.h>
#include <Wire.h>

#define GROVE_ADR (0x29)
#define GROVE_CMD (0x80)
#define GROVE_ID (0x12)
#define INTEGRATIONTIME (0xEB)
#define GAIN (0x02)

#define RED (0x01)
#define GREEN (0x02)
#define BLUE (0x03)
#define YELLOW (0x04)
#define RIEN (0x05)

struct RGB
{
    int16_t Red;
    int16_t Green;
    int16_t Blue;
    int16_t Clear;
    uint16_t Kelvin;
};

void ColorCapteurWrite8(uint8_t reg, uint32_t value);
uint8_t ColorCapteurRead8(uint8_t reg);
uint16_t ColorCapteurRead16(uint8_t reg);
void ColorCapteurEnable(void);
void ColorCapteurDisable(void);
void ColorCapteurBegin(void);
void setIntegrationTime(uint8_t time);
void setGain(uint8_t gain);
void ColorCapteurRawData(struct RGB *Couleurs);
void ColorCapteurKelvin(struct RGB *ADR_Couleurs);
float powf(const float x, const float y);

void ColorCapteurWrite8(uint8_t reg, uint32_t value)
{
    Wire.beginTransmission(GROVE_ADR);
    Wire.write(GROVE_CMD | reg);
    Wire.write(value & 0xFF);
    Wire.endTransmission();
}

uint8_t ColorCapteurRead8(uint8_t reg)
{
    Wire.beginTransmission(GROVE_ADR);
    Wire.write(GROVE_CMD | reg);
    Wire.endTransmission();

    Wire.requestFrom(GROVE_ADR, 1);
    return Wire.read();
}

uint16_t ColorCapteurRead16(uint8_t reg)
{
    uint16_t x = 0;
    uint16_t t = 0;

    Wire.beginTransmission(GROVE_ADR);
    Wire.write(GROVE_CMD | reg);
    Wire.endTransmission();

    Wire.requestFrom(GROVE_ADR, 2);
    t = Wire.read();
    x = Wire.read();
    x <<= 8;
    x |= t;
    return x;
}

void ColorCapteurEnable(void)
{
    ColorCapteurWrite8(0x00, 0x01);
    delay(3);
    ColorCapteurWrite8(0x00, 0x01 | 0x02);
}

void ColorCapteurDisable(void)
{
    uint8_t reg = 0;
    reg = ColorCapteurRead8(0x00);
    ColorCapteurWrite8(0x00, reg & ~(0x01 | 0x02));
}

void ColorCapteurBegin(void)
{
    Wire.begin();

    /* Set default integration time and gain */
    setIntegrationTime(INTEGRATIONTIME);
    setGain(GAIN);

    /* Note: by default, the device is in power down mode on bootup */
    ColorCapteurEnable();
}

void setIntegrationTime(uint8_t time)
{
    ColorCapteurWrite8(0x01, time);
}

void setGain(uint8_t gain)
{
    ColorCapteurWrite8(0x0F, gain);
}

void ColorCapteurRawData(struct RGB *ADR_Couleurs)
{
    ADR_Couleurs->Clear = ColorCapteurRead16(0x14);
    ADR_Couleurs->Red = ColorCapteurRead16(0x16);
    ADR_Couleurs->Green = ColorCapteurRead16(0x18);
    ADR_Couleurs->Blue = ColorCapteurRead16(0x1A);
    delay(50);
}

void ColorCapteurKelvin(struct RGB *ADR_Couleurs)
{
    float X, Y, Z; /* RGB to XYZ correlation      */
    float xc, yc;  /* Chromaticity co-ordinates   */
    float n;       /* McCamy's formula            */

    /* 1. Map RGB values to their XYZ counterparts.    */
    /* Based on 6500K fluorescent, 3000K fluorescent   */
    /* and 60W incandescent values for a wide range.   */
    /* Note: Y = Illuminance or lux                    */
    X = (-0.14282F * ADR_Couleurs->Red) + (1.54924F * ADR_Couleurs->Green) + (-0.95641F * ADR_Couleurs->Blue);
    Y = (-0.32466F * ADR_Couleurs->Red) + (1.57837F * ADR_Couleurs->Green) + (-0.73191F * ADR_Couleurs->Blue);
    Z = (-0.68202F * ADR_Couleurs->Red) + (0.77073F * ADR_Couleurs->Green) + (0.56332F * ADR_Couleurs->Blue);

    /* 2. Calculate the chromaticity co-ordinates      */
    xc = (X) / (X + Y + Z);
    yc = (Y) / (X + Y + Z);

    /* 3. Use McCamy's formula to determine the CCT    */
    n = (xc - 0.3320F) / (0.1858F - yc);

    /* Calculate the final CCT */
    ADR_Couleurs->Kelvin = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;
}

float powf(const float x, const float y)
{
    return (float)(pow((double)x, (double)y));
}

uint16_t LectureCouleur(struct RGB *ADR_Couleurs)
{
    ColorCapteurRawData(ADR_Couleurs);
    ColorCapteurKelvin(ADR_Couleurs);
    // delay(1000);
    // Serial.print(Couleurs.Red,DEC);
    // Serial.print("  ");
    // Serial.print(Couleurs.Green,DEC);
    // Serial.print("  ");
    // Serial.print(Couleurs.Blue,DEC);
    // Serial.print("  ");
    // Serial.print(Couleurs.Clear,DEC);
    // Serial.print("\n");
    // Serial.print(Couleurs.Kelvin,DEC);
    // Serial.print("\n");
    if ((ADR_Couleurs->Kelvin > 11000) & (abs(ADR_Couleurs->Red - ADR_Couleurs->Blue) > 500))
    {
        // Serial.print("Bleu");
        return (BLUE);
    }
    else if ((ADR_Couleurs->Kelvin > 11000) & (abs(ADR_Couleurs->Red - ADR_Couleurs->Blue) < 500))
    {
        // Serial.print("Rouge");
        return (RED);
    }
    else if ((ADR_Couleurs->Kelvin < 7000) & (ADR_Couleurs->Kelvin > 5000))
    {
        // Serial.print("Jaune");
        return (YELLOW);
    }
    else if ((ADR_Couleurs->Kelvin < 11000) & (ADR_Couleurs->Kelvin > 7000))
    {
        // Serial.print("Vert");
        return (GREEN);
    }
    // Serial.print(abs(Couleurs.Red - Couleurs.Blue),DEC);                       // Affichage de la valeur absolue de rouge - bleu
    // Serial.print("\n");
    return (RIEN);
}