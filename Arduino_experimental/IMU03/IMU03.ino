#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;

#define Gyr_Gain 0.00763358 

#define LED_PIN 13
bool blinkState = false;

float AccelX;
float AccelY;
float AccelZ;

float GyroX;
float GyroY;
float GyroZ;

float mixX;
float mixY;

float pitchAccel, rollAccel;

long previousMillis = 0;    // set up timers
unsigned long currentMillis;    
long interval = 20;        // time constant for timers

float dt=0.02;

void setup() {
  Wire.begin();
  accelgyro.initialize();
  pinMode(LED_PIN, OUTPUT);  
  Serial.begin(115200);
}

void loop() {

  currentMillis = millis();          // bookmark current time
  
  if (currentMillis - previousMillis >= interval)      // if the right amount of time has passed then...
      {
      previousMillis = currentMillis;                      // boomark the time again
    
      accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
      AccelX = ax;
      AccelY = ay;
      AccelZ = az;
      GyroX = Gyr_Gain * (gx);
      GyroY = Gyr_Gain * (gy)*-1;
      GyroZ = Gyr_Gain * (gz);
    
      AccelY = (atan2(AccelY, AccelZ) * 180 / PI);
      AccelX = (atan2(AccelX, AccelZ) * 180 / PI);

      float K = 0.8;
      float A = K / (K + dt);
    
      mixX = A *(mixX+GyroX*dt) + (1-A)*AccelY;    
      mixY = A *(mixY+GyroY*dt) + (1-A)*AccelX; 
     
       
      //Serial.print(AccelY);
      //Serial.print(" , ");
      //Serial.print(GyroX);
      //Serial.print(" , ");
            
      Serial.print(mixY, 4);
      Serial.print(",");
      Serial.print(mixX, 4);
      Serial.print('\n');

      // blink LED to indicate activity
      blinkState = !blinkState;
      digitalWrite(LED_PIN, blinkState);
    
      }
      


}








