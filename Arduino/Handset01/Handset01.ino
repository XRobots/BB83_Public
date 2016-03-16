//transmitter

int ch1;
int ch2;
int ch3;
int ch4;
int ch5;
int ch6;
int but1;
int but2;
int but3;
int but4;
int but5;
int but6;

long previousMillis;
long interval = 40;

void setup()
{
   Serial.begin(115200); 
   pinMode  (2, INPUT_PULLUP);
   pinMode  (3, INPUT_PULLUP);
   pinMode  (4, INPUT_PULLUP);
   pinMode  (5, INPUT_PULLUP);
   pinMode  (6, INPUT_PULLUP);
   pinMode  (7, INPUT_PULLUP);
   
}

void loop()
{  

  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis; 
   
  but1 = digitalRead(2);
  but2 = digitalRead(3);
  but3 = digitalRead(4);
  but4 = digitalRead(5);
  but5 = digitalRead(6);
  but6 = digitalRead(7);
  ch1 = analogRead(A0); 
  ch2 = analogRead(A1); 
  ch3 = analogRead(A2); 
  ch4 = analogRead(A3); 
  ch5 = analogRead(A4);
  ch6 = analogRead(A5);  
 
      Serial.print (ch1);
      Serial.print (",");
      Serial.print (ch2);
      Serial.print (",");
      Serial.print (ch3);
      Serial.print (",");
      Serial.print (ch4);
      Serial.print (",");
      Serial.print (ch5);
      Serial.print (",");
      Serial.print (ch6);
      Serial.print (",");
      Serial.print (but1);
      Serial.print (",");
      Serial.print (but2);
      Serial.print (",");
      Serial.print (but3);
      Serial.print (",");
      Serial.print (but4);
      Serial.print (",");
      Serial.print (but5);
      Serial.print (",");
      Serial.print (but6);
      Serial.print ("\n");
     
  } 

}




