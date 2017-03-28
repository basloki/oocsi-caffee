# oocsi-caffee
A connected coffee machine module that works with the OOCSI server

## Ordering a coffee
Just send a OOCSI request to the **coffee_channel** containing the following data.
* **caffee_who** *int* which is the unique identifier of orderer. Cannot be 0.
* **caffee_amount** *int* with the number of cups you want to order
* **caffee_time_to_wait** *int* time (in minutes) you want the coffee to be ready

Example request:
```Processing
  // order a coffee
  oocsi .channel("coffee_channel")
        // some kind of user identifier, cannot be 0
        .data("caffee_who", int(random(1,99999)))
        // the amount of coffee you want
        .data("caffee_amount", 3) 
        // how long to wait for the coffee, in minutes
        .data("caffee_time_to_wait", 2)
        .send(); 
```

## Receiving status updates on your coffee order
You can receive status updates on your coffee order by listening to the **coffee_channel**. Different messages will be send containing the following data:
* **output_type** *int* which is the identifier of output message type, ranging from 1 to 4.
* **amount** *int* number of cups ordered
* **who** *int* unique identifier of orderer
* **message** *String* description of output message

### Output types
There are four different messages (**output_types**) that you can expect.
* **output_type = 1**: Order is confirmed
* **output_type = 2**: Order cannot be confirmed (e.g. because the queue on the module is full)
* **output_type = 3**: The order is being prepared
* **output_type = 4**: The order is ready
* **output_type = 5**: The order has been changed
* **output_type = 6**: The order cannot be found
* **output_type = 7**: The order has been removed

Example of receiving the messages:
```Processing

void handleOOCSIEvent(OOCSIEvent event) {
  int output_type;
  int amount;
  int who;
  String message;
  
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
  case 5:
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("ORDER CHANGED:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
  }
  case 6:
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("ORDER CANNOT BE FOUND:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
  }
  case 7:
      message = event.getString("message");
      amount = event.getInt("amount", 0);
      who = event.getInt("who", 0);
      println("ORDER CANCELED:");
      println("\t Amount: "+amount);
      println("\t Who: "+who);
      println("\t Message: "+message);
      println("");
    break;
  }
}
```

## Full example
```Processing
import nl.tue.id.oocsi.*;
OOCSI oocsi;

void setup() {
  // connect to OOCSI server
  oocsi = new OOCSI(this, "your_name", "oocsi.id.tue.nl");

  // subscribe to channel "coffee_channel"
  oocsi.subscribe("coffee_channel", "handleOOCSIEvent");

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


void handleOOCSIEvent(OOCSIEvent event) {
  int output_type;
  int amount;
  int who;
  String message;
  
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
```
