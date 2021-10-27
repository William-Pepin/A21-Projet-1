#include <LibRobus.h>
#include <math.h>
#include <Wire.h>


#define GROVE_ADR  (0x29)
#define GROVE_CMD  (0x80)
#define GROVE_ID   (0x12)
#define INTEGRATIONTIME (0xEB)
#define GAIN (0x01)

void setup()
{
  BoardInit();
  delay(100);
 
}

void loop()
{
    //ColorCapteurRawData();
    Serial.print("hello");
}


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

    Wire.requestFrom(GROVE_ADR,1);
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







