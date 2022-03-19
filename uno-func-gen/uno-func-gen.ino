#include <stdlib.h>

float scanrate;     //scan rate in mV/s 
int d_i=2000;   //Initial Delay 
  
const int steps =2000;//No. of Steps in 0-5V 
const int outPin = 5;  
 
void(* reset) (void) = 0;
 
void setup() { 
Serial.begin(9600);
Serial.setTimeout(60000);
Serial.print("Enter scanrate (mv/s) [50 - 1250] : ");
String value = Serial.readStringUntil('\r');
scanrate = strtol(value.c_str(),0,10);
if (scanrate < 50 || scanrate  > 1250)
   {
   Serial.println("Wrong scanrate");
   Serial.flush();
   reset();
   }
   Serial.println(scanrate);
} 
 
void loop() { 
     float t=5000/((steps/1000)*(steps/1000)*scanrate); 
     int T=(int)t;
     int dT = (t-T)*(steps);
     int cutVoltage = steps/5;
    analogWrite(outPin, 1);                               
    delay(d_i); 
    int i = 0,m,ei;
    while(true) { 
       ei=(m%2)?cutVoltage-i:i;
       pinMode(outPin, OUTPUT); 
       for(int j=0;j<T;j++) { 
          digitalWrite(outPin, HIGH); 
          delayMicroseconds(ei); 
          digitalWrite(outPin, LOW); 
          delayMicroseconds(steps-ei); 
        }
	if(dT) {
	pinMode(outPin, INPUT);
	delayMicroseconds(dT);
	}
	if(i==cutVoltage) {i=1;m++;}
	i++;
    } 
 }