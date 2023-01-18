#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <time.h>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ctime>
#pragma comment(lib, "Ws2_32.lib")
#define IDLE 1
#define SEND 2
#define LISTEN 1
#define RECEIVE 2
using namespace std;

struct SocketState
{
	SOCKET id;
	int	recv;
	int	send;
	int sendSubType;
	char buffer[1024];
	int len;
};
const int MAX_SOCKETS = 60;
const int TIME_PORT = 8080;
int socketsCount = 0;
struct SocketState sockets[MAX_SOCKETS] = { 0 };


string getBody(string request)
{
	int size = request.size();
	char len[1024];
	int bodySize;
	char body[1024];
	int lenIndex = request.find("Content-Length:") + 16;
	int num;
	int n = 0;
	int m;

	while (request[lenIndex] != '\n' && request[lenIndex] != '\r') {
		len[n] = request[lenIndex];
		lenIndex++;
		n++;
	}

	bodySize = atoi(len);
	m = bodySize;

	for (int i = 0; i < bodySize + 1; i++) {

		body[m] = request[size];
		size--;
		m--;
	}
	return body;
}
string getResource(string request) {

	int i = 0;
	int j = 0;
	string resource;
	while (request[i] != ' ')
	{
		i++;
	}
	i++;
	j = i;

	int indexLen = request.find("?");
	if (indexLen != -1) {
		resource = request.substr(i, indexLen - i);
	}
	else
	{
		resource = " ";
	}
	return resource;
}

string getFileName(string request) {
	int i = 0;
	int j = 0;
	while (request[i] != ' ')
	{
		i++;
	}
	i++;
	j = i;
	while (request[j] != ' ')
	{
		j++;
	}

	string fileName = request.substr(i, j - i);


	return fileName;
}

string getLanguage(string request) {

	string lang = "";
	int langIndex = request.find("lang");
	int fromIndex = langIndex + 5;
	int toIndex = 0;
	int i = fromIndex;

	while (request[i] != ' ') {
		toIndex++;
		i++;
	}

	lang = request.substr(fromIndex, toIndex);

	return lang;
}


string GetRequestType(string req)
{
	string reqType = "";

	int j = 0;
	while (req[j] != ' ')
	{
		j++;
	}
	reqType = req.substr(0, j);

	return reqType;

}

string GetNowTime() {
	char buf[1024];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S GMT", &tm);
	return buf;
}



string DeleteResponseCreator(string request)
{

	string path = "C:\\temp\\" + getFileName(request);
	string res;
	ifstream file(path.c_str());

	if (file.is_open())
	{

		file.close();
		remove(path.c_str());
		res =
			"HTTP/1.1 200 OK\r\n"
			"Date: " + GetNowTime() + "\r\n"
			"Server: myServer\r\n"
			"Connection: Keep-Alive\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 21\r\n\r\n"
			"We deleted the file.\n";
	}
	else
	{
		res =
			"HTTP/1.1 400 Bad Request\r\n"
			"Date: " + GetNowTime() + "\r\n"
			"Server: myServer\r\n"
			"Connection: Keep-Alive\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 14\r\n\r\n"
			"No such file.\n";
	}

	return res;
}
string PostResponseCreator() {


	string res =
		"HTTP/1.1 200 OK\r\n"
		"Date: " + GetNowTime() + "\r\n"
		"Server: myServer\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 18\r\n\r\n"
		"We got your POST!\n";

	return res;
}

string GetLastModified(string i_FileName) {
	struct stat res;
	stat(i_FileName.c_str(), &res);
	struct tm  time;
	time = *localtime(&res.st_mtime);
	char lastMod[100];
	strftime(lastMod, sizeof(lastMod), "%a, %d %b %Y %X GMT", &time);
	return lastMod;
}

string applicationCreator(string filename, int type) {
	string app, header, file;
	int fileSize;

	getline(ifstream(filename), file, '\0');
	fileSize = strlen(file.c_str()) - 1;
	if (strcmp(filename.c_str(), "C:\\temp\\error404.html") == 0) {

		header = "HTTP/1.1 404 Not Found\r\n";
	}
	else {
		header = "HTTP/1.1 200 OK\r\n";
	}

	header +=
		"Date: " + GetNowTime() + "\r\n"
		"Server: myServer\r\n"
		"Last-Modified: " + GetLastModified(filename) + "\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + to_string(fileSize) + "\r\n\r\n";

	if (type == 0) {
		app = header + file;
		return app;
	}
	else {
		return header;
	}


}


string HeadAndGetResponseCreator(string request, int type) {

	string res;
	string source = getResource(request);
	string lang = getLanguage(request);

	if (strcmp(source.c_str(), "/fileHTML.html") != 0) {
		res = applicationCreator("C:\\Temp\\error404.html", type);
	}
	else {

		if (strcmp(lang.c_str(), "he") == 0 || strcmp(lang.c_str(), "fr") == 0 || strcmp(lang.c_str(), "en") == 0)
		{
			res = applicationCreator("C:\\Temp\\fileHTML_" + lang + ".html", type);
		}

		else
		{
			res = applicationCreator("C:\\Temp\\error404.html", type);
		}
	}

	return res;
}

string PutResponseCreator(string request) {

	string path = "C:\\temp\\" + getFileName(request);
	string res;
	ifstream file(path.c_str());


	if (file.is_open())
	{

		file.close();
		res =
			"HTTP/1.1 204 No Content\r\n"
			"Date: " + GetNowTime() + "\r\n"
			"Connection: Keep-Alive\r\n"
			"Server: myServer\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 0\r\n\r\n\n";
	}
	else
	{

		res =
			"HTTP/1.1 201 Created\r\n"
			"Date: " + GetNowTime() + "\r\n"
			"Connection: Keep-Alive\r\n"
			"Server: myServer\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 17\r\n\r\n"
			"We got your PUT.\n";

	}
	ofstream file1;
	file1.open(path);
	file1 << getBody(request);
	file1.close();


	return res;
}

string OptionsResponseCreator() {

	string res =
		"HTTP/1.1 200 OK\r\nDate: " + GetNowTime() + "\r\nServer: myServer\r\nConnection: Keep-Alive\r\n"
		"Content-Type: text/html\r\nContent-Length: 20\r\n"
		"Allow: HEAD, GET, POST, PUT, DELETE, OPTIONS, TRACE\r\n\r\n"
		"We got your OPTIONS\n";
	return res;
}

string TraceResponseCreator(string request) {
	string res =
		"HTTP/1.1 200 OK\r\nDate: " + GetNowTime() + "\r\nServer: myServer\r\nConnection: Keep-Alive\r\n"
		"Content-Type: message/http\r\n"
		"Content-Length: " + to_string(request.length() + 1) + "\r\n\r\n"
		+ request + "\r\n";

	return res;
}

string ErrorResponseCreator() {

	string res =
		"HTTP/1.1 405 Method Not Allowed\r\n"
		"Date: " + GetNowTime() + "\r\n"
		"Server: myServer\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 29\r\n"
		"Allow:  GET, HEAD, POST, OPTIONS, TRACE, PUT, DELETE\r\n\r\n"
		"Error : Not supported method\n";

	return res;
}

string BuildTimeoutResponse() {

	string response =
		"HTTP/1.1 408 Request Timeout\r\n"
		"Date: " + GetNowTime() + "\r\n"
		"Server: WebServer\r\n"
		"Connection: close\r\n\r\n\n";

	return response;
}

bool addSocket(SOCKET id, int what)
{
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].recv == 0)
		{
			sockets[i].id = id;
			sockets[i].recv = what;
			sockets[i].send = IDLE;
			sockets[i].len = 0;
			socketsCount++;
			return (true);
		}
	}
	return (false);
}

void removeSocket(int index)
{
	sockets[index].recv = 0;
	sockets[index].send = 0;
	socketsCount--;
}

void acceptConnection(int index)
{
	SOCKET id = sockets[index].id;
	struct sockaddr_in from;
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket)
	{
		cout << "Time Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	cout << "Time Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;


	unsigned long flag = 1;
	if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0)
	{
		cout << "Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
	}

	if (addSocket(msgSocket, RECEIVE) == false)
	{
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
	return;
}

void receiveMessage(int index)
{
	SOCKET msgSocket = sockets[index].id;

	int len = sockets[index].len;
	int bytesRecv = recv(msgSocket, &sockets[index].buffer[0], sizeof(sockets[index].buffer), 0);

	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Server: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);
		removeSocket(index);
		return;
	}
	if (bytesRecv == 0)
	{
		closesocket(msgSocket);
		removeSocket(index);
		return;
	}
	else
	{
		sockets[index].buffer[bytesRecv] = '\0';
		sockets[index].len += bytesRecv;
		sockets[index].send = SEND;
	}

}

void sendMessage(int index)
{
	int bytesSent = 0;
	char sendBuff[1024];
	string httpRes;
	SOCKET msgSocket = sockets[index].id;
	string requestType = GetRequestType(sockets[index].buffer);

	if (strcmp(requestType.c_str(), "GET") == 0)
	{
		httpRes = HeadAndGetResponseCreator(sockets[index].buffer, 0);
	}
	else if (strcmp(requestType.c_str(), "POST") == 0)
	{
		cout << getBody(sockets[index].buffer) << endl;
		httpRes = PostResponseCreator();
	}
	else if (strcmp(requestType.c_str(), "OPTIONS") == 0)
	{
		httpRes = OptionsResponseCreator();
	}
	else if (strcmp(requestType.c_str(), "TRACE") == 0)
	{
		httpRes = TraceResponseCreator(sockets[index].buffer);
	}
	else if (strcmp(requestType.c_str(), "HEAD") == 0)
	{
		httpRes = HeadAndGetResponseCreator(sockets[index].buffer, 1);
	}
	else if (strcmp(requestType.c_str(), "PUT") == 0)
	{
		httpRes = PutResponseCreator(sockets[index].buffer);
	}
	else if (strcmp(requestType.c_str(), "DELETE") == 0)
	{
		httpRes = DeleteResponseCreator(sockets[index].buffer);
	}

	else
	{
		httpRes = ErrorResponseCreator();
	}

	strcpy(sendBuff, httpRes.c_str());
	bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff), 0);
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}

	sockets[index].send = IDLE;
}


void main()
{
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	// Server side:
	// Create and bind a socket to an internet address.
	// Listen through the socket for incoming connections.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called listenSocket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							streaming sockets (SOCK_STREAM), 
	//							and the TCP/IP protocol (IPPROTO_TCP).
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The if statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == listenSocket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a server to communicate on a network, it must bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigned long 
	// (4 bytes) data type).
	// inet_addr (Iternet address) is used to convert a string (char *) 
	// into unsigned long.
	// The IP address is INADDR_ANY to accept connections on all interfaces.
	serverService.sin_addr.s_addr = INADDR_ANY;
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order 
	// (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	// Listen on the Socket for incoming connections.
	// This socket accepts only one connection (no pending connections 
	// from other clients). This sets the backlog parameter.
	if (SOCKET_ERROR == listen(listenSocket, 5))
	{
		cout << "Time Server: Error at listen(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}
	addSocket(listenSocket, LISTEN);

	// Accept connections and handles them one by one.
	while (true)
	{
		// The select function determines the status of one or more sockets,
		// waiting if necessary, to perform asynchronous I/O. Use fd_sets for
		// sets of handles for reading, writing and exceptions. select gets "timeout" for waiting
		// and still performing other operations (Use NULL for blocking). Finally,
		// select returns the number of descriptors which are ready for use (use FD_ISSET
		// macro to check which descriptor in each set is ready to be used).
		fd_set waitRecv;
		FD_ZERO(&waitRecv);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if ((sockets[i].recv == LISTEN) || (sockets[i].recv == RECEIVE))
				FD_SET(sockets[i].id, &waitRecv);
		}

		fd_set waitSend;
		FD_ZERO(&waitSend);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (sockets[i].send == SEND)
				FD_SET(sockets[i].id, &waitSend);
		}

		//
		// Wait for interesting event.
		// Note: First argument is ignored. The fourth is for exceptions.
		// And as written above the last is a timeout, hence we are blocked if nothing happens.
		//
		int nfd;
		nfd = select(0, &waitRecv, &waitSend, NULL, NULL);
		if (nfd == SOCKET_ERROR)
		{
			cout << "Time Server: Error at select(): " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitRecv))
			{
				nfd--;
				switch (sockets[i].recv)
				{
				case LISTEN:
					acceptConnection(i);
					break;

				case RECEIVE:
					receiveMessage(i);
					break;
				}
			}
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitSend))
			{
				nfd--;
				switch (sockets[i].send)
				{
				case SEND:
					sendMessage(i);
					break;
				}
			}
		}
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(listenSocket);
	WSACleanup();
}