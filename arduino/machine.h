#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Servo.h>
#include "OOCSI.h"
#include "list.h"
#include "vector.h"

struct Order
{
	long id, amount, number;
	unsigned long time;

	inline bool operator==(const Order &o)  { return (o.number == number); }
	inline bool operator==(const long l) { return (l == number); }
};

class Coffee
{
 public:
	 Coffee();
	 ~Coffee();

	 bool Initialize();
	 void AddToQueue(const Order &order);
	 void ChangeOrder(const Order &neworder);
	 void RemoveOrder(const Order &oldorder);
	 void Run();

	 static void Callback();

private:
	void _AddToList(const Order &order);
	void _SendMessage(int msgid, const Order &order);
	void _CheckProgress();
	void _CheckQueue();
	void _MakeCoffee(int amount, unsigned long time);
	long _GetTime();
	void _PowerOn(bool set);
	void _AddCoffee(int amount);

	const char* _name = "thecoffeegrouparduino546";
	const char* _server = "oocsi.id.tue.nl";
	const char* _ssid = "G3_7241";
	const char* _psswd = "titooooo";
	const char* _channel = "coffee_channel";
	const int _bundletime = 30*60;
	const int _timepcup = 2*60;
	const int _maxorders = 10;
	const uint8_t _servopin = 0;
	const uint8_t _powerpin = 2;

	long _totalorders;
	unsigned long _ready;
	std::list<Order> _orders;
	std::vector<Order> _inprogress;

	static Coffee* _this;
	OOCSI _oocsi;
	Servo _servo;
};