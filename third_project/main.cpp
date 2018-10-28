#include<iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")
int main(){
	//����ͼ��
	GDALDataset* picture1;
	GDALDataset* picture2;
	int imgXlen, imgYlen;
	//���ͼ��
	GDALDataset* result;
	//����ͼ��·��
	char* pic1_path = "space.jpg";
	char* pic2_path = "superman.jpg";
	char* res_path = "respic.jpg";
	int bandNum;//ͼ�񲨶���
	//ͼ���ڴ�洢
	GByte* buffTmp1;
	GByte* buffTmp2;
	GByte* buffTmp3;
	GByte* buffTmp4;
	GByte* buffTmp5;
	GByte* buffTmp6;
	//ע������
	GDALAllRegister();
	//��ͼ��
	picture1 = (GDALDataset*)GDALOpenShared(pic1_path, GA_ReadOnly);
	picture2 = (GDALDataset*)GDALOpenShared(pic2_path, GA_ReadOnly);
	imgXlen = picture1->GetRasterXSize();
	imgYlen = picture1->GetRasterYSize();
	bandNum = picture1->GetRasterCount();
	cout << imgXlen << " " << imgYlen << " " << bandNum << endl;

	//�������ͼ��
	result = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(res_path, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	//����ͼ������ڴ�
	buffTmp1 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp2 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp3 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp4 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp5 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	buffTmp6 = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));

	//��superman.jpg��space.jpg��RGB�ֱ������
	//buffTmp1,2,3�洢����superman.jpg
	//buffTmp4,5,6�洢����space.jpg
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
	//��superman.jpg��������Ҫ��ʱд��space.jpgͼƬ��
	result->GetRasterBand(1)->RasterIO(GF_Write,0, 0, imgXlen, imgYlen, buffTmp1, imgXlen, imgYlen, GDT_Byte, 0, 0);
	result->GetRasterBand(2)->RasterIO(GF_Write,0, 0, imgXlen, imgYlen, buffTmp2, imgXlen, imgYlen, GDT_Byte, 0, 0);
	result->GetRasterBand(3)->RasterIO(GF_Write,0, 0, imgXlen, imgYlen, buffTmp3, imgXlen, imgYlen, GDT_Byte, 0, 0);
	//����ڴ�
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