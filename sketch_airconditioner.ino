/*Device : 
 * WeMos D1 R1
 * 
 *Library :
 * Blynk by Volodymyr Shymanskyy V1.1.0
 * IRremote by Arimin Joachimsmeyer V.2.2.3
 * IRemoteESP8266 by David Conran... V2.8.2
*/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "YourTemplateId"
#define BLYNK_DEVICE_NAME "YourTemplateName(Airconditioner)"
#define BLYNK_AUTH_TOKEN "YourToken"


#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <IRsend.h>
//#include <IRsend.h>  // Needed if you want to send IR commands.
//#include <IRrecv.h>  // Needed if you want to receive IR commands.

/*
Using IRremote-IRrecvDemo.ino and your remote control
0xB23F40 //냉방 24도

0xB23FE4 //송풍

0xB27BE0 //꺼짐

BF1A6878 //송풍 30분 꺼짐 예약*/

#define COOLING 0xB23F40
#define BLOWING 0xB23FE4
#define BLOWING_30_MIN BF1A6878
#define OFF 0xB27BE0


//Edit Your IR Pin
IRsend irsend(4); //an IR emitter led is connected to GPIO pin 4 (4)

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YourWiFissid(iptime...)";
char pass[] = "WiFipassword";

//Using own timer
BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  Serial.print("value :");
  Serial.println(value);
  /*value of V0
   * 0: off
   * 1: print blowing notification
   * 2: on
  */ 

  if (value == 1) {
    Serial.println("송풍");
  }
  else if(value == 0) {
    //Use loops in case of ignoring the signal
     for (int i = 0; i < 3; i++){
          irsend.sendCOOLIX(OFF, 24, 1);
          Serial.print("off");         
    }
    //This Updates state of Virtual pin
    Blynk.virtualWrite(V1, 0);
    ESP.restart();//init     
    }
    else if(value == 2) {
    Serial.println("냉방");
    }

else{}
}

//Timer set
BLYNK_WRITE(V1)
{ /*V1: Remain Cooling Time*/
  // Set value incoming from pin V1
  int minute = param.asInt();
  Serial.print("Time value :");
  Serial.print(minute);
  Serial.println("minute");
  
   for (int i = 0; i < 3; i++){
    //sendCOOLIX is depends on air conditioner modle (I'm using Carrier)
      irsend.sendCOOLIX(COOLING, 24, 1);
      Serial.print("on");     
   }
   Blynk.virtualWrite(V0, 2);
   
   for(int x = minute; x>=0; x--){
      delay(60000);
      Blynk.virtualWrite(V1, x);//V1 is decreased every minute
    }
    Serial.println("송풍 20분 뒤 꺼짐");

    for (int i = 0; i < 3; i++){
      irsend.sendCOOLIX(BLOWING, 24, 1);
    }Blynk.virtualWrite(V0, 1);
    delay(1200000);
    for (int i = 0; i < 3; i++){
          irsend.sendCOOLIX(OFF, 24, 1);
          Serial.print("off");         
    }Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V0, 0);
    ESP.restart();

}

  
// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{                                                 
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  irsend.begin();
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, 0);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
