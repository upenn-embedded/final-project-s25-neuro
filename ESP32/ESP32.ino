#define F_CPU 8000000UL


#define RXD1 7  // GPIO 7 = D7 on XIAO
#define TXD1 6  // GPIO 6 = D6 on XIAO

void setup() {
  pinMode(RXD1, INPUT);
  pinMode(TXD1, OUTPUT);
  Serial.begin(9600);  // USB debug
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);  // Feather link
  delay(1000);
  Serial.println("ESP32-C3 XIAO Ready");
}

void loop() {
  String message = "";
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n') {
      Serial.print("Full message: ");
      Serial.println(message);
      message = "";  // reset for next line
    } else if (c != '\r') {
      message += c;  // skip \r, keep clean message
    }
  }
  delay(100);
}