#include<iostream>
#include<math.h>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

//float trans1[3][3] = {1/3, 1/3, 1/3,      
//						-sqrt(2*1.0)/6, -sqrt(2*1.0)/6, sqrt(2*1.0)/3,    
//						1/sqrt(2*1.0), -1/sqrt(2*1.0), 0};
//float trans2[3][3] = {1, -1/sqrt(2*1.0), 1/sqrt(2*1.0),      
//						1, -1/sqrt(2*1.0), -1/sqrt(2*1.0),  
//						1, sqrt(2*1.0), 0};
int main(){
	//����ͼ��
	GDALDataset* picture1_in;
	GDALDataset* picture2_in;
	//���ͼ��
	GDALDataset* picture_out;

	int imgXlen, imgYlen, bandNum;

	//�������ͼ��·��
	char* picture1_path = "American_MUL.bmp";
	char* picture2_path = "American_PAN.bmp";
	char* out_picture_path = "American_FUS1.bmp";

	//ע������
	GDALAllRegister();
	//��ͼ��
	picture1_in = (GDALDataset*)GDALOpenShared(picture1_path, GA_ReadOnly);
	picture2_in = (GDALDataset*)GDALOpenShared(picture2_path, GA_ReadOnly);
	//��ȡͼ�ο�ȣ��߶ȺͲ�������
	imgXlen = picture1_in->GetRasterXSize();
	imgYlen = picture1_in->GetRasterYSize();
	bandNum = picture1_in->GetRasterCount();
	//cout << imgXlen << endl << imgYlen;
	
	//ͼ���ڴ�洢
	float *R, *G, *B; 
	float *I, *H, *S;
	float* buffTmp;
	float trans_res[3][1024 * 1024];
	//����ͼ��Ŀ�Ⱥ͸߶ȷ����ڴ�
	R = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	G = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	B = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	I = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	H = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	S = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	buffTmp = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	//�������ͼ��
	picture_out = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		out_picture_path, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	
	picture1_in->GetRasterBand(1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, R, imgXlen, imgYlen, GDT_Float32, 0, 0);
	picture1_in->GetRasterBand(2)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, G, imgXlen, imgYlen, GDT_Float32, 0, 0);
	picture1_in->GetRasterBand(3)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, B, imgXlen, imgYlen, GDT_Float32, 0, 0);
	picture2_in->GetRasterBand(1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Float32, 0, 0);
	for(int i = 0; i < imgXlen * imgYlen; i++){
		H[i] = -sqrt(2.0f)/6.0f*R[i]-sqrt(2.0f)/6.0f*G[i]+sqrt(2.0f)/3.0f*B[i];
		S[i] = 1.0f/sqrt(2.0f)*R[i]-1/sqrt(2.0f)*G[i];

		R[i] = buffTmp[i]-1.0f/sqrt(2.0f)*H[i]+1.0f/sqrt(2.0f)*S[i];
        G[i] = buffTmp[i]-1.0f/sqrt(2.0f)*H[i]-1.0f/sqrt(2.0f)*S[i];
        B[i] = buffTmp[i]+sqrt(2.0f)*H[i];
	}
	picture_out->GetRasterBand(1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, R, imgXlen, imgYlen, GDT_Float32, 0, 0);
	picture_out->GetRasterBand(2)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, G, imgXlen, imgYlen, GDT_Float32, 0, 0);
	picture_out->GetRasterBand(3)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, B, imgXlen, imgYlen, GDT_Float32, 0, 0);
	CPLFree(R);
	CPLFree(G);
	CPLFree(B);
	CPLFree(I);
	CPLFree(H);
	CPLFree(S);
	CPLFree(buffTmp);
	GDALClose(picture1_in);
	GDALClose(picture2_in);
	GDALClose(picture_out);

	system("PAUSE");
	return 0;

}