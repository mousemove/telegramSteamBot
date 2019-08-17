/*
 * NetworkDispetcher.cpp
 *
 *  Created on: 14 авг. 2019 г.
 *      Author: alab
 */

#include "NetworkDispetcher.h"
using namespace std;
using json = nlohmann::json;
NetworkDispetcher::NetworkDispetcher(const char * token,int port, const char * privateKeyPath,const char * certPath,const char * addr,Parser * parser)
: _token(token),_privateKeyPath(privateKeyPath),_certPath(certPath),_addr(addr),_parser(parser),_port(port){
	// TODO Auto-generated constructor stub

	_desc = initializeWebhookSocket();
	_sslctx = InitializeSSL();
	_parser->setND(this);
	_type = USESOCKET;
	//signal(SIGINT,  this->listener );
}

NetworkDispetcher::NetworkDispetcher(const char * token,Parser * parser)
:_token(token),_parser(parser)
{
	_parser->setND(this);
	_type = USECURL;
}

NetworkDispetcher::~NetworkDispetcher() {
	// TODO Auto-generated destructor stub
	if(_desc != -1) close(_desc);
	_desc = -1;
}

void NetworkDispetcher::closeSocket()
{

	if(_desc != -1) { close(_desc);
	_desc = -1;
	}
}



int NetworkDispetcher::initializeWebhookSocket()
{
    int sockd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockd < 0) exit(EXIT_FAILURE);

    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(_port);

    if (bind(sockd, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
        std::cout << "Socket bind error" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket initialization DONE" << std::endl;
    return sockd;
}


SSL_CTX * NetworkDispetcher::InitializeSSL() {
    //OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_library_init();
    SSL_CTX * sslctx = SSL_CTX_new(TLSv1_2_method());

    if (SSL_CTX_use_certificate_file(sslctx,  _certPath , SSL_FILETYPE_PEM) <= 0) {
        exit(-2);
    }
    if (SSL_CTX_use_PrivateKey_file(sslctx,   _privateKeyPath, SSL_FILETYPE_PEM) <= 0) {
        exit(-2);
    }
    if (!SSL_CTX_check_private_key(sslctx)) {
       exit(-2);
    }
    std::cout << "SSL initialize DONE" << std::endl;
    return sslctx;
}

void NetworkDispetcher::sendMessageCURL(const char *chat,const char * message)
{
  int result;
  string url = "https://api.telegram.org/bot"+string(_token)+"/sendMessage?chat_id="+string(chat)+"&text="+string(message);
  //cout << url << endl;
  HTTPgetCURL(url.c_str());

}



size_t readFunction(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string NetworkDispetcher::HTTPgetCURL(const char *url)
{
	  CURL *curl;
	  CURLcode res;
	  string result = "";
	  curl = curl_easy_init();
	  if(curl) {
	      curl_easy_setopt(curl, CURLOPT_URL, url);
	      /* example.com is redirected, so we tell libcurl to follow redirection */
	      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	      /* Perform the request, res will get the return code */
	      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &readFunction);
	      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
	      //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &in_message);
	      res = curl_easy_perform(curl);
	      /* always cleanup */
	      curl_easy_cleanup(curl);
	      return result;
	    }
}

void NetworkDispetcher::operator()(){

	if(_type == USESOCKET) {
	listen(_desc, 5);
	 char CorrectPostPathHeader[1024];
	 sprintf(CorrectPostPathHeader,"POST /%s/ HTTP/1.1\r\n", _token);
	 const char * CorrectJsonHeader = "Content-Type: application/json";

	while (1) {

	    int client = accept(_desc, NULL, NULL);

	    int acceptState = 0;
	    SSL * ssl = SSL_new(_sslctx);
	    SSL_set_fd(ssl, client);

	    acceptState = SSL_accept(ssl);
	    if (acceptState  <= 0) {
	        SSL_clear(ssl);//
	        cout << "SSL accept error =  " << SSL_get_error(ssl,acceptState) << endl;
	        close(client);
	        continue;
	    }
	  auto k =  std::thread([=]{

	        const char * response = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n";

	        char answer[4096];
	        memset(answer,0,sizeof(answer));
	        int n = 0;
	       n = SSL_read(ssl,answer,4096);
	       if(n!=0)

	       if (strstr(answer,CorrectPostPathHeader) == NULL || strstr(answer,CorrectJsonHeader) == NULL ) {
		       SSL_clear(ssl);
		       SSL_free(ssl);
		       close(client);
		       return;
	       }

	        SSL_write(ssl, response, (int)strlen(response)) ;
	       n = SSL_read(ssl,answer,4096);
	       if(n!=0)

	        SSL_write(ssl, response, (int)strlen(response)) ;
	       SSL_clear(ssl);
	       SSL_free(ssl);
	       close(client);

	     if(n > 0 )
	     {
	    	 unique_lock<std::mutex> ul(_parser->rMutex);
	    	_parser->MessagesQueue.push(string(answer));
	    	 _parser->cvar.notify_all();
	     }

	    });
	  k.detach();

	}
	}
	else if(_type == USECURL){
		 char url[128];
		int offset = 0;
		while (1) {
			sprintf(url,"https://api.telegram.org/bot%s/getUpdates?limit=1&offset=%d&timeout=10", _token,offset);
			string data =  NetworkDispetcher::HTTPgetCURL(url);
			json j;
			try
			{
			j = json::parse(data);
			}
			catch (std::exception e) {

				cout << "isnt json" << endl;
			    continue;
			}
			if(j.contains("ok") && j["ok"] == true && j.contains("result") && j["result"].size() > 0 && j["result"].at(0).contains("update_id") && j["result"].at(0)["message"]["from"].contains("id")  )
			{
				offset = j["result"].at(0)["update_id"];
				offset++;
		    if(j["result"].at(0)["message"].contains("text")){
				unique_lock<std::mutex> ul(_parser->rMutex);
		    	 string s = j["result"].at(0).dump();
			    _parser->MessagesQueue.push(s);
			    _parser->cvar.notify_all();
		    }
			}

		}

	}
}
