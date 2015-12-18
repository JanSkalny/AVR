#include <stdint.h>
#include <stdlib.h>
#include "crc.h"

uint32_t* crc32_table = 0;

uint32_t* crc32_init_table()
{
	uint32_t poly32 = 0, *table32 = 0, part32=0;
	uint8_t poly_bits_32[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26}; //,32
	uint32_t i,j;

	for (i=0; i<sizeof(poly_bits_32); i++)
		poly32 |= (uint32_t)(1) << (31-poly_bits_32[i]);

	table32 = (uint32_t*) malloc(sizeof(uint32_t) * 256);

	for (i=0; i<256; i++) {
		part32 = (uint32_t)i;
			
		for (j=0; j<8; j++)
			part32 = (part32 & 1) ? (part32>>1)^poly32 : (part32>>1);

		table32[i] = part32;
	}

	return table32;
}

uint32_t crc32(uint8_t* buffer, uint32_t length)
{	
	return crc32_append(buffer, length, 0);
}

uint32_t crc32_append(uint8_t* buffer, uint32_t length, uint32_t initstate)
{	
	register uint32_t crc = CRC_INVERT32(initstate);

	if (!crc32_table)
		crc32_table = crc32_init_table();

	while (length--) 
		crc = crc32_table[(uint8_t)(crc)^(*buffer++)] ^ (crc>>8);

	return CRC_INVERT32(crc);
}


/*
void CCRC::InitTable8()
{
	BYTE Poly8 = 0;
	BYTE PolyBits8[]={0,2,4,6,7}; // ,8	
	DWORD i,j;

	for (i = 0; i < sizeof(PolyBits8); i++)
		Poly8 |= (BYTE)(1) << (7-PolyBits8[i]);

	Table8 = new DWORD[256];

	for (i = 0; i < 256; i++)
	{
		BYTE part8 = (BYTE)(i);

		for (j = 0; j < 8; j++)
			part8 = (part8 & 1) ? (part8>>1)^Poly8 : (part8>>1);

		Table8[i] = part8;
	}
}


void CCRC::InitTable16()
{
	WORD Poly16 = 0;
	BYTE PolyBits16[]={0,2,15}; // ,16
	DWORD i,j;

	for (i = 0; i < sizeof(PolyBits16); i++)
		Poly16|= (WORD)(1) << (15-PolyBits16[i]);

	Table16 = new DWORD[256];

	for (i = 0; i < 256; i++)
	{
		WORD part16 = (WORD)(i);

		for (j = 0; j < 8; j++)
			part16 = (part16 & 1) ? (part16>>1)^Poly16 : (part16>>1);

		Table16[i] = part16;
	}
}

void CCRC::InitTable64()
{
	QWORD Poly64 = 0;
	BYTE PolyBits64[]={0,3,4,6,9,10,12,13,16,19,22,23,26,28,31,32,34,36,37,41,44,46,47,48,49,52,55,56,58,59,61,63}; // ,64
	DWORD i,j;

	for (i = 0; i < sizeof(PolyBits64); i++)
		Poly64|= (QWORD)(1) << (63-PolyBits64[i]);

	Table64 = new QWORD[256];

	for (i = 0; i < 256; i++)
	{
		QWORD part64 = (QWORD)(i);

		for (j = 0; j < 8; j++)
			part64 = (part64 & 1) ? (part64>>1)^Poly64 : (part64>>1);

		Table64[i] = part64;
	}
}

BYTE CCRC::ComputeCRC8(BYTE* buffer, DWORD length, BYTE initstate)
{
	if (Table8 == NULL) 
		InitTable8();

	BYTE* pBuf = buffer;

	register DWORD crc = INVERT8(initstate);
	
	while (length--) 
		crc = Table8[(BYTE)(crc)^(*buffer++)] ^ (crc>>8);

	return (BYTE) INVERT8(crc);
};

WORD CCRC::ComputeCRC16(BYTE* buffer, DWORD length, WORD initstate)
{
	if (Table16 == NULL) 
		InitTable16();

	BYTE* pBuf = buffer;

	register DWORD crc = INVERT16(initstate);
	
	while (length--) 
		crc = Table16[(BYTE)(crc)^(*buffer++)] ^ (crc>>8);

	return (WORD) INVERT16(crc);
};

QWORD CCRC::ComputeCRC64(BYTE* buffer, DWORD length, QWORD initstate)
{
	if (Table64 == NULL) 
		InitTable64();

	register QWORD crc = INVERT64(initstate);

	while (length--) 
		crc = Table64[(BYTE)(crc)^(*buffer++)] ^ (crc>>8);

	return INVERT64(crc);
};
*/

