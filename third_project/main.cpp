#include<iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")
int main(){
	//输入图像
	GDALDataset* picture1;
	GDALDataset* picture2;
	int imgXlen, imgYlen;
	//输出图像
	GDALDataset* result;
	//输入图像路径
	char* pic1_path = "space.jpg";
	char* pic2_path = "superman.jpg";
	char* res_path = "respic.jpg";
	int bandNum;//图像波段数
	//图像内存存储
	GByte* buffTmp1;
	GByte* buffTmp2;
	GByte* buffTmp3;
	GByte* buffTmp4;
	GByte* buffTmp5;
	GByte* buffTmp6;
	//注册驱动
	GDALAllRegister();
	//打开图像
	picture1 = (GDALDataset*)GDALOpenShared(pic1_path, GA_ReadOnly);
	picture2 = (GDALDataset*)GDALOpenShared(pic2_path, GA_ReadOnly);
	imgXlen = picture1->GetRasterXSize();
	imgYlen = picture1->GetRasterYSize();
	bandNum = picture1->GetRasterCount();
	cout << imgXlen << " " << imgYlen << " " << bandNum << endl;

	//创建输出图像
	result = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(res_path, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	//根据图像分配内存
	buffTmp1 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp2 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp3 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp4 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp5 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp6 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));

	//把superman.jpg和space.jpg的RGB分别读出来
	//buffTmp1,2,3存储的是superman.jpg
	//buffTmp4,5,6存储的是space.jpg
	picture2->GetRasterBand(1)->RasterIO(GF_Read,0, 0, imgXlen, imgYlen, buffTmp1, imgXlen, imgYlen, GDT_Byte, 0, 0);
	picture2->GetRasterBand(2)->RasterIO(GF_Read,0, 0, imgXlen, imgYlen, buffTmp2, imgXlen, imgYlen, GDT_Byte, 0, 0);
	picture2->GetRasterBand(3)->RasterIO(GF_Read,0, 0, imgXlen, imgYlen, buffTmp3, imgXlen, imgYlen, GDT_Byte, 0, 0);
	picture1->GetRasterBand(1)->RasterIO(GF_Read,0, 0, imgXlen, imgYlen, buffTmp4, imgXlen, imgYlen, GDT_Byte, 0, 0);
	picture1->GetRasterBand(2)->RasterIO(GF_Read,0, 0, imgXlen, imgYlen, buffTmp5, imgXlen, imgYlen, GDT_Byte, 0, 0);
	picture1->GetRasterBand(3)->RasterIO(GF_Read,0, 0, imgXlen, imgYlen, buffTmp6, imgXlen, imgYlen, GDT_Byte, 0, 0);
	for(int j = 0; j < imgYlen; j ++){
		for(int i = 0; i < imgXlen; i ++){
			if(buffTmp1[j * imgXlen + i] >= (GByte)10 && buffTmp1[j * imgXlen + i] <= (GByte)160 &&
				buffTmp2[j * imgXlen + i] >= (GByte)100 && buffTmp2[j * imgXlen + i] <= (GByte)220 &&
				buffTmp3[j * imgXlen + i] >= (GByte)10&& buffTmp3[j * imgXlen + i] <= (GByte)110) {
							buffTmp1[j * imgXlen + i] = buffTmp4[j * imgXlen + i];
							buffTmp2[j * imgXlen + i] = buffTmp5[j * imgXlen + i];
							buffTmp3[j * imgXlen + i] = buffTmp6[j * imgXlen + i];
			}
		}
	}
	//当superman.jpg满足像素要求时写入space.jpg图片中
	result->GetRasterBand(1)->RasterIO(GF_Write,0, 0, imgXlen, imgYlen, buffTmp1, imgXlen, imgYlen, GDT_Byte, 0, 0);
	result->GetRasterBand(2)->RasterIO(GF_Write,0, 0, imgXlen, imgYlen, buffTmp2, imgXlen, imgYlen, GDT_Byte, 0, 0);
	result->GetRasterBand(3)->RasterIO(GF_Write,0, 0, imgXlen, imgYlen, buffTmp3, imgXlen, imgYlen, GDT_Byte, 0, 0);
	//清除内存
	CPLFree(buffTmp1);
	CPLFree(buffTmp2);
	CPLFree(buffTmp3);
	CPLFree(buffTmp4);
	CPLFree(buffTmp5);
	CPLFree(buffTmp6);
	GDALClose(picture1);
	GDALClose(picture2);
	GDALClose(result);
	system("PAUSE");
	return 0;
}