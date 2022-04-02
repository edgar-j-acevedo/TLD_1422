
//NOTE: Confirm sketch pin-out and wiring schematic match, discrepancies will result in failure.
//defining analog pins for demux and mux
#define OUT_pin A1
#define OUT_pin_1 A2 

int fsrVoltage;               // Value for storing arduino ADC to voltage - 'map' function in line 125-127.
unsigned long fsrResistance;  // The voltage converted to resistance
unsigned long fsrConductance; // variable is unused - will use next week 4/4
long fsrForce;               // Finally, the resistance converted to force

int teamLegDayArray[19][16]; //setting up the matrix size
//Mux control pins for analog signal (SIG_pin) default for arduino mini pro
const byte s0 = 13; 
const byte s1 = 12;
const byte s2 = 11;
const byte s3 = 10;

//used for writing to deMUX1 or deMUX2
const byte en1 = 3; 
const byte en2 = 2;

//deMux control pins for Output signal (OUT_pin) default for arduino mini pro
//Both deMux use these pins, output signal is controlled with enabler pins
const byte w0 = 9;
const byte w1 = 8;
const byte w2 = 7;
const byte w3 = 6;

//Mux in "SIG" pin default for arduino mini pro
const byte SIG_pin = 0; // Analog read pin for single multiplexer. 

//Logic table for MUX and the two deMUX
const boolean muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

//setup function for setting pin mode
void setup(){
   
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
 
  pinMode(w0, OUTPUT);
  pinMode(w1, OUTPUT);
  pinMode(w2, OUTPUT);
  pinMode(w3, OUTPUT);

  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);

  pinMode(OUT_pin, OUTPUT);
  pinMode(OUT_pin_1, OUTPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
 
  digitalWrite(w0, LOW);
  digitalWrite(w1, LOW);
  digitalWrite(w2, LOW);
  digitalWrite(w3, LOW);

  Serial.begin(115200); //baud rate for Arduino Pro Mini. Confirm Baud rate on python function 'def readSerial():'
 
}
void loop(){
      //for loop to set up 19by16 matrix with zeros
     for(byte j = 0; j < 19; j ++)
     {
      if (j < 16)
       {
        writeMux(j);
       }
      else
        {
        writeMux(j-16);
        }
      for(byte i = 0; i < 16; i ++)
      teamLegDayArray[j][i] = 0;
      }
     
   //Nested for loops for controlling EN pins, 
   for(byte j = 0; j < 19; j ++)
    {
     if (j < 16)
     {
        digitalWrite(OUT_pin, HIGH);
        digitalWrite(OUT_pin_1, LOW);
        digitalWrite(en1, LOW); //Activates 1st DeMUX
        digitalWrite(en2, HIGH); //deactivates 2nd DeMUX
        writeMux(j);
     }      
     else
     {
        digitalWrite(OUT_pin, LOW);
        digitalWrite(OUT_pin_1, HIGH);
        digitalWrite(en1, HIGH); //Deactivates 1st DeMUX
        digitalWrite(en2, LOW); //Activates 2nd DeMUX
        writeMux(j-16);
      }
      //serial print for matrix Alignment. Removed in 'def readSerial():'- python. Removal of this line results in matrix shift
      Serial.print(j);
      Serial.print('\t');

      // 
      for(byte i = 0; i < 16; i ++)
      {
        teamLegDayArray[j][i] = teamLegDayArray[j][i] + readMux(i);
        fsrVoltage = map(teamLegDayArray[j][i], 0, 720, 0, 3700); //mapping bits to mV
       // Work on converting bits to mmHg - Roger & Edgar "FILL IN HERE"
        Serial.print(fsrVoltage);
        Serial.print('\t');
      }
      
     Serial.println();
     
    }
    //for(;;){ } //uncomment to print matrix once. Used for testing alignment and image quality between Arduino-Python
}

//Multiplexer analogRead function.
int readMux(byte channel){
  byte controlPin[] = {s0, s1, s2, s3};//Uses Logic table from line 34
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  int val = analogRead(SIG_pin);
  return val;
}

//functions below to write to both Demultiplexers
void writeMux(byte channel){
  byte controlPin[] = {w0, w1, w2, w3}; //Uses Logic table from line 34
  
  for(byte i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
}

