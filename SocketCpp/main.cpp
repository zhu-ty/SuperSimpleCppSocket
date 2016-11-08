#include<iostream>
#include"LaserServer.h"
using namespace std;

int main()
{
	int x = 0, y = 0;
	mutex mtx;
	shared_ptr<int> xp, yp;
	shared_ptr<mutex> mtxp;

	xp.reset(&x);
	yp.reset(&y);
	mtxp.reset(&mtx);

	LaserServer ls(xp, yp, mtxp);
	ls.start();
	while (1)
	{
		x++;
		y++; y++;
		Sleep(1000);
	}
	
	return 0;
}