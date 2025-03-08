#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "..\MorpionC++Webserver\Bank.h"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

DWORD WINAPI ConnectionThread(LPVOID);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);// Message loop

HWND hwnd;

Bank bank = Bank();


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // Create a window 

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
    hwnd = CreateWindowEx(0, L"ServerWindowClass", L"Server Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        return FALSE;
    }
    HANDLE hConnectionThread;
    DWORD dwThreadId;

    // Création du thread de connexion
    hConnectionThread = CreateThread(
        NULL, // attributs de sécurité par défaut
        0,    // taille de pile par défaut
        ConnectionThread, // fonction du thread
        NULL, // aucun argument passé au thread
        0,    // création immédiate du thread
        &dwThreadId); // ID du thread

    if (hConnectionThread == NULL) {
        cerr << "CreateThread failed with error: " << GetLastError() << endl;
        return 1;
    }

    // Attendre que le thread de connexion se termine (optionnel)
    //WaitForSingleObject(hConnectionThread, INFINITE);

    // faire un select
    //ShowWindow(hwnd, nCmdShow);

    //GetMessage

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        UpdateWindow(hwnd);
    }



    // Fermeture du handle du thread
    CloseHandle(hConnectionThread);

    // Nettoyage
    WSACleanup();

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


    SOCKET SocketInfo;
    SOCKET newSocket;
    WSABUF dataBuf;
    DWORD RecvBytes;
    DWORD Flags = 0;
    char recvBuffer[512];
    dataBuf.buf = recvBuffer;
    dataBuf.len = 512;

    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }

    case WM_USER + 1:
        // Determine what event occurred on the socket
        switch (WSAGETSELECTEVENT(lParam))
        {
        case FD_ACCEPT:
            // Accept an incoming connection
            newSocket = accept(wParam, NULL, NULL);
            // Prepare accepted socket for read, write, and close notification
            WSAAsyncSelect(newSocket, hwnd, WM_USER + 1, FD_READ);
            //send(newSocket, "Server Responds", (int)strlen("Server says hi 2"), 0);
            break;

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
                string message(recvBuffer, recvBuffer + RecvBytes);
                if (RecvBytes > 0) {
                    // Envoyer une réponse HTTP simple
                    if (recvBuffer[0] == 'G' && recvBuffer[1] == 'E' && recvBuffer[2] == 'T') {
                        std::string httpResponseString = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <html><title>Centered Empty Tic Tac Toe Board</title> <style> body, html { height: 100%; margin: 0; display: flex; justify-content: center; align-items: center; background-color: #f0f0f0; } .board { display: grid; grid-template-columns: repeat(3, 150px); /* Increased cell size */ grid-template-rows: repeat(3, 150px); /* Increased cell size */ gap: 3px; /* Adjusted for visual separation */ border: 3px solid black; /* Outer border */ } .cell { width: 150px; /* Increased size */ height: 150px; /* Increased size */ background-color: #fff; border: 2px solid #000; /* Cell border */ display: flex; align-items: center; justify-content: center; font-size: 3em; /* Adjusted for bigger cells, though initially empty */ } </style> </head> <body> <div class='board'> <div class='cell'>";
                        
                        std::vector<char> game = bank.GetBank();
                        std::vector<string> grid;

                        for (int i = 0; i < 9; i++) {
                            grid.push_back(string(1, game[i]));
                            if (i == 8) {
                                httpResponseString.append(grid[i]);
                                httpResponseString.append("</div></div></body></html>");
                            }
                            else {
                                httpResponseString.append(grid[i]);
                                httpResponseString.append("</div><div class='cell'>");
                            }
                        }
                        const char* httpResponse = httpResponseString.c_str();
                        send(SocketInfo, httpResponse, strlen(httpResponse), 0);

                    }

                }
            }
            break;
        case FD_CLOSE:



            // The connection is now closed

            closesocket((SOCKET)wParam);

            break;
        }
        break;
    }
}


DWORD WINAPI ConnectionThread(LPVOID lpParam) {

    WSADATA wsaData;
    SOCKET listenSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize, result;

    // Initialiser Winsock
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
        return 1;
    }

    // Créer la socket d'écoute
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        cerr << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Lier la socket
    WSAAsyncSelect(listenSocket, hwnd, WM_USER + 1, FD_ACCEPT);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(80); // Port HTTP standard
    if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed with error: " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Écouter sur la socket
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed with error: " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

}