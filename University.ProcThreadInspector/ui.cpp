#include "ui.h"
#include "Resource.h"

void CreateControls(HWND hWnd)
{
    CreateWindowW(L"BUTTON", L"Click Me", WS_VISIBLE | WS_CHILD,
        0, 0, 100, 30, hWnd, (HMENU)1, NULL, NULL);

    CreateWindowW(L"BUTTON", L"О программе", WS_VISIBLE | WS_CHILD,
        50, 100, 150, 30, hWnd, (HMENU)IDM_ABOUT, NULL, NULL);
}
