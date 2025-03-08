#pragma comment(lib, "ws2_32.lib")

#define NOMINMAX 
#define PORT 6969
#define DATA_BUFSIZE 8192
#define IPADDRESS "10.1.144.26"

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <json/json.h>
#include <json/writer.h>

#include "ClientGame.h"

using namespace std;

typedef struct _SOCKET_INFORMATION {
	BOOL RecvPosted;
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	DWORD BytesSEND;
	DWORD BytesRECV;
	struct _SOCKET_INFORMATION* Next;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

#define WM_SOCKET (WM_USER + 1)

LPSOCKET_INFORMATION SocketInfoList;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

ClientGame thisGame;

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
int main()
{
	HINSTANCE hInstance = GetModuleHandle(0);

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// Create a window
	HWND hwnd;
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"ServerWindowClass";
	RegisterClassEx(&wc);
	hwnd = CreateWindowEx(0, L"ServerWindowClass", L"Server Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);

	if (hwnd == NULL)
	{
		return FALSE;
	}

	//Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		//Tell the user that we could not find a usable
		//Winsock DLL.                                  
		MessageBox(NULL, L"WSAStartup failed with error: %d", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	int status, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 };

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, IPADDRESS, &serv_addr.sin_addr)
		<= 0) {
		MessageBox(NULL, L"Invalid address / Address not supported", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//First connection

	// MAKE SURE THIS IS ACTUALLY CONNECTING TO THE SERVER !! 
	// IT MIGHT JUST BE CONNECTING THE CLIENT TO ITSELF !!! :(
	if ((status = connect(clientSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
		MessageBox(NULL, L"Connection Failed", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}
	WSAAsyncSelect(clientSocket, hwnd, WM_SOCKET, FD_READ | FD_CLOSE);
	//////////////////////////////////////////////////////////////////////////////////////////////

	if (hwnd == NULL) {
		MessageBox(NULL, L"Window creation failed", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	thisGame.window->LoadingDisplay();

	bool loggedIn = false;

	// Message loop
	string username;
	MSG msg;
	
	//GETS HERE TOO LATE, SERVER ALREADY SENT DATA BEFORE
	//while we're ready to receive messages from the server, and we're still playing.
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (thisGame.playing != true) {
			
			thisGame.window->Display(thisGame.currentGrid);

			system("PAUSE");
			break;
		}


		//if logged in, check for events!
		if (loggedIn) {
			thisGame.window->Display(thisGame.currentGrid);
			if (thisGame.inputs->EventCheck(username)) {
				if (thisGame.inputs->readyToSend) {
					send(clientSocket, thisGame.inputs->currentMessage.c_str(), strlen(thisGame.inputs->currentMessage.c_str()), 0);
					thisGame.inputs->readyToSend = false;
					thisGame.inputs->EmptyMessage();
				}
			}
		} else if (!loggedIn) {
			////////////////////////////////////////// LOGGING IN //////////////////////////////////////
			// entering username.
			cout << "Enter your Username : " << endl;
			getline(cin, username);
			cout << username << endl;

			// creating the request line for assigning the player's role in the game.
			Json::Value firstReq;
			firstReq["requestType"] = "Login";
			firstReq["playerName"] = username;
			firstReq["clientSocket"] = clientSocket;

			//making the Json into a string.
			Json::FastWriter fastWriter;
			std::string output = fastWriter.write(firstReq);
			const char* tmp = output.c_str();

			//sending the message to the server.
			send(clientSocket, tmp, strlen(tmp), 0);
			printf("User Data sent\n");
			printf("%s\n", buffer);

			loggedIn = true;
		}
	}

	// Cleanup
	closesocket(clientSocket);
	WSACleanup();

	return (int)msg.wParam;
	
}

void FreeSocketInformation(SOCKET s)
{
	SOCKET_INFORMATION* SI = SocketInfoList;
	SOCKET_INFORMATION* PrevSI = NULL;

	while (SI)
	{
		if (SI->Socket == s)
		{
			if (PrevSI) {
				PrevSI->Next = SI->Next;
			}
			else {
				SocketInfoList = SI->Next;
				closesocket(SI->Socket);
				GlobalFree(SI);
				return;
			}
		}
		PrevSI = SI;
		SI = SI->Next;
	}
}

void CreateSocketInformation(SOCKET s)
{
	LPSOCKET_INFORMATION SI;

	if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL)
	{
		printf("GlobalAlloc() failed with error %d\n", GetLastError());
		return;
	}
	else
		printf("GlobalAlloc() for SOCKET_INFORMATION is OK!\n");
	// Prepare SocketInfo structure for use
	SI->Socket = s;
	SI->RecvPosted = FALSE;
	SI->BytesSEND = 0;
	SI->BytesRECV = 0;
	SI->Next = SocketInfoList;
	SocketInfoList = SI;
}

LPSOCKET_INFORMATION GetSocketInformation(SOCKET s)
{
	SOCKET_INFORMATION* SI = SocketInfoList;
	while (SI)
	{
		if (SI->Socket == s)
			return SI;
		SI = SI->Next;
	}
	return NULL;
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

{
	SOCKET SocketInfo;
	WSABUF dataBuf;
	DWORD RecvBytes;
	DWORD SendBytes;
	DWORD Flags = 0;
	char recvBuffer[512];
	dataBuf.buf = recvBuffer;
	dataBuf.len = 512;
	Json::Reader reader;
	Json::FastWriter fastWriter;

	if (uMsg == WM_SOCKET)
	{
		if (WSAGETSELECTERROR(lParam))
		{
			cout << "Socket failed with error " << WSAGETSELECTERROR(lParam) << endl;
			FreeSocketInformation(wParam);
		}
		else
		{

			// Determine what event occurred on the socket
			switch (WSAGETSELECTEVENT(lParam)) {
			case FD_READ:
				


				SocketInfo = wParam;

				if (WSARecv(SocketInfo, &dataBuf, 1, &RecvBytes, &Flags, NULL, NULL) == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						printf("WSARecv() failed with error %d\n", WSAGetLastError());
						closesocket(SocketInfo);
						return 0;
					}
				}
				else {

					//turn string into Json
					string message(recvBuffer, recvBuffer + RecvBytes);

					Json::Value board;

					bool parsingSuccessful = reader.parse(message, board);
					if (!parsingSuccessful)
					{
						cout << "Error parsing the string" << endl;
					}
					else {

						//when the server sends an updated grid
						if (board["requestType"] == "sendGrid") {

							string gridString = board["grid"].asString();

							for (int i = 0; i < 9; i++)
							{
								thisGame.currentGrid[i] = gridString.at(i);
							}

							

						}
						//when the server says the game is over
						if (board["requestType"] == "end") {

							if (board["winner"] == "draw") {
								cout << "it's a draw!" << endl;
							}
							else {
								cout << board["winner"] << "wins!" << endl;
								cout << "your score is" << board["score"] << endl;
							}
							thisGame.playing = false;
							//ask to replay or end the game
						}
					}
				}
				break;

			case FD_CLOSE:
				printf("Closing socket %d\n", (int)wParam);
				FreeSocketInformation(wParam);
				thisGame.playing = false;
				break;
			}
		}
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);

}
