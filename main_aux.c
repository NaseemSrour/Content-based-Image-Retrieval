/*
 * main_aux.c
 *
 *  Created on: 08 Sep 2016
 *      Author: Yazan
 */

#include "main_aux.h"
#include <stdbool.h>
#define strLen 1024
#define allocationError "Allocation Error\n\n"

SP_LOGGER_MSG CheckLoggerLevelAndCreateLogger(SPConfig config){
	char loggerFileName[strLen+1];
	SP_LOGGER_LEVEL loggerLevel;
	spConfigGetLoggerLevel(config,&loggerLevel);
	spConfigGetLoggerFileName(config,loggerFileName);
	if(strcmp(loggerFileName,"stdout")!=0)
		return spLoggerCreate(loggerFileName,loggerLevel);
	else
		return spLoggerCreate(NULL,loggerLevel);
}


TreeNode BuildKDTree(SPConfig config,SPPoint** featurePoints,int numOfAllFeats,int* numOfFeatsArray){
	int i,j,s=0;
	TreeNode root;
	SP_CONFIG_MSG msg;
	SPPoint* allFeaturesArray=(SPPoint*)malloc(sizeof(SPPoint)*numOfAllFeats);
	if(allFeaturesArray==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		return NULL;
	}
	for (i = 0; i < spConfigGetNumOfImages(config,&msg); ++i) {
		for (j = 0; j < numOfFeatsArray[i]; ++j) {
			allFeaturesArray[s]=spPointCopy(featurePoints[i][j]);
			s++;
		}
	}
	for (i = 0; i < spConfigGetNumOfImages(config,&msg); ++i) {
		for (j = 0; j < numOfFeatsArray[i]; ++j) {
			spPointDestroy(featurePoints[i][j]);
		}
		free(featurePoints[i]);
	}
	free(featurePoints);
	root=SPKDTreeCreate(allFeaturesArray,numOfAllFeats,config);
	for (i = 0; i < numOfAllFeats; ++i) {
		spPointDestroy(allFeaturesArray[i]);
	}
	free(allFeaturesArray);
	return root;
}

int GetMaxIndexInArray(int* array,int size){
	int i,max=-1,index;
	for (i = 0; i < size; ++i) {
		if(array[i]>max){
			max=array[i];
			index=i;
		}
	}
	return index;
}


//bulid number of appearance for each img array
int* KnnSearchAndFillNumOfAppearenceForEachImg(SPConfig config,int numOfQueryFeats,TreeNode root,SPPoint* queryFeaturePoints){
	int i,j,indexElement;
	SPBPQueue queue;
	SPList list;
	SP_CONFIG_MSG msg=SP_CONFIG_SUCCESS;
	SPListElement listElement;
	int* numOfApearanceForEachImgArray=(int*)malloc(sizeof(int)*spConfigGetNumOfImages(config,&msg));
	if(numOfApearanceForEachImgArray==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		return NULL;
	}
	for (i = 0; i < spConfigGetNumOfImages(config,&msg); ++i) {
		numOfApearanceForEachImgArray[i]=0;
	}
	for (i = 0; i < numOfQueryFeats; ++i) {
		queue=kNearestNeighborsSearch(root,queryFeaturePoints[i],spConfigGetKNN(config,&msg));
		if(queue==NULL){
			free(numOfApearanceForEachImgArray);
			return NULL;
		}
		list=spBPQueueGetList(queue);
		if(list==NULL){
			spBPQueueDestroy(queue);
			free(numOfApearanceForEachImgArray);
			return NULL;
		}
		listElement=spListGetFirst(list);
		if(listElement==NULL){
			spBPQueueDestroy(queue);
			free(numOfApearanceForEachImgArray);
			return NULL;
		}
		for (j = 0; j < spConfigGetKNN(config,&msg); ++j) {
			indexElement=spListElementGetIndex(listElement);
			numOfApearanceForEachImgArray[indexElement]++;
			listElement=spListGetNext(list);
		}
		spBPQueueDestroy(queue);
	}
	return numOfApearanceForEachImgArray;

}


bool spFillFeatsFiles(char* imagePathFeat,int index ,SPPoint* pointsArray,int numOfFeats){
	int i,j,dim;
	remove(imagePathFeat);
	FILE* file=fopen(imagePathFeat,"w");
	if(file==NULL){
		return false;
	}
	dim=spPointGetDimension(pointsArray[0]);
	fprintf(file,"FeatsNumber= %d\n",numOfFeats);
	fprintf(file,"ImageIndex= %d\n",index);
	fprintf(file,"Dimension= %d\n",dim);
	fprintf(file,"FeatureArray: data\n");
	for (i = 0; i < numOfFeats; ++i) {
		for (j = 0; j < dim; ++j) {
			fprintf(file,"%lf ",spPointGetAxisCoor(pointsArray[i],j));
		}
		fprintf(file,"\n");
	}
	fclose(file);
	return true;
}


void spDestroyimagesFeaturePointsArray(SPPoint** imagesFeaturePointsArray,int size){
	int j;
	for (j = 0; j < size; ++j) {
		spKDArrayDestroyPoints(imagesFeaturePointsArray[j],j);
	}
	free(imagesFeaturePointsArray);
}


SPPoint** GetFeaturesFromFeatsFiles(SPConfig config,int* numOfAllFeats,int* numOfFeatsArray){
	int i,n,j,featsNumber,pointIndex,pointdim,fileReturnNumber;
	double* pointData;
	double x;
	SP_CONFIG_MSG Configmsg=SP_CONFIG_SUCCESS;
	FILE* file;
	SPPoint point;
	char tmp[100];
	char imagePathFeat[strLen+1]={"\0"};
	char line[strLen+1]={"\0"};
	SPPoint** imagesFeaturePointsArray=(SPPoint**)malloc(sizeof(SPPoint*)*spConfigGetNumOfImages(config,&Configmsg));
	if(imagesFeaturePointsArray==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		return NULL;
	}
	for (i = 0; i < spConfigGetNumOfImages(config,&Configmsg); ++i) {
		Configmsg=spConfigGetImagePathFeat(imagePathFeat,config,i);
		if(Configmsg!=SP_CONFIG_SUCCESS){
			spDestroyimagesFeaturePointsArray(imagesFeaturePointsArray,i);
			return NULL;
		}
		file=fopen(imagePathFeat,"r");
		if(file==NULL){
			free(imagesFeaturePointsArray);
			return NULL;
		}
		for (j = 0; j < 4; ++j) {
			fscanf(file,"%s%d",tmp,&fileReturnNumber);
			if(strcmp(tmp,"FeatsNumber=")==0){
				featsNumber=fileReturnNumber;
				*numOfAllFeats+=featsNumber;
				numOfFeatsArray[i]=featsNumber;
			}
			if(strcmp(tmp,"ImageIndex=")==0)
				pointIndex=fileReturnNumber;
			if(strcmp(tmp,"Dimension=")==0)
				pointdim=fileReturnNumber;
			fgets(line, 1025, file);
		}
		imagesFeaturePointsArray[i]=(SPPoint*)malloc(sizeof(SPPoint)*featsNumber);
		if(imagesFeaturePointsArray[i]==NULL){
			spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
			spDestroyimagesFeaturePointsArray(imagesFeaturePointsArray,i);
			return NULL;
		}
		for (n = 0; n < featsNumber; ++n) {
			pointData=(double*)malloc(sizeof(double)*pointdim);
			if(pointData==NULL){
				spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
				spDestroyimagesFeaturePointsArray(imagesFeaturePointsArray,i);
				return NULL;
			}
			for (j = 0; j < pointdim; ++j) {
				fscanf(file,"%lf",&x);
				pointData[j]=x;
			}
			point=spPointCreate(pointData,pointdim,pointIndex);
			free(pointData);
			imagesFeaturePointsArray[i][n]=spPointCopy(point);
			spPointDestroy(point);
			fgets(line, 1025, file);
		}
		fclose(file);

	}
	return imagesFeaturePointsArray;
}


