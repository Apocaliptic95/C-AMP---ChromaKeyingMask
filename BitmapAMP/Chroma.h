#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <Windows.h>

using namespace std;

int ReadFileIntoChar(string FileName, BITMAPFILEHEADER* &Out_FileHeader, BITMAPINFOHEADER* &Out_InfoHeader, unsigned char* &Out_BitmapData)
{
	//Open image file
	ifstream InFileStream(FileName, ios::binary);
	if (!InFileStream.is_open())
	{
		return 1;
	}
	//Read file header
	char* temp = new char[sizeof(BITMAPFILEHEADER)];
	InFileStream.read(temp, sizeof(BITMAPFILEHEADER));
	Out_FileHeader = (BITMAPFILEHEADER*)(temp);

	//Read info header
	temp = new char[sizeof(BITMAPINFOHEADER)];
	InFileStream.read(temp, sizeof(BITMAPINFOHEADER));
	Out_InfoHeader = (BITMAPINFOHEADER*)(temp);
	//Read bitmap data
	if (Out_InfoHeader->biBitCount == 24 || Out_InfoHeader->biBitCount == 32)
	{
		InFileStream.seekg(Out_FileHeader->bfOffBits, ios::beg);
		int Size = Out_FileHeader->bfSize - Out_FileHeader->bfOffBits;
		Out_BitmapData = new unsigned char[Size];
		InFileStream.read((char*)Out_BitmapData, Size);
	}
	return 0;
}

int CharMapToIntMap_24bit(unsigned char* In_BitmapData, BITMAPINFOHEADER* InfoHeader, unsigned int* &Out_IntBitmapData)
{
	Out_IntBitmapData = new unsigned int[InfoHeader->biHeight*InfoHeader->biWidth];
	int n = 0;
	for (int i = 0; i < InfoHeader->biHeight*InfoHeader->biWidth; i++)
	{
		//BLUE
		Out_IntBitmapData[i] = 0;
		Out_IntBitmapData[i] |= In_BitmapData[n];
		//GREEN
		Out_IntBitmapData[i] <<= 8;
		Out_IntBitmapData[i] |= In_BitmapData[n + 1];
		//RED
		Out_IntBitmapData[i] <<= 8;
		Out_IntBitmapData[i] |= In_BitmapData[n + 2];
		n += 3;
	}
	return 0;
}

int CharMapToIntMap_32bit(unsigned char* In_BitmapData, BITMAPINFOHEADER* InfoHeader, unsigned int* &Out_IntBitmapData)
{
	Out_IntBitmapData = new unsigned int[InfoHeader->biHeight*InfoHeader->biWidth];
	int n = 0;
	for (int i = 0; i < InfoHeader->biHeight*InfoHeader->biWidth; i++)
	{
		//ALPHA
		Out_IntBitmapData[i] = 0;
		Out_IntBitmapData[i] |= In_BitmapData[n];
		//RED
		Out_IntBitmapData[i] <<= 8;
		Out_IntBitmapData[i] |= In_BitmapData[n + 1];
		//GREEN
		Out_IntBitmapData[i] <<= 8;
		Out_IntBitmapData[i] |= In_BitmapData[n + 2];
		//BLUE
		Out_IntBitmapData[i] <<= 8;
		Out_IntBitmapData[i] |= In_BitmapData[n + 3];
		n += 4;
	}
	return 0;
}

int IntMapToCharMap_24bit(unsigned int* In_BitmapData, BITMAPINFOHEADER* InfoHeader, unsigned char* &Out_CharBitmapData)
{
	int n = 0;
	Out_CharBitmapData = new unsigned char[InfoHeader->biHeight*InfoHeader->biWidth * (InfoHeader->biBitCount / 8)];
	for (int i = 0; i < InfoHeader->biHeight*InfoHeader->biWidth; i++)
	{
		Out_CharBitmapData[n] = 0;
		Out_CharBitmapData[n + 1] = 0;
		Out_CharBitmapData[n + 2] = 0;

		//RED
		Out_CharBitmapData[n + 2] |= In_BitmapData[i];
		In_BitmapData[i] >>= 8;
		//GREEN
		Out_CharBitmapData[n + 1] |= In_BitmapData[i];
		In_BitmapData[i] >>= 8;
		//BLUE
		Out_CharBitmapData[n] |= In_BitmapData[i];
		n += 3;
	}
	return 0;
}

int IntMapToCharMap_32bit(unsigned int* In_BitmapData, BITMAPINFOHEADER* InfoHeader, unsigned char* &Out_CharBitmapData)
{
	int n = 0;
	Out_CharBitmapData = new unsigned char[InfoHeader->biHeight*InfoHeader->biWidth * (InfoHeader->biBitCount / 8)];
	for (int i = 0; i < InfoHeader->biHeight*InfoHeader->biWidth; i++)
	{
		Out_CharBitmapData[n] = 0;
		Out_CharBitmapData[n + 1] = 0;
		Out_CharBitmapData[n + 2] = 0;
		Out_CharBitmapData[n + 3] = 0;
		//ALPHA
		Out_CharBitmapData[n + 3] |= In_BitmapData[i];
		In_BitmapData[i] >>= 8;
		//BLUE
		Out_CharBitmapData[n + 2] |= In_BitmapData[i];
		In_BitmapData[i] >>= 8;
		//GREEN
		Out_CharBitmapData[n + 1] |= In_BitmapData[i];
		In_BitmapData[i] >>= 8;
		//RED
		Out_CharBitmapData[n] |= In_BitmapData[i];
		n += 4;
	}
	return 0;
}

int SaveFileFromChar(string FileName, BITMAPFILEHEADER* FileHeader, BITMAPINFOHEADER* InfoHeader, unsigned char* BitmapData)
{
	ofstream OutFileStream(FileName, ios::binary);
	OutFileStream.write((char*)FileHeader, sizeof(BITMAPFILEHEADER));
	OutFileStream.write((char*)InfoHeader, sizeof(BITMAPINFOHEADER));
	int Size = FileHeader->bfSize - FileHeader->bfOffBits;
	OutFileStream.write((char *)BitmapData, Size);
	return 0;
}

unsigned char RGB_To_Y(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char y;
	y = (unsigned char)round(0.299*r + 0.587*g + 0.114*b);
	return (y);
}

unsigned char RGB_To_Cb(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char cb;
	cb = (unsigned char)round(128 + -0.168736*r - 0.331264*g + 0.5*b);
	return (cb);
}

unsigned char RGB_To_Cr(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char cr;
	cr = (unsigned char)round(128 + 0.5*r - 0.418688*g - 0.081312*b);
	return (cr);
}
