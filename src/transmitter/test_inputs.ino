#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print(analogRead(A0));
  Serial.print(",");
  Serial.print(analogRead(A1));
  Serial.print(",");
  Serial.print(analogRead(A2));
  Serial.print(",");
  Serial.print(analogRead(A3));
  Serial.print(",");
  Serial.print(analogRead(A4));
  Serial.print(",");
  Serial.print(analogRead(A5));
  Serial.print(",");
  Serial.print(analogRead(A6));
  Serial.print(",");
  Serial.print(digitalRead(2));
  Serial.print(",");
  Serial.print(digitalRead(3));
  Serial.print(",");
  Serial.println(digitalRead(4));

}
