//////////       scheme to include timing data


//for clock
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;

// for lights
// 1.68A Full draw white light; roughly 0.047A perbulb
#include <Adafruit_NeoPixel.h>
#define ledStripPin 3
Adafruit_NeoPixel stairLights = Adafruit_NeoPixel(36, ledStripPin, NEO_GRB + NEO_KHZ800);


int schemeChoice=0;
int scheme[][5][3]=
{
  {
    {
      11,255,164            }
    ,
    {
      10 ,229,232            }
    ,
    {
      2,160,255            }
    ,
    {
      10,69,232            }
    ,
    {
      27,3,255            }
  }
  ,
  {
    {
      20,204,82            }
    ,
    {
      11 ,255,162            }
    ,
    {
      255,81,0            }
    ,
    {
      153,92,0            }
    ,
    {
      204,131,20            }
  }
  ,
  {
    {
      255,146,0            }
    ,
    {
      255,98,12            }
    ,
    {
      255,61,0            }
    ,
    {
      232,30,12            }
    ,
    {
      255,2,64            }
  }
  ,
  {
    {
      204,59,0            }
    ,
    {
      153,60,23            }
    ,
    {
      255,0,4            }
    ,
    {
      50,255,48            }
    ,
    {
      73,204,0            }
  }
};



int 

selectedColour[3]={
  0,0,0}
,
selectedColour2[3]={
  0,0,0}
,
selectedColour3[3]={
  0,0,0}
,selectedColour4[3]={
  0,0,0}
,selectedColour5[3]={
  0,0,0}
,
offColour[3] = {
  0,0,0}
,
lastColour[3] = {
  0,0,0};
boolean finished = 0;






//for light sensor
#define ldrPin 14  
int ldrThresh = 60;
boolean lightOn=1;

//radio
#include <VirtualWire.h>
#define rfPin 6

// pir
#define pirPin 9
boolean 
goingUp=0, 
goingDown=0, 
finishUp = 0, 
finishDown = 0, 
hasPlayed=0;
int sensorTimeoutValue=1500;
int autoShutdownTime=20000;

// tempory testing shit
unsigned long  
sensorTimeout=millis()-1500,
autoShutdown=millis();


void setup () {
  randomSeed(analogRead(ldrPin));
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_rx_pin(rfPin);
  vw_setup(300);  // Bits per sec
  vw_rx_start(); 
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  stairLights.begin();
  stairLights.show();
  //rtc.adjust(DateTime(2014, 12, 30, 15, 50, 0));
  pinMode(ldrPin,INPUT);
  pinMode(rfPin,INPUT);

}

void loop () {

  //Serial.print("waiting");
  // Room brightness check.



  if(analogRead(ldrPin)>ldrThresh)lightOn=0;
  else lightOn=1;

  //Serial.println(analogRead(ldrPin));

  schemeChooser();
  //schemeChoice=3;

  for(int i=0; i<3; i++){
    for (int i = 0;i<3;i++)
    {
      selectedColour[i]=scheme[schemeChoice][0][i];
      selectedColour2[i]=scheme[schemeChoice][1][i];
      selectedColour3[i]=scheme[schemeChoice][2][i];
      selectedColour4[i]=scheme[schemeChoice][3][i];
      selectedColour5[i]=scheme[schemeChoice][4][i];

    }
  }

  if (lightOn==1 || hasPlayed == 1){

    if (millis()-autoShutdown>autoShutdownTime && hasPlayed == 1){

      goingUp=0;
      goingDown=0;
      hasPlayed=0;

      changeAll(lastColour, offColour,1,0);

    }

    stairCheck();
    stairAction();
  }

  // End loop()
}













void changeStep(int _stair_number, int _from_colour[], int _to_colour[],int _fade_speed, int _delay){

  // starts from 0,1,2,3...11
  int stair_number = _stair_number * 3;
  int directions[3] = {
    0,0,0      };
  int from_colour[3]={
    0,0,0          };
  int to_colour[3]={
    0,0,0          };
  int finished = 0;



  for(int i=0; i<3; i++){
    from_colour[i] = _from_colour[i];
    to_colour[i] = _to_colour[i];

    // if(_to_colour[0]!=0 && _to_colour[1]!=0 && _to_colour[2]!=0){
    //     lastColour[i]= _to_colour[i];
    //   }

    if(from_colour[i] < to_colour[i]) {
      directions[i]=1;
    }
    else if(from_colour[i] > to_colour[i]){
      directions[i]=0;
    }
    else directions[i]=2;
  }


  while(finished==0){
    //
    //stairCheck(); ////////////////////////////////////////////////////////////////////////////////////////////
    //
    int counter = 0;
    for(int i=0; i<3; i++){




      if(from_colour[i]>to_colour[i]){
        from_colour[i] -= _fade_speed;
      }
      else if(from_colour[i]<to_colour[i]){
        from_colour[i] += _fade_speed;
      }
      else if(from_colour[i]==to_colour[i]){
        counter++;
      }

      if(from_colour[i]>to_colour[i] && directions[i]==1){
        from_colour[i]=to_colour[i];
        counter++;
      }
      else if(from_colour[i]<to_colour[i] && directions[i]==0){
        from_colour[i]=to_colour[i];
        counter++;
      }

      stairLights.setPixelColor(stair_number+i, stairLights.Color(from_colour[0],from_colour[1],from_colour[2]));
      stairLights.show();
      delay(_delay);
    }

    if (counter>=3) finished = 1;
  }
}






void changeAll(int _from_colour[], int _to_colour[],int _fade_speed, int _delay){

  // starts from 0,1,2,3...11
  int directions[3] = {
    0,0,0      };
  int from_colour[3]={
    0,0,0          };
  int to_colour[3]={
    0,0,0          };
  int finished = 0;



  for(int i=0; i<3; i++){
    from_colour[i] = _from_colour[i];
    to_colour[i] = _to_colour[i];

    if(from_colour[i] < to_colour[i]) {
      directions[i]=1;
    }
    else if(from_colour[i] > to_colour[i]){
      directions[i]=0;
    }
    else directions[i]=2;
  }


  while(finished==0){
    //
    //stairCheck(); ////////////////////////////////////////////////////////////////////////////////////////////
    //
    int counter = 0;
    for(int i=0; i<3; i++){

      // if(_to_colour[0]!=0 && _to_colour[1]!=0 && _to_colour[2]!=0){
      //       lastColour[i]= _to_colour[i];
      //     }



      if(from_colour[i]>to_colour[i]){
        from_colour[i] -= _fade_speed;
      }
      else if(from_colour[i]<to_colour[i]){
        from_colour[i] += _fade_speed;
      }
      else if(from_colour[i]==to_colour[i]){
        counter++;
      }

      if(from_colour[i]>to_colour[i] && directions[i]==1){
        from_colour[i]=to_colour[i];
        counter++;
      }
      else if(from_colour[i]<to_colour[i] && directions[i]==0){
        from_colour[i]=to_colour[i];
        counter++;
      }

      for(int i=0; i<36; i++){
        stairLights.setPixelColor(i, stairLights.Color(from_colour[0],from_colour[1],from_colour[2]));


      }
      stairLights.show();

      delay(_delay);

    }

    if (counter>=3) finished = 1;
  }
}









// Check for motion and select lighting patterns.
void stairCheck (){

  // radio
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    if (millis()-sensorTimeout >sensorTimeoutValue){
      if(buf[0]=='1'){
        if (goingUp==0){
          goingDown=1;
        }
        else if (goingUp == 1){
          finishUp = 1;
          goingUp = 0;

        }
      }
      sensorTimeout=millis();

    }  
  }

  // local
  if (millis()-sensorTimeout >sensorTimeoutValue){
    boolean pirReading = digitalRead(pirPin);
    if(pirReading==1){
      if(goingDown == 0){
        goingUp = 1;
      }
      else if(goingDown == 1){
        finishDown = 1;
        goingDown = 0 ;
      }   

      sensorTimeout=millis();
    }
  }
}








void stairAction(){
  int ranNum = random(0, 100), choice = 0;

  if(ranNum >66) choice=1;
  else if(ranNum>33) choice = 2;   
  else if (ranNum<=33) choice = 3; 
  else choice = 1;
  // int choice = 3;

  if(goingUp == 1 && hasPlayed == 0){

    upSequence(choice);

    autoShutdown = millis();

    hasPlayed = 1;

  }
  else if(goingDown == 1 && hasPlayed == 0){

    downSequence(choice);


    autoShutdown = millis();

    hasPlayed = 1;

  }
  else if (finishUp == 1){
    for(int i=0; i<12; i++){
      changeStep(i,lastColour  ,offColour   ,1,0);
    }

    finishUp = 0 ;
    goingUp = 0;
    hasPlayed = 0;
    sensorTimeout=millis();
  }
  else if (finishDown == 1){


    for(int i=11; i>=0; i--){
      changeStep(i,lastColour  ,offColour   ,1,0);
    }

    finishDown = 0 ;
    goingDown = 0;
    hasPlayed=0;
    sensorTimeout=millis();



  }
}



void upSequence(int _selection){
  switch (_selection) {
  case 1:
    for (int i = 0;i < 12;i++) {
      changeStep(i, offColour, selectedColour, 4,0);
    }


    changeAll(selectedColour,selectedColour2,2,0);

    changeAll( selectedColour2, selectedColour3, 2,0);





    for (int i = 0;i < 12;i++) {
      changeStep(i, selectedColour3,selectedColour4,2,0);
    }

    changeAll(selectedColour4, selectedColour5, 1,0);

    for(int i=0; i<3; i++){
      lastColour[i]=selectedColour5[i];

    }

    break;
  case 2:
    for(int i=0; i<12; i+=3){
      changeStep(i, offColour, selectedColour,4,0);
      changeStep(i+1, offColour, selectedColour3,3,0);
      changeStep(i+2, offColour, selectedColour5,2,0);
    }
    for(int i=0; i<12; i+=3){
      changeStep(i, selectedColour, selectedColour2,2,0);
      changeStep(i+1, selectedColour3, selectedColour2,2,0);
      changeStep(i+2, selectedColour5, selectedColour2,1,0);
    }
    changeAll(selectedColour2, selectedColour,1,0);
    for(int i=0; i<3; i++){
      lastColour[i]=selectedColour[i];

    }
    break;


  case 3:
    for(int i=0; i<12; i+=2){
      changeStep(i, offColour, selectedColour,3,0);
    }
    for(int i=1; i<12; i+=2){
      changeStep(i, offColour, selectedColour4,3,0);
    }
    for(int i=0; i<12; i+=2){
      changeStep(i, selectedColour, selectedColour4,3,0);
    }
    changeAll(selectedColour4, selectedColour5,1,0);


    for(int i=0; i<3; i++){
      lastColour[i]=selectedColour5[i];
    }
    break;

  default:
    break;
  }

}

void downSequence(int _selection){
  switch (_selection) {
  case 1:
    for (int i = 11;i >= 0 ;i--) {

      changeStep(i, offColour, selectedColour, 4,0);
    }
    changeAll(selectedColour,selectedColour2,2,0);

    changeAll( selectedColour2, selectedColour3, 2,0);





    for (int i = 11;i >= 0 ;i--) {
      changeStep(i, selectedColour3,selectedColour4,2,0);
    }

    changeAll(selectedColour4, selectedColour5, 1,0);

    for(int i=0; i<3; i++){
      lastColour[i]=selectedColour5[i];

    }


    break;


  case 2:
    for(int i=12; i>=0; i-=3){
      changeStep(i, offColour, selectedColour,4,0);
      changeStep(i-1, offColour, selectedColour3,3,0);
      changeStep(i-2, offColour, selectedColour5,2,0);
    }
    for(int i=12; i>=0; i-=3){
      changeStep(i, selectedColour, selectedColour2,2,0);
      changeStep(i-1, selectedColour3, selectedColour2,2,0);
      changeStep(i-2, selectedColour5, selectedColour2,2,0);
    }
    changeAll(selectedColour2, selectedColour,1,0);
    for(int i=0; i<3; i++){
      lastColour[i]=selectedColour[i];

    }
    break;



  case 3:
    for(int i=12; i>=0; i-=2){
      changeStep(i, offColour, selectedColour,3,0);
    }
    for(int i=11; i>=0; i-=2){
      changeStep(i, offColour, selectedColour4,3,0);
    }
    for(int i=12; i>=0; i-=2){
      changeStep(i, selectedColour, selectedColour4,3,0);
    }
    changeAll(selectedColour4, selectedColour5,1,0);


  default:
    break;
  }

}

void schemeChooser(){

  // Get date
  DateTime now = rtc.now();
  //Serial.println(analogRead(ldrPin));

  // Select colour based on time of day
  if (now.hour()>=6 && now.hour() < 9){     // Early Morning
    schemeChoice = 0;
  }
  else if(now.hour()>=9 && now.hour() < 15){    // Morning to afternoon
    schemeChoice = 1;
  }
  else if(now.hour()>=15 && now.hour() < 19){     // afternoon to evening
    schemeChoice = 2;
  }
  else if(now.hour()>=19 || now.hour()<6){      // evening to night
    schemeChoice = 3;
  }


}



