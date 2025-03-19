#include "ui.h"

void CreateControls(HWND hWnd)
{
    CreateWindowW(L"BUTTON", L"Click Me", WS_VISIBLE | WS_CHILD,
        0, 0, 100, 30, hWnd, (HMENU)1, NULL, NULL);

    CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        50, 100, 200, 25, hWnd, (HMENU)2, NULL, NULL);
}
