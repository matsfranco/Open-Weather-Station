#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

#define bufferLen 10

String comm;
int serialConnected;

/**********************************************************/
/************************* DHT11 PARAMS *******************/
/**********************************************************/
#define dhtPin A1
DHT dht(dhtPin, DHT22);
float dhtTemp;
float dhtHumi;

void get_dhtTemp() {
  int i;
  float v = 0;
  for(i = 0; i < bufferLen; i++) {
    v += dht.readTemperature();
  }
  dhtTemp = v/bufferLen;
}

void get_dhtHumi() {
  int i;
  float v = 0;
  for(i = 0; i < bufferLen; i++) {
    v += dht.readHumidity();
  }
  dhtHumi = v/bufferLen;
}
/**********************************************************/


/**********************************************************/
/********************** BMP180 PARAMS *********************/
/**********************************************************/
Adafruit_BMP085 bmp;
float bmpTemp;
float bmpPres;

void get_bmpTemp() {
  int i;
  float v = 0;
  for(i = 0; i < bufferLen; i++) {
    v += bmp.readTemperature();
  }
  bmpTemp = v/bufferLen;
}

void get_bmpPres() {
  int i;
  float v = 0;
  for(i = 0; i < bufferLen; i++) {
    v += bmp.readPressure();
  }
  bmpPres = v/bufferLen;
}
/**********************************************************/

/**********************************************************/
/********************** FC-37 PARAMS **********************/
/**********************************************************/
#define fc37Pin A2
bool rain;

void rainCheck() {
  float v = 0;
  int i;
  for(i = 0; i < 10*bufferLen; i++) {
    if(analogRead(fc37Pin) < 800) v += 1;
  }
  v = v/10*bufferLen;
  if(v > 0.7) {
    rain = 1; // It's raining, man!
  } else {
    rain = 0;
  }
}
/**********************************************************/

/**********************************************************/
/********************* BH1750 PARAMS **********************/
/**********************************************************/
int ambLum;
int BH1750_address = 0x23; // i2c Addresse
byte buff[2];

void BH1750_Init(int address){
  
  Wire.beginTransmission(address);
  Wire.write(0x10); // 1 [lux] aufloesung
  Wire.endTransmission();
}

byte BH1750_Read(int address){
  
  byte i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()){
    buff[i] = Wire.read(); 
    i++;
  }
  Wire.endTransmission();  
  return i;
}

void get_ambLum() {
  float valf=0;

  if(BH1750_Read(BH1750_address)==2){
    
    valf=((buff[0]<<8)|buff[1])/1.2;
    
    if(valf<0) ambLum = 65535;
    else ambLum = ((int)valf); 
  }
}

/**********************************************************/



/**********************************************************/
/****************** SERIAL PORT READING *******************/
/**********************************************************/
void readSerial(){   
  while(Serial.available() > 0){
    int c = Serial.read();
    if(isDigit(c)) {      
      comm  += (char)c; //INCREMENTA LETRA POR LETRA    
    }
    if(c == '\n') break; 
  }
}
/**********************************************************/


/**********************************************************/
/******************** BUZZER COMMANDER ********************/
/**********************************************************/
#define buzzer 2
void signalBeep(int TONE, int t) {
  tone(buzzer,TONE);   
  delay(t);
  noTone(buzzer);
}
/**********************************************************/

/**********************************************************/
/******************** COMMAND EXECUTE *********************/
/**********************************************************/
void commExecute() {
   if(comm.length() > 0) {
    //Serial.println(comm);
    int idCode = comm.toInt();
    if(serialConnected == 1) {  // CONECTADO
      
      switch(idCode) {
        
        case 1: // Responde com valor da media entre DHT11 e BMP180
          get_bmpTemp();
          get_dhtTemp();
          Serial.println((bmpTemp+dhtTemp)/2);
          signalBeep(1500,100);
        break;
  
        case 2: // Responde com umidade relativa do DHT11
          get_dhtHumi();
          Serial.println(dhtHumi);
          signalBeep(1500,100);
        break;

        case 3: // Responde com pressao do BMP180
          get_bmpPres();
          Serial.println(bmpPres/100);
          signalBeep(1500,100);
        break;

        case 4: // Responde se há precipitacao ou nao
          rainCheck();
          Serial.println(rain);
          signalBeep(1500,100);
        break;
  
        case 5: // Responde com luminosidade em lux
          get_ambLum();
          Serial.println(ambLum);
          signalBeep(1500,100);
        break;

        case 6: // Disconnect
          serialConnected = 0;
          Serial.print("serialConnected: ");
          Serial.println(serialConnected);
          digitalWrite(12,HIGH);
          signalBeep(500,70);
          signalBeep(500,70);
          signalBeep(500,70);
        break;

        default: // Comandos nao listados
          signalBeep(500,100);
          signalBeep(500,100);
        break;
      }
    } else {
      if(idCode == 0) { // NAO CONECTADO
        serialConnected = 1;
        Serial.print("serialConnected: ");
        Serial.println(serialConnected);
        digitalWrite(12,LOW);
        signalBeep(1000,100);
        signalBeep(1000,100);
      } 
    }
  }
}
/**********************************************************/

void setup() {
  serialConnected = 0;
  Serial.begin(9600);
  Wire.begin();
  BH1750_Init(BH1750_address);
  dht.begin();
  bmp.begin();
  pinMode(buzzer,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(12, LOW);
  signalBeep(1000,500);
}

void loop() {
  readSerial();
  commExecute();
  comm = "";
}
