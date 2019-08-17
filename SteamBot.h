
#ifndef STEAMBOT_H_
#define STEAMBOT_H_
#include "NetworkDispetcher.h"
#include <map>
#include <ctime>
#include "AbstractBot.h"
struct gameData
{

	std::string fprice;
	std::string initialprice;
	unsigned int percent;
};

class SteamBot : public AbstractBot {
public:

	void subscribeThread();
private:

	long int dbUpdateTimestamp = 0;
	long int dbUpdateSeconds = 86400;//Апдейт базы данных игр Steam - 1 раз в день.
	long int subUpdateSeconds = 86400;//Рассылка сабскрайберам - 1 раз в сутки.
	void loadSteamDB();
	void loadSteamDBFromFile();
	std::string replaceAll(std::string& str, const std::string& from, const std::string& to);
	std::map<std::string,int> gameDB;
	std::map<int,std::string> gameDBByIndex;//сделано для обеспечения хранения int в SQL
	std::string getSaleData(std::string game);
	std::map<std::string,gameData> priceDB;
	void createDatabase();
	void newSubscribe(int chat,int gameId);
	void removeSubscribe(int chat,int gameId);
	void removeGame(int gameId);
	void updateSubscribe(int chat,int gameId);
	void getGameSubsFromDB();
	static int getGameSubsFromDB_sqlite3callback(void *data, int argc, char **argv, char **azColName);
	std::pair<std::string,std::string> formMessage(std::pair<std::string,std::string> & message);
	std::string checkSubscribe(std::pair<std::string,std::string> & message);
	std::map< int, std::map<int,long int> > gameSubs;





public:

	SteamBot();
	virtual void operator()() override;
	virtual ~SteamBot();
};

#endif /* STEAMBOT_H_ */
