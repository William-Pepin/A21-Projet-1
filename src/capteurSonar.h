#include <LibRobus.h>

bool lastObstacleDetected;

float SONAR_IsObstacleDetected()
{
    return SONAR_GetRange(0) < 15;
}

void DetectObstacle()
{
    if (SONAR_IsObstacleDetected())
    {
        MOTOR_SetSpeed(0, 0);
        MOTOR_SetSpeed(1, 0);
        lastObstacleDetected = true;
    }
    else if (lastObstacleDetected)
    {
        MOTOR_SetSpeed(0, -0.2);
        MOTOR_SetSpeed(1, -0.2);
        lastObstacleDetected = false;
    }
}