#include <EasyTransfer.h>

//create object
EasyTransfer ET; 

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;
  int16_t ch5;
  int16_t ch6;
  int16_t but1;
  int16_t but2;
  int16_t but3;
  int16_t but4;
  int16_t but5;
  int16_t but6;
  
};

SEND_DATA_STRUCTURE mydata;


int state = 0;

long previousMillis;
long interval = 40;

void pair()

{
   digitalWrite(9, LOW);  // turn LED on pin 9 off
      
   while(state == 0) {
          state = digitalRead(8);  // check to see when BT is paired
          }

   digitalWrite(9, HIGH);   // turn LED on pin 9 on
   delay(200);
   digitalWrite(9, LOW);   // turn LED on pin 9 off
   delay(200);
   digitalWrite(9, HIGH);   // turn LED on pin 9 on
   delay(200);
   digitalWrite(9, LOW);   // turn LED on pin 9 off
   delay(500); // wait  before transmitting data
   digitalWrite(9, HIGH);   // turn LED on pin 9 on   
}

void setup()
{
   Serial.begin(115200); 
   pinMode  (2, INPUT_PULLUP);
   pinMode  (3, INPUT_PULLUP);
   pinMode  (4, INPUT_PULLUP);
   pinMode  (5, INPUT_PULLUP);
   pinMode  (6, INPUT_PULLUP);
   pinMode  (7, INPUT_PULLUP);
   pinMode  (8, INPUT_PULLUP);
   pinMode  (9, OUTPUT);

   ET.begin(details(mydata), &Serial);

   pair();
}


void loop()
{  

  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis; 

  state = digitalRead(8);  // check to see when BT is paired
  if (state == 0) {
    pair();
  }
   
  mydata.but1 = digitalRead(2);
  mydata.but2 = digitalRead(3);
  mydata.but3 = digitalRead(4);
  mydata.but4 = digitalRead(5);
  mydata.but5 = digitalRead(6);
  mydata.but6 = digitalRead(7);
  mydata.ch1 = analogRead(A0); 
  mydata.ch2 = analogRead(A1); 
  mydata.ch3 = analogRead(A2); 
  mydata.ch4 = analogRead(A3); 
  mydata.ch5 = analogRead(A4);
  mydata.ch6 = analogRead(A5);

  mydata.ch1 = map(mydata.ch1, 0,1023,512,0);
  mydata.ch2 = map(mydata.ch2, 0,1023,512,0);
  mydata.ch3 = map(mydata.ch3, 0,1023,512,0);
  mydata.ch4 = map(mydata.ch4, 0,1023,512,0);
  mydata.ch5 = map(mydata.ch5, 0,1023,512,0);
  mydata.ch6 = map(mydata.ch6, 0,1023,512,0);

  ET.sendData();
     
  } 

}




