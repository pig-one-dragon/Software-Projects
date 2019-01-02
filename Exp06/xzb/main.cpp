#include<iostream>
#include<string>
#include<sstream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")
void dividedByBlock(int tx,int ty,int imgXlen,GDALDataset* mulPic,GDALDataset* panPic,GDALDataset* outPic,float* buffR,float* buffG,float* buffB,float* buffP,float* buffH,float* buffS){
    char* outPath = "Out_large.tif";
    outPic = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(outPath,imgXlen,imgYlen,bandNum,GDT_Byte,NULL);
    //分块处理转换
	for(int i = 0;i < imgXlen;i+=256){
        for(int j = 0 ; j < imgYlen;j+=256){
            if(i + ty > imgXlen || j + tx > imgYlen)
                break;
            printf("图像的x:%d  y:%d\n",i,j);
            //读入缓存
            mulPic->GetRasterBand(1)->RasterIO(GF_Read,i,j,tx,ty,buffR,tx,ty,GDT_Float32,0,0);
            mulPic->GetRasterBand(2)->RasterIO(GF_Read,i,j,tx,ty,buffG,tx,ty,GDT_Float32,0,0);
            mulPic->GetRasterBand(3)->RasterIO(GF_Read,i,j,tx,ty,buffB,tx,ty,GDT_Float32,0,0);
            panPic->GetRasterBand(1)->RasterIO(GF_Read,i,j,tx,ty,buffP,tx,ty,GDT_Float32,0,0);
            //转换处理
            for(int k = 0;k < tx * ty ;k++){
                buffH[k] = -sqrt(2.0f)/6.0f * buffR[k] - sqrt(2.0f)/6.0f*buffG[k] + sqrt(2.0f)/3.0f*buffB[k];
                buffS[k] = 1.0f/sqrt(2.0f)*buffR[k] - 1/sqrt(2.0f)*buffG[k];

                buffR[k] = buffP[k] - 1.0f/sqrt(2.0f) * buffH[k] + 1.0f/sqrt(2.0f) * buffS[k];
                buffG[k] = buffP[k] - 1.0f/sqrt(2.0f) * buffH[k] - 1.0f/sqrt(2.0f) * buffS[k];
                buffB[k] = buffP[k] + sqrt(2.0f) * buffH[k];
            }
            //写入新图像
            outPic->GetRasterBand(1)->RasterIO(GF_Write,i,j,tx,ty,buffR,tx,ty,GDT_Float32,0,0);
            outPic->GetRasterBand(2)->RasterIO(GF_Write,i,j,tx,ty,buffG,tx,ty,GDT_Float32,0,0);
            outPic->GetRasterBand(3)->RasterIO(GF_Write,i,j,tx,ty,buffB,tx,ty,GDT_Float32,0,0);
        }
	}
}
//分行处理
void dividedByLine(int tx,int ty,int imgXlen, GDALDataset* mulPic,GDALDataset* panPic,GDALDataset* outPic,float* buffR,float* buffG,float* buffB,float* buffP,float* buffH,float* buffS){
    char* outPath = "Out_large2.tif";
    outPic = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(outPath,imgXlen,imgYlen,bandNum,GDT_Byte,NULL);

    //分行处理  256行一组
    for(int i = 0 ; i < imgXlen;i+=256){
        printf("图像的x:%d  y:%d\n",i,0);
        if(i + tx > imgXlen)
            break;


        //读入缓存
        mulPic->GetRasterBand(1)->RasterIO(GF_Read,i,0,tx,ty,buffR,tx,ty,GDT_Float32,0,0);
        mulPic->GetRasterBand(2)->RasterIO(GF_Read,i,0,tx,ty,buffG,tx,ty,GDT_Float32,0,0);
        mulPic->GetRasterBand(3)->RasterIO(GF_Read,i,0,tx,ty,buffB,tx,ty,GDT_Float32,0,0);
        panPic->GetRasterBand(1)->RasterIO(GF_Read,i,0,tx,ty,buffP,tx,ty,GDT_Float32,0,0);
        //转换处理
        for(int k = 0;k < tx * ty ;k++){
            buffH[k] = -sqrt(2.0f)/6.0f * buffR[k] - sqrt(2.0f)/6.0f*buffG[k] + sqrt(2.0f)/3.0f*buffB[k];
            buffS[k] = 1.0f/sqrt(2.0f)*buffR[k] - 1/sqrt(2.0f)*buffG[k];

            buffR[k] = buffP[k] - 1.0f/sqrt(2.0f) * buffH[k] + 1.0f/sqrt(2.0f) * buffS[k];
            buffG[k] = buffP[k] - 1.0f/sqrt(2.0f) * buffH[k] - 1.0f/sqrt(2.0f) * buffS[k];
            buffB[k] = buffP[k] + sqrt(2.0f) * buffH[k];
        }
        //写入新图像
        outPic->GetRasterBand(1)->RasterIO(GF_Write,i,0,tx,ty,buffR,tx,ty,GDT_Float32,0,0);
        outPic->GetRasterBand(2)->RasterIO(GF_Write,i,0,tx,ty,buffG,tx,ty,GDT_Float32,0,0);
        outPic->GetRasterBand(3)->RasterIO(GF_Write,i,0,tx,ty,buffB,tx,ty,GDT_Float32,0,0);
    }
}
int main()
{
    char* mulPath = "Mul_large.tif";
    char* panPath = "Pan_large.tif";
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

	//输出获取的结果
	cout << "IMG  X Length:" << imgXlen << endl;
	cout << "IMG  Y Length:" << imgYlen << endl;
	cout << "Band Number:" << bandNum << endl;
	int tx = 256,ty = imgYlen;
	//开辟内存空间
    buffR = (float *)CPLMalloc(tx * ty * sizeof(float));
    buffG = (float *)CPLMalloc(tx * ty * sizeof(float));
    buffB = (float *)CPLMalloc(tx * ty * sizeof(float));
    buffP = (float *)CPLMalloc(tx * ty * sizeof(float));
    buffH = (float *)CPLMalloc(tx * ty * sizeof(float));
    buffS = (float *)CPLMalloc(tx * ty * sizeof(float));
    cout << "you can choose the way to create the picture" << endl;
    cout << "1、 divided by line" << endl;
    cout << "2、 divided by block" << endl;
    cin >> choose;
    //选择处理方式
    if(choose == 1){
        dividedByLine(tx,ty,imgXlen,mulPic,panPic,outPic,buffR,buffG,buffB,buffP,buffH,buffS);
    }else if(choose == 2){
        dividedByBlock(tx,ty,imgXlen,mulPic,panPic,outPic,buffR,buffG,buffB,buffP,buffH,buffS);
    }
    //释放缓存
    CPLFree(buffR);
    CPLFree(buffG);
    CPLFree(buffB);
    CPLFree(buffP);
    CPLFree(buffH);
    CPLFree(buffS);
    //关闭数据集
    GDALClose(panPic);
    GDALClose(mulPic);
    GDALClose(outPic);

	return 0;
}
