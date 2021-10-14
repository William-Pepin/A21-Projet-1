#include <LibRobus.h>
#include <math.h>

const double WHEEL_CIRCONFERENCE = 2 * PI * 1.5;
const double ROBOT_CIRCONFERENCE_RIGHT = 2 * PI * 3.75;
const double ROBOT_CIRCONFERENCE_LEFT = 2 * PI * 3.85;
const double MIN_SPEED = 0.07;

void setup()
{
  BoardInit();
  delay(100);
}

void loop()
{
  exit(0);
}

/** Function to accelerate exponentially to a certain distance

@param distance, distance travelled in inches

@param speed, represents direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_Accelerate(double distance, double speed)
{
  double acceleration;

  MOVEMENTS_ResetEncoder();
  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance);
  int32_t currentEncoder = MOVEMENTS_ReadEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadEncoder();
    acceleration = MOVEMENTS_CalculateAcceleration(speed, currentEncoder, targetEncoder);

    MOVEMENTS_Forward(acceleration);
  }
  MOVEMENTS_ResetEncoder();
}

/** Function to deccelerate exponentially to a certain distance

@param distance, distance travelled in inches

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_Deccelerate(double distance, double speed)
{
  double decceleration;

  MOVEMENTS_ResetEncoder();
  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance);
  int32_t currentEncoder = MOVEMENTS_ReadEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadEncoder();
    decceleration = MOVEMENTS_CalculateDecceleration(speed, currentEncoder, targetEncoder);

    MOVEMENTS_Forward(decceleration);
  }
  MOVEMENTS_Stop();
  MOVEMENTS_ResetEncoder();
}

/** Function to go accelerate until reached speed,
 * then deccelerate at the desired distance. 

@param distance, distance travelled in inches

@param speed, represents direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_Forward(double distance, double speed)
{
  MOVEMENTS_Accelerate(distance * .10, speed);

  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance * .80);
  int32_t currentEncoder = MOVEMENTS_ReadEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadEncoder();
    MOVEMENTS_Forward(speed);
  }

  MOVEMENTS_Deccelerate(distance * .10, speed);
}

/** Function to accelerate exponentially to a certain distance while turning

@param direction, 0 left, 1 right

@param distance, distance travelled in inches

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_AccelerateTurn(bool direction, double distance, double speed)
{
  double acceleration;

  MOVEMENTS_ResetEncoder();
  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance);
  int32_t currentEncoder = MOVEMENTS_ReadAbsEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadAbsEncoder();
    acceleration = MOVEMENTS_CalculateAcceleration(speed, currentEncoder, targetEncoder);

    if (direction)
      MOVEMENTS_TurnRight(acceleration);
    else
      MOVEMENTS_TurnLeft(acceleration);
  }
  MOVEMENTS_ResetEncoder();
}

/** Function to deccelerate exponentially to a certain distance while turning

@param direction, 0 left, 1 right

@param distance, distance travelled in inches

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_DeccelerateTurn(bool direction, double distance, double speed)
{
  double decceleration;

  MOVEMENTS_ResetEncoder();
  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance);
  int32_t currentEncoder = MOVEMENTS_ReadAbsEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadAbsEncoder();
    decceleration = MOVEMENTS_CalculateDecceleration(speed, currentEncoder, targetEncoder);

    if (direction)
      MOVEMENTS_TurnRight(decceleration);
    else
      MOVEMENTS_TurnLeft(decceleration);
  }
  MOVEMENTS_Stop();
  MOVEMENTS_ResetEncoder();
}

/** Function to turn smoothly to a certain angle

@param direction, 0 left, 1 right

@param angle, angle in degrees

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_Turn(bool direction, double angle, double speed)
{

  double distance = MOVEMENTS_DistanceForAngle(direction, angle);

  MOVEMENTS_AccelerateTurn(direction, distance * .20, speed);

  int32_t targetEncoder = MOVEMENTS_EncoderForDistance(distance * .60);
  int32_t currentEncoder = MOVEMENTS_ReadAbsEncoder();

  while (currentEncoder < targetEncoder)
  {
    currentEncoder = MOVEMENTS_ReadAbsEncoder();
    if (direction)
      MOVEMENTS_TurnRight(speed);
    else
      MOVEMENTS_TurnLeft(speed);
  }
  MOVEMENTS_DeccelerateTurn(direction, distance * .20, speed);
}

/** Function to correct the right wheel based on the left wheel

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

*/
double MOVEMENTS_SyncRightWheel(double speed)
{
  const double factor = 1.05;

  double leftEncoder = abs(ENCODER_Read(0));
  double rightEncoder = abs(ENCODER_Read(1));
  if (leftEncoder == rightEncoder)
    return speed;
  else if (leftEncoder > rightEncoder + 1)
    return speed * factor;
  else if (leftEncoder < rightEncoder + 1)
    return (speed - (speed * (factor - 1)));
}

/** Function to get the number of encoder ticks needed to reach a certain distance

@param distance, distance travelled in inches

*/
double MOVEMENTS_EncoderForDistance(double distance)
{
  return (distance / WHEEL_CIRCONFERENCE) * 3200;
}

/** Function to get the number of distance to travel to reach a certain angle

@param direction, 0 left, 1 right

@param angle, angle in degrees

*/
double MOVEMENTS_DistanceForAngle(bool direction, double angle)
{

  if (direction)
    return ROBOT_CIRCONFERENCE_RIGHT * (angle / 360);
  return ROBOT_CIRCONFERENCE_LEFT * (angle / 360);
}

/** Function to calculate the acceleration based on the current encoder tick and the target encoder tick

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

@param currentEncoder, encoder tick at the instant

@param targetEncoder, encoder tick to reach

*/
double MOVEMENTS_CalculateAcceleration(double speed, int32_t currentEncoder, int32_t targetEncoder)
{
  double acceleration = speed * currentEncoder / targetEncoder;

  if (acceleration < MIN_SPEED)
    acceleration = MIN_SPEED;

  return acceleration;
}

/** Function to calculate the decceleration based on the current encoder tick and the target encoder tick

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

@param currentEncoder, encoder tick at the instant

@param targetEncoder, encoder tick to reach

*/
double MOVEMENTS_CalculateDecceleration(double speed, int32_t currentEncoder, int32_t targetEncoder)
{
  double decceleration = speed - speed * currentEncoder / targetEncoder;

  if (decceleration < MIN_SPEED)
    decceleration = MIN_SPEED;

  return decceleration;
}

/** Function to read the main encoder */
int32_t MOVEMENTS_ReadEncoder()
{
  return ENCODER_Read(0);
}

/** Function to read the absolute value of the main encoder */
int32_t MOVEMENTS_ReadAbsEncoder()
{
  return abs(ENCODER_Read(0));
}

/** Function to read the main encoder and the slave encoder */
int32_t MOVEMENTS_ResetEncoder()
{
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
}

/** Function to turn both wheel at the same time

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

*/
void MOVEMENTS_Forward(double speed)
{
  MOTOR_SetSpeed(0, speed);
  MOTOR_SetSpeed(1, MOVEMENTS_SyncRightWheel(speed));
}

/** Function to turn left using both wheel

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]

*/
void MOVEMENTS_TurnLeft(double speed)
{
  MOTOR_SetSpeed(0, -speed);
  MOTOR_SetSpeed(1, MOVEMENTS_SyncRightWheel(speed));
}

/** Function to turn right using both wheel

@param speed, represents the starting direction and amplitude of PWM
floating value between [-1.0, 1.0]
*/
void MOVEMENTS_TurnRight(double speed)
{
  MOTOR_SetSpeed(0, speed);
  MOTOR_SetSpeed(1, -MOVEMENTS_SyncRightWheel(speed));
}

/** Function to stop both wheel at the same time */
void MOVEMENTS_Stop()
{
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
}
