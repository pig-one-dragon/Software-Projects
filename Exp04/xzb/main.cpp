#include<iostream>
#include<string>
#include<sstream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

//卷积核

double ker1[3][3] = {0,1,0,
                    1,1,1,
                    0,1,0};
double ker2[5][5] = {1,0,0,0,0,
                    0,1,0,0,0,
                    0,0,1,0,0,
                    0,0,0,1,0,
                    0,0,0,0,1};
double ker3[3][3] = {-1,-1,-1,
                    -1, 8,-1,
                    -1,-1,-1};
double ker4[3][3] = {-1,-1,-1,
                    -1, 9,-1,
                    -1,-1,-1};
double ker5[3][3] = {-1,-1, 0,
                    -1, 0, 1,
                     0, 1, 1};
double ker6[5][5] = {0.0120,0.1253,0.2736,0.1253,0.0120,
                     0.1253,1.3054,2.8514,1.3054,0.1253,
                     0.2736,2.8514,6.2279,2.8514,0.2736,
                     0.1253,1.3054,2.8514,1.3054,0.1253,
                     0.0120,0.1253,0.2736,0.1253,0.0120};

///卷积图像处理
/**
图像原路径  图像目标路径  选择卷积核 卷积核长度
*/
int juanji(char* srcPath,char* dstPath,int s,int l){
    //图像的宽度和高度
	int imgXlen, imgYlen;
	//图像波段数
	int i, bandNum;
    //乘积参数，乘积之和
    double z,t,*temp;
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	//图像内存存储
	GByte *buffTmp[2];
	//注册驱动
	GDALAllRegister();
	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
  	//获取图像宽度，高度，波段数
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	//输出获取的结果
	cout << "IMG  X Length:" << imgXlen << endl;
	cout << "IMG  Y Length:" << imgYlen << endl;
	cout << "Band Number:" << bandNum << endl;

	//根据图像的宽度和高度分配内存
	for(i = 0 ; i < 2;i++){
        buffTmp[i] = (GByte *) CPLMalloc(imgXlen * imgYlen * sizeof(GByte));
	}
	//创建输出图像
    poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
                            dstPath,imgXlen,imgYlen,bandNum,GDT_Byte,NULL);
    //初始化
    for(i = 0 ; i < imgYlen ; i++){
        for(int j = 0;j < imgXlen ;j++){
            buffTmp[0][i * imgXlen + j] = buffTmp[1][i * imgXlen + j] = 0;
        }
    }
    //分波段处理图像
    for(i = 1 ; i <= bandNum ; i++){
        //取出该波段的像素
        poSrcDS->GetRasterBand(i)->RasterIO(GF_Read,
                                               0,0,imgXlen,imgYlen,buffTmp[0],imgXlen,imgYlen,GDT_Byte,0,0);
        //计算卷积
        for(int j = l / 2;j < imgYlen - 1;j++){
            for(int k = l / 2; k < imgXlen - 1 ; k++){
                //对各个像素计算
                t = 0;
                z = 1;
                for(int q = 0;q < l;q++){
                    for(int p = 0 ; p < l;p++){
                        //选择核
                        if(s == 1){
                            temp = ker1[0];
                            z = 0.2;
                        }
                        if(s == 2){
                            temp = ker2[0];
                            z = 0.2;
                        }
                        if(s == 3){
                            temp = ker3[0];
                        }
                        if(s == 4){
                            temp = ker4[0];
                        }
                        if(s == 5){
                            temp = ker5[0];
                        }
                        if(s == 6){
                            temp = ker6[0];
                            z = 0.04;
                        }
                        t +=(double) buffTmp[0][( j + q - l/2) * imgXlen + ( k + p - l/2)] * temp[q*l+p];
                    }
                }
                //浮雕 +128
                if(s == 5){
                    t += 128;
                }
                //乘上参数
                t *= z;
                //超过数值范围的处理
                if(t < 0)
                    t = 0;
                else if(t > 255)
                    t = 255;
                //赋值给新图缓存区
                buffTmp[1][j * imgXlen + k] = (GByte)t;
            }
        }
        //将计算好的像素填充图片内
        poDstDS->GetRasterBand(i)->RasterIO(GF_Write,
            0,0,imgXlen,imgYlen,buffTmp[1],imgXlen,imgYlen,GDT_Byte,0,0);
        printf("... ... band %d processing ... ...\n",i);
    }
    //清除内存
	for(i = 0;i < 2;i++)
        CPLFree(buffTmp[i]);
    //关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);
    return 1;
}

int main()
{
	//输入图像路径
	char* srcPath = "lena.jpg";
	//输出图像的路径
	string dp = "lena";
    string dp2 = ".tif";
    string dp3 ;
    std::stringstream ss;
    //存储每个核的维数
    int ll[] = {0,3,5,3,3,3,5};
    printf("welcome!\n");
    //循环x形成不同输出文件名
    for( int x = 1;x <= 6;x++){
        //拼接输出文件名
        ss.clear();
        ss << dp << x << dp2;
        ss >> dp3;
        cout << dp3 << endl;
        if(juanji(srcPath,&dp3[0],x,ll[x])){
            //回显提示正确
            cout << dp3 << "  success!"  << endl <<  endl;
        }else{
            cout << dp3 << "failed." << endl << endl;
        }
    }
	return 0;
}
