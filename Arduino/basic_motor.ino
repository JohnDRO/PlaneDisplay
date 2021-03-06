// 
// Project : Plane Display
// B.
//
// This code receive three angles as input.
// It then drives a stepper motor thanks to a DRV8825
// DRV8825 is a nice Texas Instrument IC. Each pulse on its STEP pin will increase the step position of the motor by one.
//
// Useful links :
// Driving the DRV8825 from an Arduino : http://gilles.thebault.free.fr/spip.php?article29


int incomingByte = 0;   // for incoming serial data
String inString = "";
String Trame = "";

int angle[3];
int index = 0;


int STEP = 4;
int DIR = 3;
int duree_pas = 1000; // durée d'un pas en us


int angle_mot;

int i;
int diff;


void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        
        // init angle
        angle[0] = 0;
        angle[1] = 0;
        angle[2] = 0;

        // STEP & DIR DRV8825 's pins
        pinMode(STEP, OUTPUT);
        pinMode(DIR, OUTPUT);
        
        angle_mot = 0;
}

void loop() {

        // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();
                
                inString += (char) incomingByte;
                Trame += (char) incomingByte;
                
                if (incomingByte == '.' && index < 3) { // format trame : "AngleX.AngleY.AngleZ.\n"                   
                    angle[index] = Trame.toInt();
                    index = index + 1;
                    
                    Trame = ""; 
                }
                
                if (incomingByte == '\n') { // Ligne complète               
                 if (angle[0] >= 0 && angle[0] <= 360) { // angle OK
                   if (angle[0] >= angle_mot) {
                     digitalWrite(DIR, HIGH);
                     diff = angle[0] - angle_mot;
                   }
                   else {
                     digitalWrite(DIR, LOW);
                     diff = angle_mot - angle[0];
                   }
                   
                   Serial.print("diff angle : ");
                   Serial.print(diff);
                   Serial.print("\n");
                   
                   
                   int nb_pas = diff / 1.8;
                   
                   Serial.print("nb pas  : ");
                   Serial.print(nb_pas);
                   Serial.print("\n");
                   
                   
                   for (i = 0; i < nb_pas; i++) {
                    digitalWrite(STEP, 1);
                    delayMicroseconds(duree_pas / 2);
                    digitalWrite(STEP, 0);
                    delayMicroseconds(duree_pas / 2);
                   }
                   
                   angle_mot = (int) angle[0];
                 }
                 else { // KO
                   ;
                 }

                  angle[0] = 0;
                  angle[1] = 0;
                  angle[2] = 0;
                  
                  // Clear the UART data
                  inString = "";
                  Trame = "";
                }
        }
}
 
