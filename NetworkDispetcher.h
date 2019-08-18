/*
 * NetworkDispetcher.h
 *
 *  Created on: 14 авг. 2019 г.
 *      Author: alab
 */

#ifndef NETWORKDISPETCHER_H_
#define NETWORKDISPETCHER_H_
#include "Parser.h"
#include <iostream>
#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/ossl_typ.h>
#include <unistd.h>
#include <openssl/err.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <thread>
#include <future>
#include <functional>
#include <signal.h>
#include <fstream>
#include "json.hpp"
#include <sstream>
#define MSIZE 1024

enum READTYPE
{
	USESOCKET,
	USECURL
};
class NetworkDispetcher {
private:

	const char * _token; //Ваш токен
	Parser * _parser;//Указатель на парсер, которому будем давать ответы
	READTYPE _type;
	//Параметры для соединения по TLS сокету через webhook
	int _port; //Должен быть виден в интернете
	const char * _privateKeyPath; //Путь к приватному ключу для SSL шифрования
	const char * _certPath; //Путь к сертификату указанному при установке webhook
	const char * _addr; //Добавление адреса в виде ip:port/_addr/ для фильтрации сообщений, должно быть установлено при установке webhook
	SSL_CTX * _sslctx;
	int _desc = -1;


private:
	SSL_CTX * InitializeSSL();
	int initializeWebhookSocket();



public:
	NetworkDispetcher(const char * token,int _port, const char * privateKeyPath,const char * certPath,const char * addr,Parser * parser);
	NetworkDispetcher(const char * token,Parser * parser);
	void operator()();
	virtual ~NetworkDispetcher();
	void closeSocket();
	void sendMessageCURL(const char *chat,const char * message);

	static std::string HTTPgetCURL(const char *url);
};

#endif /* NETWORKDISPETCHER_H_ */
