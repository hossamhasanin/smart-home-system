#include <TimedAction.h>

#include <LiquidCrystal.h>

// temp
#define tempsensor A0 
const int Air_condi_Led = 1 ;
const int Heating_Led = 0 ;
int reading ;
float temp = 0.0;

// lcd
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// water
int motor=6;
int trigPin = 9;//white
int echoPin = 10;//brown
int waterlvl;

char* snetences_list[] = {
    // temprature
    "Tmp is" ,
    // cooler on or heater
    "No cooler or heater",
    // water level
    "No water lvl is ",
    // pumb on or off
    "pumb off"
  };

void measureTemp(){
  // Temperature alarm  
  reading = analogRead(tempsensor);
  temp = (reading *(5.0/1024.0)*100.0);
  if(temp >= 30 ){
     digitalWrite(Air_condi_Led,HIGH);
     strcpy(snetences_list[1] , "Cooler is on");
  }
  else if(temp <= 20 ){
     digitalWrite(Heating_Led,HIGH);
     strcpy(snetences_list[1] , "Heater is on"); 
   }
  else if (temp >= 20 and temp <=30 ){
     digitalWrite(Air_condi_Led,LOW);
     digitalWrite(Heating_Led,LOW);
     strcpy(snetences_list[1] , "Temp is good"); 
   }
}

void measureWaterLvl(){
  // water level  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  waterlvl= duration*0.034/2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance:");
  Serial.println(waterlvl);

  controlPumb();
}

void controlPumb(){
  // start the pumb
  if (waterlvl > 100){
    digitalWrite(motor,HIGH);
    strcpy(snetences_list[3] , "Pumb on");
  }else{
    digitalWrite(motor,LOW);
    strcpy(snetences_list[3] , "Pumb off");
  }
}

TimedAction tempThread = TimedAction(700,measureTemp);
TimedAction waterThread = TimedAction(700,measureWaterLvl);

void setup() {
lcd.begin(16, 2);  
pinMode(Air_condi_Led,OUTPUT);
pinMode(Heating_Led,OUTPUT);
// water lvl
pinMode(motor,OUTPUT);
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
 
}
void loop(){
  
  tempThread.check();
  waterThread.check();
  // lcd in the main thread to display the sentences we want to show
  // the lcd functionality in the main thread becasue it is the most frequante operation
  // the temp thread is suposed to happen every specific time interval measures the temp and change the value 
  // in the snetences_list array about the temprature;
  int i;
  if (temp != 0 and waterlvl != 0){
    for (i = 0; i <= sizeof(snetences_list)/sizeof(char *); i++){
  
      tempThread.check();
      waterThread.check();
      
      lcd.setCursor(0, 0);
      lcd.print(snetences_list[i]);
  
      if (i == 0){
        lcd.setCursor(0, 1);
        lcd.print(temp);
      } else if (i == 2){
        // write the water lvl
        lcd.setCursor(0, 1);
        lcd.print(waterlvl);
      }
  
      delay(700);
      lcd.clear();
    }
  }
  
}
