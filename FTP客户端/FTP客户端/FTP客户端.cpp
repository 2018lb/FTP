// FTP客户端.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "iostream"
#include "fstream"
#include "winsock2.h"
#include  "direct.h"   
#pragma warning(disable:4996)

struct fileMessage
{
	char fileName[50];
	int fileSize;
};

#define PORT 65432        //定义要连接的服务器端的端口号
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int main()
{
	/***定义网络连接的相关的变量***/
	int sock_client;     //定义套接字
	struct sockaddr_in  server_addr;  //用于存放服务器地址的变量
	int addr_len = sizeof(struct sockaddr_in); //地址长度
	/***初始化winsock DLL***/
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);  //生成版本号
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		cout << "加载winsock动态连接库失败！\n";
		return 0;
	}
	/***创建套接字***/
	if ((sock_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) //建立一个socket
	{
		cout << "创建套接字失败！\n";
		WSACleanup();
		return 0;
	}
	/***连接服务器***/
	memset((void*)&server_addr, 0, addr_len);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.8.0.8");
	if (connect(sock_client, (struct sockaddr*)&server_addr, addr_len) != 0)
	{
		cout << "连接失败！\n";
		closesocket(sock_client);
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "连接成功，等待传输\n";
	}
	/***定义文件传输所需变量***/
	struct fileMessage fileMsg;
	long int filelen;
	char filename[500] = "D:\\ClientDownload\\";//指定接收到的文件的保存目录
	char ok[3] = "OK";
	char fileBuffer[1000];//接收文件数据的缓冲区
	/***创建文件的保存目录***/
	_mkdir(filename); //_mkdir()用于创建文件夹
	/***接收文件名及文件长度信息***/
	if ((filelen = recv(sock_client, (char*)&fileMsg, sizeof(fileMsg), 0)) <= 0)
	{
		cout << "未接收到文件名字及文件长度！\n";
		closesocket(sock_client);
		WSACleanup();
		return 0;
	}
	filelen = ntohl(fileMsg.fileSize);
	strcat(filename, fileMsg.fileName);//连接两个字符串
	/***创建文件准备接收文件内容***/
	ofstream outFile(filename, ios::out | ios::binary);
	if (!outFile.is_open())
	{
		cout << "Cannot open " << filename << endl;
		closesocket(sock_client);
		WSACleanup();
		return 0;  //文件打开失败则退出
	}
	send(sock_client, ok, sizeof(ok), 0);//发送接收文件数据的确认信息
  /***接收文件数据并写入文件***/
	int size = 0;  //接收到的数据长度
	do
	{
		size = recv(sock_client, fileBuffer, sizeof(fileBuffer), 0);
		outFile.write(fileBuffer, size);
		filelen -= size;
	} while (size != 0 && filelen > 0);  
	/***文件传输完成结束程序***/
	cout << "文件传输成功，保存在D:\\ClientDownload\n";
	outFile.close();
	closesocket(sock_client);
	WSACleanup();
	system("pause");
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
