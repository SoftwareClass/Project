#include <iostream>
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "./gdal_i.lib")
using namespace std;

//https://blog.csdn.net/liminlu0314/article/details/7072224

const int blockSize = 256;

int main() {
	//origin picture
	GDALDataset *poMulDS, *poPanDS, *poFusDS;

	//some pathes of picture
	char * mulPath = "./src_img/Mul_large.tif";
	char * panPath = "./src_img/Pan_large.tif";
	char * fusPath = "./Fus_large.tif";
	int imgXlen, imgYlen;
	float *bandR, *bandG, *bandB;
	float *bandH, *bandS;
	float *bandP;

	//register dirver
	GDALAllRegister();

	// open datasets
	poMulDS = (GDALDataset*)GDALOpenShared(mulPath, GA_ReadOnly);
	poPanDS = (GDALDataset*)GDALOpenShared(panPath, GA_ReadOnly);
	imgXlen = poMulDS->GetRasterXSize();
	imgYlen = poMulDS->GetRasterYSize();
	poFusDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		fusPath, imgXlen, imgYlen, 3, GDT_Byte, NULL);
	
	
	bandR = (float*)CPLMalloc(blockSize*blockSize * sizeof(float));
	bandG = (float*)CPLMalloc(blockSize*blockSize * sizeof(float));
	bandB = (float*)CPLMalloc(blockSize*blockSize * sizeof(float));
	bandP = (float*)CPLMalloc(blockSize*blockSize * sizeof(float));
	bandH = (float*)CPLMalloc(blockSize*blockSize * sizeof(float));
	bandS = (float*)CPLMalloc(blockSize*blockSize * sizeof(float));

	poMulDS->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen,
		bandR, imgXlen, imgYlen, GDT_Float32, 0, 0);
	poMulDS->GetRasterBand(2)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen,
		bandG, imgXlen, imgYlen, GDT_Float32, 0, 0);
	poMulDS->GetRasterBand(3)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen,
		bandB, imgXlen, imgYlen, GDT_Float32, 0, 0);
	poPanDS->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen,
		bandP, imgXlen, imgYlen, GDT_Float32, 0, 0);
	
		
	return 0;
}