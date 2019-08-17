
#include "SteamBot.h"
#include <sqlite3.h>
#include <algorithm>
#include <chrono>
using json = nlohmann::json;

using namespace std;
SteamBot::SteamBot() {

	createDatabase();
	getGameSubsFromDB();
	loadSteamDB();
	dbUpdateTimestamp = static_cast<long int>(std::time(0));

}

SteamBot::~SteamBot() {

}

void SteamBot::createDatabase()
{
	sqlite3 *db;
	char *zErrMsg = 0;
	char *sql;
	int rc;

	rc = sqlite3_open("test.db", &db);

	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	} else {
		fprintf(stderr, "Opened database successfully\n");
	}

	/* Create SQL statement */
	sql = "CREATE TABLE SUBS("  \
			"ID INT PRIMARY KEY     ," \
			"CHAT            INT     NOT NULL," \
			"GAME            INT     NOT NULL," \
			"LASTTIMESTAMP   INT NOT NULL);";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Table created successfully\n");
	}
	sqlite3_close(db);
}


void SteamBot::newSubscribe(int chat,int gameId)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[200];

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return ;
	} else {
		//fprintf(stderr, "Opened database successfully\n");
	}
	/* Create SQL statement */
	sprintf (sql,"INSERT INTO SUBS (CHAT,GAME,LASTTIMESTAMP) VALUES (%d,%d, %d ); ",chat,gameId,static_cast<long int>(std::time(0)));
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		//fprintf(stdout, "Records created successfully\n");
	}
	sqlite3_close(db);

}


void SteamBot::updateSubscribe(int chat,int gameId)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[200];

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return ;
	} else {
		// fprintf(stderr, "Opened database successfully\n");
	}
	/* Create SQL statement */
	sprintf (sql,"UPDATE SUBS set LASTTIMESTAMP = %d where CHAT = %d AND GAME =%d; ",static_cast<long int>(std::time(0)),chat,gameId);
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		//  fprintf(stdout, "Records update successfully\n");
	}
	sqlite3_close(db);

}
void SteamBot::removeSubscribe(int chat,int gameId)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[200];

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return ;
	} else {
		//fprintf(stderr, "Opened database successfully\n");
	}
	/* Create SQL statement */
	sprintf (sql,"DELETE from SUBS where CHAT = %d  AND GAME = %d ; ",chat,gameId);
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		//fprintf(stdout, "Records removed successfully\n");
	}
	sqlite3_close(db);
}

void SteamBot::removeGame(int gameId)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[200];

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return ;
	} else {
		//fprintf(stderr, "Opened database successfully\n");
	}
	/* Create SQL statement */
	sprintf (sql,"DELETE from SUBS where GAME = %d ; ",gameId);
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		//fprintf(stdout, "Records removed successfully\n");
	}
	sqlite3_close(db);
}
int SteamBot::getGameSubsFromDB_sqlite3callback(void *data, int argc, char **argv, char **azColName)
{
	int i;
	std::map< int, std::map<int, long int> > * gs = ((std::map< int, std::map<int, long int> >  *) data);
	for(i = 1; i<argc; i++){
		int chat = stoi(argv[1]);
		int game = stoi(argv[2]);
		long int ts = stoi(argv[3]);

		auto b = gs->find(game);
		if (b == gs->end())
		{
			std::map<int,long int> map1;
			map1[chat] = ts;
			gs->insert( std::pair<int, std::map<int,long int> >(game,map1) );
		}
		else
		{
			b->second.insert(std::pair<int,int>(chat,ts));
		}


	}



	return 0;
}

void SteamBot::getGameSubsFromDB()
{

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[200];

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return ;
	} else {
		fprintf(stderr, "Opened database successfully\n");
	}
	/* Create SQL statement */
	sprintf (sql,"SELECT *  from SUBS ;");
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, SteamBot::getGameSubsFromDB_sqlite3callback, (void *)&gameSubs, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
	}

	sqlite3_close(db);

}


void SteamBot::loadSteamDB()
{
	const char * url = "http://api.steampowered.com/ISteamApps/GetAppList/v2";
	string data =  NetworkDispetcher::HTTPgetCURL(url);

	json j;
	try
	{
		j = json::parse(data);

	}
	catch(std::exception & e)
	{
		loadSteamDBFromFile();
		return;
	}
	for(auto game : j["applist"]["apps"])
	{
		gameDB[game["name"]] = game["appid"];
		gameDBByIndex[game["appid"]] = game["name"];
	}
	std::ofstream out;
	out.open("file.txt");
	if(out.is_open())
	{
		out << data;
	}
}

void SteamBot::loadSteamDBFromFile()
{
	std::ifstream textFile("file.txt");
	std::string data,tmp;
	while(true){
		tmp=textFile.get();
		if(textFile.eof()){ break;}
		data+=tmp;
	}

	json j = json::parse(data);
	for(auto game : j["applist"]["apps"])
	{
		gameDB[game["name"]] = game["appid"];
		gameDBByIndex[game["appid"]] = game["name"];
	}


}


string SteamBot::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return str;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
	return str;
}


string SteamBot::getSaleData(std::string game)
{
	if (gameDB.find(game) == gameDB.end()) return "Данной игры нет в списке. Просьба вводить полное название из Steam. Например не PUBG или ПУбГ а PLAYERUNKNOWN'S BATTLEGROUNDS, т.е. так же как игра называется в Steam";
        int idgame = gameDB[game];
	auto findIterator = priceDB.find(game);
	if ( findIterator  != priceDB.end() )
        {
		if(findIterator->second.percent > 0)
                {
			return "На игру "+game+" объявлена скидка в размере "+to_string(findIterator->second.percent)+" %! Цена составляет " + findIterator->second.fprice +" Изначальная цена ="+findIterator->second.initialprice;
		}
		else if(findIterator->second.initialprice != "0")
                {
			return "На игру "+game+" нет скидки. Цена = "+findIterator->second.fprice;
		}
		else if(findIterator->second.fprice == "-1")
                {
                        return "По игре "+game+"(ID:"+to_string(idgame)+") нет однозначной информации. Возможно повторения в базе Стима по этому названию. ";
		}
		else
                {
			return "Игра "+game+" бесплатна. ";
		}
	}


        string url = "https://store.steampowered.com/api/appdetails?appids="+to_string(idgame)+"&cc=ru&l=ru";
	string c =  NetworkDispetcher::HTTPgetCURL(url.c_str());
	json j;
	try
	{
		j = json::parse(c);

	}
	catch (std::exception & e) {
		cout << "Alert!Нет доступа к серверам Steam!" << endl;
		return "";
	}
	if(j[to_string(idgame)]["data"].contains("price_overview") != NULL)
	{
		string sale = to_string(j[to_string(idgame)]["data"]["price_overview"]["discount_percent"]);
		if(sale != "0")
		{
			string initial = j[to_string(idgame)]["data"]["price_overview"]["initial_formatted"];
			string final = j[to_string(idgame)]["data"]["price_overview"]["final_formatted"];
			gameData data;
			data.fprice = final;
			data.initialprice = initial;
			data.percent = stoi(sale);
			priceDB[game] = data;
			return "На игру "+game+" объявлена скидка в размере "+sale+" %! Цена составляет " + final +" Изначальная цена ="+initial;

		}
		else
		{
			string final = j[to_string(idgame)]["data"]["price_overview"]["final_formatted"];
			gameData data;

			data.fprice = final;
			data.initialprice = final;
			data.percent = 0;//
			priceDB[game] = data;
			return "На игру "+game+" нет скидки. Цена = "+final;
		}
	}
	else if ( j[to_string(idgame)]["success"] == false)
	{
		gameData data;
		data.fprice = "-1";
		data.initialprice = "0";
		data.percent = 0;//для неоднозначных ответов
		priceDB[game] = data;
                return "По игре "+game+"(ID:"+to_string(idgame)+") нет однозначной информации. Возможно повторения в базе Стима по этому названию. ";
	}
        else if( j[to_string(idgame)]["data"].contains("package_groups") && j[to_string(idgame)]["data"]["package_groups"].size() >0 && j[to_string(idgame)]["data"]["package_groups"].at(0)["subs"].at(0)["percent_savings_text"] == "-100%")
	{
		gameData data;
		data.fprice = "0";
		data.initialprice = "0";
		data.percent = 100;
		priceDB[game] = data;
		return "Игра "+game+" сейчас бесплатна на какое-то время. ";
	}
	else
	{
		gameData data;
		data.fprice = "0";
		data.initialprice = "0";
		data.percent = 0;
		priceDB[game] = data;
		return "Игра "+game+" бесплатна. ";
	}

}

void SteamBot::operator()()
{

	while(1)
	{


		unique_lock<std::mutex> bul(parseMutex);
		cvarForReading.wait(bul,[this]()->int{return  !eventsQueue.empty() ;});

		auto message =  eventsQueue.front();
		eventsQueue.pop();
		if (message.second != "/start" )
		{
			auto result = formMessage(message);
			if (result.second != "")//Если нет связи со Steam - то не отвечаем и не проверяем/выполняем подписку
			{
				unique_lock<std::mutex> ul(sendMutex);
				sendQueue.push(std::pair<string,string>(result.first,result.second+checkSubscribe(message)));
			}
		}
		else
		{
                        sendQueue.push(std::pair<string,string>(message.first,"Привет! Для подписки на скидку введи её полное название(как указано в Steam). Когда на игру будет скидка, мы тебя оповестим. Если захочешь отписаться от рассылки по требуемой игре - просто введи её название еще раз. В базе данных стима есть повторения некоторых тайтлов, поэтому по некоторым тайтлам однозначно отобразить информацию нельзя. Но мы работаем над этим ;) Let's do this!"));
		}
		cvarForParsing.notify_all();

	}
}

std::pair<std::string,std::string> SteamBot::formMessage(std::pair<std::string,std::string> & message)
{
	std::pair<std::string,std::string> result;
	result.first = message.first;
	result.second = getSaleData(message.second);
	return result;
}

string SteamBot::checkSubscribe(std::pair<std::string,std::string> & message)
{

	int chat = stoi(message.first);
	if (gameDB.find(message.second) == gameDB.end())  return "";
	int game = gameDB[message.second];
	auto b  = gameSubs.find(game);

	if (b == gameSubs.end())
	{
		std::map<int,long int> map1;
		map1[chat] = static_cast<long int>(std::time(0));
		gameSubs.insert(std::pair<int,std::map<int,long int>>(game,map1));
		newSubscribe(chat,game);
		return "Вы подписались на игру.";
	}
	else
	{
		auto r =    b->second.find(chat);
		if(r != b->second.end())
		{
			b->second.erase(chat);
			removeSubscribe(chat,game);
			return "Вы отписались от игры.";
		}
		else
		{
			b->second[chat] = static_cast<long int>(std::time(0));
			newSubscribe(chat,game);
			return "Вы подписались на игру.";
		}
	}



}

void SteamBot::subscribeThread()
{
	while(1)
	{
		long int timestamp = static_cast<long int>(std::time(0));
		if(timestamp - dbUpdateTimestamp > dbUpdateSeconds)
		{
			loadSteamDB();
			dbUpdateTimestamp = timestamp;
			priceDB.clear();
		}

		for(auto & iterator :gameSubs)
		{
			string name = gameDBByIndex[iterator.first];
			if ( gameDBByIndex.find(iterator.first) == gameDBByIndex.end() ) {removeGame(iterator.first); continue; }

			if ( priceDB.find(name) == priceDB.end() )
			{
				if (getSaleData(name) == "") continue;//Получаем данные, если сервера Steam недоступны - идем к следующей игре.
			}
			for(auto & iteratorSub :iterator.second)
			{
				timestamp = static_cast<long int>(std::time(0));

				if (priceDB[name].percent > 0 && timestamp - iteratorSub.second  > subUpdateSeconds)
				{
					unique_lock<std::mutex> ul(sendMutex);

					sendQueue.push(std::pair<string,string>(to_string(iteratorSub.first) ,
							"Актуальная скидка на игру "+name+"! В размере "+to_string(priceDB[name].percent)+"%! Текущая цена равна "+priceDB[name].fprice+"" ));

					iteratorSub.second = timestamp;

					updateSubscribe(iteratorSub.first,iterator.first);
					ul.unlock();

					cvarForParsing.notify_all();
				}
				else if (timestamp - iteratorSub.second  > subUpdateSeconds)
				{
					iteratorSub.second = timestamp;
					updateSubscribe(iteratorSub.first,iterator.first);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::microseconds(360000));

	}
}

