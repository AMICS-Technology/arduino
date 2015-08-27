#include "LiquidCrystal.h"
#include <SPI.h>
#include <WiFi.h>
#include <HttpClient.h>

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

#define FLOWSENSORPIN 2

inline void digitalInputWithPullup(byte pin, boolean b)
{
  pinMode(pin, INPUT);
  digitalWrite(pin, b?HIGH:LOW);
}

volatile uint16_t pulses=0;
volatile uint16_t totalpulses=0;
volatile uint8_t lastflowpinstate;
//volatile uint32_t lastflowratetimer=0;
volatile float flowrate;

boolean latch1=false;
boolean latch2=false;

SIGNAL(TIMER0_COMPA_vect) {
  int signalA=digitalRead(FLOWSENSORPIN);
    if(signalA==1)
    {
      latch1=true;
    }
    if(signalA==0)
    {
      latch2=true;
    }
    if(latch1==true && latch2==true)
    {
      pulses++;
      latch1=false;
      latch2=false;
    }
  
  /*uint8_t x = digitalRead(FLOWSENSORPIN);
  
  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; // nothing changed!
  }
  
  if (x == HIGH) {
    //low to high transition!
    pulses++;
    //sendData();
  }
  lastflowpinstate = x;
 // pulses=0;*/
}

const int BLED=3;
const int GLED=5;
const int RLED=6;
int ledMode=0;
//float adjustedValue;

char ssid[]="JK003";       //Home Internet: 2WIRE244   //windbell       //JK003
char pass[]="25pacifica";  //Home Pass: 7616949307     //7145223450     //25pacifica
int status=WL_IDLE_STATUS;

//float previousLiters=0;

unsigned long previousMillis=0;
unsigned long interval=20000; //interval at which to reset to 0 1day=86400000

//byte mac[]={0x90, 0xA2, 0xDA, 0x0F, 0xEF, 0x66};

char server[] = "vulpestech.herokuapp.com";

WiFiClient client;

char inString[32];
int stringPos=0;
boolean startRead=false;

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
  }
}

void printMacAddress()
{
  byte mac[6];
  
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void printWifiData()
{
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);  
}

void printCurrentNet()
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption,HEX);
  Serial.println();
}

void sendData()
{   

  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;
  liters *= 1000;
  
    Serial.print("Starting connection to server...");
    if(client.connect(server, 80))
    {
      Serial.println("connected to server");
      client.println("POST /api/test/insertSession HTTP/1.1");
      client.println("Host: vulpestech.herokuapp.com");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Connection: close");
      client.println("Content-Length: 23");
      client.println();
      client.print("faucetId=1&usage=");
      client.print(liters);
    }
    if(client.connected())
    {
      client.stop();
    }
    delay(2000);
}

void moodLamp()
{
  //Green Light
  digitalWrite(GLED, LOW);
    digitalWrite(RLED, LOW);
    digitalWrite(BLED, LOW);
    
    for(int i=0; i<256; i++)
    {
      analogWrite(BLED, i);
      delay(3);
    }
   //Teal Light
   digitalWrite(RLED, LOW);
        
    for(int i=255; i>=230; i--)
    {
      analogWrite(BLED, i);
      delay(3);
    }
    for(int i=0; i<90; i++)
    {
      analogWrite(GLED, i);
      delay(3);
    }
    //Blue Light
    digitalWrite(BLED, LOW);
    digitalWrite(RLED, LOW);
        
    for(int i=89; i<256; i++)
    {
      analogWrite(GLED, i);
      delay(3);
    }
    //Purple Light
    digitalWrite(RLED, LOW);
    digitalWrite(BLED, LOW);
    
    for(int i=255; i>=128; i--)
    {
      analogWrite(GLED, i);
      delay(3);   
    }
        
    for(int i=0; i<128; i++)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    //White Light
    digitalWrite(BLED, LOW);
    
    for(int i=128; i>=70; i--)
    {
      analogWrite(GLED, i);
      delay(6);
    }
    for(int i=128; i>=30; i--)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    for(int i=0; i<230; i++)
    {
      analogWrite(BLED, i);
      delay(5);
    }
    //Orange Light
    digitalWrite(GLED, LOW);
    
    for(int i=30; i<128; i++)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    for(int i=229; i>=128; i--)
    {
      analogWrite(BLED, i);
      delay(3);
    }
    //Red Light
    digitalWrite(GLED, LOW);
    digitalWrite(BLED, LOW);
        
    for(int i=127; i<256; i++)
    {
      analogWrite(RLED, i);
      delay(3);
    }
}

void setup() {
  
  float liters = pulses;
    liters /= 7.5;
    liters /= 60.0;
    liters *= 1000; 
    
  lcd.begin(16, 2);
  
  //pinMode(FLOWSENSORPIN, INPUT);
  //digitalWrite(FLOWSENSORPIN, HIGH);
  lastflowpinstate = digitalRead(FLOWSENSORPIN);
  useInterrupt(true);
  
  pinMode(BLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(RLED, OUTPUT);
  
  lcd.print("Hello! <(^^)>");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("My name is LANA");
  delay(1300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Let's save water!");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Water's our Life");
  delay(1300);
  
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Initializing WiFi...");
  printMacAddress();  
  delay(500);

  Serial.println("Attempting to connect to WPA network...");
  status=WiFi.begin(ssid, pass);
  
  while(status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No wifi..");
    delay(5000);
  }
  
  Serial.println("Connected to network");
  printCurrentNet();
  printWifiData();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wifi Connected! ^^");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  delay(2000);
  
  
  Serial.println("Starting upload to server...");
  Serial.println();
  
  moodLamp();
  
  digitalInputWithPullup(FLOWSENSORPIN, true);
}

void setMode(int mode)
{
  String ledColor = inString;
  
  if(mode==1) //Green Light
  { 
    digitalWrite(GLED, LOW);
    digitalWrite(RLED, LOW);
    digitalWrite(BLED, LOW);
    
    for(int i=0; i<256; i++)
    {
      analogWrite(BLED, i);
      delay(3);
    }
    lcd.clear();
    
    while(ledColor = "GREEN")
    {
      
    float liters = pulses;
    liters /= 7.5;
    liters /= 60.0;
    liters *= 1000;
   
    totalpulses=pulses;
    delay(1500);
   
    if(liters==0)
    {
     previousMillis=millis();
    } 
    
    if((totalpulses==pulses) && (millis()-previousMillis >= interval) && (liters!=0))
    {
      previousMillis=millis();
      sendData();
      pulses=0;
    }
    
    if(millis()-previousMillis < 0)
    previousMillis=0;
    
    if((totalpulses==pulses) && (liters==0))
    {
      loopRead();
      delay(1500);
      ledColor=inString;
    }
                    
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("mL ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Think Green!");
    delay(10);
            
    if(ledColor != "GREEN")
    break;
    }
  }
  if(mode==2) //Blue Light
  {
    digitalWrite(BLED, LOW);
    digitalWrite(RLED, LOW);
        
    for(int i=89; i<256; i++)
    {
      analogWrite(GLED, i);
      delay(3);
    }
    while(ledColor = "BLUE")
    {
    float liters = pulses;
    liters /= 7.5;
    liters /= 60.0;
    liters *= 1000;
    
    totalpulses=pulses;
    delay(1500);
   
    if(liters==0)
    {
     previousMillis=millis();
    } 
    
    if((totalpulses==pulses) && (millis()-previousMillis >= interval) && (liters!=0))
    {
      previousMillis=millis();
      sendData();
      pulses=0;
    }
    
    if(millis()-previousMillis < 0)
    previousMillis=0;
    
    if((totalpulses==pulses) && (liters==0))
    {
      loopRead();
      delay(1500);
      ledColor=inString;
    }
       
    lcd.setCursor(0,0);
    lcd.print(liters);
    lcd.print("mL ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Save Water YAY^^");
    delay(10);
        
    if(ledColor != "BLUE")
    break;
    }
  }
  if(mode==3) //Red Light
  {
    digitalWrite(GLED, LOW);
    digitalWrite(BLED, LOW);
        
    for(int i=127; i<256; i++)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    while(ledColor = "RED")
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0;
     liters *= 1000;    
 
     totalpulses=pulses;
     delay(1500);
   
    if(liters==0)
    {
     previousMillis=millis();
    } 
    
    if((totalpulses==pulses) && (millis()-previousMillis >= interval) && (liters!=0))
    {
      previousMillis=millis();
      sendData();
      pulses=0;
    }
    
    if(millis()-previousMillis < 0)
    previousMillis=0; 
 
    if((totalpulses==pulses) && (liters==0))
    {
      loopRead();
      delay(1500);
      ledColor=inString;
    }   
     
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("mL ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Water=Life");
    delay(10);    
       
    if(ledColor != "RED")
    break;
    }
  }
  if(mode==4) //Purple Light
  {
    digitalWrite(RLED, LOW);
    digitalWrite(BLED, LOW);
    
    for(int i=255; i>=128; i--)
    {
      analogWrite(GLED, i);
      delay(3);   
    }
        
    for(int i=0; i<128; i++)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    while(ledColor = "PURPLE")
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0; 
     liters *= 1000;  
    
     totalpulses=pulses;
     delay(1500);
   
    if(liters==0)
    {
     previousMillis=millis();
    } 
    
    if((totalpulses==pulses) && (millis()-previousMillis >= interval) && (liters!=0))
    {
      previousMillis=millis();
      sendData();
      pulses=0;
    }
    
    if(millis()-previousMillis < 0)
    previousMillis=0; 
    
    if((totalpulses==pulses) && (liters==0))
    {
      loopRead();
      delay(1500);
      ledColor=inString;
    }  
          
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("mL ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Still not bad ^^");
    delay(10);  
        
    if(ledColor != "PURPLE")
    break;
    }
  }
  if(mode==5) //Teal Light
  {
    digitalWrite(RLED, LOW);
        
    for(int i=255; i>=230; i--)
    {
      analogWrite(BLED, i);
      delay(3);
    }
    for(int i=0; i<90; i++)
    {
      analogWrite(GLED, i);
      delay(3);
    }
    while(ledColor = "TEAL")
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0;
     liters *= 1000;

    totalpulses=pulses;
    delay(1500);
   
    if(liters==0)
    {
     previousMillis=millis();
    } 
    
    if((totalpulses==pulses) && (millis()-previousMillis >= interval) && (liters!=0))
    {
      previousMillis=millis();
      sendData();
      pulses=0;
    }
    
    if(millis()-previousMillis < 0)
    previousMillis=0;

    if((totalpulses==pulses) && (liters==0))
    {
      loopRead();
      delay(1500);
      ledColor=inString;
    }      
          
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("mL ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Doing Great!*-*");
    delay(10);   
    
    if(ledColor != "TEAL")
    break;
    }
   }
  if(mode==6) //Orange Light
  {
     
    digitalWrite(GLED, LOW);
    
    for(int i=30; i<128; i++)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    for(int i=229; i>=128; i--)
    {
      analogWrite(BLED, i);
      delay(3);
    }
    while(ledColor = "ORANGE")
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0;
     liters *= 1000; 
 
    totalpulses=pulses;
    delay(1500);
   
    if(liters==0)
    {
     previousMillis=millis();
    } 
    
    if((totalpulses==pulses) && (millis()-previousMillis >= interval) && (liters!=0))
    {
      previousMillis=millis();
      sendData();
      pulses=0;
    }
    
    if(millis()-previousMillis < 0)
    previousMillis=0;    
    
    if((totalpulses==pulses) && (liters==0))
    {
      loopRead();
      delay(1500);
      ledColor=inString;
    }  
           
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("mL ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Running out!");
    delay(10);    
       
    if(ledColor != "ORANGE")
    break;
    }
  }
  if(mode==7) //White Light
  {
    digitalWrite(BLED, LOW);
    
    for(int i=128; i>=70; i--)
    {
      analogWrite(GLED, i);
      delay(6);
    }
    for(int i=128; i>=30; i--)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    for(int i=0; i<230; i++)
    {
      analogWrite(BLED, i);
      delay(5);
    }
            
    while(ledColor = "WHITE")
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0;
     liters *= 1000;    

    totalpulses=pulses;
    delay(1500);
   
    if(liters==0)
    {
     previousMillis=millis();
    } 
    
    if((totalpulses==pulses) && (millis()-previousMillis >= interval) && (liters!=0))
    {
      previousMillis=millis();
      sendData();
      pulses=0;
    }
    
    if(millis()-previousMillis < 0)
    previousMillis=0;

    if((totalpulses==pulses) && (liters==0))
    {
      loopRead();
      delay(1500);
      ledColor=inString;
    }      
     
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("mL ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Start saving -_-");
    delay(10);    
        
    if(ledColor != "WHITE")
    break;
    }
  }
  else //OFF
  {
    digitalWrite(RLED, LOW);
    digitalWrite(BLED, LOW);
    digitalWrite(GLED, LOW);
    
    lcd.clear();
    lcd.setCursor(0, 0);
   }
  }
  
String loopRead()
{
  Serial.println("connecting...");
  if(client.connect(server, 80))
  {
    Serial.println("connected");
    client.println("GET /api/v1/arduino/1 HTTP/1.1");
    client.println("Host: vulpestech.herokuapp.com");
    client.println("Connection: close");
    client.println();
    
    return readPage();
  }
  else{
    return "Connection failed";
  }
}
  
  
String connectAndRead()
{
  Serial.println("connecting...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  
  if(client.connect(server, 80))
  {
    Serial.println("connected");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connected! ^^");
    
    client.println("GET /api/v1/arduino/1 HTTP/1.1");
    client.println("Host: vulpestech.herokuapp.com");
    client.println("Connection: close");
    client.println();
    
    return readPage();
  }
  else{
    return "Connection failed";
  }
}

String readPage()
{
  Serial.println("Enter readPage()");
  stringPos = 0;
  memset( &inString, 0, 32);
  
  
  while(true)
  {
    if(client.available())
    {
      char c = client.read();
      Serial.print(c);
      
      if(c == '<')
      {
        startRead = true;
      }
      else if(startRead)
      {
        if(c != '>')
        {
          inString[stringPos]=c;
          stringPos++;
        }
      
        else
        {
          startRead=false;
          client.stop();
          client.flush();
          Serial.println("disconnecting.");
          
          return inString;
        }
      
    }
  }
}
}   

void loop() {
  
  //Liters = Pulses /(7.5*60)
  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;
  liters *= 1000;
  
  //ledColor[32] = inString[32];
  
  String pageValue=connectAndRead();
  Serial.println(pageValue);
  
  String ledColor = inString;
  
  if(ledColor == "GREEN")
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ledColor);
    delay(3000);
    
    int j=1;
    setMode(j);
  }
  if(ledColor == "TEAL")
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ledColor);
    delay(3000);
    
    int j=5;
    setMode(j);
  }
  if(ledColor == "BLUE")
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ledColor);
    delay(3000);
    
    int j=2;
    setMode(j);
  }
  if(ledColor == "PURPLE")
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ledColor);
    delay(3000);
    
    int j=4;
    setMode(j);
  }
  if(ledColor == "WHITE")
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ledColor);
    delay(3000);
    
    int j=7;
    setMode(j);
  }
  if(ledColor == "ORANGE")
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ledColor);
    delay(3000);
    
    int j=6;
    setMode(j);
  }
  if(ledColor == "RED")
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ledColor);
    delay(3000);
    
    int j=3;
    setMode(j);
  }
}
  
  
  /*while(previousMillis<interval)
  { 
  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;
    
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gathering Data..");
  
  lcd.setCursor(0, 1);
  lcd.print(liters);
  lcd.print("Liters Used");
  
  unsigned long currentMillis = millis();
  
  moodLamp();
  
  sendData();  

  if(currentMillis - previousMillis < 0)
  {
    previousMillis = 0;
  } 
    
  if(currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
        
    pulses=0;
  
    break;  
  }
  }*/
  
  //set a timer for one month so that after one month the data gets re-calculated
  
  /*String readValue=inString;
  
  char finalValue[readValue.length()+1];
  readValue.toCharArray(finalValue, sizeof(finalValue));
  adjustedValue = atof(finalValue);
  
  //Add an if condition for when server is connected and not
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Adjusted Value: ");
  lcd.setCursor(0, 1);
  lcd.print(adjustedValue);
  delay(5000);
  
  float updatedValue = adjustedValue;
  
  Serial.println(liters);
  Serial.println(updatedValue);
  
  if(liters <= ((0.143)*updatedValue)) //Green Light
  {
    Serial.println("breakpoint1");
    digitalWrite(GLED, LOW);
    digitalWrite(RLED, LOW);
    digitalWrite(BLED, LOW);
    
    for(int i=0; i<256; i++)
    {
      analogWrite(BLED, i);
      delay(3);
    }
    lcd.clear();
    Serial.println("breakpoint3");
    while(liters <= ((0.143)*updatedValue))
    {
    float liters = pulses;
    liters /= 7.5;
    liters /= 60.0;
                    
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("Liters ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Think Green!");
    delay(10);
            
    if(liters>((0.143)*updatedValue))
    break;
    }
  }
  
  if((liters >((0.143)*updatedValue)) && (liters<=((0.286)*updatedValue))) //Teal Light
  {
   digitalWrite(RLED, LOW);
        
    for(int i=255; i>=230; i--)
    {
      analogWrite(BLED, i);
      delay(3);
    }
    for(int i=0; i<90; i++)
    {
      analogWrite(GLED, i);
      delay(3);
    }
    while((liters >((0.143)*updatedValue)) && (liters<=((0.286)*updatedValue)))
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0;    
          
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("Liters ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Doing Great!*-*");
    delay(10);    
    
    if(liters>((0.286)*updatedValue))
    break;
    }
   
  }
  if((liters>((0.286)*updatedValue)) && (liters<=((0.429)*updatedValue))) //Blue Light
  {
    digitalWrite(BLED, LOW);
    digitalWrite(RLED, LOW);
        
    for(int i=89; i<256; i++)
    {
      analogWrite(GLED, i);
      delay(3);
    }
    while((liters>((0.286)*updatedValue)) && (liters<=((0.429)*updatedValue)))
    {
    float liters = pulses;
    liters /= 7.5;
    liters /= 60.0;
       
    lcd.setCursor(0,0);
    lcd.print(liters);
    lcd.print("Liters ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Save Water YAY^^");
    delay(10);
        
    if(liters>((0.429)*updatedValue))
    break;
    }
  }
  if((liters>((0.429)*updatedValue)) && (liters<=((0.572)*updatedValue))) //Purple Light
  {
    digitalWrite(RLED, LOW);
    digitalWrite(BLED, LOW);
    
    for(int i=255; i>=128; i--)
    {
      analogWrite(GLED, i);
      delay(3);   
    }
        
    for(int i=0; i<128; i++)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    while((liters>((0.429)*updatedValue)) && (liters<=((0.572)*updatedValue)))
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0;   
          
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("Liters ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Still not bad ^^");
    delay(10);    
        
    if(liters>((0.572)*updatedValue))
    break;
    }
  }
  if((liters>((0.572)*updatedValue)) && (liters<=((0.715)*updatedValue))) //White Light
  {
    digitalWrite(BLED, LOW);
    
    for(int i=128; i>=70; i--)
    {
      analogWrite(GLED, i);
      delay(6);
    }
    for(int i=128; i>=30; i--)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    for(int i=0; i<230; i++)
    {
      analogWrite(BLED, i);
      delay(5);
    }
            
    while((liters>((0.572)*updatedValue)) && (liters<=((0.715)*updatedValue)))
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0;         
     
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("Liters ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Start saving -_-");
    delay(10);    
        
    if(liters>((0.715)*updatedValue))
    break;
    }
  }
  if((liters>((0.715)*updatedValue)) && (liters<=((0.858)*updatedValue))) //Orange Light
  {
    digitalWrite(GLED, LOW);
    
    for(int i=30; i<128; i++)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    for(int i=229; i>=128; i--)
    {
      analogWrite(BLED, i);
      delay(3);
    }
    while((liters>((0.715)*updatedValue)) && (liters<=((0.858)*updatedValue)))
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0;    
           
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("Liters ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Running out!");
    delay(10);    
       
    if(liters>((0.858)*updatedValue))
    break;
    }
  }
  if((liters>((0.858)*updatedValue)) && (liters<=updatedValue)) //Red Light
  {
    digitalWrite(GLED, LOW);
    digitalWrite(BLED, LOW);
        
    for(int i=127; i<256; i++)
    {
      analogWrite(RLED, i);
      delay(3);
    }
    while((liters>((0.858)*updatedValue)) && (liters<=updatedValue))
    {
     float liters = pulses;
     liters /= 7.5;
     liters /= 60.0;        
     
    lcd.setCursor(0, 0);
    lcd.print(liters);
    lcd.print("Liters ^^;");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Water=Life");
    delay(10);    
       
    if(liters>updatedValue)
    break;
    }
  }

}

       
  /*if(liters<=0.3)
  {
       
    int j=1; //Green Light
    setMode(j);
  }
  if((liters>0.3) && (liters<=0.6))
  {
    int j=5; //Teal Light
    setMode(j);
  }
  if((liters>0.6) && (liters<=0.9))
  {
   int j=2; //Blue Light
   setMode(j);
  }
  if((liters>0.9) && (liters<=1.2))
  {
    int j=4; //Purple Light
    setMode(j);
  }
  if((liters>1.2) && (liters<=1.5))
  {
    int j=7; //White Light
    setMode(j);
  }
  if((liters>1.5) && (liters<=1.8))
  {
    int j=6; //Orange Light
    setMode(j);
  }
  if((liters>1.8) && (liters<=2.1))
  {
    int j=3; //Red Light
    setMode(j);
  }
  if(liters>2.1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LANA <3 Water (^^)");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("LANA Restarting..");
    delay(2000);
    
    pulses=0;
   }*/
  



  


