#include <iostream>
#include <cmath>
using namespace std;
#include"./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

int main() {
	char* mulPath = "Mul_large.tif";
	char* panPath = "Pan_large.tif";
	char* fusPath = "American_Fus.tif";

	GDALAllRegister();

	// basic parameters
	GDALDataset *poMulDS, *poPanDS, *poFusDS;
	int imgXlen, imgYlen;
	int xBlock, yBlock;
	int i;
	float *bandR, *bandG, *bandB;
	float *bandI, *bandH, *bandS;
	float *bandP;

	// open datasets
	poMulDS = (GDALDataset*)GDALOpenShared(mulPath, GA_ReadOnly);
	poPanDS = (GDALDataset*)GDALOpenShared(panPath, GA_ReadOnly);
	imgXlen = poMulDS->GetRasterXSize();
	imgYlen = poMulDS->GetRasterYSize();

	printf(" ... ... image xlen : %d pixels ... ...\n", imgXlen);
	printf(" ... ... image ylen : %d pixles ... ...\n", imgYlen);
	printf(" ... ... Mul image band number : %d ...\n", poMulDS->GetRasterCount());
	printf(" ... ... Pan image band number : %d ...\n", poPanDS->GetRasterCount());


	poFusDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		fusPath, imgXlen, imgYlen, 3, GDT_Byte, NULL);

	xBlock = imgXlen;
	yBlock =  256;


	// allocating memory
	bandR = (float*)CPLMalloc(xBlock*yBlock * sizeof(float));
	bandG = (float*)CPLMalloc(xBlock*yBlock * sizeof(float));
	bandB = (float*)CPLMalloc(xBlock*yBlock * sizeof(float));
	bandP = (float*)CPLMalloc(xBlock*yBlock * sizeof(float));
	bandI = (float*)CPLMalloc(xBlock*yBlock * sizeof(float));
	bandH = (float*)CPLMalloc(xBlock*yBlock * sizeof(float));
	bandS = (float*)CPLMalloc(xBlock*yBlock * sizeof(float));
	for (int j = 0; j < (imgYlen / 256); j++) {


		poMulDS->GetRasterBand(1)->RasterIO(GF_Read, 0, j*yBlock, xBlock, yBlock,
			bandR, xBlock, yBlock, GDT_Float32, 0, 0);
		poMulDS->GetRasterBand(2)->RasterIO(GF_Read, 0, j*yBlock, xBlock,yBlock,
			bandG, xBlock, yBlock, GDT_Float32, 0, 0);
		poMulDS->GetRasterBand(3)->RasterIO(GF_Read, 0, j*yBlock, xBlock,yBlock,
			bandB, xBlock, yBlock, GDT_Float32, 0, 0);
		poPanDS->GetRasterBand(1)->RasterIO(GF_Read, 0, j*yBlock, xBlock,  yBlock,
			bandP, xBlock, yBlock, GDT_Float32, 0, 0);

		//poMulDS->GetRasterBand(1)->RasterIO(GF_Read,0, startY, imgXlen, blockLen, bandR, imgXlen, blockLen, GDT_Float32, 0, 0);


		for (i = 0; i < xBlock*yBlock; i++)
		{
			bandH[i] = -sqrt(2.0f) / 6.0f*bandR[i] - sqrt(2.0f) / 6.0f*bandG[i] + sqrt(2.0f) / 3.0f*bandB[i];
			bandS[i] = 1.0f / sqrt(2.0f)*bandR[i] - 1 / sqrt(2.0f)*bandG[i];

			bandR[i] = bandP[i] - 1.0f / sqrt(2.0f)*bandH[i] + 1.0f / sqrt(2.0f)*bandS[i];
			bandG[i] = bandP[i] - 1.0f / sqrt(2.0f)*bandH[i] - 1.0f / sqrt(2.0f)*bandS[i];
			bandB[i] = bandP[i] + sqrt(2.0f)*bandH[i];
		}

		poFusDS->GetRasterBand(1)->RasterIO(GF_Write, 0, j*yBlock, xBlock, yBlock,
			bandR, xBlock,  yBlock, GDT_Float32, 0, 0);
		poFusDS->GetRasterBand(2)->RasterIO(GF_Write, 0, j*yBlock, xBlock, yBlock,
			bandG, xBlock, yBlock, GDT_Float32, 0, 0);
		poFusDS->GetRasterBand(3)->RasterIO(GF_Write, 0, j*yBlock, xBlock, yBlock,
			bandB, xBlock, yBlock, GDT_Float32, 0, 0);
	}
	CPLFree(bandR);
	CPLFree(bandG);
	CPLFree(bandB);
	CPLFree(bandI);
	CPLFree(bandH);
	CPLFree(bandS);
	CPLFree(bandP);

	GDALClose(poMulDS);
	GDALClose(poPanDS);
	GDALClose(poFusDS);
	return 0;
}
 