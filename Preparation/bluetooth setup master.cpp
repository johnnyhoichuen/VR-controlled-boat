#include <SoftwareSerial.h>

const byte rx = 10;
const byte tx = 11;
SoftwareSerial BTMaster(rx, tx); // RX | TX

byte incomingByte;
String readBuffer = "";

void setup()
{
  Serial.begin(38400);  
  BTMaster.begin(38400);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  delay(200);
  Serial.println("Enter AT commands:");
  delay(200);
  BTMaster.write("AT+B38400");
  delay(200);
  BTMaster.write("AT+C007"); // C001-C100
  delay(200);
  BTMaster.write("AT+FU3"); // FU1-FU3
  delay(200);
  BTMaster.write("AT+P3"); // P1-P8
  delay(200);
  digitalWrite(9,HIGH);

  // setting: https://kitetalksabout.com/hc12-easy-arduino-wireless-communication/
}

void loop()
{
//  String test = "test";
////  String t = "t";
////  String es = "es"
//  String ing = "ing";
//  String nve = "-1";

  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTMaster.available())
    Serial.write(BTMaster.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTMaster.write(Serial.read());

//    Serial.println("///////////////////////");
//    while (BTMaster.available()) {             // If HC-12 has data
//      incomingByte = BTMaster.read();          // Store each incoming byte from HC-12
//      readBuffer += char(incomingByte);    // Add each byte to ReadBuffer string variable
//    }
//    delay(100);
//    Serial.print(readBuffer);
//
//
//    while (Serial.available()) {
//      BTMaster.write(Serial.read());
//    }
//
//    
//    BTMaster.write("Testing, testing...123");
//    Serial.println("Sending Signal...");
////    while (BTMaster.available()) {
////      Serial.write(BTMaster.read());
////      Serial.println("");
////      Serial.println("***********************");
////    }
//    readBuffer = "";
//    delay(1000);

    

////  BTMaster.print(test);
//  delay(100);
////  BTMaster.print(ing);
//  BTMaster.print(nve);
}

//#include <SoftwareSerial.h>
//SoftwareSerial serial2(10,11);
//void setup() {
//  Serial.begin(9600);
//  //把hc05模块的串口波特率从38400改成9600
//  serial2.begin(9600);
//  Serial.println("Start");
//}
// 
//void loop() {
//  if(Serial.available())
//    serial2.write(Serial.read());
//  if(serial2.available())
//    Serial.write(serial2.read());
//}