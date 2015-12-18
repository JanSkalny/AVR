#pragma once

const unsigned char CMD__RESERVED_	= 0;
const unsigned char CMD_INIT		= 1;
const unsigned char CMD_TEST		= 2;
const unsigned char CMD_RESET		= 3;
const unsigned char CMD_FLUSH		= 4;
const unsigned char CMD_SET_LED		= 5;
const unsigned char CMD_FADE_LED	= 6;
const unsigned char CMD_SPEED		= 7;

class CLEDController
{
public:
	CLEDController(void);
	~CLEDController(void);

	bool Open(CString sPort=_T(""));
	void Close(void);
	
	bool Reset(void);
	bool RunTest(void);
	bool Flush(void);
	bool SetLED(unsigned char nId, COLORREF crColor);
	bool FadeLED(unsigned char nId, COLORREF crColor, int nDuration);

protected:
	HANDLE m_hComm;
	CString m_sPort;

	bool Send(unsigned char * pData, int nLen);
	bool IsReady(void);	
};
