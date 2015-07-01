#include "Wire.h"
#include "SYSdef.h"
#include "CRTdef.h"
#include "INTdef.h"
#include "CSLdef.h"
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

// Enter a MAC address for your controller below.
byte mac[] = {  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
char serverName[] = "www.xenydiensten.nl";
int status = 0;
String location = "/~smartmirror";


/*==========================================================================*/
/*  Program                                                                 */
/*==========================================================================*/
CSYS objSYS;
CCRT objCRT;
CCSL objCSL;
CINT objINT;

String urenstring;
String minutenstring;
String weather;
String tempstring;
String stringdatum;
unsigned long urenteller;
unsigned long minutenteller;
int timer1_counter;
int teller = 0;
int inactiviteit = 0;
int start = 1;
double temp;

void setup()
{
    
   
 // start the serial library:
    Serial.begin(9600);
    sei();
    
}

 void loop()
{  
  while(status == 1){
  weather = "leeg";
  EthernetClient client;
  Serial.println("Gegevens ophalen");
    
  // start the Ethernet connection:
   if (Ethernet.begin(mac) == 0) {
  Serial.println("Failed to configure Ethernet using DHCP");
  // no point in carrying on, so do nothing forevermore:
  while(true);
 }
 // give the Ethernet shield a second to initialize:
 delay(1000);
 Serial.println("connecting...");

 // if you get a connection, report back via serial:
 
 if (client.connect(serverName, 80)) {
   Serial.println("connected");
   // Make a HTTP request:
   client.println("GET http://www.xenydiensten.nl/smartmirror/index.php HTTP/1.1.");
   client.println();
 } 
 else {
   // if you didn't get a connection to the server:
   Serial.println("connection failed");
 }
     StaticJsonBuffer<200> jsonBuffer;
     // if there are incoming bytes available 
    // from the server, read them and print them:

      unsigned int i = 0; //timeout counter
      int n = 0; // char counter
      char json[200];
       while (!client.find("close")){}
        while (i<500) {
        if(client.available()) {
          char c = client.read();
 
          json[n]=c;

           Serial.println(c);
          n++;
          i=0;
        }
        i++;
      }
      
      
      
     JsonObject& root = jsonBuffer.parseObject(json);
     temp = root["temp"];
   
     
     const char* datum = root["datum"];
     
     const char* uren = root["uren"];
     const char* minuten = root["minuten"];
     
     const char* description = root["description"];

     urenteller = strtoul( uren, NULL, 10 );
     minutenteller = strtoul( minuten, NULL, 10 );

     stringdatum = String(datum);
     weather = String (description);
     urenstring = String(uren);
     minutenstring = String(minuten);
     tempstring = String(temp);  
     
     Serial.println("Weer: "+ weather);
     Serial.println("Temp C: " + tempstring);
     Serial.println("Uren: " + urenstring);
     Serial.println("Minuten: " + minutenstring);
     Serial.println("Datum " + stringdatum);
         
    // if the server's disconnected, stop the client:
     if (!client.connected()) {
       Serial.println("disconnecting.");
     }
     if(start == 1){      
        Serial.println("Scherm initialiseren");
      /* re-initialize after Arduini IDE init()   */
       objSYS.Ini();             
       objCRT.ChangeMode( CRT_TEXT );   
       start = 0;    
       status = 2;
     }  
    status = 2;
    
  }

  while (status == 2){
      
   Serial.println("Scherm aan!"); 
      
   unsigned char ColorData[7] ={   COL_RED, COL_GRN, COL_BLU, COL_YEL,
                                                                      COL_CYA, COL_MAG, COL_WHI };
   if(inactiviteit == 60){
      status = 0;   
      inactiviteit = 0;
      Serial.println("scherm uitzetten");  
      
   }
   
   if(teller == 30){
    
      minutenteller = minutenteller + 01;
      minutenstring = String(minutenteller);
      teller = 0; 
    Serial.println(minutenstring);                                                               
   }
   
   objCSL.set_cursor( 0, 0 );
   objCSL.print(stringdatum.c_str()); objSYS.delay66( 5 );
   objCSL.set_cursor( 3, 1 );
   objCSL.print(urenstring.c_str()); objSYS.delay66( 5 );
   objCSL.print(":"); objSYS.delay66( 5 );
   objCSL.print(minutenstring.c_str()); objSYS.delay66( 5 );
   objCSL.set_cursor( 0, 12 );
   objCSL.print(tempstring.c_str()); objSYS.delay66( 5 );
   objCSL.set_cursor( 0, 13 );
   objCSL.print(weather.c_str()); objSYS.delay66( 5 );

   Serial.println(teller);
   teller++;
   inactiviteit++;
   
   
}
}


