// FTP服务器.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "iostream"
#include "fstream"
#include "winsock2.h"
#pragma warning(disable:4996)
struct fileMessage
{
	char fileName[50];
	int fileSize;
};

#define PORT 65432         //定义监听端口号
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int main()
{
	/***定义网络连接的相关的变量***/
	SOCKET sock_server;     //定义监听套接字
	struct sockaddr_in  addr, client_addr;  //存放本地地址和客户地址的变量
	int addr_len = sizeof(struct sockaddr_in); //地址长度
	SOCKET newsock;  //存储accept()返回的套接字描述符
	/***初始化winsock DLL***/
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);  //生成版本号
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		cout << "加载winsock动态连接库失败！\n";
		return 0;
	}
	/***创建监听套接字***/
	if ((sock_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cout << "创建套接字失败！\n";
		WSACleanup();
		return 0;
	}
	/***绑定IP地址与端口***/
	memset((void*)&addr, 0, addr_len);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);//允许使用本机的任何IP地址

	if (bind(sock_server, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		cout << "绑定失败！\n";
		closesocket(sock_server);  //关闭套接字
		WSACleanup();  //注销WinSock动态连接库
		return 0;  //结束程序
	}
	/***开始监听***/
	if (listen(sock_server, 5) != 0)
	{
		cout << "listen函数调用失败！\n";
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}
	else
		cout << "等待用户连接，listenning......\n";

	/***输入要传输的文件的路径***/
	char filename[500];//用于存储要传输的文件的文件路径
	cout << "输入要传输的文件路径：";
	cin.getline(filename, 500);

	while (1)
	{
		/***接收客户连接请求***/
		if ((newsock = accept(sock_server, (struct sockaddr*)&client_addr, &addr_len)) == INVALID_SOCKET)
		{
			cout << "accept函数调用失败！\n";
			closesocket(sock_server);
			WSACleanup();
			return 0;
		}
		cout << "connect from " << inet_ntoa(client_addr.sin_addr) << endl; //输出客户机地址
		/***定义文件传输所需变量***/
		char OK[3], fileBuffer[1000];  //接收“OK”的缓冲区和发送缓冲区
		struct fileMessage fileMsg;    //定义保存文件名及文件长度的结构变量
		/***从文件路径中提取文件名保存到结构变量fileMsg中***/
		int size = strlen(filename);
		while (filename[size] != '\\' && size > 0)
			size--;
		strcpy(fileMsg.fileName, filename + size);
		/***打开要传输的文件***/
		ifstream inFile(filename, ios::in | ios::binary);
		if (!inFile.is_open())
		{
			cout << "Cannot open " << filename << endl;
			closesocket(newsock);    //关闭socket，对方等待的recive()函数将返回0
			closesocket(sock_server);
			WSACleanup();
			return 0;  //文件打开失败则退出
		}
		/***获取文件长度***/
		inFile.seekg(0, ios::end);    //将文件的位置指针移到文件末尾
		size = inFile.tellg();		//获取当前文件位置指针值，该值即为文件长度
		inFile.seekg(0, ios::beg);    //将文件的位置指针返回到文件头
		/***发送文件名及文件长度***/
		fileMsg.fileSize = htonl(size);  //将文件长度存入结构变量fileMsg
		send(newsock, (char*)&fileMsg, sizeof(fileMsg), 0); //发送fileMsg
		/***接收对方发送来的OK信息***/
		if (recv(newsock, OK, sizeof(OK), 0) <= 0)
		{
			cout << "接收OK失败，程序退出！\n";
			closesocket(newsock);
			closesocket(sock_server);
			WSACleanup();
			return 0;
		}
		/***发送文件内容***/
		if (strcmp(OK, "OK") == 0)
		{
			while (!inFile.eof())
			{
				inFile.read(fileBuffer, sizeof(fileBuffer));
				size = inFile.gcount();//获取实际读取的字节数
				send(newsock, fileBuffer, size, 0);
			}
			cout << "文件传输成功";//显示传输完成
			inFile.close();//关闭文件
		}
		else
			cout << "对方无法接收文件! ";
		/***文件传输完成结束程序***/
		closesocket(newsock);



		cout << "\n\n等待用户连接，listenning......\n";

		/***输入要传输的文件的路径***/
		char filename[500];//用于存储要传输的文件的文件路径
		cout << "输入要传输的文件路径：";
		cin.getline(filename, 500);
	}


	closesocket(sock_server);
	WSACleanup();
	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
