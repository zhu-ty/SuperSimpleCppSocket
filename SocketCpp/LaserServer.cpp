#include "LaserServer.h"



LaserServer::LaserServer(shared_ptr<int> x, shared_ptr<int> y, shared_ptr<mutex> mtx)
{
	_x = x;
	_y = y;
	_mtx = mtx;
}

LaserServer::~LaserServer()
{
}

void LaserServer::start()
{
	int err;
	///////////////////�����ֿ�////////////////
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		cout << "faild when WSAStartup!" << endl;
		system("pause");
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		cout << "faild when WSAStartup!" << endl;
		system("pause");
		return;
	}
	int max_binner = 20;
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);//���������ڶ���������ѡ�������׽��֣�UDPѡ��SOCK_DGRAM���ݱ��׽���
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;//��������
	addrSrv.sin_port = htons(port);//����������Ķ˿�
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//���������ip��ַɸѡ,��Ϊ�Ƿ�����������ʱlisten�����Բ�����ANY�����Զ�ѡ��Ĭ�ϵı�������������ǿͻ��������ʱconnet����Ҫ��������ΪĿ��ip�� 

	if (::bind(listen_socket, (SOCKADDR*)&addrSrv, sizeof(addrSrv)) == -1)
	{
		cout << "bind() failed." << endl;
		closesocket(listen_socket);//�����ڴ򿪣�ʧ���ˣ�Ӧ���ں����ڹر�
		return;
	}
	if (listen(listen_socket, max_binner) == SOCKET_ERROR)//���Կ�ʼ����,����2�ǵȴ�������󳤶ȣ�Ҳ���Ǽ���ʱ����������
	{
		cout << "Error listening on socket." << endl;
		return;
	}
	cout << "Server Created!" << endl << "Now,Listening``````" << endl;
	listen_thread = new thread(&LaserServer::ListenThread, this);
	listen_thread->detach();
	return;
}

void LaserServer::ListenThread()
{
	while (1)
	{
		communicate_socket = accept(listen_socket, 0, 0);
		if (communicate_thread != nullptr)
		{
			delete communicate_thread;
		}
		communicate_thread = new thread(&LaserServer::CommunicateThread, this, &communicate_socket);
		communicate_thread->detach();
	}
}

void LaserServer::CommunicateThread(LPVOID lparam)
{
	SOCKET *s = (SOCKET*)lparam;
	SOCKADDR_IN dest_add;
	int nAddrLen = sizeof(dest_add);
	//ͨ��RCVSocket��öԷ��ĵ�ַ
	if (::getpeername(*s, (SOCKADDR*)&dest_add, &nAddrLen) != 0)
	{
		std::cout << "Get IP address by socket failed!" << endl;
		return;
	}
	cout << "IP: " << ::inet_ntoa(dest_add.sin_addr) <<
		"  PORT: " << ntohs(dest_add.sin_port) << "connected!" << endl;
	while (1)
	{
		int byte_rev;
		char buffer[DATA_LEN] = { 0 };
		byte_rev = recv(*s, buffer, DATA_LEN, 0);
		if (buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T')
		{
			_mtx->lock();
			char buffer_send[DATA_LEN] = { 0 };
			char buffer_tmp[4];
			intToByte(*_x, buffer_tmp);
			memcpy(buffer_send, buffer_tmp, 4);
			intToByte(*_y, buffer_tmp);
			memcpy(buffer_send + 4, buffer_tmp, 4);
			if(*_x == -1 || *_y == -1)
				intToByte(0, buffer_tmp);
			else
				intToByte(1, buffer_tmp);
			memcpy(buffer_send + 8, buffer_tmp, 4);
			_mtx->unlock();
			send(*s, buffer_send, DATA_LEN, 0);
		}

	}
}

void LaserServer::intToByte(int i, char * bytes, int size)
{
	//byte[] bytes = new byte[4];
	memset(bytes, 0, sizeof(char) *  size);
	bytes[0] = (char)(0xff & i);
	bytes[1] = (char)((0xff00 & i) >> 8);
	bytes[2] = (char)((0xff0000 & i) >> 16);
	bytes[3] = (char)((0xff000000 & i) >> 24);
	return;
}


