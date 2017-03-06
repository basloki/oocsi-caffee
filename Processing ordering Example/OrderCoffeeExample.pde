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
  oocsi = new OOCSI(this, "basloki", "oocsi.id.tue.nl");

  // subscribe to channel "testchannel"
  // either the channel name is used for looking for a handler method...
  //oocsi.subscribe("coffee_channel", "handleOOCSIEvent");
  // ... or the handler method name can be given explicitly
  // oocsi.subscribe("testchannel", "testchannel");
  
  println("Started");
  
  // order a coffee
  oocsi .channel("coffee_channel")
        // some kind of user identifier, cannot be 0
        .data("caffee_who", 1)
        // the amount of coffee you want
        .data("caffee_amount", 3) 
        // how long to wait for the coffee, in minutes
        .data("caffee_time_to_wait", 2)
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
        
  println("Order sent");
  
  oocsi .channel("coffee_channel")
        // some kind of user identifier, cannot be 0
        .data("output_type", 1)
        // the amount of coffee you want
        .data("message", "test message") 
        // how long to wait for the coffee, in minutes
        .data("amount", 2)
        .data("who", 2)
        .send(); 
  
  println("test sent");
  
  println("Started");
  
  oocsi.subscribe("coffee_channel", "handleOOCSIEvent");


  
}


void handleOOCSIEvent(OOCSIEvent event) {
  int output_type;
  int amount;
  int who;
  String message;
  
  output_type = event.getInt("output_type", 0);
  switch(output_type){
    case 1:
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("ORDER CONFIRMED:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
    case 2:
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("ORDER NOT CONFIRMED:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
    case 3:
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("PREPARING COFFEE:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
    case 4:
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("COFFEE READY:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
  }
}