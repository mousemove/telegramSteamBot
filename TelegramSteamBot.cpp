//============================================================================
// Name        : TelegramSteamBot.cpp
// Author      : Alab
// Version     : 1.0
// Copyright   : (c) github.com/mousemove
// Description : Телеграм бот информирующий о скидках в Steam
//============================================================================

#include <iostream>
#include "NetworkDispetcher.h"
#include "Parser.h"
#include <thread>
#include "SteamBot.h"
using namespace std;
NetworkDispetcher * dispLink = nullptr;
void listener(int sig) {
	if(dispLink != nullptr) dispLink->closeSocket();
    exit(1);
}

int main() {



	Parser p;//Вспомогательный класс для обработки сообщений
	SteamBot bot; //Непосредственно класс бота. Можете попробовать сделать простой AbstractBot, который является его родителем вместо него.

	p.setBot(&bot);
	//Если вы хотите обоабатывать сообщения через webhook, установили корректный сертификат - раскомментируйте 2 следующие строки, и закомментируйте другой конструктор
	//NetworkDispetcher disp("===YOUR TOKEN HERE====",8443,"===PRIVATE KEY PATH =====","==PUBLIC KEY PATH =====","===YOUR additional adres(may be token) HERE====",&p);
	//signal(SIGINT, listener);
	//Если вы хотите читать сообщения с помощью cURL - оставьте этот вариант.
	NetworkDispetcher disp("===Your TOKEN HERE====",&p);

	dispLink = &disp;
	std::thread tdisp(std::ref(disp));//Поток диспетчера для приема сообщений от телеграм


	std::thread tparser(std::ref(p)); //Поток "парсинга" данных от диспетчера. При желании можно объеденить с потоком отправки/потоком бота. Разделены исключительно для наглядности/примера
	std::thread tsender(&Parser::sendTFunction,&p);//Поток отправки  сообщений в телеграм. При желании можно объеденить с потоком отправки/потоком бота. Разделены исключительно для наглядности/примера


	std::thread tsubscribers(&SteamBot::subscribeThread,&bot); //Поток обработки подписчиков/ежедневного обновления данных по скидкам и товарам. Если вы будете запускать Abstractbot вместо SteamBot данную строку надо закомментировать
	std::thread tmomentum(std::ref(bot));//Поток для подписки/чтения сообщений в прямом эфире

	tdisp.join();
	return 0;
}

