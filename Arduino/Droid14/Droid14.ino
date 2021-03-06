// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class using DMP (MotionApps v2.0)
// 6/21/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added seamless Fastwire support
//                 - added note about gyro calibration
//      2012-06-21 - added note about Arduino 1.0.1 + Leonardo compatibility error
//      2012-06-20 - improved FIFO overflow handling and simplified read process
//      2012-06-19 - completely rearranged DMP initialization code and simplification
//      2012-06-13 - pull gyro and accel data from FIFO packet instead of reading directly
//      2012-06-09 - fix broken FIFO read sequence and change interrupt detection to RISING
//      2012-06-05 - add gravity-compensated initial reference frame acceleration output
//                 - add 3D math helper file to DMP6 example sketch
//                 - add Euler output and Yaw/Pitch/Roll output formats
//      2012-06-04 - remove accel offset clearing for better results (thanks Sungon Lee)
//      2012-06-01 - fixed gyro sensitivity to be 2000 deg/sec instead of 250
//      2012-05-30 - basic DMP initialization working

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h

#include "Wire.h"

#include <PID_v1.h>  //PID loop from http://playground.arduino.cc/Code/PIDLibrary

#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;


// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

/* =========================================================================
   NOTE: In addition to connection 3.3v, GND, SDA, and SCL, this sketch
   depends on the MPU-6050's INT pin being connected to the Arduino's
   external interrupt #0 pin. On the Arduino Uno and Mega 2560, this is
   digital I/O pin 2.
 * ========================================================================= */

/* =========================================================================
   NOTE: Arduino v1.0.1 with the Leonardo board generates a compile error
   when using Serial.write(buf, len). The Teapot output uses this method.
   The solution requires a modification to the Arduino USBAPI.h file, which
   is fortunately simple, but annoying. This will be fixed in the next IDE
   release. For more info, see these links:

   http://arduino.cc/forum/index.php/topic,109987.0.html
   http://code.google.com/p/arduino/issues/detail?id=958
 * ========================================================================= */




// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL


#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// XRobots vars

int ch1;
int ch2;
int ch3;
int ch3a;
int ch4;
int ch4a;
int ch5;
int ch5a;
int ch6;
int ch6a;
int but1 = 1;
int but2 = 1;
int but3 = 1;
int but4 = 1;
int but5 = 1;
int but6 = 1;

int current_pos_headturn;  // variables for smoothing head rotation
int target_pos_headturn;
int pot_headturn;   // target position/inout
int diff_headturn; // difference of position
double easing_headturn;

int current_pos_drive;  // variables for smoothing main drive
int target_pos_drive;
int pot_drive;   // target position/inout
int diff_drive; // difference of position
double easing_drive;

int current_pos_trousers;  // variables for smoothing trousers
int target_pos_trousers;
int pot_trousers;   // target position/inout
int diff_trousers; // difference of position
double easing_trousers;

int current_pos_head1;  // variables for smoothing head back/forward
int target_pos_head1;
int pot_head1;   // target position/inout
int diff_head1; // difference of position
double easing_head1;

int varServo1;
int varServo2;

float roll;
float pitch;
float bodge = 7;

int pot;
int BTstate = 0;

double Pk1 = 12;  //speed it gets there
double Ik1 = 0;
double Dk1 = 0.05;
double Setpoint1, Input1, Output1, Output1a;    // PID variables - tousers SERVO

PID PID1(&Input1, &Output1, &Setpoint1, Pk1, Ik1 , Dk1, DIRECT);    // PID Setup - tousers SERVO


double Pk2 = 0.6; // position it goes to
double Ik2 = 0;
double Dk2 = 0;
double Setpoint2, Input2, Output2, Output2a;    // PID variables - Trouser stability

PID PID2(&Input2, &Output2, &Setpoint2, Pk2, Ik2 , Dk2, DIRECT);    // PID Setup - Trouser stability


double Pk3 = 3; // position it goes to
double Ik3 = 0;
double Dk3 = 0.4;
double Setpoint3, Input3, Output3, Output3a;    // PID variables - Main drive motor

PID PID3(&Input3, &Output3, &Setpoint3, Pk3, Ik3 , Dk3, DIRECT);    // Main drive motor




// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}



// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)

    Wire.begin();
    TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz). Comment this line if having compilation difficulties with TWBR.

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    Serial.begin(115200);
    Serial1.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
    // Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
    // the baud timing being too misaligned with processor ticks. You must use
    // 38400 or slower in these cases, or use some kind of external separate
    // crystal solution for the UART timer.

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    /* // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again

    */

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);

    pinMode(31, OUTPUT);  // enable pin
    pinMode(33, INPUT);  // BT paired status
    pinMode(3, OUTPUT);  // enable pinMode
    pinMode(5, OUTPUT);  // enable pin

    servo1.attach(9);
    servo2.attach(10);
    servo3.attach(8);


    // *********** PID setup ***********

    PID1.SetMode(AUTOMATIC);              // PID Setup - trousers SERVO
    PID1.SetOutputLimits(-255, 255);
    PID1.SetSampleTime(20);

    PID2.SetMode(AUTOMATIC);              // PID Setup - trousers Stability
    PID2.SetOutputLimits(-255, 255);
    PID2.SetSampleTime(20);

    PID3.SetMode(AUTOMATIC);              // PID Setup - main drive motor
    PID3.SetOutputLimits(-255, 255);
    PID3.SetSampleTime(20);

    
    }



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        // other program behavior stuff here
        // .
        // .
        // .
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
        // .
        // .
        // .
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        //mpu.resetFIFO();  // added by XRobots
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            

            // ****************XRobots main loop code*****************************//
            
            pitch = (ypr[1] * 180/M_PI);
            roll = (ypr[2] * 180/M_PI);
/*
            Serial.print("Roll: ");
            Serial.print(roll);
            Serial.print("  Pitch: ");
            Serial.print(pitch);
*/
         #endif



    if (Serial1.available() > 0) {

    // look for the next valid integer in the incoming serial stream:

        ch1 = Serial1.parseInt();
        ch2 = Serial1.parseInt();
        ch3 = Serial1.parseInt();
        ch4 = Serial1.parseInt();
        ch5 = Serial1.parseInt();
        ch6 = Serial1.parseInt();
        but1 = Serial1.parseInt();
        but2 = Serial1.parseInt();
        but3 = Serial1.parseInt();
        but4 = Serial1.parseInt();  //enable pin
        but5 = Serial1.parseInt();
        but6 = Serial1.parseInt();  
          
        
        // look for the newline. That's the end of your sentence:
        if (Serial1.read() == '\n') {     
        } 
             }
/*
        Serial.print ("  Remote: ");
        Serial.print (ch1);
        Serial.print (" , ");
        Serial.print (ch2);
        Serial.print (" , ");
        Serial.print (ch3);
        Serial.print (" , ");
        Serial.print (ch4);
        Serial.print (" , ");
        Serial.print (ch5);
        Serial.print (" , ");
        Serial.print (ch6);
        Serial.print (" , ");
        Serial.print (but1);
        Serial.print (" , ");
        Serial.print (but2);
        Serial.print (" , ");
        Serial.print (but3);
        Serial.print (" , ");
        Serial.print (but4);  //enable pin
        Serial.print (" , ");
        Serial.print (but5);
        Serial.print (" , ");
        Serial.println (but6);
*/


        //********** BT enable******************
        
        BTstate = digitalRead(33);

        if(but4 == 0 && BTstate == 1) {
          digitalWrite(31, HIGH);

          }
        else if (but4 == 1 || BTstate == 0) {
          digitalWrite(31, LOW);
        }

        //**********Trousers stability PID**************
         target_pos_trousers = map(ch1, 0,512,30,-30);
      
         easing_trousers = 50;          //modify this value for sensitivity
         easing_trousers /= 1000;
        
         // Work out the required travel.
         diff_trousers = target_pos_trousers - current_pos_trousers;    
        
         // Avoid any strange zero condition
         if( diff_trousers != 0.00 ) {
            current_pos_trousers += diff_trousers * easing_trousers;
         }

         Setpoint2 = current_pos_trousers;

         pot = analogRead(A0);   // read trousers pot

         Input2 = (roll*-1)+bodge;   // ****add a bit to the IMU to get the real middle point

         Setpoint2 = map(ch2, 0,512,30,-30); 
         Setpoint2 = constrain(Setpoint2, -45,45);
         PID2.Compute();
         Setpoint1 = Output2;

         pot = map(pot, 0, 1024, -255,255);
         pot = pot+36;
         
         Input1  = pot;
         Input1 = constrain(Input1,-45,45);
         Setpoint1 = constrain(Setpoint1, -45,45);

         PID1.Compute();

         // ************** drive trousers motor *************

         if (Output1 <= -1)                                      // decide which way to turn the wheels based on deadSpot variable
            {
            Output1a = abs(Output1);
            analogWrite(5, Output1a);                                // set PWM pins 
            analogWrite(3, 0);
            }
         else if (Output1 >= 1)                          // decide which way to turn the wheels based on deadSpot variable
            { 
            Output1a = abs(Output1);
            analogWrite(3, Output1a);  
            analogWrite(5, 0);
            } 
         else
            {
            analogWrite(5, 0);  
            analogWrite(3, 0);
            }

         //***************Main Drive PID***************


         target_pos_drive = map(ch1, 0,512,65,-65);
      
         easing_drive = 100;          //modify this value for sensitivity
         easing_drive /= 1000;
        
         // Work out the required travel.
         diff_drive = target_pos_drive - current_pos_drive;    
        
         // Avoid any strange zero condition
         if( diff_drive != 0.00 ) {
            current_pos_drive += diff_drive * easing_drive;
         }

         Setpoint3 = current_pos_drive;
      
         Input3 = pitch+3;

         PID3.Compute();

         
         if (Output3 <= 1)                                      // decide which way to turn the wheels based on deadSpot variable
            {
            Output3a = abs(Output3);
            analogWrite(11, Output3a);                                // set PWM pins 
            analogWrite(12, 0);
            }
         else if (Output3 > 1)                          // decide which way to turn the wheels based on deadSpot variable
            { 
            Output3a = abs(Output3);
            analogWrite(12, Output3a);  
            analogWrite(11, 0);
            }

         //****************move head servo******************

         ch3a=map(ch3,50,590,0,180);
         ch3a = ch3a-(pitch*4.5);
         ch3a=constrain(ch3a,0,180);

         target_pos_head1 = ch3a;
      
         easing_head1 = 100;          //modify this value for sensitivity
         easing_head1 /= 1000;
        
         // Work out the required travel.
         diff_head1 = target_pos_head1 - current_pos_head1;    
        
         // Avoid any strange zero condition
         if( diff_head1 != 0.00 ) {
            current_pos_head1 += diff_head1 * easing_head1;
            }

         varServo1 = current_pos_head1;
         varServo2 = current_pos_head1;

         ch4a=map(ch4,0,512,-50,50);
         
         varServo1 = varServo1-ch4a;
         varServo2 = varServo2+ch4a;
       
         
         varServo2 = map(varServo2,0,180,180,0);

         varServo1 = constrain(varServo1,10,170);
         varServo2 = constrain(varServo2,10,170);


         if (but4 == 1) {      // enable pin is off
          servo1.write(60);    //set servos to back/middle position
          servo2.write(120); 
         }

         else if (but4 == 0) {
         servo1.write(varServo1);    //set servos to stick positions
         servo2.write(varServo2); 
         }

         //****************spin flywheel***************************

         ch6a = map(ch6,0,512,255,-255);

         if (ch6a <= -10)                                      // decide which way to turn the wheels based on deadSpot variable
         {
            ch6a = abs(ch6a);
            analogWrite(6, ch6a);                                // set PWM pins 
            analogWrite(7, 0);
            }
         else if (ch6a > 10)                          // decide which way to turn the wheels based on deadSpot variable
            { 
            ch6a = abs(ch6a);
            analogWrite(7, ch6a);  
            analogWrite(6, 0);
            }

         else {
            analogWrite(7, 0);  
            analogWrite(6, 0);
         }

         //********************* turn head***************************

         target_pos_headturn = map(ch5, 0,512,600,2500);
      
         easing_headturn = 50;          //modify this value for sensitivity
         easing_headturn /= 1000;
        
         // Work out the required travel.
         diff_headturn = target_pos_headturn - current_pos_headturn;    
        
         // Avoid any strange zero condition
         if( diff_headturn != 0.00 ) {
            current_pos_headturn += diff_headturn * easing_headturn;
         }
      
         servo3.writeMicroseconds(current_pos_headturn);

             

    }
}
