//* На деле класс должен быть абстрактным и оператор () чисто виртуальным, но был дополнен в качестве примера, это бот, повторяющий за вами ответы *//
#ifndef ABSTRACTBOT_H_
#define ABSTRACTBOT_H_
#include "NetworkDispetcher.h"

class AbstractBot {
public:
	std::condition_variable cvarForReading;
	std::condition_variable cvarForParsing;
	std::mutex  parseMutex;
	std::mutex  sendMutex;
	std::queue<std::pair<std::string,std::string>> eventsQueue;
	std::queue<std::pair<std::string,std::string>> sendQueue;

	AbstractBot();
	virtual ~AbstractBot();
	virtual void operator()();
};

#endif /* ABSTRACTBOT_H_ */
