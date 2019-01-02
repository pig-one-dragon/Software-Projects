#include<iostream>
#include<string>
#include<sstream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")



int main()
{
    char* mulPath = "American_MUL.bmp";
    char* panPath = "American_PAN.bmp";
    char* outPath = "American.tif";
    //图像的宽度和高度
	int imgXlen, imgYlen;
	//图像波段数
	int bandNum;
    //输入图像
	GDALDataset* mulPic,*panPic;
	//输出图像
	GDALDataset* outPic;
	//图像内存存储
	float *buffR,*buffG,*buffB,*buffP,*buffH,*buffS;
	//注册驱动
	GDALAllRegister();

	//打开图像
	mulPic = (GDALDataset*)GDALOpenShared(mulPath, GA_ReadOnly);
	panPic = (GDALDataset*)GDALOpenShared(panPath, GA_ReadOnly);
	//获取图像宽度，高度，波段数
	imgXlen = mulPic->GetRasterXSize();
	imgYlen = mulPic->GetRasterYSize();
	bandNum = mulPic->GetRasterCount();
    //创建输出图像
	outPic = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(outPath,imgXlen,imgYlen,bandNum,GDT_Byte,NULL);
  	//输出获取的结果
	cout << "IMG  X Length:" << imgXlen << endl;
	cout << "IMG  Y Length:" << imgYlen << endl;
	cout << "Band Number:" << bandNum << endl;
	//开辟缓存空间
    buffR = (float *)CPLMalloc(imgXlen * imgYlen * sizeof(float));
    buffG = (float *)CPLMalloc(imgXlen * imgYlen * sizeof(float));
    buffB = (float *)CPLMalloc(imgXlen * imgYlen * sizeof(float));
    buffP = (float *)CPLMalloc(imgXlen * imgYlen * sizeof(float));
    buffH = (float *)CPLMalloc(imgXlen * imgYlen * sizeof(float));
    buffS = (float *)CPLMalloc(imgXlen * imgYlen * sizeof(float));
    //读取多光谱图像的R G B，全色图的P
    mulPic->GetRasterBand(1)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffR,imgXlen,imgYlen,GDT_Float32,0,0);
    mulPic->GetRasterBand(2)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffG,imgXlen,imgYlen,GDT_Float32,0,0);
    mulPic->GetRasterBand(3)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffB,imgXlen,imgYlen,GDT_Float32,0,0);
    panPic->GetRasterBand(1)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffP,imgXlen,imgYlen,GDT_Float32,0,0);
    //对每个像素进行处理
    //先计算出H S，然后转换为R G B，转换时P代替其中的I，I被代替所以也没有必要计算
    for(int i = 0 ;i < imgXlen * imgYlen ;i++){
        buffH[i] = -sqrt(2.0f)/6.0f * buffR[i] - sqrt(2.0f)/6.0f*buffG[i] + sqrt(2.0f)/3.0f*buffB[i];
        buffS[i] = 1.0f/sqrt(2.0f)*buffR[i] - 1/sqrt(2.0f)*buffG[i];

        buffR[i] = buffP[i] - 1.0f/sqrt(2.0f) * buffH[i] + 1.0f/sqrt(2.0f) * buffS[i];
        buffG[i] = buffP[i] - 1.0f/sqrt(2.0f) * buffH[i] - 1.0f/sqrt(2.0f) * buffS[i];
        buffB[i] = buffP[i] + sqrt(2.0f) * buffH[i];
    }
    //写入图像
    outPic->GetRasterBand(1)->RasterIO(GF_Write,0,0,imgXlen,imgYlen,buffR,imgXlen,imgYlen,GDT_Float32,0,0);
    outPic->GetRasterBand(2)->RasterIO(GF_Write,0,0,imgXlen,imgYlen,buffG,imgXlen,imgYlen,GDT_Float32,0,0);
    outPic->GetRasterBand(3)->RasterIO(GF_Write,0,0,imgXlen,imgYlen,buffB,imgXlen,imgYlen,GDT_Float32,0,0);
    //释放缓存
    CPLFree(buffR);
    CPLFree(buffG);
    CPLFree(buffB);
    CPLFree(buffP);
    CPLFree(buffH);
    CPLFree(buffS);
    //关闭图像
    GDALClose(panPic);
    GDALClose(mulPic);
    GDALClose(outPic);

	return 0;
}
