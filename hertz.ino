//Frequency timer
// Author: Nick Gammon
// Date: 10th February 2012

volatile boolean first;
volatile boolean triggered;
volatile unsigned long overflowCount;
volatile unsigned long startTime;
volatile unsigned long finishTime;

// here on rising edge
void setup () {
  Serial.begin(115200);       
  Serial.println("Frequency Counter");
  
  // reset Timer 1
  TCCR1A = 0;
  TCCR1B = 0;
  // Timer 1 - interrupt on overflow
  TIMSK1 = _BV (TOIE1);   // enable Timer1 Interrupt
  // zero it
  TCNT1 = 0;     
  // start Timer 1
  TCCR1B =  _BV (CS10);  //  no prescaling

  // set up for interrupts
  prepareForInterrupts ();   
  
} // end of setup
void isr () 
{
  unsigned int counter = TCNT1;  // quickly save it
  
  // wait until we noticed last one
  if (triggered)
    return;

  if (first)
    {
    startTime = (overflowCount << 16) + counter;
    first = false;
    return;  
    }
    
  finishTime = (overflowCount << 16) + counter;
  triggered = true;
  detachInterrupt(0);   
}  // end of isr

// timer overflows (every 65536 counts)
ISR (TIMER1_OVF_vect) 
{
  overflowCount++;
}  // end of TIMER1_OVF_vect


void prepareForInterrupts ()
  {
  // get ready for next time
  EIFR = _BV (INTF0);  // clear flag for interrupt 0
  first = true;
  triggered = false;  // re-arm for next time
  attachInterrupt(0, isr, RISING);     
  }  // end of prepareForInterrupts
  



void loop () 
  {

  if (!triggered)
    return;

  unsigned long elapsedTime = finishTime - startTime;
  float freq = 1.0 / ((float (elapsedTime) * 62.5e-9));  // each tick is 62.5 nS
  
  Serial.print ("Took: ");
  Serial.print (elapsedTime);
  Serial.print (" counts. ");

  Serial.print ("Frequency: ");
  Serial.print (freq);
  Serial.println (" Hz. ");

  // so we can read it  
  delay (500);

  prepareForInterrupts ();   
}   // end of loop
