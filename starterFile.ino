  
//**********************************************************************************************
#include<GSM.h>
#define GPSmodule Serial3                               // Hardware serial 3 on Arduino
//**********************************************************************************************
  String textMessage;                                   // Variable to store text message
  const int ignitionOutput = 53;                        // LED connected to digital pin 53 of Arduino
  const int starterOutput  = 51;                        // LED connected to digital pin 53 of Arduino
  bool engineRunning = false;                           // The engine by default is not running
  void setup(){
      Serial.begin(9600);                               // Initialise serial commmunications at 9600 bps
      GPSmodule.begin(9600);    
      pinMode(ignitionOutput, OUTPUT);                  // Set ignitionOutput as OUTPUT
      pinMode(starterOutput, OUTPUT);                   // Set starterOutput as OUTPUT
      digitalWrite(ignitionOutput, HIGH);               // Ignition defaults to off
      digitalWrite(starterOutput, HIGH);                // Ignition defaults to off
      delay(2000);                                      // Give time for GSM GPS module logon to GSM network
      Serial.println("GPSmodule ready...");             //Print test in Serial Monitor
      GPSmodule.println("AT");
      delay(50);  
      GPSmodule.println("AT+CMEE=1");
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
    delay(10);                                         // Incoming SMS misses first if statement ocasionally without this delay  
    
    if (textMessage.indexOf("Start") >= 0) {
      Serial.println("Revied start text"); 
      textMessage = "";                                // Clear message for engine off text
      startEngine();
      //String message = "Start message recived";
      //serialSMS(message);
    }
  }

// Function that sends output to serial
  void serialSMS(String message) {
    Serial.println(message);
    }

  void startEngine () {
    digitalWrite(ignitionOutput, LOW);               // Turn on ignition of vehcile via IGN Relay
    delay(1500);                                     // Let fuel pump build pressure
    digitalWrite(starterOutput, LOW);                // Run starter for 1.5 seconds or untill engine is running
    delay(1500);
    digitalWrite(starterOutput, HIGH);                                  
    engineRunning = true;                            // Set engine to running

    // for loop checking for engine off text evey second for 10 mins
    
    for(int i=0; i < 600; i++) {
      textMessage = GPSmodule.readString();
      if (textMessage.indexOf("Off") >= 0) {
        stopEngine();
        break;
        } else {
          delay(1000);
          }
      }
    stopEngine();
    }

  void stopEngine () {
    digitalWrite(ignitionOutput, HIGH);
    engineRunning = false;
    }
