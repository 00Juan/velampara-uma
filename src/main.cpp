#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);
int stepindex=0;
bool flagNTC=false;
bool flagMicro=false;

bool cmdRelayON=false;

int pinRelay =7;
int pinNTC=1;
int pinMicro=0;

// --- Configuration ---
const int NTC_PIN = A1;      // Pin to read the NTC voltage
const int RELAY_PIN = 7;     // Pin to activate the relay
const int RESET_PIN = A0;    // NEW: Analog pin for the reset sensor

// --- Tuning Parameters ---
const float ALPHA = 0.05;    // Controls the NTC filter's "memory"
const int TRIGGER_THRESHOLD = 30; // NTC delta to trigger the relay ON
const int RESET_THRESHOLD = 512;  // Value (0-1023) for RESET_PIN to
                                  // trigger the relay OFF. 512 is the
                                  // midpoint (2.5V).

// --- Global Variables ---
float filteredValue = 0.0;   // Holds the "normal" NTC average
bool isRelayActive = false; // This is our main state flag (latch)

bool checkSuddenChange();
void printDebug(int current, float filtered, int delta);
void controlMethodSimple(bool &relay);
void controlMethodSimple2(bool &relay);

void debug()
{
  Serial.print(analogRead(A0));
  Serial.print("    ");
  Serial.println(analogRead(A1));
}


void setup() {

  Serial.begin(9600);
  pinMode(pinRelay,OUTPUT);
}

void loop() {
 
bool a;
 controlMethodSimple2(a);
  digitalWrite(pinRelay,cmdRelayON);
  debug();
  
}

void controlMethodSimple2(bool &relay)
{

  int valMicroAnalog=analogRead(A0);
  int valNTCAnalog=analogRead(A1);

  bool stsTriggerMicro=valMicroAnalog>800;
  bool stsTriggerNTC=checkSuddenChange();

  if(stsTriggerNTC)
  {
    cmdRelayON=false;
    Serial.println("ONNNN");
  }
  
  else if(stsTriggerMicro)
  {
      cmdRelayON=true;
      Serial.println("OFFFFF");
  }

}



void controlMethodSimple(bool &relay)
{

  int valMicroAnalog=analogRead(A0);
  int valNTCAnalog=analogRead(A1);

  bool stsTriggerMicro=valMicroAnalog>600;
  bool stsTriggerNTC=checkSuddenChange();
  if(stsTriggerMicro && !flagMicro)
  {
    cmdRelayON=false;
    flagMicro=true;
  }
  else if(!stsTriggerMicro && flagMicro)
  {
    flagMicro=false;
  }


  if(stsTriggerNTC && !flagNTC)
  {
    cmdRelayON=true;
    flagNTC=true;
  }
  else if(!stsTriggerNTC && flagNTC)
  {
    flagNTC=false;
  }

}


bool checkSuddenChange() {
  bool returnValue=false;
  // 1. Get the current raw reading
  int currentValue = analogRead(0);

  // 2. Update the slow-moving average
  filteredValue = (ALPHA * currentValue) + ((1.0 - ALPHA) * filteredValue);

  // 3. Calculate the difference (delta)
  // (Heat on NTC = lower resistance = lower analog value)
  int delta = filteredValue - currentValue;

  // 4. Check for a trigger event
  if (delta > TRIGGER_THRESHOLD) {
    // --- EVENT DETECTED ---
    isRelayActive = true; // Set the latch!
    
    Serial.print("Lighter DETECTED! Delta: ");
    Serial.println(delta);
    Serial.println("RELAY LATCHED ON.");
    returnValue=true;
  }
  
  // Optional: Uncomment to see the NTC values for tuning
   printDebug(currentValue, filteredValue, delta);
   return returnValue;
}


void printDebug(int current, float filtered, int delta) {
  Serial.print("Current: ");
  Serial.print(current);
  Serial.print("\t Average: ");
  Serial.print((int)filtered);
  Serial.print("\t Delta: ");
  Serial.println(delta);
}