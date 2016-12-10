#pragma once

#include "resource.h"

class VolumeScreenWebSpeed
{
public:

    HWND Create(HWND hWnd);
    void Show();

private:
    HINSTANCE hInst_;
};
