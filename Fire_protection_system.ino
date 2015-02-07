#define speakerPin 8
#include <IRremote.h>
//#include "pitches.h"

// 定義七段顯示器各節段與 Arduino 的腳位對應關係
#define ASeg 5
#define BSeg 7
#define CSeg 4
#define DSeg 11
#define ESeg 10
#define FSeg 3
#define GSeg 6

// 定義共陽極與 Arduino 的腳位對應關係
#define CA1 13
#define CA2 12
#define CA3 9
#define CA4 2

// 定義每個七段顯示器顯示的時間 (延遲時間), 預設 1ms 
int delay_time = 1;

////////////////////////////////////////////////////////////////
// 
//   FILE:  dht11_test1.pde
// PURPOSE: DHT11 library test sketch for Arduino
//

//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
	return 1.8 * celsius + 32;
}

// fast integer version with rounding
//int Celcius2Fahrenheit(int celcius)
//{
//  return (celsius * 18 + 5)/10 + 32;
//}


//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
	return celsius + 273.15;
}

// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
	// (1) Saturation Vapor Pressure = ESGG(T)
	double RATIO = 373.15 / (273.15 + celsius);
	double RHS = -7.90298 * (RATIO - 1);
	RHS += 5.02808 * log10(RATIO);
	RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
	RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
	RHS += log10(1013.246);

        // factor -3 is to adjust units - Vapor Pressure SVP * humidity
	double VP = pow(10, RHS - 3) * humidity;

        // (2) DEWPOINT = F(Vapor Pressure)
	double T = log(VP/0.61078);   // temp var
	return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
	double a = 17.271;
	double b = 237.7;
	double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
	double Td = (b * temp) / (a - temp);
	return Td;
}
////////////////////////////////////////////////////////////////////

#include <dht11.h>

dht11 DHT11;

#define DHT11PIN 19

// notes in the melody:
int melody[] = {523, 698};

int duration = 800;  // 500 miliseconds

float temp;   //建立一個浮點型變數temp作為存儲空間準備存放資料
int   warning = 0; //警報開關

const int led1Pin =  15;         // LED pin
const int btnPin =  16;         // btn pin

int ledstatus =0;  //ledstatus
int selectledPin = 15;

long previousTime = 0;          // 用來保存前一次 LED 更新狀態的時間
long interval = 1000;           // 燈號閃爍間隔時間，單位為毫秒(miliseconds)


//--------



int RECY_PIN = 18; //定義紅外線接收腳位2


IRrecv irrecv(RECY_PIN);//紅外線接收腳

decode_results signalresults;//定義解碼結果
unsigned int signal;//= signalresults.value;
char incomingByte;
////////////////////////////////////////////////////////////////////////

void setup(){
  Serial.begin(9600);             //使用9600的串列傳輸速率進行通訊
  //　七段顯示器接腳
  pinMode(ASeg, OUTPUT);
  pinMode(BSeg, OUTPUT);
  pinMode(CSeg, OUTPUT);
  pinMode(DSeg, OUTPUT);
  pinMode(ESeg, OUTPUT);
  pinMode(FSeg, OUTPUT);
  pinMode(GSeg, OUTPUT);
  pinMode(CA1, OUTPUT);
  pinMode(CA2, OUTPUT);
  pinMode(CA3, OUTPUT);
  pinMode(CA4, OUTPUT);
  
  pinMode(led1Pin, OUTPUT);      // 把 ledPin 設置成 output pin 
  
  irrecv.enableIRIn();//紅外線接收腳致能c
}
void loop(){
  if(Serial.available()>0){
    incomingByte=Serial.read();
    if(incomingByte=='N'){
      warning = 0;
      digitalWrite(led1Pin, LOW);
      Serial.println("System:Safe!");
    }
  
  }

    
  if(warning ==0){
    //偵測預警火災
    int V1 = analogRead(A0);        //從A0口讀取電壓資料存入剛剛創建整數型變數V1，類比的電壓測量範圍為0-5V 返回的值為0-1024   
    float vol = V1*(5.0 / 1023.0);  //我們將 V1的值換算成實際電壓值存入浮點型變數 vol
    /*
    Serial.print(vol);            //串列輸出電壓值，並且不換行  
    Serial.println(" V");         //串列輸出字元V，並且換行
    */
      int chk = DHT11.read(DHT11PIN);
      unsigned long startTime = millis();
      for (unsigned long elapsed=0; elapsed < 1000; elapsed = millis() - startTime) {   
      // 顯示c字
      pickDigit(1);
      pickWord('c');
      delay(delay_time);
      // 顯示度字
      pickDigit(2);
      pickWord('o');
      delay(delay_time);
      
      // 顯示個位數
      pickDigit(3);
      pickNumber(((int)DHT11.temperature)%10);
      delay(delay_time);
      // 顯示十位數
      pickDigit(4);
      pickNumber(((int)DHT11.temperature/10)%10);
      delay(delay_time);
      }
   
    if ( vol > 0.2 ){
       warning=1;
       Serial.println("System:Warning!");
    }
   
    //delay(1000);                  //輸出完成後等待1秒鐘，用於控制資料的刷新速度。
  }else if (warning == 1){

  
    //偵測火災時，發出警報聲音  
      unsigned long startTime2 = millis();      
      unsigned long currentTime = millis();
      /*
      if(currentTime - previousTime > interval) {  
      // 切換燈號
      digitalWrite(selectledPin, HIGH);
      selectledPin +=1;
      if (selectledPin == 18)
        selectledPin = 15;
      // 記錄更新時間
      previousTime = currentTime;        
      }
      */      
      if(ledstatus == 1){
        ledstatus = 0;
        digitalWrite(led1Pin, LOW);
      }else{
        ledstatus = 1;
        digitalWrite(led1Pin, HIGH);      
      }
      
      for (unsigned long elapsed=0; elapsed < 1000; elapsed = millis() - startTime2) {   
      // 顯示f字
      pickDigit(4);
      pickWord('f');
      delay1ms(delay_time);

      // 顯示i字
      pickDigit(3);
      pickWord('i');
      delay1ms(delay_time);
      // 顯示r字
      pickDigit(2);
      pickWord('r');
      delay1ms(delay_time);
       // 顯示e字
      pickDigit(1);
      pickWord('e');
      delay1ms(delay_time);
      } 
      
    for (int thisNote = 0; thisNote < 2; thisNote++) {
    // 在 pin8 上輸出聲音，每個音階響 0.5 秒
      tone(8, melody[thisNote], duration);   
      delay1ms(500);
    }
    
    int Vbtn = analogRead(btnpin);        //從A0口讀取電壓資料存入剛剛創建整數型變數V1，類比的電壓測量範圍為0-5V 返回的值為0-1024   
    float vol = Vbtn*(5.0 / 1023.0);
    if(vol > 0.2){
      warning = 0;
      digitalWrite(led1Pin, LOW);
      Serial.println("System:Safe!");
      
    }
 
  }  
}
//＝＝＝＝＝ｓｕｂ　ａｒｅａ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
////////////////
// 選擇顯示的位數 (4:千、3:百、2:十、或 1:個位數)
void pickDigit(int x) {
  digitalWrite(CA1, HIGH);
  digitalWrite(CA2, HIGH);
  digitalWrite(CA3, HIGH);
  digitalWrite(CA4, HIGH);
 
  switch(x) {
    case 1: digitalWrite(CA1, LOW); break;
    case 2: digitalWrite(CA2, LOW); break;
    case 3: digitalWrite(CA3, LOW); break;
    case 4: digitalWrite(CA4, LOW); break;    
  }
}
void pickWord(char x) {
     switch(x) {
     case 'o': degree(); break;
     case 'c': c(); break;
     case 'f': f(); break;
     case 'i': i(); break;
     case 'r': r(); break;
     case 'e': e(); break;
     }  
}

// 顯示文字 'o' 
void degree() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, LOW);
  digitalWrite(DSeg, LOW);
  digitalWrite(ESeg, LOW);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, HIGH); 
}

// 顯示數字 'c' 
void c() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, LOW);
  digitalWrite(CSeg, LOW);
  digitalWrite(DSeg, HIGH);
  digitalWrite(ESeg, HIGH);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, LOW); 
}

// 顯示數字 'f' 
void f() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, LOW);
  digitalWrite(CSeg, LOW);
  digitalWrite(DSeg, LOW);
  digitalWrite(ESeg, HIGH);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, HIGH); 
}

// 顯示數字 'i' 
void i() {
  digitalWrite(ASeg, LOW);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, LOW);
  digitalWrite(ESeg, LOW);
  digitalWrite(FSeg, LOW);
  digitalWrite(GSeg, LOW); 
}

// 顯示數字 'f' 
void r() {
  digitalWrite(ASeg, LOW);
  digitalWrite(BSeg, LOW);
  digitalWrite(CSeg, LOW);
  digitalWrite(DSeg, LOW);
  digitalWrite(ESeg, HIGH);
  digitalWrite(FSeg, LOW);
  digitalWrite(GSeg, HIGH); 
}

// 顯示數字 'e' 
void e() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, LOW);
  digitalWrite(CSeg, LOW);
  digitalWrite(DSeg, HIGH);
  digitalWrite(ESeg, HIGH);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, HIGH); 
}

//

////////////pickNumber
void pickNumber(int x) {
   switch(x) {
     case 1: one(); break;
     case 2: two(); break;
     case 3: three(); break;
     case 4: four(); break;
     case 5: five(); break;
     case 6: six(); break;
     case 7: seven(); break;
     case 8: eight(); break;
     case 9: nine(); break;
     default: zero(); break;
   } 
}

// 顯示數字 '1' 
void one() {
  digitalWrite(ASeg, LOW);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, LOW);
  digitalWrite(ESeg, LOW);
  digitalWrite(FSeg, LOW);
  digitalWrite(GSeg, LOW); 
}

// 顯示數字 '2' 
void two() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, LOW);
  digitalWrite(DSeg, HIGH);
  digitalWrite(ESeg, HIGH);
  digitalWrite(FSeg, LOW);
  digitalWrite(GSeg, HIGH); 
}

// 顯示數字 '3' 
void three() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, HIGH);
  digitalWrite(ESeg, LOW);
  digitalWrite(FSeg, LOW);
  digitalWrite(GSeg, HIGH); 
}

// 顯示數字 '4' 
void four() {
  digitalWrite(ASeg, LOW);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, LOW);
  digitalWrite(ESeg, LOW);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, HIGH); 
}

// 顯示數字 '5' 
void five() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, LOW); 
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, HIGH);
  digitalWrite(ESeg, LOW);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, HIGH); 
}

// 顯示數字 '6' 
void six() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, LOW);
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, HIGH);
  digitalWrite(ESeg, HIGH);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, HIGH); 
}
 
// 顯示數字 '7' 
void seven() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, LOW);
  digitalWrite(ESeg, LOW);
  digitalWrite(FSeg, LOW);
  digitalWrite(GSeg, LOW); 
}

// 顯示數字 '8' 
void eight() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, HIGH);
  digitalWrite(ESeg, HIGH);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, HIGH); 
}
 
// 顯示數字 '9' 
void nine() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, HIGH);
  digitalWrite(ESeg, LOW);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, HIGH); 
}

// 顯示數字 '0' 
void zero() {
  digitalWrite(ASeg, HIGH);
  digitalWrite(BSeg, HIGH);
  digitalWrite(CSeg, HIGH);
  digitalWrite(DSeg, HIGH);
  digitalWrite(ESeg, HIGH);
  digitalWrite(FSeg, HIGH);
  digitalWrite(GSeg, LOW);
}
void delay1ms(int count){
  int i,j;
  for(i=0;i<count;i++)
    for(j=0;j<500;j++){
                     //消防系統啟動
    if(irrecv.decode(&signalresults)){
    if( signalresults.value != 0xFFFFFFFF)
      signal = signalresults.value;

      if(signal == 255 && warning==1){
      Serial.println("System:Safe!");
      digitalWrite(led1Pin, LOW);
      digitalWrite(led2Pin, LOW);
      digitalWrite(led3Pin, LOW);
      warning =0;
      }

//      'Serial.println(signal);
      irrecv.resume();
        }
    }
}
