#include <ESP8266WiFi.h>
#include "OOCSI.h"

const char* OOCSIName = "CAFFEEE";    //name for connecting with OOCSI
const char* hostserver = "oocsi.id.tue.nl";     //put the adress of your OOCSI server here, can be a web page or an IP adres string
const char* ssid = "G3_4279";                    //SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* password = "titooooo";            //Password of your Wifi network.
const char* OOCSIChannel = "coffee_channel";       //name of the channel you want to subscribe to (can subscribe to multiple channels)

void proccessOOCSI();

OOCSI oocsi(OOCSIName, hostserver, ssid, password, proccessOOCSI);  //setting up OOCSI. processOOCSI is the name of the fucntion to call when receiving messages, can be a random function name.

const int c_queueSize = 10;
int coffeeLine[c_queueSize][3]; // [int userID, int amountOfCoffee, int whenToMake]

void setup() {

  for(int i = 0; i < c_queueSize; i = i + 1){
      coffeeLine[i][0] = 0;
      coffeeLine[i][1] = 0;
      coffeeLine[i][2] = 0;
    }
  
  // put your setup code here, to run once:
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
  // put your main code here, to run repeatedly:
  oocsi.check();  //function needs to be checked in order for OOCSI to process incoming data.

  // check if coffee needs to be made
  for(int i = 0; i < c_queueSize; i = i + 1){
    if(coffeeLine[i][0] > 0 && coffeeLine[i][2] < millis()/60000){ // find orders
      // make the coffee
      iWantCoffeeNow(coffeeLine[i][1], coffeeLine[i][0]);
      // clear the line in the queue
      coffeeLine[i][0] = 0;
      coffeeLine[i][1] = 0;
      coffeeLine[i][2] = 0;
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

  //displaySomeOOCSIData();
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
  Serial.println("CAFFEEEE: OK I WILL MAKE COFFEE!");
  Serial.print("CAFFEEEE: ");
  Serial.print(howMuchCoffee);
  Serial.print(" CUPS");
  // output someting to oocsi
  coffeeoutput(3, howMuchCoffee, who, "COFFEE IS BEING PREPARED");
  // output someting to oocsi
  delay(1000); // for now the coffee is ready immediately
  coffeeoutput(4, howMuchCoffee, who, "COFFEE IS READY"); 
}

void displaySomeOOCSIData(){
  Serial.println("\t sender: ");
  Serial.println(oocsi.getSender());
  Serial.println("\t recipient: ");
  Serial.println(oocsi.getRecipient());
  Serial.println("\t sender: ");
  Serial.println(oocsi.getSender()); 
  Serial.println("\t Timestamp: ");
  Serial.println(oocsi.getTimeStamp());
  Serial.println();

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
