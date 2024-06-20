#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#define buzzer 3
#include <Wire.h>
#include "RTClib.h"
#include "StringSplitter.h"

float stime;
uint8_t shour;
uint8_t sminute;
float           floatnow=0;

/////// Seting RTC via sermon
char c;
String phrase;
int Y,M,D,Hh,Mm,Ss;

RTC_DS3231 rtc;
float la   = -7.271733;  //-7.271733, 112.736177
float lo   = 112.736177;
float zona = 7;
float iht  = 0.03;

uint8_t   hD = 0;
uint8_t   hM = 0;
uint16_t  hY = 0;
    
float sholatT[7];
uint8_t dotSt = 0;
char locBuff[50];

//analog clock
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords


#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_TFTLCD.h> // Hardware-specific library


char buff[50];
char daysOfTheWeek[7][10] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};
const char Buln[12][4]= {"Jan","Feb","Mar","Apr","Mei","Jun","Jul","Ags","Sep","Okt","Nop","Des"};

void Pray_Time(float TimeZone, float Latitude, float Longitude,float Altitude,float Declination, float EquationOfTime );

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
int ss,mm,hh,d,m,y;

TFT_eSPI tft=TFT_eSPI();

void setup(void) {

  Serial.begin(115200);   

    //tft.begin(ID);
    tft.init();
    tft.setRotation(3);
    rtc.begin();

    tft.fillScreen(BLACK);
    //tft.setRotation(3);
    pinMode(buzzer,OUTPUT);
    tft.fillScreen(BLACK);
    fiil();
    
    // rtc only
    if(! rtc.begin()) {
      Serial.println("RTC tidak terhubung, Cek kembai wiring! ");
      Serial.flush();
      while (1);
      }
    if (! rtc.lostPower()) {
      Serial.println("RTC tidak bekerja, Setel ulang Waktu!");
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      //rtc.adjust(DateTime(2020, 4, 22, 19, 42, 0));
      }
     Serial.println("Test modul RTC DS 3231!");
     delay (500);
     print_intro(); 
  
}

void loop(void) {
  updateTime();
  DateTime now = rtc.now();
  ss = now.second();
  mm = now.minute();
  hh = now.hour();
  d  = now.day();
  m  = now.month();
  y  = now.year();
  sholatCal();
  //analogClock ();
  print_intro(); 

  while (Serial.available() > 0) {
    c = Serial.read();
    phrase += c;
    if (c == '\n') {
      Serial.println();
      decode_c();
    }
  }




  
  tft.fillRect(80, 0, 160, 25, BLACK); //x,y,w,h,c
  sprintf(buff,"((( %02d:%02d:%02d )))",hh,mm,ss);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 3);
  tft.println(buff);
  tft.setTextColor(MAGENTA);
  //sprintf(buff,"%02d/%02d/%02d",d,m,y);
  sprintf(buff,"%02d %s %02d",d,Buln[m-1],y);
  tft.setTextSize(2);
  tft.setCursor(44, 74);
  tft.println(buff);
  
  /// judul jws///  /// 
  /*
  tft.setTextColor (RED);
  tft.setTextSize(2);
  tft.setCursor(1,95);
  tft.println(F("Jadwal Sholat "));
  */
  /*
  // Angaka jam 12
  tft.setTextColor (RED);
  tft.setTextSize(1);
  tft.setCursor(60,120);
  tft.println(F("12"));

  tft.setTextColor (RED);
  tft.setTextSize(1);
  tft.setCursor(115,171);
  tft.println(F("3"));
  
  tft.setTextColor (RED);
  tft.setTextSize(1);
  tft.setCursor(60,220);
  tft.println(F("6"));

  tft.setTextColor (RED);
  tft.setTextSize(1);
  tft.setCursor(8,171);
  tft.println(F("9"));
  */
  
  
  ///////// Menampilkan hari///////////
  tft.fillRect(0,30,220,30,GREEN);
  tft.setTextColor(BLACK,GREEN);
  tft.setTextSize(2);
  tft.setCursor(ss*1.8,34); //2.1
  tft.print(F("Hari:"));
  tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
  /*
  stime = sholatT[0]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(5, 153);
  tft.print("Subuh =");
  tft.print(locBuff);

  stime = sholatT[2]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.setCursor(5, 183);
  tft.print("Dhuha =");
  tft.print(locBuff);

  stime = sholatT[3]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(BLUE);
  tft.setTextSize(2);
  tft.setCursor(5, 213);
  tft.print("Dzuhur=");
  tft.print(locBuff);
  */

  stime = sholatT[0]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(1, 105);
  tft.print("Subuh =");
  tft.print(locBuff);

  stime = sholatT[2]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.setCursor(1, 130);
  tft.print("Dhuha =");
  tft.print(locBuff);

  stime = sholatT[3]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(BLUE);
  tft.setTextSize(1);
  tft.setCursor(1, 158);
  tft.print("Dzuhur=");
  tft.print(locBuff);
  
  stime = sholatT[4]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.setCursor(83, 105);
  tft.print("Ashar =");
  tft.print(locBuff);

  stime = sholatT[5]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.setCursor(83, 130);
  tft.print("Magrib=");
  tft.print(locBuff);

  stime = sholatT[6]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-(float)shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(83, 158);
  tft.print("Isya  =");
  tft.print(locBuff);
  
  if (hh == 0 && mm == 0 && ss < 5){fiil();}
  delay(500);
  check_time();
}

/*
//Jam analog
void analogClock () {
  boolean initial = 1;
  uint16_t xpos = 65; // x posisi jam

    sdeg = ss * 6;
    mdeg = mm * 6 + sdeg * 0.01666667;
    hdeg = hh * 30 + mdeg * 0.0833333; 
    hx = cos((hdeg - 90) * 0.0174532925);
    hy = sin((hdeg - 90) * 0.0174532925);
    mx = cos((mdeg - 90) * 0.0174532925);
    my = sin((mdeg - 90) * 0.0174532925);
    sx = cos((sdeg - 90) * 0.0174532925);
    sy = sin((sdeg - 90) * 0.0174532925);

    if (ss == 0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
      tft.drawLine(ohx, ohy, xpos,  171, BLACK);
      ohx = hx * 35 + xpos + 1;
      ohy = hy * 35 +  171;
      tft.drawLine(omx, omy, xpos,  171, BLACK);
      omx = mx * 45 + xpos;
      omy = my * 45 +  171;
    }

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    tft.drawLine(osx, osy, xpos,  171, BLACK);
    osx = sx * 50 + xpos + 1;
    osy = sy * 50 +  171;
    tft.drawLine(osx, osy, xpos,  171, RED);
    tft.drawLine(ohx, ohy, xpos,  171, CYAN);
    tft.drawLine(omx, omy, xpos,  171, WHITE);
    tft.drawLine(osx, osy, xpos,  171, RED);
    tft.fillCircle(xpos,  171, 3, RED);
    //tft.drawCircle(xpos,171,60,WHITE);
    //tft.drawRoundRect(xpos-50,  121, 100, 100, 10, WHITE);
    tft.drawRoundRect(xpos-60,  115, 120, 120,10, WHITE);
}

*/

void fiil(){
       /*
      tft.fillRect(0, 60, 200, 40, GREEN);
      tft.fillRect(0, 150, 157, 25, BLUE);
      tft.fillRect(0, 180, 157, 25, CYAN);
      tft.fillRect(0, 210, 157, 25, GREEN);
      tft.fillRect(160, 150, 310, 25, YELLOW);
      tft.fillRect(160, 180, 310, 25, RED);
      tft.fillRect(160, 210, 310, 25, MAGENTA); 
      */
      //tft.fillRect(30, 68, 150, 25, GREEN); // Tanggal
      tft.drawRoundRect(30, 68, 150, 25, 3, RED); // Tanggal
      tft.fillRoundRect(0, 96, 75, 25, 3, BLUE); // Subuh
      tft.fillRoundRect(0, 122, 75, 25, 3, CYAN); // Duha
      tft.fillRoundRect(0, 148, 75, 25, 3, GREEN); // Duhur
      tft.fillRoundRect(80, 96, 75, 25, 3, YELLOW); //Ashar
      tft.fillRoundRect(80, 122, 75, 25, 3, RED); // Magrib
      tft.fillRoundRect(80, 148, 75, 25, 3, MAGENTA); //Isya

}

void check_time()
    {
      for(uint8_t i=0; i <7; i++)
        {
          if (not((i == 1) or (i == 2))){
            if( (floatnow >= (sholatT[i])) and (floatnow < (sholatT[i]+0.015)))
              { 
               digitalWrite(buzzer, HIGH);
               delay(5000);
               digitalWrite(buzzer, LOW);
               for (int x = 0; x < 10; x++){
                  digitalWrite(buzzer, HIGH);
                  delay(1000);
                  digitalWrite(buzzer, LOW);
                  delay(1000);
               }
               delay(50000);
              }
          }
       }
    }


void updateTime()
    {
      DateTime now = rtc.now();
      floatnow = float(now.hour()) + float(now.minute())/60 + float(now.second())/3600;
      //Serial.println(floatnow);
    }

void decode_c(){
  if (phrase.charAt(phrase.length() - 1) == '\n') {
        Serial.print(phrase);
        StringSplitter *splitter = new StringSplitter(phrase, ',', 6);  
        int itemCount = splitter->getItemCount();
        Serial.println("Data count: " + String(itemCount));
       
        Y = splitter->getItemAtIndex(0).toInt();
        M = splitter->getItemAtIndex(1).toInt();
        D = splitter->getItemAtIndex(2).toInt();
        Hh = splitter->getItemAtIndex(3).toInt();
        Mm = splitter->getItemAtIndex(4).toInt();
        Ss = splitter->getItemAtIndex(5).toInt();
        
        //validasi
        if((Y<2020)||(M>12)||(D>31)||(hh>24)||(mm>60)||(ss>60)){
          Serial.println("Pengaturan tidak diterima!");
          Serial.println("Sesuaikan dengan tanggal dan waktu yang berlaku");
          Serial.println();
        }
        else{
          Serial.println("Pengaturan diterima!");
          rtc.adjust(DateTime(Y, M, D, Hh, Mm, Ss));
          Serial.print("Mohon tunggu sebentar .");
          int count=0;
          while(count != 10){
            Serial.print(".");
            count++;
            delay(500);     
          } 
        }
        Serial.println();
        phrase="";
        delay(2000);
  } 
}
    

void print_intro(){
  Serial.println("### Konfigurasi DS3231 Via Serial Command ###");
  Serial.println("    format -> Y,M,D,h,m,s");
  Serial.println("    contoh -> 2020,5,10,7,0,0");
  Serial.println("              10 mei 2020 jam 7:00");
  Serial.println();

    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
  delay(500);
}
