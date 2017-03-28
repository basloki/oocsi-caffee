#include "machine.h"

Coffee caffee;

void setup()
{
	Serial.begin(115200);
	caffee.Initialize();
}

void loop()
{
	caffee.Run();
}

/*
 * Proof of broken default library
*/
/*#include <list>

void setup()
{
	std::list<int> test;

	test.assign(5, 0);

	for (auto it = test.begin(); it != test.end(); ++it) {
		test.insert(it, 5);
	}

	for (auto i : test) {
		Serial.println(i);
	}
}

void loop()
{
}*/