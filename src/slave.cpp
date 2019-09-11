/*
  The slave is for gimbal system
  Main mission:
  1. receive data from another arduino board to control the gimbal
  2. stablize the camera according to boat movement
*/

#include <SoftwareSerial.h>
#include <EasyTransfer.h>
#include <Servo.h>

#define MOTOR_1_PIN 5
#define MOTOR_1_PIN 6
#define SERVO_1_PIN 7
#define SERVO_2_PIN 8
#define BT_EN_PIN 9

const byte rx = 10;
const byte tx = 11;
SoftwareSerial slave(rx, tx); // HC-12 TX Pin, HC-12 RX Pin
SoftwareSerial boat(4,5);

Servo servo_1;
Servo servo_2;

byte incomingByte;
char data;
String readBuffer_1 = "";
String readBuffer_2 = "";

int count = 0;

long timer;
long time_1;
long time_2;
float diff;

int gx = 0;
int gy = 0;

int currentAngle = 0;
int lastAngle = 0;
int rotate = 0;

void setup() {
  Serial.begin(38400);
  Serial.println("Initializing");
  slave.begin(38400);

  pinMode(BT_EN_PIN, OUTPUT);
  digitalWrite(BT_EN_PIN, LOW);
  delay(200);
  slave.print("AT+RX");
  delay(200);
  digitalWrite(BT_EN_PIN, HIGH);
  delay(200);;

  servo_1.attach(SERVO_1_PIN);
  servo_2.attach(SERVO_2_PIN);
  servo_1.write(90);
  servo_2.write(90);

  timer = micros();
}


void loop() {

  // if (slave.available())
  //   Serial.write(slave.read());
  //
  // // Keep reading from Arduino Serial Monitor and send to HC-05
  // if (Serial.available())
  //   slave.write(Serial.read());

  boolean start_gx = false;
  boolean start_gy = false;

  boolean in_loop = false;
  boolean end_loop = false;

//  while(slave.available()){
//    Serial.write(slave.read());
//  }
  int delay_time = 10;

  while(slave.available() > 0 && end_loop == false) { // if bluetooth has data
    time_1 = micros();
    in_loop = true;
    data = slave.read();
    // Serial.println(data);

    // Read gx data between the start "s" and end marker "a"
    if(start_gx == true){
      if(data != 'a'){
        readBuffer_1 += data;
        data = "";
        delay(delay_time);
      }else if(data == 'a'){
        start_gx = false;
        delay(delay_time);
      }
    }else if (data == 's') { // Checks whether the received message statrs with the start marker "s"
      start_gx = true; // If true start reading the message
      data = "";
      delay(delay_time);
    }

    // Read gy data between the start "a" and end marker "e"
    if(start_gy == true){
      if(data != 'e'){
        readBuffer_2 += data;
        data = "";
        delay(delay_time);
      }else if(data == 'e'){
        start_gy = false; // when it sees 'e'
        data = "";
        end_loop = true;
        delay(delay_time);
      }
    }else if(data == 'a'){
      start_gy = true;
      data = "";
      delay(delay_time);
    }
  }

  // // check runtime
  // if(in_loop){
  //   diff = micros() - time_1;
  //   Serial.print("runtime: ");
  //   Serial.println(diff/1000000);
  // }

  // Converts string into integer
  gx = readBuffer_1.toInt();
  gy = readBuffer_2.toInt();
  delay(10);

  // controlling servo
  if(gx != 0) {
    servo_1.write(gx);
    Serial.print("gx: ");
    Serial.print(gx); Serial.print(" ");
  }
  if(gy != 0){
     servo_2.write(gy);
     Serial.print("gy: ");
     Serial.println(gy);
  }

  readBuffer_1 = "";
  readBuffer_2 = "";

  // if(micros() - timer > 1000000){ // print every second
  //   Serial.print("buffer: ");
  //   Serial.print(readBuffer_1);Serial.print(' ');
  //   Serial.print(readBuffer_2);
  //
  //   Serial.print(micros()/1000000); Serial.print(" : ");
  //   Serial.print(gx); Serial.print(" ");
  //   Serial.println(gy);
  //   timer = micros();
  //   // count = 0;
  // }

  // // Makes sure it uses angles between 0 and 180
  // if (currentAngle > 0 && currentAngle < 180) {
  //   // Convert angle value to steps (depending on the selected step resolution)
  //   // A cycle = 200 steps, 180deg = 100 steps ; Resolution: Sixteenth step x16
  //   currentAngle = map(currentAngle, 0, 180, 0, 1600);
  //   //Serial.println(currentAngle); // Prints the angle on the serial monitor
  //   digitalWrite(dirPin, LOW); // Enables the motor to move in a particular direction
  //   // Rotates the motor the amount of steps that differs from the previous positon
  //   if (currentAngle != lastAngle) {
  //     if (currentAngle > lastAngle) {
  //       rotate = currentAngle - lastAngle;
  //       for (int x = 0; x < rotate; x++) {
  //         // digitalWrite(stepPin, HIGH);
  //         // delayMicroseconds(400);
  //         // digitalWrite(stepPin, LOW);
  //         // delayMicroseconds(400);
  //
  //         // servo_1.writeMicroseconds();
  //         // servo_2.writeMicroseconds();
  //
  //       }
  //     }
  //     // rotate the other way
  //     if (currentAngle < lastAngle) {
  //       rotate = lastAngle - currentAngle;
  //       digitalWrite(dirPin, HIGH);        //Changes the rotations direction
  //       for (int x = 0; x < rotate; x++) {
  //         digitalWrite(stepPin, HIGH);
  //         delayMicroseconds(400);
  //         digitalWrite(stepPin, LOW);
  //         delayMicroseconds(400);
  //       }
  //     }
  //   }
  //   lastAngle = currentAngle;  // Remembers the current/ last positon
  // }
}