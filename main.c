#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Indicate compatibility with at least Windows 2000. */
#ifndef WINVER
#define WINVER 0x0500
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef STRICT
#define STRICT
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define OVERLAY_TRANSPARENCY false

static VOID ShowError(LPCSTR const msg)
{
    MessageBox(NULL, msg, NULL, MB_OK | MB_ICONERROR | MB_DEFBUTTON1);
}

static LRESULT CALLBACK WndProc(HWND const hWnd, UINT const uMsg,
                                WPARAM const wParam, LPARAM const lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0U);
        return 0U;
    case WM_PAINT:
        /* Windows will paint the window in the background color by default. */
        break;
    default:
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE const hInst, HINSTANCE const hPrevInst,
                   PSTR const pCmdLine, INT const nCmdShow)
{
    /* Register the window class. */
    static char const className[] = "LG No Sleep";

    /* Create a blank icon. */
    static BYTE const iconMaskAND[] = {0xFF};
    static BYTE const iconMaskXOR[] = {0xFF};
    HICON const hIcon =
        CreateIcon(hInst, 1U, 1U, 1U, 1U, iconMaskAND, iconMaskXOR);

    WNDCLASSEX const wcex = {
        .cbSize = sizeof(wcex),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WndProc,
        .hInstance = hInst,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = CreateSolidBrush(RGB(21, 21, 21)),
        .lpszClassName = className,
        .hIcon = hIcon,
        .hIconSm = hIcon,
    };

    if (RegisterClassEx(&wcex) == 0U)
    {
        ShowError("Failed to register window class.\n");
        return -1;
    }

    HWND const hWnd = CreateWindowEx(
        WS_EX_TOPMOST | (OVERLAY_TRANSPARENCY ? WS_EX_LAYERED : 0U), className,
        className, WS_POPUP | WS_VISIBLE, 0U, 0U, 2U, 2U, NULL, NULL, hInst,
        NULL);
    if (hWnd == NULL)
    {
        ShowError("Failed to create window.\n");
        return -1;
    }

    if (OVERLAY_TRANSPARENCY)
    {
        /* Make window semi-transparent, and mask out background color */
        if (SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 128,
                                       LWA_ALPHA | LWA_COLORKEY) == 0U)
        {
            ShowError("Failed to set layered window attributes.\n");
            return -1;
        }
    }

    ShowWindow(hWnd, nCmdShow);

    /* Run the message loop. */
    MSG msg = {0U};
    while (GetMessage(&msg, NULL, 0U, 0U) > 0)
    {
        if (TranslateMessage(&msg) != 0)
        {
            ShowError("Failed to translate message.\n");
            break;
        }
        DispatchMessage(&msg);
    }
    return 0;
}
