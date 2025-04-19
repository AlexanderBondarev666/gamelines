
#if !defined(AFX_PIECES_H__7E4A1F66_7169_11D6_B3E5_F7902CEDBEE4__INCLUDED_)
#define AFX_PIECES_H__7E4A1F66_7169_11D6_B3E5_F7902CEDBEE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

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
#define MAXFIGURES 64

enum DIR {DIR_LEFT=0, DIR_UP, DIR_RIGHT, DIR_DOWN};

typedef int MAP[7][7]; // Карта позиции
MAP FigMap;


// Структура, задающая фигуру
struct FIGURE
{
	int iFigureType;
	int xPos;
	int yPos;
	int iStatus;
};
FIGURE aFigures[MAXFIGURES];

extern RECT MainRect;

extern int CurrX, CurrY;

HANDLE lpBitmaps[NFIGURETYPES]={0}; 

extern HANDLE lpBoard;
extern BITMAP Bitmap;
extern int nFigures;		// Количество фигур 
extern int iActiveFig;		// Активная фигура

// Флаги
extern int fSolved;			// Решена ли позиция
extern int fEscape;			// Была ли нажата клавиша ESC
extern int fReplay;
extern int fPrepare;		// Идет ли подготовка к передвижению


#endif // !defined(AFX_PIECES_H__7E4A1F66_7169_11D6_B3E5_F7902CEDBEE4__INCLUDED_)
