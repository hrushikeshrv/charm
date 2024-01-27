const int REED_PIN = 2;


void setup() {

  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(REED_PIN, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  int triggered = digitalRead(REED_PIN);

  if (triggered == LOW) {

    Serial.println("Switch closed");
  }
  else {
    Serial.println("Switch open");
  }
  delay(1000);
}
