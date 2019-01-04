#include <iostream>
#include <cstdio>
using namespace std;
#include"./gdal/gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")


int main()
{
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像w
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen,StartX,StartY,tmpXlen,tmpYlen;
	//输入图像路径
	char* srcPath = "input.jpg";
	//输出图像路径
	char* dstPath = "output.tif";
	//图像内存存储
	GByte* buffTmp,* buffTmp1;
	//图像波段数
	int i,j, bandNum;
	//注册驱动
	GDALAllRegister();
	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	 //获取图像宽度，高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	//输出获取的结果
	/*cout << "Image X Length:" << imgXlen << endl;
	cout << "Image Y Length:" << imgYlen << endl;
	cout << "Band number:" << bandNum << endl;
	//根据图像的宽度和高度分配内存

*/
buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
		//一个个波段的输入，然后一个个波段的输出
	for (i = 0; i < bandNum; i++)
		{
			poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
				0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
			poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
				0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
			cout << "... ... band " << i << "processing ... ... \n" ;
//			printf("... ... band %d processing ... ... \n", i);
		}
	//起始位置坐标
	StartX = 100;
	StartY = 100;
	//区域宽度和高度
	tmpXlen = 200;
	tmpYlen = 150;
	//分配内存
	buffTmp1 = (GByte*)CPLMalloc(tmpXlen*tmpYlen*sizeof(GByte));
	//读取红色通道缓存在buffTmp中
	poSrcDS->GetRasterBand(1)->RasterIO(GF_Read,
                                     StartX,StartY,tmpXlen,tmpYlen,buffTmp1,tmpXlen,tmpYlen,GDT_Byte,0,0);
    //遍历区域，逐像素置为255
    for(j=0;j < tmpYlen; j ++)
    {
        for(i=0;i<tmpXlen;i++)
        {
            buffTmp1[j*tmpXlen+i] = (GByte)255;
        }
    }
//数据写入poDstDS
poDstDS->GetRasterBand(1)->RasterIO(GF_Write,
                                    StartX,StartY,tmpXlen,tmpYlen,buffTmp1,tmpXlen,tmpYlen,GDT_Byte,0,0);



	//清除内存
	CPLFree(buffTmp);
    CPLFree(buffTmp1);

	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);
//	system("PAUSE");
    return 0;
}
