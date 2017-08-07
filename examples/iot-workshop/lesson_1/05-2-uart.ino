int i = 1;
void setup() {
  Serial.begin(115200);
}

void loop() {
  String str = "Msg sent from Arduino ";
  Serial.print(str);
  Serial.println(i);
  while(Serial.available()) {
    char ch = Serial.read();
    Serial.print(ch);
    delay(3);
  }  
  i++;
  delay(1000);
}
