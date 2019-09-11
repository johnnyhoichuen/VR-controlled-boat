/*
  The master is for MPU6050
  Main mission:
  1. sends the acc & gyro data to slave
  2. changes mode when phone sends command
    a. spectator mode
    b. VR controller mode
*/

//--- Accelerometer Register Addresses
// #define Power_Register 0x2D
// #define X_Axis_Register_DATAX0 0x32 // Hexadecima address for the DATAX0 internal register.
// #define X_Axis_Register_DATAX1 0x33 // Hexadecima address for the DATAX1 internal register.
// #define Y_Axis_Register_DATAY0 0x34
// #define Y_Axis_Register_DATAY1 0x35
// #define Z_Axis_Register_DATAZ0 0x36
// #define Z_Axis_Register_DATAZ1 0x37
//
// int ADXAddress = 0x53;  //Device address in which is also included the 8th bit for selecting the mode, read in this case.
// int X0, X1, X_out;
// int Y0, Y1, Y_out;
// int Z1, Z0, Z_out;
// float Xa, Ya, Za;

// float angle;
// int lastAngle = 0;
// int count = 0;
// int angleSum = 0;

#include <SoftwareSerial.h>
#include <Servo.h>
#include <MPU6050.h>
#include <Wire.h>

#define BT_EN_PIN 9
SoftwareSerial master(10,11);
MPU6050 mpu;

// for bluetooth communication
// byte incomingByte;
// String readBuffer = "";
String gxString;
String gyString;

// acc and gyro data
int16_t ax, ay, az;
int16_t gx, gy, gz;
int count = 0;
int iteration = 0;
int temp_1 = 0;
int temp_2 = 0;

int gyro_x, gyro_y, gyro_z;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
boolean set_gyro_angles;
long acc_x, acc_y, acc_z, acc_total_vector;

float angle_roll_acc, angle_pitch_acc;
float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
float angle_pitch_output, angle_roll_output;
long loop_timer;
int temp;

void setup() {
  Wire.begin();
  Serial.begin(38400);
  master.begin(38400);

  pinMode(BT_EN_PIN, OUTPUT);
  digitalWrite(BT_EN_PIN, LOW);
  delay(200);
  master.print("AT+RX");
  delay(200);
  digitalWrite(BT_EN_PIN, HIGH);
  delay(200);;



  Serial.println("Initializing MPU6050");
  mpu.initialize( );
  Serial.println(mpu.testConnection( ) ? "Successfully Connected" : "Connection failed");
  delay(300);
  Serial.println("Taking Values from the mpu");
  delay(300);

  setup_mpu_6050_registers();
  Serial.println("Setting up register");

  for (int cal_int = 1; cal_int <= 1000 ; cal_int ++){
    read_mpu_6050_data();
    gyro_x_cal += gyro_x;          //Add the gyro x offset to the gyro_x_cal variable
    gyro_y_cal += gyro_y;          //Add the gyro y offset to the gyro_y_cal variable
    gyro_z_cal += gyro_z;          //Add the gyro z offset to the gyro_z_cal variable
    delay(3);                      //Delay 3us to have 250Hz for-loop
    if(cal_int == 500){
      Serial.println("Init 50% done");
    }else if(cal_int == 1000){
      Serial.println("Init 100% done");
    }
  }

  gyro_x_cal /= 1000;
  gyro_y_cal /= 1000;
  gyro_z_cal /= 1000;

  /* CAUSION!!!
    Nano board has a resolution of four microseconds
    (i.e. the value returned is always a multiple of four)
  */

  loop_timer = micros(); //Reset the loop timer
}



void loop(){
  read_mpu_6050_data();

  /* TODO:
  Calibrate the mpu using this method:
  http://wired.chillibasket.com/2015/01/calibrating-mpu6050/
  */

  //Calibrate the
  gyro_x -= gyro_x_cal;
  gyro_y -= gyro_y_cal;
  gyro_z -= gyro_z_cal;

  //Gyro angle calculations . Note 0.0000611 = 1 / (250Hz x 65.5)
  angle_pitch += gyro_x * 0.0000611;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_roll += gyro_y * 0.0000611;                                    //Calculate the traveled roll angle and add this to the angle_roll variable
  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  angle_pitch += angle_roll * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
  angle_roll -= angle_pitch * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the pitch angle to the roll angel

  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));  //Calculate the total accelerometer vector
  //57.296 = 1 / (PI / 180) The Arduino asin function is in radians
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;       //Calculate the roll angle

  /* TODO:
    find the calibration value and subtract
    - angle_pitch_acc
    - angle_roll_acc
  */

  //Acc calibration value for pitch
  angle_pitch_acc -= 0.0;
  //Ac calibration value for roll
  angle_roll_acc -= 0.0;

  if(set_gyro_angles){  //If IMU is already started
    //Correct the drift of the gyro pitch angle with the acc pitch angle
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;
    //Correct the drift of the gyro roll angle with the acc roll angle
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;
  }
  else{ //At first start
    angle_pitch = angle_pitch_acc;
    angle_roll = angle_roll_acc;
    set_gyro_angles = true; // IMU starts
  }

  // //To dampen the pitch and roll angles a complementary filter is used
  // angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  // angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value
  // Serial.print(" | Angle  = "); Serial.println(angle_pitch_output);

  while(micros() - loop_timer < 4000);                                 //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  loop_timer = micros();//Reset the loop timer

// -------------------------------------------------------

  // getting value from gyro
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
//  gx = map(gx, -32700, 32700, -178, 0);  // servo_1
//  gy = map(gy, -150000, 150000, -180, 0);  // servo_2
//  gx = -gx;
//  gy = -gy;

  gx = map(gyro_x, -32700, 32700, 0, 182);  // servo_1
  gy = map(gyro_y, -150000, 150000, 0, 182);  // servo_2



  const int total = 20; // number of data that is taking avg
  count++;
  temp_1 += gx;
  temp_2 += gy;

  // send the avg value to slave bluetooth
  if(count >= total){
    temp_1 = temp_1/total;
    temp_2 = temp_2/total;

    Serial.print(micros()/1000000);
    Serial.print("s: Angle (x,y): ");
    Serial.print(angle_pitch_acc); Serial.print(" ");
    Serial.print(angle_roll_acc);
    Serial.print(" Gyro (x,y): ");
    Serial.print(temp_1); Serial.print(" ");
    Serial.println(temp_2);

    // bluetooth communication
    gxString = String(temp_1);
    gyString = String(temp_2);

    temp_1 = temp_2 = 0;
    count = 0;
    iteration++;

    // sending data to another bluetooth
    int delay_time = 10;
    master.print("s");
    delay(delay_time);
    master.print(gxString);
    delay(delay_time);
    master.print("a");
    delay(delay_time);
     master.print(gyString);
//    master.print("90");
    delay(delay_time);
    master.print("e");
    delay(delay_time);

    // // Some more smoothing of acceleromter reading - sends the new angle only if it differes from the previous one by +-2
    // if (angleInt > lastAngle + 2 || angleInt < lastAngle - 2) {
    //   Serial.println(angleInt);
    //   String angleString = String(angleInt);
    //   //sends the angle value with start marker "s" and end marker "e"
    //   HC12.print("s" + angleString + "e");
    //   delay(10);
    //   lastAngle = angleInt;
    //   angleSum = 0;
    //   count = 0;
    // }
  }
  // if(master.available()){
    // master.print("s" + gyString + "a");// + gyString + "e");
  //
  // }
  // String longString = "s" + gyString + "a" + gyString + "e";

}

void setup_mpu_6050_registers(){
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);             //Start communicating with the MPU-6050
  Wire.write(0x6B);                         //Send the requested starting register
  Wire.write(0x00);                         //Set the requested starting register
  Wire.endTransmission();
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);             //Start communicating with the MPU-6050
  Wire.write(0x1C);                         // ACCEL_CONFIG register(0x1C)
  Wire.write(0x10);                         // Send (00010000) for 8g
  Wire.endTransmission();
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);             //Start communicating with the MPU-6050
  Wire.write(0x1B);                         // GYRO_CONFIG register(0x1B)
  Wire.write(0x08);                         // Send (00001000) for 500dps
  Wire.endTransmission();

  /*  For more info look at the register map:
   *  https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
  */
}

void read_mpu_6050_data(){              //Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);         //Start communicating with the MPU-6050
  Wire.write(0x3B);                     //Send the requested starting register
  Wire.endTransmission();               //End the transmission

  Wire.requestFrom(0x68,14);            //Request 14 bytes from the MPU-6050

  while(Wire.available() < 14);         //Wait until all the bytes are received

  acc_x = Wire.read()<<8|Wire.read();
  acc_y = Wire.read()<<8|Wire.read();
  acc_z = Wire.read()<<8|Wire.read();
  temp = Wire.read()<<8|Wire.read();
  gyro_x = Wire.read()<<8|Wire.read();
  gyro_y = Wire.read()<<8|Wire.read();
  gyro_z = Wire.read()<<8|Wire.read();
}

  // // Combine X and Y values for getting the angle value from 0 to 180 degrees
  // if (Y_out > 0) {
  //   angle = map(Y_out, 0, 256, 90, 0);
  // }
  // else if (Y_out < 0) {
  //   angle = map(Y_out, 256, 0, 90, 0);
  //   angle = 90 - angle;
  // }
  // if (X_out < 0 & Y_out < 0) {
  //   angle = 180;
  // }
  // if (X_out < 0 & Y_out >0) {
  //   angle = 0;
  // }
  //
  // // float to int
  // int angleInt = int(angle);
  // // Makes 100 accelerometer readings and sends the average for smoother result
  // angleSum = angleSum + angleInt;
  // count++;
  // if (count >= 100) {
  //   angleInt = angleSum / 100;
  //   angleSum = 0;
  //   count = 0;
  //   // Some more smoothing of acceleromter reading - sends the new angle only if it differes from the previous one by +-2
  //   if (angleInt > lastAngle + 2 || angleInt < lastAngle - 2) {
  //     Serial.println(angleInt);
  //     String angleString = String(angleInt);
  //     //sends the angle value with start marker "s" and end marker "e"
  //     HC12.print("s" + angleString + "e");
  //     delay(10);
  //     lastAngle = angleInt;
  //     angleSum = 0;
  //     count = 0;
  //   }
  // }