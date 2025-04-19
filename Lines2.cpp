// Lines2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <commdlg.h>
#include <mmsystem.h>

#define MAX_LOADSTRING 100

const int WBOARD = 479;
const int HBOARD = 479;

#include "Lines2.h"

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LINES2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LINES2);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LINES2);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_LINES2;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}
/////////////////////////////////////
// void AdjustWindowSize(HWND hWnd)
/////////////////////////////////////
void AdjustWindowSize(HWND hWnd)
{
	// Изменяем размеры окна в начале работы программы
	lpBoard = LoadImage(hInst, "IDB_BOARD", IMAGE_BITMAP, 0, 0, 0);
	
	RECT rcWnd1;		GetWindowRect(hWnd, &rcWnd1);	// Определяем размер всего окна
	RECT rcWnd2;		GetClientRect(hWnd, &rcWnd2);	// Определяем размер клиентской части окна
	int dx = rcWnd1.right - rcWnd1.left - rcWnd2.right;	// Вычисляем разность
	int dy = rcWnd1.bottom - rcWnd1.top - rcWnd2.bottom;
	//MoveWindow(hWnd, rcWnd1.left, rcWnd1.top, lpBoard->GetW()+dx, lpBoard->GetH()+dy, TRUE);
	//EnableMenuItem(GetMenu(hWnd), IDM_MAKEALLSTEPS, MF_GRAYED);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable
	
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, //WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 487, 525, NULL, NULL, hInstance, NULL);
   
   if (!hWnd)
   {
      return FALSE;
   }

    srand( (unsigned)time( NULL ) );
    InitStartPos(hWnd);
	AdjustWindowSize(hWnd);
    //InitFigureTypes();


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

/////////////////////////////////////
// void UpdateMainRect(HWND hWnd)
/////////////////////////////////////
void UpdateMainRect(HWND hWnd)
{
	InvalidateRect(hWnd, &MainRect, FALSE);
	UpdateWindow(hWnd);
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;

				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
			hdc = BeginPaint(hWnd, &ps);
			HDC hMemDC = CreateCompatibleDC(hdc);
			if (!hMemDC) break;
			SelectObject(hMemDC, lpBoard);
			BitBlt(hdc, 0, 0, WBOARD, HBOARD, hMemDC, 0, 0, SRCCOPY);

			DrawFigures(hdc, hMemDC);					// Прорисовываем фигуры

			DrawBorder(hdc);

			DeleteDC (hMemDC);

			EndPaint(hWnd, &ps);
			break;
			}
		case WM_LBUTTONDOWN: // Нажали на мышь
			DefWindowProc(hWnd, message, wParam, lParam);
			PostMessage(hWnd, EWM_CLICK, wParam, lParam);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case EWM_CLICK: // Нажали на мышь: обрабатываем
			{
				DefWindowProc(hWnd, message, wParam, lParam);
				//if (fWait==1) break; // Если вычисляем - не обрабатывать
				OnClick(hWnd, lParam);
				break;
			}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
