
const int TRIG_PIN = 6;
const int ECHO_PIN = 5;

const double SPEED_OF_SOUND_CM_PER_US = 0.0343;

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}



void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  double duration_us = pulseIn(ECHO_PIN, HIGH);
  double distance_cm = duration_us / 2 * SPEED_OF_SOUND_CM_PER_US;

  String message = "Distance " + String(distance_cm) + " cm";

  Serial.println(message);

}