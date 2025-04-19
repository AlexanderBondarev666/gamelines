#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Pieses.h"

extern HINSTANCE hInst;

//////////////////////////////////
// DrawFigure(s)
//////////////////////////////////
void DrawFigure(HDC hdc, HDC hmdc, int iFig)
{
	FIGURE *pFig = aFigures+iFig;
	int iFigureType = pFig->iFigureType;
	int xPos = pFig->xPos;
	int yPos = pFig->yPos;

	GetObject(lpBitmaps[iFigureType], sizeof(BITMAP), &Bitmap);
	SelectObject(hmdc, lpBitmaps[iFigureType]);
	BitBlt(hdc, RCX(xPos), RCY(yPos), 
		Bitmap.bmWidth, Bitmap.bmHeight,
		hmdc, 0, 0, SRCCOPY);
}
void DrawFigures(HDC hdc, HDC hmdc)
{
    //nFigures = 3;
	for(int iFig=0; iFig<nFigures; iFig++)
		DrawFigure(hdc, hmdc, iFig);
}
//////////////////////////////////
// Инициализируем типы фигур
//////////////////////////////////
void InitFigureTypes()
{
	if (lpBitmaps[0]) return;
	char sFigureBitmap[16];
	//sprintf(sFigureBitmap, "IDB_SQ00");
	//lpBitmaps[0] = LoadImage(hInst, sFigureBitmap, IMAGE_BITMAP, 0, 0, 0);
	for(int idFT=0; idFT<NFIGURETYPES; idFT++)
	{	sprintf(sFigureBitmap,"IDB_SQ1%d", idFT + 1);
		lpBitmaps[idFT] = LoadImage(hInst, sFigureBitmap, IMAGE_BITMAP, 0, 0, 0);
	}
	CurrX = 0; CurrY = 0;
} 
//////////////////////////////////
// Инициализируем сами фигуры
//////////////////////////////////
void InitFigures()
{
	int nRec = 4; // Сколько новых шариков будет добавляться
	int rn;
	int x, y;
	FIGURE *pFig = aFigures + nFigures;
	int nClear = 49 - nFigures;
	for(int iRec=0; iRec < nRec; iRec++)
	{
		if (nClear==0)  // Свободных мест больше нет!!!
		{
			MessageBox((HWND)0, "Свободных мест больше нет", "Message", MB_OK);
			break;
		}
		rn = rand()%nClear;
		// Найти свободное место с номером rn
		int cnt=0;
		bool find = false;
		for(x=0; x<7; x++)
		{
			for(y=0; y<7; y++)
				if (FigMap[y][x] == 0) // Пустое место
				{
					if (cnt==rn) {find = true; break;}
					cnt++;
				
				}
				if (find) break;
		}
		// ... Положить туда фигуру
		pFig->xPos = x;
		pFig->yPos = y;
		FigMap[y][x]=nFigures+1;


		pFig->iFigureType = rand()%NFIGURETYPES;
		pFig->iStatus = 0;
		pFig++;
		nClear--;
		nFigures ++;
	}
}
/////////////////////////////////////
// Инициализируем начальное положение
/////////////////////////////////////
void InitStartPos(HWND hWnd)
{
	iActiveFig = 0;	// Начальный номер активной фигуры
	fPrepare = 0;
	nFigures = 0;					// Not valid yet
	memset(FigMap, 0, sizeof(FigMap));	// Очистить основную карту
	InitFigureTypes();				// Инициализируем типы фигур
	InitFigures();					// Инициализируем сами фигуры
}
/////////////////////////////////////////////
// void SetFigDir(DIR dir)
/////////////////////////////////////////////
void SetFigDir(DIR dir)
{
	if (dir==DIR_UP)
	{	CurrY--;
		if (CurrY<0) CurrY=0;

	} else
	if (dir==DIR_DOWN)
	{	CurrY++;
		if (CurrY>6) CurrY=6;

	} else
	if (dir==DIR_LEFT)
	{	CurrX--;
		if (CurrX<0) CurrX=0;

	} else
	if (dir==DIR_RIGHT)
	{	CurrX++;
		if (CurrX>6) CurrX=6;

	}
	return;
}
//////////////////////////////////
// Прорисовать рамку
//////////////////////////////////
void DrawBorder(HDC hdc)
{
	if (!fPrepare) return;
	HBRUSH hB = CreateSolidBrush(RGB(252, 244, 222));
	RECT rect; // = GetFigRect(iActiveFig);
	rect.right=	(rect.left=RCX(CurrX)) + VP;
	rect.bottom= (rect.top=RCY(CurrY)) + HP;

	FrameRect(hdc, &rect, hB);
}
//////////////////////////////////
// Получить прямоугольник фигуры
//////////////////////////////////
RECT GetFigRect(int iFig)
{
	FIGURE *pFig = aFigures+iFig;
	int iKind = pFig->iFigureType;
	int xPos = pFig->xPos;
	int yPos = pFig->yPos;

	RECT rect;
	rect.right=	(rect.left=V0 + xPos * VP) + VP;
	rect.bottom= (rect.top=H0 + yPos * HP) + HP;
	return rect;
}
//////////////////////////////////
// FindFigure
//////////////////////////////////
int FindFigure(POINT pt)
{
	RECT rect2; // после нажатия

	// Ищем фигуру, на которую нажали
	for(int iFig=0; iFig<nFigures; iFig++)
	{   rect2 = GetFigRect(iFig);
		if (PtInRect(&rect2, pt)) return iFig;// нашли!
	}
	return -1;
}

////////////////////////////////////////
// Удаление полных линий; DelFullLinesXY()
////////////////////////////////////////
bool DelFullLinesXY(int xc, int yc)
{
	POINT pt;   // Текущая клетка
	int iActiveFig; // Текущий шарик
	int type;	// Тип текущей клетки
	int iDir;	// Направление
	int dx, dy;	// Сдвиг на 1 шаг
	RECT rect;	// Прямоугольник поля
	int x1, y1, x2, y2; // Начальные и конечная точки на линии
	int n1, n2;	// Количество шагов в разных направлениях
	int i;		// Номер шага
	bool flag = false;
	
	if (nFigures<5) return flag;

	rect.top = rect.left = 0;
	rect.right = rect.bottom = 7;

			iActiveFig = FigMap[yc][xc];
		    if (iActiveFig == 0) return flag; // на клетке нет шарика
			iActiveFig--;
			type = aFigures[iActiveFig].iFigureType;

			// Цикл по всем направлениям
			for(iDir=0; iDir<4; iDir++)
			{
				switch(iDir)
				{
					case 0: dx = 1; dy = 1;  break;
					case 1: dx = 1; dy = -1; break;	
					case 2: dx = 0; dy = 1;	 break;	
					case 3: dx = 1; dy = 0;  break;
				} // switch
				// Исследовать прямое направление
				x1 = xc; y1 = yc; n1 = 0;
				pt.x = x1 + dx;
				pt.y = y1 + dy;
				while (PtInRect(&rect, pt) &&  
						(FigMap[pt.y][pt.x] != 0) &&
						(aFigures[FigMap[pt.y][pt.x]-1].iFigureType == type))
				{
					x1 += dx;
					y1 += dy;
					n1 ++;
					pt.x = x1 + dx;
					pt.y = y1 + dy;
				} // while
				// Исследовать обратное направление
				dx = - dx; dy = - dy;
				x2 = xc; y2 = yc; n2 = 0;
				pt.x = x2 + dx;
				pt.y = y2 + dy;
				while (PtInRect(&rect, pt) &&  
						(FigMap[pt.y][pt.x] != 0) &&
						(aFigures[FigMap[pt.y][pt.x]-1].iFigureType == type))
				{
					x2 += dx;
					y2 += dy;
					n2 ++;
					pt.x = x2 + dx;
					pt.y = y2 + dy;
				} // while
				// Определить количество шариков
				dx = - dx; dy = - dy;
				if (n1+n2>3)
				{
					// Если больше четырех, то пометить
					flag = true;
					aFigures[iActiveFig].iStatus = 1;
					for(i=1;i<=n1; i++)
						aFigures[FigMap[yc + i*dy][xc + i*dx]-1].iStatus = 1;
					for(i=1;i<=n2; i++)
						aFigures[FigMap[yc - i*dy][xc - i*dx]-1].iStatus = 1;
				} // if
			} // for iDir

	if (!flag) return flag;

	// Удалить все помеченные шарики
	int iFigure=0; // Текущий шарик
	while (iFigure<nFigures)
	{
		if (aFigures[iFigure].iStatus == 1)
		{
			memmove(aFigures+iFigure /*dest*/, aFigures+iFigure+1, 
				    sizeof(FIGURE)*(nFigures-iFigure-1));
			nFigures--;
		}
		else
			iFigure++;
	} // while
	// Обновить карту
	memset(FigMap, 0, sizeof(FigMap));	// Очистить основную карту
	for(iFigure=0;iFigure<nFigures;iFigure++)
		FigMap[aFigures[iFigure].yPos][aFigures[iFigure].xPos] = iFigure + 1; 
	return flag;
}
////////////////////////////////////////
// Удаление полных линий; DelFullLines()
////////////////////////////////////////
bool DelFullLines()
{
	int xc, yc; // Текущая клетка
	bool flag= false;

	// Цикл по всем клеткам
	for(xc=0; xc<7; xc++)
	{
		for(yc=0; yc<7; yc++)
		{
			if (DelFullLinesXY(xc, yc)) flag = true;

		} // for yc
	} // for xc
	return flag;
}

/////////////////////////////////////
// bExistPath(int x1, int y1, int x2, int y2)
/////////////////////////////////////
bool bExistPath(int x1, int y1, int x2, int y2, bool clr = false)
{
	static bool flag = false;
	static int fMap[7][7];
	if (clr)
	{
		memset(fMap, 0, 49*sizeof(int));
		flag = false;
	}
	if (flag) return true;
	if (x1==x2 && y1==y2) { fMap[y1][x1] = 1; flag = true; return true; }
	if ((x1<0 || y1<0) ||
		(x1>6 || y1>6) ||
		(x2<0 || y2<0) ||
		(x2>6 || y2>6)) return flag;
	fMap[y1][x1] = 1;
	if (x1>0 && fMap[y1][x1-1] == 0 && FigMap[y1][x1-1]==0 && bExistPath(x1-1, y1, x2, y2)) { flag = true; return true; }
	if (y1>0 && fMap[y1-1][x1] == 0 && FigMap[y1-1][x1]==0 && bExistPath(x1, y1-1, x2, y2)) { flag = true; return true; }
	if (x1<6 && fMap[y1][x1+1] == 0 && FigMap[y1][x1+1]==0 && bExistPath(x1+1, y1, x2, y2)) { flag = true; return true; }
	if (y1<6 && fMap[y1+1][x1] == 0 && FigMap[y1+1][x1]==0 && bExistPath(x1, y1+1, x2, y2)) { flag = true; return true; }
	return flag;
}
/////////////////////////////////////
// OnClick(HWND hWnd, LPARAM lParam)
/////////////////////////////////////
void OnClick(HWND hWnd, LPARAM lParam)
{
	int i, j;
	RECT rect;
	POINT pt; // куда кликнули ?
	pt.x = LOWORD((long)lParam);
	pt.y = HIWORD((long)lParam);
	
	// Находим, в каком квадрате кликнули 
	bool flag = false;
	for( i=0; i<7; i++)
	{
	  for( j=0; j<7; j++)
	  {
		rect.right=	(rect.left=RCX(i)) + VP;
		rect.bottom= (rect.top=RCY(j)) + HP;
		if (PtInRect(&rect, pt)) {flag = true; break;}// нашли!
	  }
	  if (flag) break;
	}	  
	
	if (!flag) return;

	//////////////////////////////////////
	// Находим, стоит ли в квадрате фигура 
	//////////////////////////////////////

	// Ищем фигуру, на которую нажали
	int NewActiveFig = FindFigure(pt);
	if(NewActiveFig!=-1) // нашли
	{
		if (!fPrepare)	// Находимся в режиме выбора фигуры
		{
			fPrepare = 1;
			CurrX = i;
			CurrY = j;
			// Новая активная фигура		
			iActiveFig = NewActiveFig;
			// Для прорисовки новой рамки
			RECT rect1 = GetFigRect(iActiveFig); 
			InvalidateRect(hWnd, &rect1, FALSE);
			UpdateWindow(hWnd);
		}
		else // Находимся в режиме перемещения фигуры
			if (iActiveFig == NewActiveFig) // Кликаем на ту же фигуру
			{
				fPrepare = 0;
				// Для удаления рамки
				RECT rect1 = GetFigRect(iActiveFig); 
				InvalidateRect(hWnd, &rect1, FALSE);
				UpdateWindow(hWnd);
			}
	} // if
	else //if (!is_find)
	{
		if (fPrepare) // Находимся в режиме перемещения фигуры
		{
			if (!bExistPath(aFigures[iActiveFig].xPos,
							aFigures[iActiveFig].yPos,	i, j, true)) 
					return;

			fPrepare = 0;
			
			// Для прорисовки новой рамки
			RECT rect1 = GetFigRect(iActiveFig); 
			InvalidateRect(hWnd, &rect1, FALSE); // где была фигура

			// Перемещение фигуры
			FIGURE *pFig = aFigures+iActiveFig;
			int xPos = pFig->xPos;
			int yPos = pFig->yPos;
			pFig->xPos = i;
			pFig->yPos = j;

			FigMap[j][i] = FigMap[yPos][xPos];
			FigMap[yPos][xPos] = 0;
			
			// Для прорисовки новой рамки
			rect1 = GetFigRect(iActiveFig); // до нажатия
			InvalidateRect(hWnd, &rect1, FALSE);  // где кликнули
			UpdateWindow(hWnd);

			Sleep(500);
			if (!DelFullLinesXY(pFig->xPos, pFig->yPos))
			{
				// Прорисовка новых фигур
				InitFigures();
				InvalidateRect(hWnd, &MainRect, FALSE);  
				UpdateWindow(hWnd);
	
				Sleep(500);
				
				// Удаление полных линий
				DelFullLines();
				InvalidateRect(hWnd, &MainRect, FALSE);  
				UpdateWindow(hWnd);
			}
			else
			{
				InvalidateRect(hWnd, &MainRect, FALSE);  
				UpdateWindow(hWnd);
			
			}

		}
	} //else
} // OnClick
