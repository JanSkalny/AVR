#include "StdAfx.h"
#include "Processor.h"
#include "LEDController.h"

extern CLEDController g_Leds;

UINT StartProcessor(LPVOID lpParam) {
	CProcessor *pProcessor = (CProcessor*) lpParam;
	return pProcessor ? pProcessor->CoatingFunc() : -1;
}

CProcessor::CProcessor(void)
{
}

CProcessor::~CProcessor(void)
{
}

UINT CProcessor::CoatingFunc() {
	int nRet = 0;

	Run();

	AfxEndThread(nRet);

	return nRet;
}

void CProcessor::Run() {

	int x,y,i;
	COLORREF cr;
	DWORD ra[10],ga[10],ba[10];
	DWORD ca[10];
	CString sDebug;
	CDC DC;
	DC.Attach(::GetDC(0));
	int min,max,r,g,b,a;

	DWORD dwStart=GetTickCount();
	int nCount=0;
	int nIter=1;

	// prepare DCs, bitmaps,..
	HDC hScreenDC = ::GetWindowDC(0);
	HDC hmemDC = CreateCompatibleDC(hScreenDC);   
	int ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES);
	int ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES);
	HBITMAP hmemBM = CreateCompatibleBitmap(hScreenDC, ScreenWidth, ScreenHeight);
	PIXEL *pScreen = new PIXEL[ScreenHeight*ScreenWidth];
	BITMAPINFO bmInfo;
	memset(&bmInfo,0,sizeof(bmInfo));
	bmInfo.bmiHeader.biSize = 40;
	bmInfo.bmiHeader.biWidth = ScreenWidth;
	bmInfo.bmiHeader.biHeight = ScreenHeight;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32; // NOT 24

	SelectObject(hmemDC, hmemBM);

	list <CRect> Regions;
	
	Regions.push_back(CRect(900,700,1250,960));
	Regions.push_back(CRect(700,400,1250,700));

	Regions.push_back(CRect(900,60,1250,450));
	Regions.push_back(CRect(600,60,950,450));
	Regions.push_back(CRect(300,60,650,450));
	Regions.push_back(CRect(0,60,350,450));

	Regions.push_back(CRect(0,300,600,700));
	Regions.push_back(CRect(0,500,800,960));	
	

	

	list<CRect>::iterator itRegion;

	while (1) {
		memset(ra,0,40);
		memset(ga,0,40);
		memset(ba,0,40);
		memset(ca,0,40);

		// copy screen to memory DC
		BitBlt(hmemDC, 0, 0, ScreenWidth, ScreenHeight, hScreenDC, 0, 0, SRCCOPY);
/*
		nIter = (nIter+1)%32768;

		if (nIter%10 == 0) {
			CString sDebug;
			DWORD dwDelta = GetTickCount() - dwStart;
			sDebug.Format(_T("%.2f eIPS (%dms per run)\n"),(float)(nCount*1000/(dwDelta-nCount*70)),(int)((dwDelta-(nCount*70))/nCount));
			OutputDebugString(sDebug);
			dwStart = GetTickCount();
			nCount = 0;
		}
		nCount++;*/

		int ret;
		// copy bitmap data into global memory
		if (ret = GetDIBits(hmemDC, hmemBM, 0, ScreenHeight, pScreen, &bmInfo, DIB_RGB_COLORS) != ScreenHeight) {
			int err = GetLastError();
			return;
		}

		

		PIXEL *pPixel;
		for (itRegion=Regions.begin(),i=0;itRegion!=Regions.end(); itRegion++,i++) {
			CRect rc = *itRegion;
			for (y=rc.top; y!=rc.bottom; y++) {
				for (x=rc.left; x!=rc.right; x++) {
					pPixel = &pScreen[ScreenWidth*(ScreenHeight-y)+(ScreenWidth-x)];

					ra[i] += pPixel->r;
					ga[i] += pPixel->g;
					ba[i] += pPixel->b;
					ca[i]++;
				}
			}
		}

		for (i=0;i!=8;i++) {
			r = ra[i]/ca[i];
			g = ga[i]/ca[i];
			b = ba[i]/ca[i];
			
/*
			r *= (r>128)?1.5:0.5;
			g *= (g>128)?1.5:0.5;
			b *= (b>128)?1.5:0.5;

			a = (r+g+b)/3;

			r = (r<255)?(r>0?r:0):255;			
			g = (g<255)?(g>0?g:0):255;
			b = (b<255)?(b>0?b:0):255;
*/
	//		g_Leds.FadeLED(4-i,RGB(r,g,b),20);
			g_Leds.SetLED(i+1,RGB(r,g,b));
		}
		
		g_Leds.Flush();
		Sleep(20);
	}
}