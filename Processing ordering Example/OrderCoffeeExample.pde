import nl.tue.id.oocsi.*;

// ******************************************************
// This example requires a running OOCSI server!
//
// How to do that? Check: Examples > Tools > LocalServer
//
// More information how to run an OOCSI server
// can be found here: https://iddi.github.io/oocsi/)
// ******************************************************

int fillColor = 255;
int position = 0;
OOCSI oocsi;

void setup() {


  // connect to OOCSI server running on the same machine (localhost)
  // with "receiverName" to be my channel others can send data to
  // (for more information how to run an OOCSI server refer to: https://iddi.github.io/oocsi/)
  oocsi = new OOCSI(this, "othername", "oocsi.id.tue.nl");

  // subscribe to channel "testchannel"
  // either the channel name is used for looking for a handler method...
  //oocsi.subscribe("coffee_channel", "handleOOCSIEvent");
  // ... or the handler method name can be given explicitly
  // oocsi.subscribe("testchannel", "testchannel");
  
  println("Started");
  
  oocsi.subscribe("coffee_channel", "handleOOCSIEvent");
  
  // order a coffee
  oocsi .channel("coffee_channel")
        // some kind of user identifier, cannot be 0
        .data("caffee_who", 1)
        // the amount of coffee you want
        .data("caffee_amount", 1) 
        // how long to wait for the coffee, in minutes
        .data("caffee_time_to_wait", 1)
        // the command
        .data("caffee_command", "add")
        .send(); 
  //delay(10000); // wait 10 seconds
  //oocsi .channel("coffee_channel")
  //      // some kind of user identifier, cannot be 0
  //      .data("caffee_who", 123)
  //      // the amount of coffee you want
  //      .data("caffee_amount", 1) 
  //      // how long to wait for the coffee, in minutes
  //      .data("caffee_time_to_wait", 3)
  //      .send(); 
  
  println("test sent");
  
  println("Started");

  
}


void handleOOCSIEvent(OOCSIEvent event) 
{
  int output_type = event.getInt("output_type", 0);
  int amount = event.getInt("amount", 0);
  int who = event.getInt("who", 0);
  int number = event.getInt("number", 0);
  String message = event.getString("message", "NULL");
  
  if (output_type > 0) {
    println(message);
    println("\t Amount: "+amount);
    println("\t Who: "+who);
    println("\t Number: "+number);
  }
}