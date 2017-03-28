#include "machine.h"

Coffee::Coffee() : _ready(0), _totalorders(1), _oocsi(_name, _server, _ssid, _psswd, Callback)
{
	_this = this;
}

Coffee::~Coffee()
{
}

bool Coffee::Initialize()
{
	// Set the servo into the right position
	_servo.attach(0);
	for (int pos = 0; pos <= 42; pos++) {
		_servo.write(pos);
		delay(5);
	}

	// Connect to the wifi
	Serial.println("Connecting to wifi...");
	if (!_oocsi.connectOocsi()) {
		Serial.println("Could not connect to wifi!");
		return false;
	}
	Serial.println("Succefully connected to wifi.");
	Serial.println("Connecting to channel...");
	_oocsi.subscribe(_channel);
	Serial.print("Connected to channel: ");
	Serial.println(_channel);
}

void Coffee::AddToQueue(const Order &order)
{
	_AddToList(order);
	_SendMessage(1, order);
}

void Coffee::ChangeOrder(const Order &neworder)
{
	bool flag = false;
	for (auto it = _orders.begin(); it != _orders.end(); ++it) {
		if (*it == neworder) {
			if (it->time != neworder.time) {
				_orders.erase(it);
				_AddToList(neworder);
			}
			else
				*it = neworder;
			flag = true;
			break;
		}
	}
	if (flag)
		_SendMessage(5, neworder);
	else
		_SendMessage(6, neworder);
}

void Coffee::RemoveOrder(const Order &oldorder)
{
	bool flag = false;
	for (auto it = _orders.begin(); it != _orders.end(); ++it) {
		if (*it == oldorder) {
			_orders.erase(it);
			flag = true;
			break;
		}
	}
	if (flag)
		_SendMessage(7, oldorder);
	else
		_SendMessage(6, oldorder);
}

void Coffee::Run()
{	
	_oocsi.check();
	
	if (_ready)
		_CheckProgress();
	else
		_CheckQueue();
}

void Coffee::Callback()
{
	String cmd;
	Order neworder;
	long tmp;

	// Get the order and command
	neworder.id = _this->_oocsi.getLong("caffee_who", 0);
	neworder.amount = _this->_oocsi.getLong("caffee_amount", 0);
	tmp = _this->_oocsi.getLong("caffee_amount", 0);
	neworder.time = (tmp < 0) ? millis()/1000 : millis()/1000+60*(unsigned long)tmp;
	cmd = _this->_oocsi.getString("caffee_command", "add");

	// Execute the command
	if (cmd.equals("add")) {
		if (neworder.amount > 0) {
			neworder.number = _this->_totalorders++;
			_this->AddToQueue(neworder);
		}
	}
	else {
		neworder.number = _this->_oocsi.getLong("number", 0);
		if (neworder.number > 0) {
			if (cmd.equals("change"))
				_this->ChangeOrder(neworder);
			else if (cmd.equals("remove"))
				_this->RemoveOrder(neworder);
			else
				_this->_SendMessage(8, neworder);
		}
	}
}

void Coffee::_AddToList(const Order &order)
{
	bool flag = true;
	for (auto it = _orders.begin(); it != _orders.end(); ++it) {
		if (it->time > order.time) {
			_orders.insert(it, order);
			flag = false;
			break;
		}
	}
	if (flag)
		_orders.push_back(order);
}

void Coffee::_SendMessage(int msgid, const Order &order)
{
	// Data to send
	_oocsi.newMessage(_channel);
	_oocsi.sendInt("output_type", msgid);
	_oocsi.sendLong("who", order.id);
	_oocsi.sendLong("amount", order.amount);
	_oocsi.sendLong("number", order.number);
	
	// Message to send
	switch (msgid) {
	case 1:
		_oocsi.sendString("message", "Order confirmed.");
		break;
	case 3:
		_oocsi.sendString("message", "Order is being prepared.");
		break;
	case 4:
		_oocsi.sendString("message", "Order is ready.");
		break;
	case 5:
		_oocsi.sendString("message", "Order has been changed.");
		break;
	case 6:
		_oocsi.sendString("message", "Order could not be found!");
		break;
	case 7:
		_oocsi.sendString("message", "Order has been removed.");
		break;
	}

	// Send and print the actual message
	_oocsi.sendMessage();
	_oocsi.printSendMessage();
}

void Coffee::_CheckProgress()
{
	unsigned long time = _GetTime();

	if (time >= _ready) {
		for (auto order : _inprogress)
			_SendMessage(4, order);
		_PowerOn(false);
		_inprogress.clear();
		_ready = 0;
	}
}

void Coffee::_CheckQueue()
{
	int totalamount = 0;
	unsigned long time = _GetTime();

	auto AddToVectorHelper = [this](int &totalamount, std::list<Order>::iterator &it) {
		_SendMessage(3, *it);
		totalamount += it->amount;
		_inprogress.push_back(*it);
		it = _orders.erase(it);
	};

	// Check the list if coffee needs to be made and add it to the "in progress" queue
	for (auto it = _orders.begin(); it != _orders.end();) {
		if (totalamount) {
			if (it->time <= time)
				AddToVectorHelper(totalamount, it);
			else
				break;
		}
		else {
			if (it->time >= time) {
				if (it->time-time <= _bundletime && totalamount < _maxorders) {
					if (totalamount+it->amount <= 10)
						AddToVectorHelper(totalamount, it);
					else
						++it;
				}
				else
					break;
			}
			else if (time-it->time <= _bundletime) {
				if (totalamount < _maxorders) {
					if (totalamount+it->amount <= 10)
						AddToVectorHelper(totalamount, it);
					else
						++it;
				}
				else
					break;
			}
			else
				++it;
		}
	}

	// If there is coffee to be made, make it
	if (totalamount) {
		_MakeCoffee(totalamount, time);
	}
}

void Coffee::_MakeCoffee(int amount, unsigned long time)
{
	// Start the coffee machine
	_PowerOn(true);
	_AddCoffee(amount);

	// Calculate the time it will take
	_ready = time + amount*_timepcup;

	// Store the time it will take
	for (auto &ordr : _inprogress)
		ordr.time = time + amount*_timepcup;
}

inline long Coffee::_GetTime()
{
	return millis()/1000+_timepcup*2;
}

inline void Coffee::_PowerOn(bool set)
{
	set ? digitalWrite(_powerpin, HIGH) : digitalWrite(_powerpin, LOW);
}

void Coffee::_AddCoffee(int amount)
{
	for (int i = 0; i < amount; i++) {
		for (int pos = 42; pos <= 102; pos++) {
			_servo.write(pos);
			delay(5);
		}
		delay(2000);
		for (int pos = 102; pos >= 42; pos--) {
			_servo.write(pos);
			delay(5);
		}
		delay(3000);
	}
}

Coffee* Coffee::_this = nullptr;