#define INTERVAL 10000
#define TX510_RX 10
#define TX510_TX 11
#define BUTTON_A_PIN  12
#define Lock_PIN 8

#include <SoftwareSerial.h>
#include "Button2.h";

Button2 button = Button2(BUTTON_A_PIN);

SoftwareSerial TX510Serial(TX510_RX, TX510_TX); // define TX510

void pressed(Button2& btn) {
 int ID = FaceRecognition();
 if (ID == 1)  {
    OpenLock();
  }
 Serial.println("  ID = " + String(ID)); 
}

void OpenLock(){
  digitalWrite(Lock_PIN, HIGH);
  delay(500);
  digitalWrite(Lock_PIN, LOW);
}

int FaceRecognition(){

  byte cmd[8] = {0xEF, 0xAA, 0x12, 0x00, 0x00, 0x00, 0x00, 0x12}; // Face recognition command
  byte response[12]; // for answer

  TX510Serial.write(cmd, 8); // Face recognition command
  // The serial stream can get out of sync. The response starts with 0xff, try to resync.
  while (TX510Serial.available() > 0 && (unsigned char)TX510Serial.peek() != 0xFF) {
    TX510Serial.read();
  }

  memset(response, 0, 12);
  TX510Serial.readBytes(response, 12);

  if (response[8] != 0x00) {
    Serial.println("Invalid face recognition!");
    return -1;
  }

  byte crc = 0;
  for (int i = 2; i < 11; i++) {
   crc += response[i];
  }

  if (response[11] == crc) {
   int responseHigh = (int) response[9];
   int responseLow = (int) response[10];
   int response_ID = (256 * responseHigh) + responseLow;   
   return response_ID;
  }else {
   Serial.println("CRC error!");
   return -1;
 }
}

void setup() {
  Serial.begin(115200); // Init console
  button.setPressedHandler(pressed);
  pinMode(Lock_PIN, OUTPUT);
  digitalWrite(Lock_PIN, HIGH);
  Serial.println("Setup started");
  //unsigned long previousMillis = millis();
  TX510Serial.begin(115200); 
}

void loop() {
  button.loop();
}
