#pragma once

UINT StartProcessor(LPVOID lpParam);

typedef struct {
	unsigned char b:8;
	unsigned char g:8;
	unsigned char r:8;
	unsigned char _pad:8;
} PIXEL;

class CProcessor
{
protected:
	void Run();
	
public:
	UINT CoatingFunc();
	CProcessor(void);
	~CProcessor(void);
};
