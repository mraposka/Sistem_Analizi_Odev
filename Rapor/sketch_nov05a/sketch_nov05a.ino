/*------------------------------------------------*/  
char junk;
String sendStr = "";
String inputString = "";
int fPin = 22;
int lPin = 44;
int seatsNumber = lPin - fPin + 1;
String seats[23];
String loopVar = "a";
#define TIMEOUT_ATTEMPTS 1000 // how many rounds to do in the loop before deciding to timeout
#define DISCHARGE_FACTOR 4 
void setup() {
  Serial.begin(9600);    
  pinMode(2, OUTPUT);
  digitalWrite(2,1);
  for (int i = fPin-2; i <= lPin; i++)
    pinMode(i, INPUT);
     
  for (int i = 0; i < seatsNumber; i++) {
    loopVar = String(i);
    seats[i] = "k_" + loopVar + "_0_0";
  }

}

void loop() {  
 if(Serial.available()){
  while (Serial.available())
    {
      char inChar = (char)Serial.read();     //Giriş değerlerini oku.
      inputString += inChar;                 //Seri monitörden gelen değerleri stringe çevir.
    } 
   if(inputString=="x"){
      for (int i = 0; i < seatsNumber; i++) { 
      int agirlik=readSensor(4);
      t(String(agirlik));
      if(agirlik>20)
      agirlik=1;
      else
      agirlik=0;
      int koltuk=digitalRead(22+i);
      seats[i]="k_" + String(i) + "_"+String(koltuk)+"_"+String(agirlik);
      delay(500);
      }
      for (int i = 0; i < seatsNumber; i++) {
        sendStr=sendStr+seats[i]+",";
      } 
      inputString=""; 
   } 
   if(sendStr!="")
 {Serial.println(sendStr);
 sendStr="";inputString="";
 } 
 else
 {inputString="";sendStr="";}
 }
 
}

void t(String x)
{
  Serial.println(x);
}

int readSensor(uint8_t senseAndChargePin)
{ 
    uint8_t myPin_mask = digitalPinToBitMask(senseAndChargePin);
    volatile uint8_t *myPin_port = portInputRegister(digitalPinToPort(senseAndChargePin));
 
    // Start charging the capacitor with the internal pullup
    int left = TIMEOUT_ATTEMPTS;
    noInterrupts();
    pinMode(senseAndChargePin, INPUT_PULLUP);
 
    // Charge to a HIGH level, somewhere between 2.6V (practice) and 3V (guaranteed)
    // Best not to use analogRead() here because it's not really quick enough
    // Want to do as little as possible in this loop to get good resolution
    do
    {
        left--;
    } while (((*myPin_port & myPin_mask) == 0) && left>0); // An iteration takes approximately 1us
 
    interrupts();
    pinMode(senseAndChargePin, INPUT);  //Stop charging
    int roundsMade = TIMEOUT_ATTEMPTS - left;
 
    // Discharge is slower than charge, typically goes through a 100K resistor where charge is a ~40K resistor
    // Charge time is approximately "roundsMade" micro-seconds, so use that approximation for discharge delay as well
    delayMicroseconds(roundsMade * DISCHARGE_FACTOR);
 
    return roundsMade;
}
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
