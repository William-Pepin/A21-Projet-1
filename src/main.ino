#include <LibRobus.h>

uint16_t track = 0;

void setup()
{
  BoardInit();
  AudioInit();
}

void loop()
{
  AUDIO_PlayBlocking(track);
  track++;
}
