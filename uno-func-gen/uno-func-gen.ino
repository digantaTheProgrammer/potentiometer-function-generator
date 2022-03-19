#include <stdlib.h>
 
int d_i=2000;   //Initial Delay 

const int32_t max_req_voltage = 1000; //Max Voltage required
const int outPin = 5;  
const int outClock = A0;
const int outRate = A1;

const uint8_t FREQ_CPU = 16; //16MHz
const uint16_t DEAD_ZONE = 58; //clock cycles required for pin state change. Experimentally measured, should change with FREQ_CPU.
const uint8_t DELAY_LOOP_CYCLE = 4; //Microcontroller dependent. Should not change
const uint16_t MULTIPLIER = 1000; //Should not change
const uint16_t MAX_VOLTAGE = 5*MULTIPLIER; //Should not change
const uint16_t steps =2*MULTIPLIER;//No. of Steps in 0-5V, should not change
const uint16_t steps_del = 42; //Should not change, Experimentally measured
const uint16_t max_steps = (max_req_voltage*steps)/MAX_VOLTAGE;

void(* reset) (void) = 0;

int calc_submultiple() {
  static_assert((FREQ_CPU%DELAY_LOOP_CYCLE)==0);
  static_assert(max_steps > 2);
  static_assert(steps_del<(steps-max_steps));
  return ((MAX_VOLTAGE*(FREQ_CPU/DELAY_LOOP_CYCLE))*(1000000/steps))/steps;
}

int hold_loops;

const int submultiple = calc_submultiple();
void setup() {
pinMode(outClock, OUTPUT);
pinMode(outRate, OUTPUT);
pinMode(outPin, OUTPUT);
digitalWrite(outClock, HIGH);
digitalWrite(outRate, LOW);

Serial.begin(9600);
Serial.setTimeout(INT32_MAX);
Serial.print(submultiple);
Serial.print(" ");
Serial.print(4);
Serial.print(" ");
Serial.println(100);
Serial.flush();

String value = Serial.readStringUntil('\n');
long scanrate = strtol(value.c_str(),0,10);

hold_loops = round((submultiple*1.0f)/scanrate);
float calc_scanrate = (submultiple*1.0f)/hold_loops;

if (scanrate < 50 || scanrate  > 1250)
   {
   Serial.println("error");
   Serial.flush();
   reset();
   }

if(abs(calc_scanrate - scanrate)>1) {
  Serial.println("error");
  Serial.flush();
  reset();
}

Serial.println(calc_scanrate);
Serial.flush();
} 

__attribute__((always_inline)) inline void pwmGen(const int low, const int high, const int val) {
       for(int j=low;j<high;j++) { 
          digitalWrite(outPin, HIGH); 
          _delay_loop_2(val); 
          digitalWrite(outPin, LOW); 
          _delay_loop_2(steps-val-steps_del); 
        }  
}

void loop() { 
    delay(d_i); 
    int i = 2,ei;
    bool increasing = true;
    digitalWrite(outRate, increasing);

    noInterrupts();
    while(true){
       ei=(increasing)?i:max_steps-i;
       digitalWrite(outClock, LOW);
       pwmGen(0, hold_loops/2, ei);
       digitalWrite(outClock, HIGH);
       pwmGen(hold_loops/2, hold_loops, ei);
       if(i==max_steps-1) {
          i=1;
          increasing=!increasing;
          digitalWrite(outRate, increasing);
        }
        i++;               
    }
 }
