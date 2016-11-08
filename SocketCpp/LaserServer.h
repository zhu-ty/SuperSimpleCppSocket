#pragma once
#include<Winsock2.h>
#include<vector>
#include<thread>
#include<mutex>
#include<iostream>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

#define DATA_LEN 16

class LaserServer
{
public:
	LaserServer(shared_ptr<int> x, shared_ptr<int> y, shared_ptr<mutex> mtx);
	~LaserServer();
	const int port = 986;

	void start();
private:
	SOCKET listen_socket;
	SOCKET communicate_socket;
	thread *listen_thread = nullptr;
	thread *communicate_thread = nullptr;
	shared_ptr<mutex> _mtx;
	shared_ptr<int> _x, _y;

	void ListenThread();
	void CommunicateThread(LPVOID lparam);

	void  intToByte(int i, char *bytes, int size = 4);


};

