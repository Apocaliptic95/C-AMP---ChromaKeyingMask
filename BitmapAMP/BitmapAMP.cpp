#include "stdafx.h"
#include "Chroma.h"
#include "gaussian_blur.h"
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <amp.h>
#include <amp_math.h>
using namespace std;
using namespace concurrency;
using namespace concurrency::fast_math;

int main()
{
	//concurrency::accelerator const device(concurrency::accelerator::direct3d_warp);
	//bool success = concurrency::accelerator::set_default(device.device_path);
	std::wcout << "Akcelerator: " << concurrency::accelerator(concurrency::accelerator::default_accelerator).description << "" << std::endl;

	char name[30];
	std::cout << "Nazwa:";
	cin >> name;
	unsigned char* pixs = 0;
	BITMAPFILEHEADER* bfh = 0;
	BITMAPINFOHEADER* bih = 0;
	std::cout << "Kolor (b/g):";
	char color;
	cin >> color;
	if (!ReadFileIntoChar(name, bfh, bih, pixs))
	{
		unsigned char R_key, G_key, B_key;
		if ((int)color == (int)'g')
		{
			std::cout << "GREEN" << endl;
			R_key = 15;
			G_key = 194;
			B_key = 78;
		}
		else if ((int)color == (int)'b')
		{
			std::cout << "BLUE" << endl;
			R_key = 67;
			G_key = 105;
			B_key = 206;
		}
		else
		{
			std::cout << "Zly kolor.";
			return 0;
		}
		int Cb_key, Cr_key;
		Cb_key = RGB_To_Cb(R_key, G_key, B_key);
		Cr_key = RGB_To_Cr(R_key, G_key, B_key);
		int Tolerance = 0;
		std::cout << "Tolerancja:";
		cin >> Tolerance;
		unsigned int* bitmap_int = 0;
		CharMapToIntMap_24bit(pixs, bih, bitmap_int);
		array_view<unsigned int, 2> Int_Map(bih->biHeight, bih->biWidth, bitmap_int);
		clock_t start = clock();
		int Height = bih->biHeight;
		int Width = bih->biWidth;
		parallel_for_each(Int_Map.extent, [=](index<2> idx) restrict(amp)
		{
			unsigned int R, G, B;
			R = (unsigned int)(Int_Map[idx] & 0xFF);
			G = (unsigned int)(Int_Map[idx] >> 8 & 0xFF);
			B = (unsigned int)(Int_Map[idx] >> 16 & 0xFF);
			unsigned int Cb, Cr;
			Cb = (unsigned int)round(128 - (168736 * R) / 1000000 - (331264 * G) / 1000000 + (5 * B) / 10);
			Cr = (unsigned int)round(128 + (5 * R) / 10 - (418688 * G) / 1000000 - (81312 * B) / 1000000);
			int D = (int) sqrt((Cb_key - Cb)*(Cb_key - Cb) + (Cr_key - Cr)*(Cr_key - Cr));
			if (D < Tolerance)
			{
				Int_Map[idx] = 0x00000000;
			}
			else
			{
				Int_Map[idx] = 0x00FFFFFF;
			}
		});

		double duration = (((double)(clock() - start)) / CLOCKS_PER_SEC);
		std::cout << "FPS: " << 1 / duration << endl;
		Int_Map.synchronize();
		unsigned char* pixs2 = 0;
		IntMapToCharMap_24bit(bitmap_int, bih, pixs2);
		SaveFileFromChar("out.bmp", bfh, bih, pixs2);

		delete bfh;
		delete bih;
		delete[] pixs;
		delete[] pixs2;
		delete[] bitmap_int;
	}
	std::cout << "Ok.";
	getchar();
	getchar();
	return 0;
}