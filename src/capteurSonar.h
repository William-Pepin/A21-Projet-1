#include <LibRobus.h>

float SONAR_IsObstacleDetected()
{
    return SONAR_GetRange(0) < 15;
}