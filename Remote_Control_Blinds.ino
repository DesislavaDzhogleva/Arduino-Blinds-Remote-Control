#include <Stepper.h>

#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>


// iRemote PIN and library variables
const int RECV_PIN = 12;
IRrecv irrecv(RECV_PIN);
decode_results results;

// Remote Control commands
unsigned long openBlindsHEX = 0x20df02fd;
unsigned long closeBlindsHEX = 0x20df827d;
unsigned long cancelActionHEX = 0x20df22dd;


// Actions and cooldown
unsigned long startedOpeningAt = 0;
unsigned long startedClosingAt = 0;
unsigned int cooldownMillis = 500;


// stepper motor
const int stepsPerRevolution = 2048;


unsigned long prev = 0;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
 Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);

  myStepper.setSpeed(5);
}

void loop() {
  unsigned long now = millis();
  unsigned long timeElapsed = now-prev;
  
  // just started opening, dont listen to commands for some time
  if(startedOpeningAt > 0 && now-startedOpeningAt < cooldownMillis){
    return;
  }
  // just started closing, dont listen to commands for some time
  if(startedClosingAt > 0 && now-startedClosingAt < cooldownMillis){
    return;
  }


  // command the stepper motor based on the actions
    // open
  if(startedOpeningAt != 0){
    myStepper.step(5);
    
  }else if(startedClosingAt != 0){
    // close
    myStepper.step(-5);
  }
  
  // read commands for opening / closing
  if (irrecv.decode(&results)){
        // cancel action
        if(results.value == cancelActionHEX){          
          startedOpeningAt = 0;
          startedClosingAt = 0;

          Serial.println("Cancel Action");
        }

        
         // start opening
        if(results.value == openBlindsHEX){  
          startedClosingAt = 0;        
          startedOpeningAt = now;
          Serial.println("Open Blinds");
        }

        
        // start closing
        else if(results.value == closeBlindsHEX){
          startedOpeningAt = 0;
          startedClosingAt = now;
          Serial.println("Close Blinds");
        }

        
        irrecv.resume();
  }
}
