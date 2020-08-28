int led = 2;

void setup() {
  pinMode(led, OUTPUT);

}

void loop() {
  digitalWrite(led, LOW);
  delay(300);
  digitalWrite(led, HIGH);
  delay(400);
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
  delay(600);
  digitalWrite(led, LOW);
  delay(700);
  digitalWrite(led, HIGH);
  delay(800);
  digitalWrite(led, LOW);
  delay(300);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, HIGH);
  delay(3100);
}
