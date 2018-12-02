#include <iostream>
#include <ctime>
using namespace std;
#include "gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")
#define testout(filename) freopen((filename) ,"w",stdout)

const int BLOCKHEIGHT = 256;

int main() {
	testout("data.out");
	// origin picture
	GDALDataset *poMulDS, *poPanDS, *poFusDS;

	// some pathes of picture
	char * mulPath = "./src_img/Mul_large.tif";
	char * panPath = "./src_img/Pan_large.tif";
	char * fusPath = "./Fus_large_block.tif";

	//char * mulPath = "./src_img/American_MUL.bmp";
	//char * panPath = "./src_img/American_PAN.bmp";
	//char * fusPath = "./Fus_small_block.tif";

	int imgXlen, imgYlen;
	float *bandR, *bandG, *bandB;
	float *bandH, *bandS;
	float *bandP;

	// register dirver
	GDALAllRegister();

	// open datasets
	poMulDS = (GDALDataset*)GDALOpenShared(mulPath, GA_ReadOnly);
	poPanDS = (GDALDataset*)GDALOpenShared(panPath, GA_ReadOnly);
	imgXlen = poMulDS->GetRasterXSize();
	imgYlen = poMulDS->GetRasterYSize();
	poFusDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		fusPath, imgXlen, imgYlen, 3, GDT_Byte, NULL);

	printf(" 高:%d  宽:%d\n", imgYlen, imgXlen);

	// allocate memory
	bandR = (float*)CPLMalloc(BLOCKHEIGHT*BLOCKHEIGHT * sizeof(float));
	bandG = (float*)CPLMalloc(BLOCKHEIGHT*BLOCKHEIGHT * sizeof(float));
	bandB = (float*)CPLMalloc(BLOCKHEIGHT*BLOCKHEIGHT * sizeof(float));
	bandP = (float*)CPLMalloc(BLOCKHEIGHT*BLOCKHEIGHT * sizeof(float));
	bandH = (float*)CPLMalloc(BLOCKHEIGHT*BLOCKHEIGHT * sizeof(float));
	bandS = (float*)CPLMalloc(BLOCKHEIGHT*BLOCKHEIGHT * sizeof(float));

	// calculate block parameters
	int row_block_cnt = imgYlen / BLOCKHEIGHT;
	int col_block_cnt = imgXlen / BLOCKHEIGHT;
	int row_len, col_len, begin_x, begin_y;

	//开始计时
	clock_t start = clock();
	for (int i = 0; i <= row_block_cnt; i++) {
		col_len = (i - row_block_cnt ? BLOCKHEIGHT : imgYlen - row_block_cnt*BLOCKHEIGHT);
		for (int j = 0; j <= col_block_cnt; j++) {
			row_len = (j - col_block_cnt ? BLOCKHEIGHT : imgXlen - col_block_cnt*BLOCKHEIGHT);
			if (!row_len || !col_len) continue;
			begin_x = j*BLOCKHEIGHT; begin_y = i*BLOCKHEIGHT;

			printf(" 分块: %d 行块 %d 列块 坐标x:%d 坐标y:%d 宽:%d 高:%d \n", i + 1, j + 1, begin_x, begin_y, row_len, col_len);

			// read img from raw image with block type one by one  
			poMulDS->GetRasterBand(1)->RasterIO(GF_Read, begin_x, begin_y, row_len, col_len,
				bandR, row_len, col_len, GDT_Float32, 0, 0);
			poMulDS->GetRasterBand(2)->RasterIO(GF_Read, begin_x, begin_y, row_len, col_len,
				bandG, row_len, col_len, GDT_Float32, 0, 0);
			poMulDS->GetRasterBand(3)->RasterIO(GF_Read, begin_x, begin_y, row_len, col_len,
				bandB, row_len, col_len, GDT_Float32, 0, 0);
			poPanDS->GetRasterBand(1)->RasterIO(GF_Read, begin_x, begin_y, row_len, col_len,
				bandP, row_len, col_len, GDT_Float32, 0, 0);

			
			for (int k = 0; k < row_len*col_len; k++)
			{
				// RGB->IHS
				bandH[k] = -sqrt(2.0f) / 6.0f*bandR[k] - sqrt(2.0f) / 6.0f*bandG[k] + sqrt(2.0f) / 3.0f*bandB[k];
				bandS[k] = 1.0f / sqrt(2.0f)*bandR[k] - 1 / sqrt(2.0f)*bandG[k];

				// IHS->RGB
				bandR[k] = bandP[k] - 1.0f / sqrt(2.0f)*bandH[k] + 1.0f / sqrt(2.0f)*bandS[k];
				bandG[k] = bandP[k] - 1.0f / sqrt(2.0f)*bandH[k] - 1.0f / sqrt(2.0f)*bandS[k];
				bandB[k] = bandP[k] + sqrt(2.0f)*bandH[k];
			}

			// save dealed img
			poFusDS->GetRasterBand(1)->RasterIO(GF_Write, begin_x, begin_y, row_len, col_len,
				bandR, row_len, col_len, GDT_Float32, 0, 0);
			poFusDS->GetRasterBand(2)->RasterIO(GF_Write, begin_x, begin_y, row_len, col_len,
				bandG, row_len, col_len, GDT_Float32, 0, 0);
			poFusDS->GetRasterBand(3)->RasterIO(GF_Write, begin_x, begin_y, row_len, col_len,
				bandB, row_len, col_len, GDT_Float32, 0, 0);
		}
	}
	//结束计时
	clock_t end = clock();
	cout << "running(ms):" << end - start << endl;

	// free all allocate memory
	CPLFree(bandR);
	CPLFree(bandG);
	CPLFree(bandB);
	CPLFree(bandH);
	CPLFree(bandS);
	CPLFree(bandP);

	GDALClose(poMulDS);
	GDALClose(poPanDS);
	GDALClose(poFusDS);
	return 0;
}