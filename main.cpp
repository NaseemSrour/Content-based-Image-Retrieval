/*
 * main.cpp
 *
 *  Created on: 06 Sep 2016
 *      Author: Yazan
 */

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "SPImageProc.h"
extern "C"{
#include "SPKDArray.h"
#include "SPConfig.h"
#include "SPKDTree.h"
#include "main_aux.h"
}
using namespace sp;

#define EXIT_MSG "Exiting...\n"
#define strLen 1024
#define allocationError "Allocation Error\n\n"
#define successConfig "Config Creation Successfully done\n\n"
#define successImageProc "imageProc creation Successfully done\n\n"





SPPoint** GetFeaturesOfAllImages(SPConfig config,ImageProc* imageProc,int* numOfAllFeats,int* numOfFeatsArray){
	int i,numOfFeats=0;
	char imagePath[strLen+1]={"\0"};
	char imagePathFeat[strLen+1]={"\0"};
	SP_CONFIG_MSG Configmsg;
	SPPoint** imagesFeaturePointsArray=(SPPoint**)malloc(sizeof(SPPoint*)*spConfigGetNumOfImages(config,&Configmsg));
	if(imagesFeaturePointsArray==NULL)
		return NULL;
	for (i = 0; i <spConfigGetNumOfImages(config,&Configmsg) ; ++i) {
		Configmsg=spConfigGetImagePath(imagePath,config,i);
		if(Configmsg!=SP_CONFIG_SUCCESS){
			free(imagesFeaturePointsArray);
			return NULL;
		}
		Configmsg=spConfigGetImagePathFeat(imagePathFeat,config,i);
		if(Configmsg!=SP_CONFIG_SUCCESS){
			free(imagesFeaturePointsArray);
			return NULL;
		}
		imagesFeaturePointsArray[i]=imageProc->getImageFeatures(imagePath,i,&numOfFeats);
		if(imagesFeaturePointsArray[i]==NULL){
			spDestroyimagesFeaturePointsArray(imagesFeaturePointsArray,i);
			return NULL;
		}
		numOfFeatsArray[i]=numOfFeats;
		*numOfAllFeats+=numOfFeats;
		if(!spFillFeatsFiles(imagePathFeat,i,imagesFeaturePointsArray[i],numOfFeats)){
			spDestroyimagesFeaturePointsArray(imagesFeaturePointsArray,i);
			return NULL;
		}
	}
	return imagesFeaturePointsArray;
}


int main(int argc, char **argv) {

	SP_LOGGER_MSG loggerMsg;
	SP_CONFIG_MSG Configmsg=SP_CONFIG_SUCCESS;
	char queryImagePath[strLen+1]={"\0"},imagePath[strLen+1]={"\0"};
	int i,index,numOfAllFeats=0,*numOFFeatsArray,numOfQueryFeats,*numOfApearanceForEachImgArray;
	ImageProc* imageProc;
	SPPoint *queryFeaturePoints,**imagesFeaturePointsArray;
	TreeNode root=NULL;
	char configFileName[strLen+1]={"\0"};
	SPConfig config;

	strcpy(configFileName,"spcbir.config");
	if(argc == 3){
		memset(configFileName,'\0',13);
		strncpy(configFileName,argv[2],strlen(argv[2]));
		if(strcmp(argv[1],"-c") != 0){
			printf("Invalid command line : use -c %s\n",configFileName);
			printf(EXIT_MSG);
			return -1;
		}
	}
	else if(argc>1){
		printf("Invalid command line : use -c %s\n",configFileName);
		printf(EXIT_MSG);
		return -1;
	}

	config=spConfigCreate(configFileName,&Configmsg);
	if(Configmsg==SP_CONFIG_CANNOT_OPEN_FILE){
		if(argc==3){
			printf("The configuration file %s could not be open\n", configFileName);
			printf(EXIT_MSG);
			return -1;
		}
		printf("The default configuration file spcbir.config could not be open\n");
		printf(EXIT_MSG);
		return -1;
	}
	if(config==NULL){
		printf(EXIT_MSG);
		return -1;
	}
	loggerMsg=CheckLoggerLevelAndCreateLogger(config);
	if(loggerMsg!=SP_LOGGER_SUCCESS){
		spConfigDestroy(config);
		printf("%d",loggerMsg);
		return -1;
	}
	spLoggerPrintMsg(successConfig);

	imageProc=new ImageProc(config);
	spLoggerPrintMsg(successImageProc);

	spLoggerPrintInfo("now we will build number of feats array that contains number of features for per image...\n");
	numOFFeatsArray=(int*)malloc(sizeof(int)*spConfigGetNumOfImages(config,&Configmsg));
	if(numOFFeatsArray==NULL){
		spLoggerPrintError(allocationError,__FILE__,"main",__LINE__);
		spConfigDestroy(config);
		delete imageProc;
		spLoggerDestroy();
	}
	spLoggerPrintMsg("building number of feats array successfully done\n\n");

	spLoggerPrintInfo("now we wil get the features for every image in the directory and put the in one Array according to the extraction mode...\n");
	if(spConfigIsExtractionMode(config,&Configmsg)){
		spLoggerPrintMsg("extraction mode is true then we get new features\n\n");
		imagesFeaturePointsArray=GetFeaturesOfAllImages(config,imageProc,&numOfAllFeats,numOFFeatsArray);
	}
	else{
		spLoggerPrintMsg("extraction mode is false then we get the features from the feats files\n");
		imagesFeaturePointsArray=GetFeaturesFromFeatsFiles(config,&numOfAllFeats,numOFFeatsArray);
	}
	if(imagesFeaturePointsArray==NULL){
		spLoggerPrintError(allocationError,__FILE__,"GetFeaturesOfAllImages()",__LINE__);
		spConfigDestroy(config);
		delete imageProc;
		spLoggerDestroy();
		free(numOFFeatsArray);
		return -1;
	}
	spLoggerPrintMsg("Getting the features and put it in one array is done successfully\n\n");

	//Build KDTree
	spLoggerPrintInfo("now we will build the KDtree According to features that we have....\n");
	root=BuildKDTree(config,imagesFeaturePointsArray,numOfAllFeats,numOFFeatsArray);
	if(root==NULL){
		spLoggerPrintError(allocationError,__FILE__,"BuildKDTree()",__LINE__);
		spConfigDestroy(config);
		delete imageProc;
		spLoggerDestroy();
		free(numOFFeatsArray);
		return -1;
	}
	spLoggerPrintMsg("Bulidind KDtree is done successfully\n\n");

	spLoggerPrintInfo("now entering images or terminate...\n");
	while(true){
		printf("Enter a query image or <> to terminate:\n");
		fflush(NULL);
		scanf("%s",queryImagePath);
		if((strcmp(queryImagePath,"<>"))==0){
			spLoggerPrintMsg("terminating is done successfully\n\n");
			spConfigDestroy(config);
			delete imageProc;
			spLoggerDestroy();
			if(root!=NULL)
				spDestroyTree(root);
			free(numOFFeatsArray);
			printf(EXIT_MSG);
			return 0;
		}

		//GetQueryFeatures
		spLoggerPrintInfo("now we will get the query image features....\n");
		queryFeaturePoints=imageProc->getImageFeatures(queryImagePath,spConfigGetNumOfImages(config,&Configmsg),&numOfQueryFeats);
		if(queryFeaturePoints==NULL){
			spLoggerPrintError(allocationError,__FILE__,"getImageFeatures()",__LINE__);
			spDestroyTree(root);
			spConfigDestroy(config);
			free(numOFFeatsArray);
			delete imageProc;
			spLoggerDestroy();
			return -1;
		}
		spLoggerPrintMsg("getting query image features successfully done\n\n");

		//KnnSearch and Fill numOfApearanceForEachImgArray
		spLoggerPrintInfo("now we do the KNN Search and get array that contains how many times each image appears for each feautre in the query...\n");
		numOfApearanceForEachImgArray=KnnSearchAndFillNumOfAppearenceForEachImg(config,
				numOfQueryFeats,root,queryFeaturePoints);
		if(numOfApearanceForEachImgArray==NULL){
			spLoggerPrintError(allocationError,__FILE__,"KnnSearchAndFillNumOfAppearenceForEachImg()",__LINE__);
			spDestroyTree(root);
			spConfigDestroy(config);
			free(numOFFeatsArray);
			delete imageProc;
			spLoggerDestroy();
			return -1;
		}
		spLoggerPrintMsg("the KNN Search and getting the number of appearance for ech Image array don successfully\n\n");

		//find images with highest num of appearence and show results
		spLoggerPrintInfo("now we show the images that have th highest numer of appearance...\n");
		for (i = 0; i < spConfigGetNumOFSimilarImages(config,&Configmsg); ++i) {
			index=GetMaxIndexInArray(numOfApearanceForEachImgArray,spConfigGetNumOfImages(config,&Configmsg));
			numOfApearanceForEachImgArray[index]=-1;
			Configmsg=spConfigGetImagePath(imagePath,config,index);
			if(spConfigMinimalGui(config,&Configmsg)){
				imageProc->showImage(imagePath);
			}
			else{
				if(i==0)
					printf("Best candidates for- %s are\n",queryImagePath);
				printf("%s\n",imagePath);
			}
		}
		spLoggerPrintMsg("showing results is done successfully\n\n");
		free(numOfApearanceForEachImgArray);
		for (i = 0; i < numOfQueryFeats; ++i) {
			spPointDestroy(queryFeaturePoints[i]);
		}
		free(queryFeaturePoints);
		spLoggerPrintInfo("now enter another image or terminate...\n");

	}
}


