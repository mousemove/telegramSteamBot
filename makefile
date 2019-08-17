all: Bot

Bot: TelegramSteamBot.o NetworkDispetcher.o Parser.o SteamBot.o AbstractBot.o
	g++ -o "TelegramSteamBot" TelegramSteamBot.o NetworkDispetcher.o Parser.o SteamBot.o AbstractBot.o -lpthread -lsqlite3 -lcurl -lcrypto -lssl 

TelegramSteamBot.o: TelegramSteamBot.cpp
	g++ -c TelegramSteamBot.cpp -std=c++11

NetworkDispetcher.o: NetworkDispetcher.cpp
	g++ -c NetworkDispetcher.cpp -std=c++11

	
Parser.o: Parser.cpp
	g++ -c Parser.cpp -std=c++11

SteamBot.o: SteamBot.cpp
	g++ -c SteamBot.cpp -std=c++11

AbstractBot.o: AbstractBot.o
	g++ -c AbstractBot.cpp -std=c++11
	

clean:
	rm -rf *.o TelegramSteamBot
 
