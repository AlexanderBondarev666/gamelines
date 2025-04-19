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
// �������������� ���� �����
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
// �������������� ���� ������
//////////////////////////////////
void InitFigures()
{
	int nRec = 4; // ������� ����� ������� ����� �����������
	int rn;
	int x, y;
	FIGURE *pFig = aFigures + nFigures;
	int nClear = 49 - nFigures;
	for(int iRec=0; iRec < nRec; iRec++)
	{
		if (nClear==0)  // ��������� ���� ������ ���!!!
		{
			MessageBox((HWND)0, "��������� ���� ������ ���", "Message", MB_OK);
			break;
		}
		rn = rand()%nClear;
		// ����� ��������� ����� � ������� rn
		int cnt=0;
		bool find = false;
		for(x=0; x<7; x++)
		{
			for(y=0; y<7; y++)
				if (FigMap[y][x] == 0) // ������ �����
				{
					if (cnt==rn) {find = true; break;}
					cnt++;
				
				}
				if (find) break;
		}
		// ... �������� ���� ������
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
// �������������� ��������� ���������
/////////////////////////////////////
void InitStartPos(HWND hWnd)
{
	iActiveFig = 0;	// ��������� ����� �������� ������
	fPrepare = 0;
	nFigures = 0;					// Not valid yet
	memset(FigMap, 0, sizeof(FigMap));	// �������� �������� �����
	InitFigureTypes();				// �������������� ���� �����
	InitFigures();					// �������������� ���� ������
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
// ����������� �����
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
// �������� ������������� ������
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
	RECT rect2; // ����� �������

	// ���� ������, �� ������� ������
	for(int iFig=0; iFig<nFigures; iFig++)
	{   rect2 = GetFigRect(iFig);
		if (PtInRect(&rect2, pt)) return iFig;// �����!
	}
	return -1;
}

////////////////////////////////////////
// �������� ������ �����; DelFullLinesXY()
////////////////////////////////////////
bool DelFullLinesXY(int xc, int yc)
{
	POINT pt;   // ������� ������
	int iActiveFig; // ������� �����
	int type;	// ��� ������� ������
	int iDir;	// �����������
	int dx, dy;	// ����� �� 1 ���
	RECT rect;	// ������������� ����
	int x1, y1, x2, y2; // ��������� � �������� ����� �� �����
	int n1, n2;	// ���������� ����� � ������ ������������
	int i;		// ����� ����
	bool flag = false;
	
	if (nFigures<5) return flag;

	rect.top = rect.left = 0;
	rect.right = rect.bottom = 7;

			iActiveFig = FigMap[yc][xc];
		    if (iActiveFig == 0) return flag; // �� ������ ��� ������
			iActiveFig--;
			type = aFigures[iActiveFig].iFigureType;

			// ���� �� ���� ������������
			for(iDir=0; iDir<4; iDir++)
			{
				switch(iDir)
				{
					case 0: dx = 1; dy = 1;  break;
					case 1: dx = 1; dy = -1; break;	
					case 2: dx = 0; dy = 1;	 break;	
					case 3: dx = 1; dy = 0;  break;
				} // switch
				// ����������� ������ �����������
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
				// ����������� �������� �����������
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
				// ���������� ���������� �������
				dx = - dx; dy = - dy;
				if (n1+n2>3)
				{
					// ���� ������ �������, �� ��������
					flag = true;
					aFigures[iActiveFig].iStatus = 1;
					for(i=1;i<=n1; i++)
						aFigures[FigMap[yc + i*dy][xc + i*dx]-1].iStatus = 1;
					for(i=1;i<=n2; i++)
						aFigures[FigMap[yc - i*dy][xc - i*dx]-1].iStatus = 1;
				} // if
			} // for iDir

	if (!flag) return flag;

	// ������� ��� ���������� ������
	int iFigure=0; // ������� �����
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
	// �������� �����
	memset(FigMap, 0, sizeof(FigMap));	// �������� �������� �����
	for(iFigure=0;iFigure<nFigures;iFigure++)
		FigMap[aFigures[iFigure].yPos][aFigures[iFigure].xPos] = iFigure + 1; 
	return flag;
}
////////////////////////////////////////
// �������� ������ �����; DelFullLines()
////////////////////////////////////////
bool DelFullLines()
{
	int xc, yc; // ������� ������
	bool flag= false;

	// ���� �� ���� �������
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
	POINT pt; // ���� �������� ?
	pt.x = LOWORD((long)lParam);
	pt.y = HIWORD((long)lParam);
	
	// �������, � ����� �������� �������� 
	bool flag = false;
	for( i=0; i<7; i++)
	{
	  for( j=0; j<7; j++)
	  {
		rect.right=	(rect.left=RCX(i)) + VP;
		rect.bottom= (rect.top=RCY(j)) + HP;
		if (PtInRect(&rect, pt)) {flag = true; break;}// �����!
	  }
	  if (flag) break;
	}	  
	
	if (!flag) return;

	//////////////////////////////////////
	// �������, ����� �� � �������� ������ 
	//////////////////////////////////////

	// ���� ������, �� ������� ������
	int NewActiveFig = FindFigure(pt);
	if(NewActiveFig!=-1) // �����
	{
		if (!fPrepare)	// ��������� � ������ ������ ������
		{
			fPrepare = 1;
			CurrX = i;
			CurrY = j;
			// ����� �������� ������		
			iActiveFig = NewActiveFig;
			// ��� ���������� ����� �����
			RECT rect1 = GetFigRect(iActiveFig); 
			InvalidateRect(hWnd, &rect1, FALSE);
			UpdateWindow(hWnd);
		}
		else // ��������� � ������ ����������� ������
			if (iActiveFig == NewActiveFig) // ������� �� �� �� ������
			{
				fPrepare = 0;
				// ��� �������� �����
				RECT rect1 = GetFigRect(iActiveFig); 
				InvalidateRect(hWnd, &rect1, FALSE);
				UpdateWindow(hWnd);
			}
	} // if
	else //if (!is_find)
	{
		if (fPrepare) // ��������� � ������ ����������� ������
		{
			if (!bExistPath(aFigures[iActiveFig].xPos,
							aFigures[iActiveFig].yPos,	i, j, true)) 
					return;

			fPrepare = 0;
			
			// ��� ���������� ����� �����
			RECT rect1 = GetFigRect(iActiveFig); 
			InvalidateRect(hWnd, &rect1, FALSE); // ��� ���� ������

			// ����������� ������
			FIGURE *pFig = aFigures+iActiveFig;
			int xPos = pFig->xPos;
			int yPos = pFig->yPos;
			pFig->xPos = i;
			pFig->yPos = j;

			FigMap[j][i] = FigMap[yPos][xPos];
			FigMap[yPos][xPos] = 0;
			
			// ��� ���������� ����� �����
			rect1 = GetFigRect(iActiveFig); // �� �������
			InvalidateRect(hWnd, &rect1, FALSE);  // ��� ��������
			UpdateWindow(hWnd);

			Sleep(500);
			if (!DelFullLinesXY(pFig->xPos, pFig->yPos))
			{
				// ���������� ����� �����
				InitFigures();
				InvalidateRect(hWnd, &MainRect, FALSE);  
				UpdateWindow(hWnd);
	
				Sleep(500);
				
				// �������� ������ �����
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
