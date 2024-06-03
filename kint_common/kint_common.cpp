#include "pch.h"
#include "framework.h"
#include "kint_common.hpp"

namespace kint_common {
	char* keypress_t::as_raw_memory() {
		return (char*)this;
	}

    std::string WsaErrorToString(DWORD error) {
        char msg[256] = { 0 };
        int res = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msg, sizeof msg, NULL);

        if (res == 0) return std::to_string(error);

        return msg;
    }

    std::string ToHumanReadable(DWORD vk) {
        switch (vk) {
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_SHIFT:
            return "<SHIFT>";
        case VK_BACK:
            return "<BACKSPACE>";
        case VK_DELETE:
            return "<DELETE>";
        case VK_TAB:
            return "<TAB>";
        case VK_ESCAPE:
            return "<ESC>";
        case VK_UP:
            return "<ARROW UP>";
        case VK_DOWN:
            return "<ARROW DOWN>";
        case VK_LEFT:
            return "<ARROW LEFT>";
        case VK_RIGHT:
            return "<ARROW RIGHT>";
        case VK_RETURN:
            return "<ENTER>";
        case VK_CAPITAL:
            return "<CAPS LOCK>";
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
            return "<CTRL>";
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
            return "<ALT>";
        case VK_LWIN:
        case VK_RWIN:
            return "<WINDOWS>";
        case VK_END:
            return "<END>";
        case VK_NEXT:
            return "<PAGE DOWN>";
        case VK_PRIOR:
            return "<PAGE UP>";
        case VK_F1:
            return "<F1>";
        case VK_F2:
            return "<F2>";
        case VK_F3:
            return "<F3>";
        case VK_F4:
            return "<F4>";
        case VK_F5:
            return "<F5>";
        case VK_F6:
            return "<F6>";
        case VK_F7:
            return "<F7>";
        case VK_F8:
            return "<F8>";
        case VK_F9:
            return "<F9>";
        case VK_F10:
            return "<F10>";
        case VK_F11:
            return "<F11>";
        case VK_F12:
            return "<F12>";
        case VK_F13:
            return "<F13>";
        case VK_F14:
            return "<F14>";
        case VK_F15:
            return "<F15>";
        case VK_F16:
            return "<F16>";
        case VK_F17:
            return "<F17>";
        case VK_F18:
            return "<F18>";
        case VK_F19:
            return "<F19>";
        case VK_F20:
            return "<F20>";
        case VK_F21:
            return "<F21>";
        case VK_F22:
            return "<F22>";
        case VK_F23:
            return "<F23>";
        case VK_F24:
            return "<F24>";
        default:
            return "<???>";
        }
    }

    bool IsHumanReadable(DWORD vk) {
        switch (vk) {
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_SHIFT:
        case VK_BACK:
        case VK_DELETE:
        case VK_TAB:
        case VK_ESCAPE:
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_RETURN:
        case VK_CAPITAL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
        case VK_CONTROL:
        case VK_LWIN:
        case VK_RWIN:
        case VK_END:
        case VK_NEXT:
        case VK_PRIOR:
        case VK_F1:
        case VK_F2:
        case VK_F3:
        case VK_F4:
        case VK_F5:
        case VK_F6:
        case VK_F7:
        case VK_F8:
        case VK_F9:
        case VK_F10:
        case VK_F11:
        case VK_F12:
        case VK_F13:
        case VK_F14:
        case VK_F15:
        case VK_F16:
        case VK_F17:
        case VK_F18:
        case VK_F19:
        case VK_F20:
        case VK_F21:
        case VK_F22:
        case VK_F23:
        case VK_F24:
            return false;
        default:
            return true;
        }
    }

    int FromWcharToChar(WCHAR* in, char* out, int inSize) {
        return WideCharToMultiByte(CP_UTF8, NULL, in, -1, out, inSize, "?", NULL);
    }
};
