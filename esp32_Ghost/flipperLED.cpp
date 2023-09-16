#include "flipperLED.h"

void flipperLED::RunSetup() {
  pinMode(B_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(R_PIN, OUTPUT);
    
  delay(50);

  digitalWrite(B_PIN, LOW);
  delay(1000);
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, LOW);
  delay(200);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, LOW);
  delay(200);
  digitalWrite(R_PIN, HIGH);
}

void flipperLED::offLED() {
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);
}