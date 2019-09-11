#include <SoftwareSerial.h>
#include <Servo.h>

const byte rx = 10;
const byte tx = 11;
SoftwareSerial BTSlave(rx, tx); // RX | TX

Servo servo;
byte incomingByte;
String readBuffer = "";
char shit;
int data;

void setup()
{
  servo.attach(7);

  Serial.begin(38400);
  BTSlave.begin(38400);
  pinMode(9, OUTPUT);
  digitalWrite(9,LOW);
  delay(250);
  Serial.println("Enter AT commands:");
  delay(250);
  BTSlave.write("AT+B38400");
  delay(250);
  BTSlave.write("AT+C007"); // C001-C100
  delay(200);
  BTSlave.write("AT+FU3"); // FU1-FU3
  delay(200);
  BTSlave.write("AT+P3"); // P1-P8
  delay(200);
//  BTSlave.write("AT+RX");
  digitalWrite(9,HIGH);

  
}

long timer = micros;

void loop()
{
//    // Keep reading from HC-05 and send to Arduino Serial Monitor
//    if (BTSlave.available()){
//      shit = BTSlave.read();
////      readBuffer += char(shit);
//      Serial.print(shit); Serial.print(" ");
//      data = int(shit);
//      Serial.println(data);
//      timer = micros();
//    }
//
//    // Keep reading from Serial Monitor
//    // Then send it to another HC-05
//    if (Serial.available())
//      BTSlave.write(Serial.read());
//
    // ORIGINAL
    // Keep reading from HC-05 and send to Arduino Serial Monitor
    if (BTSlave.available())
      Serial.write(BTSlave.read());

    // Keep reading from Arduino Serial Monitor and send to HC-05
    if (Serial.available())
      BTSlave.write(Serial.read());

//    while (BTSlave.available()) {             // If HC-12 has data
//      incomingByte = BTSlave.read();          // Store each icoming byte from HC-12
//      readBuffer += char(incomingByte);    // Add each byte to ReadBuffer string variable
//    }
//    delay(100);
//    while (Serial.available()) {
//      BTSlave.write(Serial.read());
//    }
//    if (readBuffer == "Testing, testing...123") {
//      Serial.println("DATA RECEIVED!!!");
//      Serial.println("**************************");
//      BTSlave.print(readBuffer);
//      delay(100);
//    }else {
//      Serial.println("yo");
//    }
//    readBuffer = "";
//    delay(1000);


      
}


//void checkATCommand () {
//  if (readBuffer.startsWith("AT")) {     // Check whether the String starts with "AT"
//    digitalWrite(setPin, LOW);           // Set HC-12 into AT Command mode
//    delay(200);                          // Wait for the HC-12 to enter AT Command mode
//    HC12.print(readBuffer);              // Send AT Command to HC-12
//    delay(200);
//    while (HC12.available()) {           // If HC-12 has data (the AT Command response)
//      Serial.write(HC12.read());         // Send the data to Serial monitor
//    }
//    digitalWrite(setPin, HIGH);          // Exit AT Command mode
//  }
//}

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