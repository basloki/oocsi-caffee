import nl.tue.id.oocsi.*;
OOCSI oocsi;

void setup() {
  // connect to OOCSI server
  oocsi = new OOCSI(this, "your_name", "oocsi.id.tue.nl");

  // subscribe to channel "coffee_channel"
  oocsi.subscribe("coffee_channel", "handleOOCSIEvent");
  
  // subscribe to channel "dbsu10-gestures"
  oocsi.subscribe("dbsu10-gestures", "handleOOCSIEvent");

}


void handleOOCSIEvent(OOCSIEvent event) {
  int output_type;
  int amount;
  int who;
  String message;
  
  // check if there is a coffee order
  int caffee_who = event.getInt("caffee_who", 0);
  if(caffee_who != 0){
    int caffee_time_to_wait = event.getInt("caffee_time_to_wait", 0);
    int caffee_amount = event.getInt("caffee_amount", 0);
    
    int hoursToWait = floor(caffee_time_to_wait / 60);
    int minutesToWait = (caffee_time_to_wait % 60);
    
    // SEND TO SMARTCLOCK
    String sc_module = "Caffee";    
    int sc_currentMin = minute() + minutesToWait; 
    int sc_currentHour = hour() + hoursToWait;
    String sc_category = "B"; // not sure this is correct

    oocsi  
       .channel("SmartClock")    
          .data("moduleName", sc_module)    
             .data("timeStampHour", sc_currentHour)    
                .data("timeStampMin", sc_currentMin)    
                   .data("actCategory", sc_category)    
                      .send();
     
     // SEND DATA TO TWEETBOT 
     if(hoursToWait < 24){ // only send a tweet if the order is in less then a day
       String tweetText_amount;
       String tweetText_time;
       if(caffee_amount == 1)
         tweetText_amount = "One coffee is being made for "+event.getSender();
       else
         tweetText_amount = caffee_amount+" cups of coffee are being made for "+event.getSender();
         
       if(hoursToWait == 0)
         tweetText_time = "in "+minutesToWait+" minutes!";
       else
         tweetText_time = "in "+hoursToWait+" hours and "+minutesToWait+" minutes!";
         
       oocsi
          .channel("tweetBot")
            .data("tweet", tweetText_amount + tweetText_time)
              .send();
     }
  }
  
  // check if there is a message with a different output_type
  output_type = event.getInt("output_type", 0);
  
  switch(output_type){
    case 1: // output_type == 1
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("ORDER CONFIRMED:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
    case 2: // output_type == 2
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("ORDER NOT CONFIRMED:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
    case 3: // output_type == 3
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("PREPARING COFFEE:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
    case 4: // output_type == 4
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

void orderCoffee(){
  // order a coffee
  oocsi .channel("coffee_channel")
        // some kind of user identifier, cannot be 0
        .data("caffee_who", int(random(1,99999)))
        // the amount of coffee you want
        .data("caffee_amount", 3) 
        // how long to wait for the coffee, in minutes
        .data("caffee_time_to_wait", 2)
        .send(); 
}