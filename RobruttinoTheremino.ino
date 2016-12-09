/* Robruttino Theremino
 * --------------------
 * FabLab Palermo 2016
 * --------------------
 * Firmware version 0.1
 */

// Configurazione PIN
#define LIGHT  A0     // resistenza 10K
#define SPK    4      // resistenza 51ohm
#define BIGLED 5      // resistenza 120ohm
#define LED    6      // resistenza 120ohm
#define IR     7

// Caratteristiche
#define ENERGY      8  // Controlla la luminosità dei led e le pause fra i toni
#define VIGILANCY   5  // Controlla il tempo di relax e l'attività
#define CRAZYNESS   8  // Controlla la velocità dei toni e i comportamenti casuali
#define SENSITIVITY 9  // Controlla la sensibilità alle variazioni di luce
#define VOICETONE   5  // Controlla il tono (bassa-alta frequenza) dei buzz

// Parametri derivati
int maxLedPower = map(ENERGY, 0, 10, 0, 100); 
int relaxTime = map(VIGILANCY, 0, 10, 0, 200);
int interval = map(CRAZYNESS, 10, 0, 0, 100);
int toneDuration = ENERGY * interval / 10;
int tollerance = map(SENSITIVITY, 10, 0, 0, 100);
int maxTone = map(VOICETONE, 0, 10, 0, 5000);

// Valori variabili
int prevLight = 0;
int relax = 0;

void setup() {

  pinMode(SPK, OUTPUT);
  pinMode(BIGLED, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(LIGHT, INPUT);
  pinMode(IR, INPUT);

  analogWrite(BIGLED, maxLedPower );
  analogWrite(LED, maxLedPower );
  
  tone(SPK, maxTone/CRAZYNESS, interval*ENERGY); 
  delay(interval*ENERGY);
  tone(SPK, maxTone/SENSITIVITY, interval*VIGILANCY); 
  delay(interval*VIGILANCY);
  
}

void loop() {

  int obstacle = !digitalRead(IR);
  int currentLight = analogRead(LIGHT);    
  int ledPower = map(currentLight, 0, 1023, 0, maxLedPower);
  int buzz = map(currentLight,0,1023, 0, maxTone);
  int diff = abs(prevLight - currentLight);
  
  // sotto una certa soglia di luce il led si spegne
  if(currentLight <= SENSITIVITY * 10) 
    ledPower = 0;

  // Il sensore IR viene chiuso
  if (obstacle) {    
    tone(SPK,buzz, toneDuration);   
    analogWrite(BIGLED, ledPower); 
    relax=0;    
  } else {    
    relax++;
    relax=constrain(relax,0,1000);
  }  

  // RELAX MODE: il relaxTime è trascorso senza disturbi
  if(relax > relaxTime){    
    
    // Il led piccolo lampeggia in funzione di CRAZYNESS
    int smallEye = random(0,ledPower);
    if(smallEye % CRAZYNESS){
        smallEye = 0;    
    }
    analogWrite(LED, smallEye); 

    // Il led grande si accende in funzione di VIGILANCY
    int bigEye = 0;
    if(random(0,100) < VIGILANCY){    
      bigEye = ledPower;
      tone(SPK,50, toneDuration);
    }
    analogWrite(BIGLED, bigEye);
       
  }

  // Il sensore LIGHT rileva una differenza sensibile
  if (diff > tollerance){
   analogWrite(BIGLED, ledPower);
   analogWrite(LED, ledPower);
   prevLight = currentLight;
   relax -= VIGILANCY;
   if(relax>relaxTime){
    tone(SPK,buzz, toneDuration);
   }
  }
  
  delay(interval);
}
