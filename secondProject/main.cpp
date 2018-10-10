#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

int main(){
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	/*
	//起始位置坐标
	int StartX = 100;
	int StartY = 100;
	//区域宽度和高度
	int tmpXlen = 200;
	int tmpYlen = 150;
	*/

	//起始位置坐标
	int StartX1 = 300;
	int StartY1 = 300;
	int StartX2 = 500;
	int StartY2 = 500;
	//区域宽度和高度
	int tmpXlen1 = 100;
	int tmpYlen1 = 50;
	int tmpXlen2 = 50;
	int tmpYlen2 = 100;

	//输入图像路径
	char* srcPath = "square.jpg";
	//输出图像路径
	char* dstPath = "square2.tif";
	//图像内存存储
	GByte* buffTmp;
	//图像波段数
	int i, bandNum;
	
	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//获取图形宽度，高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();

	//输出获取的结果
	/*cout << " Image X Length: " << imgXlen << endl;
	cout << " Image Y Length: " << imgYlen << endl;
	cout << " Band number :   " << bandNum << endl;*/

	//根据图像的宽度和高度分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen * imgYlen * sizeof(GByte));

	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	
	//一个个波段的输入，然后一个个波段的输出
	for(i = 0; i < bandNum; i ++){
		poSrcDS->GetRasterBand(i+1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i+1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
	}

	//读取红色通道缓存在buffTmp中
	for(int i = 1; i <= 3; i ++){
		poSrcDS->GetRasterBand(i)->RasterIO(GF_Read,
			StartX1, StartY1, tmpXlen1, tmpYlen1, buffTmp, tmpXlen1, tmpYlen1, GDT_Byte, 0, 0);
	}
	for(int i = 1; i <= 3; i ++){
		poSrcDS->GetRasterBand(i)->RasterIO(GF_Read,
			StartX2, StartY2, tmpXlen2, tmpYlen2, buffTmp, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
	}
	//遍历区域，逐像素置为255
	for (int j = 0; j < tmpYlen1; j++) {
		for (int k = 0; k < tmpXlen1; k++) {
			buffTmp[j*tmpXlen1 + k] = (GByte)255;
		}
	}
	for(int i = 1; i <=3; i ++){
		poDstDS->GetRasterBand(i)->RasterIO(GF_Write,
			StartX1, StartY1, tmpXlen1, tmpYlen1, buffTmp, tmpXlen1, tmpYlen1, GDT_Byte, 0, 0);
	}
	for (int j = 0; j < tmpYlen2; j++) {
		for (int k = 0; k < tmpXlen2; k++) {
			buffTmp[j*tmpXlen2 + k] = (GByte)0;
		}
	}

	//数据写入posDstDS
	for(int i = 1; i <=3; i ++){
		poDstDS->GetRasterBand(i)->RasterIO(GF_Write,
			StartX2, StartY2, tmpXlen2, tmpYlen2, buffTmp, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
	}
	//清除内存
	CPLFree(buffTmp);

	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");
	return 0;
}