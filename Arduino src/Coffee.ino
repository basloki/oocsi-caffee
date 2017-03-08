#include <ESP8266WiFi.h>
#include "OOCSI.h"
#include <Servo.h> 

const char* OOCSIName = "your_name";    //name for connecting with OOCSI
const char* hostserver = "oocsi.id.tue.nl";  //adress of OOCSI server
const char* ssid = "wifiname"; //SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* password = "wifipass"; //Password of your Wifi network.
const char* OOCSIChannel = "coffee_channel"; //name of the channel

Servo myservo;

void proccessOOCSI();

OOCSI oocsi(OOCSIName, hostserver, ssid, password, proccessOOCSI);  //setting up OOCSI. processOOCSI is the name of the fucntion to call when receiving messages, can be a random function name.

// declare caffee variables
const int c_queueSize = 9001; // max number of orders that can be in the queue, currently it is just over 9000
const int c_machineTime = 5; // minutes the machine takes to prepare a cup of coffee
const int c_bundleOrdersTime = 30 // minutes that other coffee orders should be bundled

// coffee system arrays
int coffeeLine[c_queueSize][3]; // [int userID, int amountOfCoffee, int whenToMake]
int prepareLine[c_queueSize][3]; // [int userID, int amountOfCoffee]

void setup() {
//*************************  
  myservo.attach(0);
//*************************
  
  for(int i = 0; i < c_queueSize; i = i + 1){
      coffeeLine[i][0] = 0;
      coffeeLine[i][1] = 0;
      coffeeLine[i][2] = 0;
    }
  
  // setup the serial
  Serial.begin(115200);
  Serial.println("connecting Wifi");
  oocsi.connectOocsi();           //connect wifi and OOCSI to the server.
  Serial.println("subscribing");
  oocsi.subscribe(OOCSIChannel);        //subscribe to a channel

  //now print out the clients and channels
  Serial.print("the clients: ");
  Serial.println(oocsi.getClients()); //WARNING: do not use the function getClients in the oocsi processing function
  Serial.print("the channels: ");
  Serial.println(oocsi.getChannels());  //WARNING: Do not use the getChannels function in the oocsi processing function

  //check if we are in the client list
  Serial.print("is ");
  Serial.print(OOCSIName);
  Serial.print(" a client?: ");
  Serial.println(oocsi.containsClient(OOCSIName));
}

void loop() {
//*************************
  int pos;

  for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
  } 
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
  }
//*************************
  // put your main code here, to run repeatedly:
  oocsi.check();  //function needs to be checked in order for OOCSI to process incoming data.
  
  // check if coffee needs to be made
  for(int i = 0; i < c_queueSize; i = i + 1){
    if(coffeeLine[i][0] > 0 && coffeeLine[i][2] < ((millis()/60000)-c_machineTime)){ // find orders
      // make the coffee
      iWantCoffeeNow(coffeeLine[i][1], coffeeLine[i][0]);
      // clear the line in the queue
      coffeeLine[i][0] = 0;
      coffeeLine[i][1] = 0;
      coffeeLine[i][2] = 0;
    }
  }
  
  // check for coffee that is ready
  for(int i = 0; i < c_queueSize; i = i + 1){
    if(prepareLine[i][0] > 0 && prepareLine[i][2] < (millis()/60000)){ // find orders
      // say to oocsi that the coffee is ready
      coffeeoutput(4, prepareLine[i][1], prepareLine[i][0], "COFFEE IS READY"); 
      // clear the line in the queue
      prepareLine[i][0] = 0;
      prepareLine[i][1] = 0;
      prepareLine[i][2] = 0;
    }
  }
  
  
}

void proccessOOCSI(){
  //function which OOCSI calls when an OOCSI message is received.
  oocsi.printMessage(); //function for printing the raw incoming message (JSON string).
  Serial.println();

  //printing the variables received
  Serial.println("caffee_amount: " );
  int caffee_amount = oocsi.getInt("caffee_amount",0);
  Serial.println(caffee_amount); 
  
  Serial.println("caffee_time_to_wait: ");
  int caffee_time_to_wait = oocsi.getInt("caffee_time_to_wait",0);
  Serial.println(caffee_time_to_wait);
  
  Serial.println("caffee_who: ");
  int caffee_who = oocsi.getInt("caffee_who",0);
  Serial.println(caffee_who);

  // check if there is a coffee order
  if(caffee_amount > 0 && caffee_who > 0){

    //order received, schedule it
    scheduleCoffee(caffee_who, caffee_amount, caffee_time_to_wait);
  }

}

void scheduleCoffee(int who, int howmuch, int when){
  bool full = true;
  int timeWhenIWantCoffee = millis()/60000 + when;
  for(int i = 0; i < c_queueSize; i = i + 1){
    if(coffeeLine[i][0] == 0){ // find an empty line in the queue
      coffeeLine[i][0] = who;
      coffeeLine[i][1] = howmuch;
      coffeeLine[i][2] = timeWhenIWantCoffee;
      full = false;
      Serial.print("CAFFEEEE: ADDED TO THE QUEUE ON PLACE: ");
      Serial.println(i);
      // output someting to oocsi
      coffeeoutput(1, howmuch, who, "ADDED TO QUEUE");
      break;
    }
  }
  // ERROR - there is no space in line
  if(full){
    Serial.println("CAFFEEEE: SORRY THE QUEUE IS FULL ");
    // output someting to oocsi
    coffeeoutput(2, howmuch, who, "QUEUE IS FULL");
  }
}

void iWantCoffeeNow(int howMuchCoffee, int who){
  
  int timeItWillBeReady = (millis()/60000)+c_machineTime;
  
  // say the order started to oocsi
  coffeeoutput(3, howMuchCoffee, who, "COFFEE IS BEING PREPARED");
  // add it to the preparation queue
  for(int i = 0; i < c_queueSize; i = i + 1){
    if(prepareLine[i][0] == 0){ // find an empty line in the queue
      prepareLine[i][0] = who;
      prepareLine[i][1] = howMuchCoffee;
      prepareLine[i][2] = timeItWillBeReady;
      break;
    }
  }

  // see if there are other orders for coffee within the next c_bundleOrdersTime minutes
  for(int i = 0; i < c_queueSize; i = i + 1){
    if(coffeeLine[i][0] > 0 && coffeeLine[i][2] < ((millis()/60000)-c_bundleOrdersTime)){ // find orders
      // add to the amount
      howMuchCoffee = howMuchCoffee + coffeeLine[i][1];
      // say the order started to oocsi
      coffeeoutput(3, coffeeLine[i][1], coffeeLine[i][0], "COFFEE IS BEING PREPARED");
      // add to the preparation queue
      for(int i = 0; i < c_queueSize; i = i + 1){
        if(prepareLine[i][0] == 0){ // find an empty line in the queue
          prepareLine[i][0] = who;
          prepareLine[i][1] = howmuch;
          prepareLine[i][2] = timeItWillBeReady;
          break;
        }
      }
      // clear the line in the order queue
      coffeeLine[i][0] = 0;
      coffeeLine[i][1] = 0;
      coffeeLine[i][2] = 0;
    }
  }
  
  // prepare the coffee
  Serial.println("CAFFEEEE: OK I WILL MAKE COFFEE!");
  Serial.print("CAFFEEEE: ");
  Serial.print(howMuchCoffee);
  Serial.print(" CUPS");

}

void coffeeoutput(int output_type, int amount, int who, String message) {
  oocsi.newMessage((String) OOCSIChannel);  //
  oocsi.sendInt("output_type", output_type);
  oocsi.sendInt("amount", amount);
  oocsi.sendInt("who", who);
  oocsi.sendString("message" , message);
  oocsi.sendMessage();
  oocsi.printSendMessage();   //prints out the message how it is send to the OOCSI server
  oocsi.check();
}
