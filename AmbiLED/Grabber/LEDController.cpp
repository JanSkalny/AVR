#include "StdAfx.h"
#include "LEDController.h"

CLEDController::CLEDController(void)
: m_sPort(_T("COM1"))
{
	m_hComm = INVALID_HANDLE_VALUE;
}

CLEDController::~CLEDController(void)
{
	// cleanup, release any resources
	Close();
}

// (re)-open communication port
bool CLEDController::Open(CString sPort)
{	
	DCB dcb;

	// close existing connection first
	if (IsReady()) 
		Close();

	if (sPort.GetLength() > 0)
		m_sPort = sPort;

	// open connection to port
	m_hComm = CreateFile(m_sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0,0);
	if (m_hComm == INVALID_HANDLE_VALUE) {
		OutputDebugString(_T("failed to open comm port\n"));
		return false;
	}

	// set the baud rate
	if (!GetCommState(m_hComm, &dcb)) {
		OutputDebugString(_T("GetCommState: failed\n"));
		Close();
		return false;
	}
	dcb.BaudRate = CBR_57600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	if (!SetCommState(m_hComm, &dcb)) {
		OutputDebugString(_T("SetCommState: failed\n"));
		Close();
		return false;
	}

	// cleanup ;p
	PurgeComm(m_hComm,PURGE_TXCLEAR|PURGE_RXCLEAR);

	return true;
}

// close communitcation port, if opened
void CLEDController::Close(void)
{
	if (m_hComm != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

// is communication port ready?
bool CLEDController::IsReady(void)
{
	return (m_hComm != INVALID_HANDLE_VALUE) ? true : false;
}

//
// Send bytes to device
bool CLEDController::Send(unsigned char * pData, int nLen)
{
	CString sErr;
	DWORD dwCount;

	if (!IsReady())
		Open();

	if (!WriteFile(m_hComm, pData, nLen,&dwCount,0)) {	
		sErr.Format(_T("write: failed (%d)\n"),GetLastError());
		OutputDebugString(sErr);
		return false;
	}
	
	return true;
}


//
// CMD_RESET
// zero all outputs
bool CLEDController::Reset(void)
{
	unsigned char buf = CMD_RESET;

	return Send(&buf,1);
}


// CMD_FLUSH
// display buffer
bool CLEDController::Flush(void)
{
	unsigned char buf = CMD_FLUSH;

	return Send(&buf,1);
}

// CMD_TEST
// run simple test (demo)
bool CLEDController::RunTest(void)
{
	unsigned char buf = CMD_TEST;

	return Send(&buf,1);
}

// CMD_SET_LED
// set led Id to color RGB
bool CLEDController::SetLED(unsigned char nId, COLORREF crColor)
{
	unsigned char buf[5];
	double r, g, b, d, t, v;
	int min,max;

	r = GetRValue(crColor);
	g = GetGValue(crColor);
	b = GetBValue(crColor);

	// color distance
	min = (r<g ? (b<r?b:r) : (b<g?b:g));
	max = (r>g ? (b>r?b:r) : (b>g?b:g));

	d = max-min;

	t=0;
	//t = (d-128)/128;
/*
	if (nId == 6) {
		CString sDebug;
		sDebug.Format(_T("id=%d,min=%d,max=%d,d=%f,t=%f\n"),nId,min,max,d,t);
		OutputDebugString(sDebug);
	}
*/
	v = (r+g+b)/3;
		
	if (v < 64 && d < 32)
		t=(d-16)/32; 

	r += r*t;
	g += g*t;
	b += b*t;

	// color correction (for hw)
	r *= 1.2;
	g *= 0.5;
	b *= 0.6;

	

	// validate results
	r = (r>255) ? 255 : (r<0 ? 0 : r);
	g = (g>255) ? 255 : (g<0 ? 0 : g);
	b = (b>255) ? 255 : (b<0 ? 0 : b);

	buf[0] = CMD_SET_LED;
	buf[1] = nId;
	buf[2] = (unsigned char)r;
	buf[3] = (unsigned char)g;
	buf[4] = (unsigned char)b;

	return Send(buf,5);
}

bool CLEDController::FadeLED(unsigned char nId, COLORREF crColor, int nDuration)
{
	unsigned char buf[7];

	ASSERT(false);

	buf[0] = CMD_FADE_LED;
	buf[1] = nId;
	buf[2] = (nDuration>>8)&0xff;
	buf[3] = nDuration&0xff;
	buf[4] = GetRValue(crColor);
	buf[5] = GetGValue(crColor)*0.7;
	buf[6] = GetBValue(crColor)*0.8;

	return Send(buf,7);
}
