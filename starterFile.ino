/******************************************************************************************************
** Written by: Chris Eheler
** TODO: add gps functionality, add engine running feature
** Suplies used: Arduino Mega 2560, 4 relay module, SIM808 EVB, Hologram.io SIM
******************************************************************************************************/
#include<GSM.h>
#define GPSmodule Serial3                               // Hardware serial 3 on Arduino

  String textMessage       = "";                        // Variable to store text message
  const int ignitionOutput = 53;                        // Ignition relay wire
  const int starterOutput  = 51;                        // Starter relay wire
  const int GSMpower       = 49;                        // Power wire for GSM board
  bool engineRunning = false;                           // The engine by default is not running
  void setup(){
      Serial.begin(9600);                               // Initialize serial communications at 9600 bps
      GPSmodule.begin(9600);
          
      pinMode(ignitionOutput, OUTPUT);                  // Set ignitionOutput as OUTPUT
      pinMode(starterOutput, OUTPUT);                   // Set starterOutput as OUTPUT
      pinMode(GSMpower, OUTPUT);                        // Set GSMpower as OUTPUT
      digitalWrite(ignitionOutput, HIGH);               // Ignition defaults to off
      digitalWrite(starterOutput, HIGH);                // Starter defaults to off
      digitalWrite(GSMpower, HIGH);                     // Turn on the GSM module
      delay(1500);
      digitalWrite(GSMpower, LOW);
      delay(5000);                                      // Give time for GSM module to connect to the network
      
      //Serial.println("GPSmodule ready...");           // Print test in Serial Monitor
      GPSmodule.println("AT");
      delay(50);  
      GPSmodule.println("AT+CMEE=1");                    // Allow AT Errors
      delay(50);
      GPSmodule.println("AT+CMGF=1");                    // AT command to set GPSmodule to SMS mode
      delay(50);
      GPSmodule.println("AT+CNMI=2,2,0,0,0");            // Set module to send SMS data to serial out upon receipt
      delay(50);
    }

  void loop() {
      if (Serial.available() > 0)
        GPSmodule.write(Serial.read());
      if (GPSmodule.available() > 0)
        Serial.write(GPSmodule.read());
      if (GPSmodule.available() > 0) {
        textMessage = GPSmodule.readString();
        Serial.print(textMessage);
      }
    delay(10);                                         // Incoming SMS misses first if statement occasionally without this delay  

    if (textMessage.indexOf("XXXXXXXXXX") <= 0 && textMessage != "") {
      Serial.println("Revied text from a stranger");   // Phone number validation loop
      textMessage = "";                                // Ignore texts from random people
    }
    
    if (textMessage.indexOf("Start") >= 0 && engineRunning == false) {
      Serial.println("Revied start text"); 
      textMessage = "";                                // Clear message for engine off text
      startEngine();
      textMessage = "";                                // Clear message so we dont get stuck in phone number validation loop
    }
  }

// Function that starts the engine

  void startEngine () {
    digitalWrite(ignitionOutput, LOW);               // Turn on ignition of vehicle via IGN Relay
    delay(1500);                                     // Let fuel pump build pressure
    digitalWrite(starterOutput, LOW);                // Turn on the starter relay
    delay(1200);                                     // Run starter for 1.2 seconds
    digitalWrite(starterOutput, HIGH);               // Turn off the starter relay               
    engineRunning = true;                            // Set engine to running

 // for loop checking for engine off text every second for 15 mins
    
    for(int i=0; i < 900; i++) {
      textMessage = GPSmodule.readString();          // Listen to the incoming text messages
      if (textMessage.indexOf("Off") >= 0) {         // Turn off the engine when the off text is received
        stopEngine();
        break;                                       // Exit the loop
        } else {
          delay(1000);                               // Repeat once a second
          }
      }
    stopEngine();                                    // Turn off the engine
    }

// Fuction that stop the engine

  void stopEngine () {
    digitalWrite(ignitionOutput, HIGH);             // Turn off the ignition relay
    engineRunning = false;
    }
