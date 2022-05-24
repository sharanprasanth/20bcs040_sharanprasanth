/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPLfzv0GFVS"
#define BLYNK_DEVICE_NAME "Energy Meter"
#define BLYNK_AUTH_TOKEN "BzQnJ7YIHEl3_uBKOU1Yz2fVZkZcuZPj"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <PZEM004Tv30.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "BzQnJ7YIHEl3_uBKOU1Yz2fVZkZcuZPj";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "FTTH-MADHAN KUMAR";
char pass[] = "04545293197";

int relay1 = 0; //D3 pin relay connect

float Volt = 0;
float Amp = 0;
float Watt = 0;
float W_h = 0;
float Hz = 0;
float pf = 0;

int On_Off = 0;
int olr_cur = 0;
char flag = 0;
char estate_on = 0; 
char estate_off = 0;

/* Use software serial for the PZEM
 * Pin 11 Rx (Connects to the Tx pin on the PZEM)
 * Pin 12 Tx (Connects to the Rx pin on the PZEM)
*/
PZEM004Tv30 pzem(5, 4);

BLYNK_WRITE (V8)
{
  olr_cur = param.asInt();
  
}

BLYNK_WRITE (V6){ //On and Off Button

  On_Off = param.asInt();

  if(On_Off){

    if(!estate_on){
      Serial.println("System ON");     
      Blynk.notify("System ON");

      estate_on = 1;
      estate_off = 0;
      flag = 1; 
    }
  }else{

    if(!estate_off){
      Serial.println("System OFF");       
      Blynk.notify("System OFF");

      estate_off = 1;
      estate_on = 0;
      flag = 0;
    }
  }
}

// This function sends the PZEM004 values to Virtual Pins.
// In the app, Widget's reading Hz should be set to PUSH. 

void Send_Values_To_Blynk(){

    // You can send any value at any time.
    // Please don't send more that 10 values per second.
    // Now, we are sending each time we call this function
    Blynk.virtualWrite(V0, Volt);
    Blynk.virtualWrite(V1, Amp);
    Blynk.virtualWrite(V2, Watt);
    Blynk.virtualWrite(V3, W_h);
    Blynk.virtualWrite(V4, Hz);
    Blynk.virtualWrite(V5, pf);
 
} 

// This function get the values from PZEM004 via serial.

void Get_Values_From_PZEM(){

    Volt = pzem.voltage();
    if( !isnan(Volt) ){
        Serial.print("Voltage: "); Serial.print(Volt); Serial.println("V");
    } else {
        Serial.println("Error reading Voltage");
    }

    Amp = pzem.current();
    if( !isnan(Amp) ){
        Serial.print("Current: "); Serial.print(Amp); Serial.println("A");
    } else {
        Serial.println("Error reading current");
    }

    Watt = pzem.power();
    if( !isnan(Watt) ){
        Serial.print("Instant Power: "); Serial.print(Watt); Serial.println("W");
    } else {
        Serial.println("Error reading Instant Power");
    }

    W_h = pzem.energy();
    if( !isnan(W_h) ){
        Serial.print("Energy: "); Serial.print(W_h,3); Serial.println("kWh");
    } else {
        Serial.println("Error reading Energy");
    }

    Hz = pzem.frequency();
    if( !isnan(Hz) ){
        Serial.print("Frequency: "); Serial.print(Hz, 1); Serial.println("Hz");
    } else {
        Serial.println("Error reading frequency");
    }

    pf = pzem.pf();
    if( !isnan(pf) ){
        Serial.print("PF: "); Serial.println(pf);
    } else {
        Serial.println("Error reading PF factor");
    }

    Serial.println();
    delay(1000);
}

void setup() {

  // Debug console
  Serial.begin(9600);

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, LOW);
  
  Blynk.begin(auth, ssid, pass);
  //Blynk.begin(auth, ssid, pass, "blynk-iot-cm.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(128,199,173,118), 8080);
  
}

void loop() {

  Blynk.run();

  if(flag){
  
  Get_Values_From_PZEM();
  Send_Values_To_Blynk();

  if ( Amp >= olr_cur )
  {
    digitalWrite(relay1, HIGH);
    Blynk.notify("System has been Overloaded and Turned OFF");
    Serial.println("System has been Overloaded and Turned OFF");
    
    Blynk.virtualWrite(V7, "System has been Overloaded and Turned OFF");
    
  }
  else
  {
    digitalWrite(relay1, LOW);
    Blynk.virtualWrite(V7, "System is Good");
    Serial.println("System is Good");
  }

  
  }
  
}
