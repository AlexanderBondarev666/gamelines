
#if !defined(AFX_LINES2_H__6B1F178F_E111_4C50_8572_71E5FEF0E86A__INCLUDED_)
#define AFX_LINES2_H__6B1F178F_E111_4C50_8572_71E5FEF0E86A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

enum DIR {DIR_LEFT=0, DIR_UP, DIR_RIGHT, DIR_DOWN};

void DrawFigures(HDC hdc, HDC hmdc);
void InitFigureTypes();
void SetFigDir(DIR dir); 
void DrawBorder(HDC hdc);
int FindFigure(POINT pt);
void OnClick(HWND hWnd, LPARAM lParam);
void InitStartPos(HWND hWnd);

HANDLE lpBoard; // Пустая доска
BITMAP Bitmap;

#define H0 26
#define V0 28
#define HP 61
#define VP 60
#define RCX(xxx) (V0 + VP * (xxx))
#define RCY(yyy) (H0 + HP * (yyy))

#define EWM_WIN			 0x7001
#define EWM_SETACTIVE	 0x7002
#define EWM_MAKEMOVE 	 0x7003
#define EWM_CLICK	 	 0x7004
#define EWM_1000	 	 0x7005
#define EWM_FALSE		 0x7006
#define EWM_FAULT		 0x7007
#define EWM_ESCAPE		 0x7008
#define EWM_PUTFIG		 0x7009

#define NFIGURETYPES 3
#define MAXFIGURES 53

RECT MainRect = {V0, H0, RCX(7), RCY(7)};

// Флаги
int fSolved=0;			// Решена ли позиция
int fEscape=0;			// Была ли нажата клавиша ESC
int fReplay=0;
int fPrepare=0;		// Идет ли подготовка к передвижению

int CurrX=0, CurrY=0;
int iActiveFig;						// Активная фигура
int nFigures;						// Количество фигур 

#endif // !defined(AFX_LINES2_H__6B1F178F_E111_4C50_8572_71E5FEF0E86A__INCLUDED_)
