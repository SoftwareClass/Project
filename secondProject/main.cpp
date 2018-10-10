#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

int main(){
	//����ͼ��
	GDALDataset* poSrcDS;
	//���ͼ��
	GDALDataset* poDstDS;
	//ͼ��Ŀ�Ⱥ͸߶�
	int imgXlen, imgYlen;
	/*
	//��ʼλ������
	int StartX = 100;
	int StartY = 100;
	//�����Ⱥ͸߶�
	int tmpXlen = 200;
	int tmpYlen = 150;
	*/

	//��ʼλ������
	int StartX1 = 300;
	int StartY1 = 300;
	int StartX2 = 500;
	int StartY2 = 500;
	//�����Ⱥ͸߶�
	int tmpXlen1 = 100;
	int tmpYlen1 = 50;
	int tmpXlen2 = 50;
	int tmpYlen2 = 100;

	//����ͼ��·��
	char* srcPath = "square.jpg";
	//���ͼ��·��
	char* dstPath = "square2.tif";
	//ͼ���ڴ�洢
	GByte* buffTmp;
	//ͼ�񲨶���
	int i, bandNum;
	
	//ע������
	GDALAllRegister();

	//��ͼ��
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//��ȡͼ�ο�ȣ��߶ȺͲ�������
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();

	//�����ȡ�Ľ��
	/*cout << " Image X Length: " << imgXlen << endl;
	cout << " Image Y Length: " << imgYlen << endl;
	cout << " Band number :   " << bandNum << endl;*/

	//����ͼ��Ŀ�Ⱥ͸߶ȷ����ڴ�
	buffTmp = (GByte*)CPLMalloc(imgXlen * imgYlen * sizeof(GByte));

	//�������ͼ��
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	
	//һ�������ε����룬Ȼ��һ�������ε����
	for(i = 0; i < bandNum; i ++){
		poSrcDS->GetRasterBand(i+1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i+1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
	}

	//��ȡ��ɫͨ��������buffTmp��
	for(int i = 1; i <= 3; i ++){
		poSrcDS->GetRasterBand(i)->RasterIO(GF_Read,
			StartX1, StartY1, tmpXlen1, tmpYlen1, buffTmp, tmpXlen1, tmpYlen1, GDT_Byte, 0, 0);
	}
	for(int i = 1; i <= 3; i ++){
		poSrcDS->GetRasterBand(i)->RasterIO(GF_Read,
			StartX2, StartY2, tmpXlen2, tmpYlen2, buffTmp, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
	}
	//����������������Ϊ255
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

	//����д��posDstDS
	for(int i = 1; i <=3; i ++){
		poDstDS->GetRasterBand(i)->RasterIO(GF_Write,
			StartX2, StartY2, tmpXlen2, tmpYlen2, buffTmp, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
	}
	//����ڴ�
	CPLFree(buffTmp);

	//�ر�dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");
	return 0;
}