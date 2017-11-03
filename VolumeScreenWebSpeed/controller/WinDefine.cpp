#include "stdafx.h"
#include "WinDefine.h"

namespace
{

    struct HotKeyData
    {
        string keyString;
        int KeyValue;
    };

    HotKeyData HotKeyStringMap[] =
    {
        { "F1", VK_F1 },
        { "F2", VK_F2 },
        { "F3", VK_F3 },
        { "F4", VK_F4 },
        { "F5", VK_F5 },
        { "F6", VK_F6 },
        { "F7", VK_F7 },
        { "F8", VK_F8 },
        { "F9", VK_F9 },
        { "F10", VK_F10 },
        { "F11", VK_F11 },
        { "F12", VK_F12 },

        { "space", VK_SPACE },
        { "back", VK_BACK },
        { "Tab", VK_TAB },
        { "Return", VK_RETURN },

        { "num0", VK_NUMPAD0 },
        { "num1", VK_NUMPAD1 },
        { "num2", VK_NUMPAD2 },
        { "num3", VK_NUMPAD3 },
        { "num4", VK_NUMPAD4 },
        { "num5", VK_NUMPAD5 },
        { "num6", VK_NUMPAD6 },
        { "num7", VK_NUMPAD7 },
        { "num8", VK_NUMPAD8 },
        { "num9", VK_NUMPAD9 },
        { "num*", VK_MULTIPLY },
        { "num+", VK_ADD },
        { "num/", VK_SEPARATOR },
        { "num-", VK_SUBTRACT },
        { "num.", VK_DECIMAL },

        { "Insert", VK_INSERT },
        { "Delete", VK_DELETE },
        { "Home", VK_HOME },
        { "End", VK_END },
        { "PageUp", VK_PRIOR },
        { "PageDown", VK_NEXT },
        { "Up", VK_UP },
        { "Down", VK_DOWN },
        { "left", VK_LEFT },
        { "right", VK_RIGHT },

        { ";", VK_OEM_1 }, // ';:' for US
        { "+", VK_OEM_PLUS }, // '+' any country
        { ",", VK_OEM_COMMA }, //',' any country
        { "-", VK_OEM_MINUS }, //  '-' any country
        { ".", VK_OEM_PERIOD }, //'.' any country
        { "/", VK_OEM_2 }, //'/?' for US
        { "`", VK_OEM_3 }, //  '`~' for US

        { "[", VK_OEM_4 }, //  '[{' for US
        { "\\", VK_OEM_5 }, // '\|' for US
        { "]", VK_OEM_6 }, //  ']}' for US
        { "'", VK_OEM_7 }, //  ''"' for US
    };
}

WinDefine::WinDefine(void)    
{

}


WinDefine::~WinDefine(void)
{
}

int WinDefine::HotKeyStrigToVkKey(const string& hotKeyString)
{
    for (int i = 0; i < sizeof(HotKeyStringMap) / sizeof(HotKeyStringMap[0]);
         ++i)
    {
        if (_stricmp(HotKeyStringMap[i].keyString.c_str(),
            hotKeyString.c_str()) == 0)
        {
            return HotKeyStringMap[i].KeyValue;
        }
    }
    return -1;
}
