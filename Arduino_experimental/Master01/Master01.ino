float pitch;
float roll;

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

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial3.begin(115200);
  Serial1.begin(115200);
}

void loop() {


// if there's any serial available, read it:
  while (Serial3.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    pitch = Serial3.parseFloat();
    // do it again:
    roll = Serial3.parseFloat();

    // look for the newline. That's the end of your
    // sentence:
    if (Serial3.read() == '\n') {         
    }   //end / newline for serial3 IMU

        if (Serial1.available() > 0) {   // read handset values
    
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
                 }  //end serial1 handset read

        // *****************main code goes here locked to IMU interrupt******************
  
        Serial.print(pitch,4);
        Serial.print(" , ");
        Serial.print(roll,4);
        Serial.print(" , "); 

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

        //******************main code end****************************
      
    } //end serial3 IMU read

    

}  // end  of main loop
