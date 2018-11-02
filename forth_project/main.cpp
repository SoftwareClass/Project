#include <iostream>
#include "./gdal/gdal_priv.h"
#include "Kernel.h"
#pragma comment(lib, "./gdal_i.lib")
using namespace std;

#define BOUNDARY_VALUE 0


bool inBoundary(int x, int y, int s_l_x, int s_h_x, int s_l_y, int s_h_y) {
	return (x >= s_l_x && x <= s_h_x && y >= s_l_y && y <= s_h_y);
}

int kernelProcess(GByte* buffer, int bufferXLen, int centerX, int centerY, const Kernel &ker, bool flag) {
	double dealVal = 0;
	int kerN = ker.getSize();
	int xBegin = centerX - (kerN >> 1);
	int yBegin = centerY - (kerN >> 1);
	for (int i = 0; i < kerN; i++)
		for (int j = 0; j < kerN; j++)
			dealVal += ker.getVal(i, j)*buffer[(xBegin + i)*bufferXLen + (yBegin + j)];
	
	return flag?(((int)dealVal+128) & 0xFF):((int)dealVal & 0xFF);
}

void testKernelPerformance(GDALDataset *srcPic, const Kernel &ker, char * outputPath, bool flag) {

	// origin picture attribute
	int imgXLen = srcPic->GetRasterXSize();
	int imgYLen = srcPic->GetRasterYSize();
	int bandNum = srcPic->GetRasterCount();

	// process range
	int beginRow, endRow, beginCol, endCol;
	beginRow = beginCol = ker.getSize() >> 1;
	endRow = imgYLen - beginRow - 1;
	endCol = imgXLen - beginRow - 1;

	// define some datastruct
	GByte* buffer;
	GDALDataset * desPic;

	//new a block memory for srcPic, to accelerate process
	buffer = (GByte*)CPLMalloc(imgXLen*imgYLen * sizeof(GByte));
	// create output image
	desPic = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		outputPath, imgXLen, imgYLen, bandNum, GDT_Byte, NULL);

	// process band one by one
	for (int bandi = 1; bandi <= bandNum; bandi++) {
		srcPic->GetRasterBand(bandi)->RasterIO(GF_Read,
			0, 0, imgXLen, imgYLen, buffer, imgXLen, imgYLen, GDT_Byte, 0, 0);

		//process begin
		for (int pi = 0; pi < imgYLen; pi++) {
			for (int pj = 0; pj < imgXLen; pj++) {
				//process this point by kernel multiplication
				if (inBoundary(pi, pj, beginRow, endRow, beginCol, endCol)) {
					buffer[pi*imgXLen + pj] = kernelProcess(buffer, imgXLen, pi, pj, ker,flag);
				}
				else
					buffer[pi*imgXLen + pj] = BOUNDARY_VALUE;
			}
		}
		printf("... processing %i band ...\n", bandi);

		desPic->GetRasterBand(bandi)->RasterIO(GF_Write,
			0, 0, imgXLen, imgYLen, buffer, imgXLen, imgYLen, GDT_Byte, 0, 0);

	}
	//process end

	// clean memory
	CPLFree(buffer);
	// close desPic system resource
	GDALClose(desPic);
}

int main() {
#pragma region Initate test data


	//test kernel
	double test1[] = {
		//均值模糊
		0, 0.2, 0,
		0.2, 0.2, 0.2,
		0, 0.2, 0
	};
	double test2[] = {
		//运动模糊
		0.2, 0, 0, 0, 0,
		0, 0.2, 0, 0, 0,
		0, 0, 0.2, 0, 0,
		0, 0, 0, 0.2, 0,
		0, 0, 0, 0, 0.2
	};
	double test3[] = {
		//全方位边缘检测
		-1,-1,-1,
		-1, 8,-1,
		-1,-1,-1
	};
	double test4[] = {
		//锐化
		-1,-1,-1,
		-1, 9,-1,
		-1,-1,-1
	};
	double test5[] = {
		//浮雕
		-1,-1, 0,
		-1, 0, 1,
		0, 1, 1
	};
	double test6[] = {
		//高斯模糊
		0.00048,0.005012,0.010944,0.005012,0.00048 ,
		0.005012,0.052216,0.103256,0.052216,0.005012,
		0.010944,0.103256,0.249104,0.103256,0.010944,
		0.005012,0.052216,0.103256,0.052216,0.005012,
		0.00048,0.005012,0.010944,0.005012,0.00048
	};
	const int testCase = 6;
	Kernel ks[] = {
		Kernel(3, test1),
		Kernel(5, test2),
		Kernel(3, test3),
		Kernel(3, test4),
		Kernel(3, test5),
		Kernel(5, test6),
	};
#pragma endregion
	//origin picture
	GDALDataset * srcPic;

	//some pathes of picture
	char * srcPicPath = "./lena.jpg";
	//char * srcPicPath = "./test.jpg";
	char * formatDesPicPath = "./res_kernel_%d.tif";
	char desPicPath[64];

	//register dirver
	GDALAllRegister();

	//open image
	srcPic = (GDALDataset*)GDALOpenShared(srcPicPath, GA_ReadOnly);

	//begin test kernel performance
	bool flag = false;
	for (int i = 0; i < testCase; i++) {
		if (i == 4) flag = 1; // special to process Embossing 
		sprintf(desPicPath, formatDesPicPath, i + 1);
		testKernelPerformance(srcPic, ks[i], desPicPath,flag);
		printf("kernel %d process done!\n", i + 1);
		flag = false;
	}

	// close dataSet
	GDALClose(srcPic);
	return 0;
}