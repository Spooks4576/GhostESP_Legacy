#include "flipperLED.h"

void simulatePurple() {
  int duration = 1000;
  
  // Display red
  digitalWrite(R_PIN, LOW);
  delay(duration);
  digitalWrite(R_PIN, HIGH);
  
  // Display blue
  digitalWrite(B_PIN, LOW);
  delay(duration);
  digitalWrite(B_PIN, HIGH);
}


void flipperLED::TurnPurple()
{
  digitalWrite(B_PIN, LOW);
  digitalWrite(R_PIN, LOW);
}


void flipperLED::RunSetup() {
  pinMode(B_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(R_PIN, OUTPUT);
  
  // Turn red
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, LOW);
  delay(500);
  
  // Simulate purple again for one second
  digitalWrite(B_PIN, LOW);
  digitalWrite(R_PIN, LOW);
  delay(500);

  // Turn red
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, LOW);
  delay(500);

  // Simulate purple again for one second
  digitalWrite(B_PIN, LOW);
  digitalWrite(R_PIN, LOW);
  delay(500);
  
  // Turn off
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);
}

void flipperLED::offLED() {
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);
}