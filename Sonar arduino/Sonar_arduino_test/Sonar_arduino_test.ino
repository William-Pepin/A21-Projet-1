
#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04

// defines variables
long duration; // variable for the duration of sound wave travel
long distance; // variable for the distance measurement
int tries = 0;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
}
void loop() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  //duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  //distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
   do
    {
        duration = pulseIn( echoPin, HIGH, 50000);
        distance = 0.034 * duration / 2;
        if ( distance >= 600 ) {
            delay(100);
            pinMode(echoPin, OUTPUT);
            digitalWrite(echoPin, LOW);
            delay(100);
            pinMode(echoPin, INPUT);
        }
    } while (distance >= 600 && ++tries < 3);
    if (distance >= 600)
      // Out of range
      Serial.println(" out of range");
    else
      // Read ok!
        // Displays the distance on the Serial Monitor
     {Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
  }
}
