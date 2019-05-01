/*
 * SPKDArray.c
 *
 *  Created on: 01 Sep 2016
 *      Author: Yazan
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "SPKDArray.h"

#define allocationError "Allocation Error\n\n"

struct sp_KDArray_t {

	int size; //number of features
	SPPoint* pointsArray; // features array
	int** Sorted2Darray; // features soretd according to each dimension

};

struct sp_point_helper_sort {
	double value;
	int index;
};

//Sorting Method//
int asc(const void* pa, const void* pb) {
	SPPointHelperSort* cpa = (SPPointHelperSort*) pa;
	SPPointHelperSort* cpb = (SPPointHelperSort*) pb;
	if ((*cpa)->value > (*cpb)->value)
		return 1;
	else if ((*cpa)->value < (*cpb)->value)
		return -1;
	else
		return 0;
}

/** Helper Method for CreateKDArray
 *
 * check tha validation of the input
 * @param arr - points array
 * @param size - array length
 * @return true if the input is valid, or false otherwise.
 */
bool spKDArrayIsInputValid(SPPoint* arr, int size) {
	int i;
	for (i = 0; i < size; ++i) {
		if (arr[i] == NULL)
			return false;
	}
	return true;
}

/** Helper Method for CreateKDArray
 *
 * free all allocation memory in the points array.
 * @param points - points array
 * @param size - array length
 */
void spKDArrayDestroyPoints(SPPoint* points, int size){
	int i;
	for(i =0;i<size;i++){
		spPointDestroy(points[i]);
	}
	free(points);
}

/** Helper Method for CreateKDArray
 *
 * Copy all the points in the points array
 * @param points - points array
 * @param size - array length
 * @return new copy of points array, or NULL otherwise.
 *
 */
SPPoint* spKDArrayCopyPoints(SPPoint* points, int size){
	int i;
	SPPoint* res = (SPPoint*)malloc(sizeof(*res)*size);
	if(res==NULL){
		return NULL;
	}
	for(i = 0; i< size;i++){
		res[i] = spPointCopy(points[i]);
		if(res[i]==NULL){
			spKDArrayDestroyPoints(res,i);
			return NULL;
		}
	}
	return res;
}

/** Helper Method for CreateKDArray
 *
 * free all allocation memory in the Sorted2Darray array.
 * @param array - two dimensional array of indexes(sorted).
 * @param size - array length
 */
void spKDArrayDestroySorted2Darray(int** array, int dimension){
	int i;
	for(i =0;i<dimension;i++){
		free(array[i]);
	}
	free(array);
}

/** Helper Method for CreateKDArray
 *
 * Create a two dimensional array of indexes Sorted2Darray by allocatinga memory.
 * @param dimension - number of rows
 * @param size - number of columns
 * @return two dimensional array of indexes Sorted2Darray, ao NULL otherwise.
 *
 */
int** spKDArrayCreateSorted2Darray(int dimension,int size){
	int i=0;
	int** arr=(int**)malloc(sizeof(int*)*dimension);
	if(arr==NULL)
		return NULL;
	for (i = 0; i < dimension; ++i) {
		 arr[i]= (int*) malloc(sizeof(int) * size);
		 if(arr[i]==NULL){
			 spKDArrayDestroySorted2Darray(arr,i);
			 return NULL;
		 }
	}
	return arr;
}

/** Helper Method for CreateKDArray
 *
 * free all allocation memory in SortArrayHelper.
 * @param SortArrayHelper - array that contains SPPointHelperSort objects
 * @param size - array length
 */
void spKDArrayDestroyPointHelperArray(SPPointHelperSort* SortArrayHelper, int size){
	int i;
	for(i =0;i<size;i++){
		free(SortArrayHelper[i]);
	}
	free(SortArrayHelper);
}

/** Helper Method for CreateKDArray
 *
 * Create a PointHelperArray array that contains SPPointHelperSort objects.
 * @param size - array length
 * @return a PointHelperArray array, or NULL otherwise.
 */
SPPointHelperSort* spKDArrayCreatePointHelperArray(int size){
	int i=0;
	SPPointHelperSort* SortArrayHelper = (SPPointHelperSort*) malloc(sizeof(*SortArrayHelper) * size);
	if(SortArrayHelper==NULL)
		return NULL;
	for (i = 0; i < size; ++i) {
		SortArrayHelper[i]= (SPPointHelperSort) malloc(sizeof(*SortArrayHelper[i]));
		 if(SortArrayHelper[i]==NULL){
			 spKDArrayDestroyPointHelperArray(SortArrayHelper,i);
			 return NULL;
		 }
	}
	return SortArrayHelper;
}

SPKDArray spKDArrayCreate(SPPoint* arr, int size) {
	if (arr == NULL || size <= 0)
		return NULL;
	int i,j;
	SPPointHelperSort* SortArrayHelper;
	if(!spKDArrayIsInputValid(arr,size)){
		return NULL;
	}
	int d = spPointGetDimension(arr[0]); //number of coordinates
	//InitKDArray
	SPKDArray KDArray = (SPKDArray) malloc(sizeof(*KDArray));
	if (KDArray == NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		return NULL;
	}
	KDArray->size = size;
	KDArray->pointsArray = spKDArrayCopyPoints(arr,size);
	if(KDArray->pointsArray==NULL){
		free(KDArray);
		return NULL;
	}
	//create 2D Array
	KDArray->Sorted2Darray = spKDArrayCreateSorted2Darray(d,size);
	if (KDArray->Sorted2Darray == NULL) {
		//Destroy Points Array
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		spKDArrayDestroyPoints(KDArray->pointsArray,size);
		free(KDArray);
		return NULL;
	}
	SortArrayHelper = spKDArrayCreatePointHelperArray(size);
	if (SortArrayHelper == NULL) {
		//Destroy Points Array and Sorted2Darray
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		spKDArrayDestroyPoints(KDArray->pointsArray,size);
		spKDArrayDestroySorted2Darray(KDArray->Sorted2Darray,size);
		free(KDArray);
		return NULL;
	}
	//fill SortArrayHelper, sort and fill Sorted2Darray
	for (i = 0; i < d; ++i) {
		for (j = 0; j < size; ++j) {
			//fill SortArrayHelper
			SortArrayHelper[j]->value = spPointGetAxisCoor(arr[j],i);
			SortArrayHelper[j]->index = j;
		}
		qsort(SortArrayHelper, size, sizeof(SPPointHelperSort), asc);
		for (j = 0; j < size; ++j) {
			(KDArray->Sorted2Darray)[i][j] = ((SortArrayHelper[j])->index);
		}
	}
	//destroy SortArrayHelper
	spKDArrayDestroyPointHelperArray(SortArrayHelper,size);
	return KDArray;
}


/***************************split***************************/

/** KDArray Split Helper Method
 *
 * check tha validation of the input
 * @param kdArr - KDArray Object
 * @param coor - coordinate
 * @return true if the input is valid, or false otherwise.
 */
bool spKDArraySpliteInputValid(SPKDArray kdArr, int coor){
	if(kdArr==NULL || coor<0)
		return false;
	if(kdArr->pointsArray==NULL || kdArr->size<0 || kdArr->Sorted2Darray==NULL)
		return false;
	return true;
}

/** KDArray Split Helper Method
 *
 * free all allocation memory in Xarray.
 * @param Xarray - array contains 0 if the point belongs to the left,
 * 		or 1 if it belongs to the right.
 *
 */
void spKDArrayDestroytXArray(int* Xarray){
	free(Xarray);
}

/** KDArray Split Helper Method
 *
 * Create XArray array by allocation a memory.
 * @param size - array length
 * @return new Xarray,or NULL otherwise.
 *
 */
int* spKDArrayCreateXArray(int size){
	int* Xarray=(int*)malloc(sizeof(int)*size);
	if(Xarray==NULL)
		return NULL;
	return Xarray;
}

/** KDArray Split Helper Method
 *
 * free all alllocation memory in the right and the left KdArray
 * @param KD2Array - SPKDArray array with length 2 that contains left KDArray and right KDArray.
 * @param index - number of sides we have to destroy
 *
 */
void spKDArrayDestroySidesKDarrays(SPKDArray* KD2Array, int index){
	int i;
	for (i = 0; i < index; ++i) {
		free(KD2Array[i]);
	}
	free(KD2Array);
}

/** KDArray Split Helper Method
 *
 * Create SPKDArray array with length 2 that contains left KDArray and right KDArray.
 * @return new SPKDArray array with lef and right KdArray
 */
SPKDArray* spKDArrayCreateSidesKDarrays(){
	int i;
	SPKDArray* KD2Array=(SPKDArray*)malloc(sizeof(SPKDArray)*2);
	if(KD2Array==NULL)
		return NULL;
	for (i = 0; i < 2; ++i) {
		KD2Array[i]=(SPKDArray)malloc(sizeof(*KD2Array[i]));
		if(KD2Array[i]==NULL){
			spKDArrayDestroySidesKDarrays(KD2Array,i);
			return NULL;
		}
	}
	return KD2Array;
}

/** KDArray Split Helper Method
 *
 * free all allocation Memory in the mapsArray.
 * @param maps - two dimensional MapsArray, left map array and right map array.
 * @param index - number of maps we have to Destroy
 */
void spKDArrayDestroyMapsArrays(int** maps,int index){
	int i;
	for (i = 0; i < index; ++i) {
		free(maps[i]);
	}
	free(maps);
}

/** KDArray Split Helper Method
 *
 * Create two dimensional MapsArray (left map array and right map array).
 * @param kdArrSize - map array size
 * @return new MapsArray(left map and right map),or NULL otherwise.
 *
 */
int** spKDArrayCreateMapsArrays(int kdArrSize){
	int i;
	int** maps=(int**)malloc(sizeof(int*)*2);
	if(maps==NULL)
		return NULL;
	for (i = 0; i < 2; ++i) {
		maps[i]=(int*)malloc(sizeof(int)*kdArrSize);
		if(maps[i]==NULL){
			spKDArrayDestroyMapsArrays(maps,i);
			return NULL;
		}
	}
	return maps;
}


/** KDArray Split Helper Method
 *
 * Fill the Xarray: 0 if the point belongs to the left,
 * 		or 1 if it belongs to the right.
 * @param Xarray - Xarray that has been created befor
 * @param Sorted2DArray - KDArray Sorted2DArray
 * @param LeftKD2ArraySize - left KDArray size
 * @param kdArrsize - KDArray size
 * @param coor - coordinate
 * @return full Xarray.
 *
 */
int* spKDArrayFillXarray(int* Xarray,int** Sorted2Darray,int LeftKD2ArraySize,int kdArrsize,int coor){
	int i;
	for (i = 0; i <kdArrsize; ++i){
		if(i<LeftKD2ArraySize)
			Xarray[Sorted2Darray[coor][i]]=0;
		else
			Xarray[Sorted2Darray[coor][i]]=1;
	}
	return Xarray;
}

/** KDArray Split Helper Method
 *
 * fill the size of the left KDArray and the right Kdarray
 * @param KD2Array - KDArray array that contains the left and the right Kdarrays
 * @param KDArrSize - the size of the original KDArray
 * @return KDArray array (left and right)
 */
SPKDArray* spKDArrayChooseSplitSize(SPKDArray* KD2Array,int kdArrSize){
	if(kdArrSize%2==0){
		KD2Array[0]->size=kdArrSize/2;
		KD2Array[1]->size=kdArrSize/2;
	}
	else{
		KD2Array[0]->size=(kdArrSize/2)+1;
		KD2Array[1]->size=kdArrSize/2;
	}
	return KD2Array;
}

/** KDArray Split Helper Method
 *
 * fill the Maps array the left and the right: -1 if the point not contain to the side
 * 		or the point index otherwise
 * @param maps - MapsArray that has been created befor
 * @param kdArrSize - the size of the original KDArray
 * @param Xarray - the Xarray full
 * return MapsArray that it is full
 *
 */
int** spKDarrayFillMAps(int** maps, int kdArrSize,int* Xarray){
	int i,countindex=0;
	for (i = 0; i < kdArrSize; ++i) {
		if(Xarray[i]==0){
			maps[0][i]=0;
			maps[1][i]=-1;
		}
		else{
			maps[0][i]=-1;
			maps[1][i]=0;
		}
	}
	for (i = 0; i < kdArrSize; ++i) {
		if(maps[0][i]!=-1){
			maps[0][i]=countindex;
			countindex++;
		}
	}
	countindex=0;
	for (i = 0; i < kdArrSize; ++i) {
		if(maps[1][i]!=-1){
			maps[1][i]=countindex;
			countindex++;
		}
	}
	return maps;
}

/** KDArray Split Helper Method
 *
 * free all allocation memory in the arr that contains points array for left and right KDArray.
 * @param arr - array that contains left and right points array
 * @param index - number of points array we want to destroy
 */
void spKDArrayDestroyPointsArr(SPPoint** arr,int index){
	int i;
	for (i = 0; i < index; ++i) {
		free(arr[i]);
	}
	free(arr);
}

/** KDArray Split Helper Method
 *
 * Create points Array for the left and the right sides of the KDArray
 * @param KD2Array - KDArray array that contains left and right KDArray
 * @return new array that contains Points array for each side left an right,or NULL otherwise
 */
SPPoint** spKDArrayCreatePointsArr(SPKDArray* KD2Array){
	int i;
	SPPoint** arr=(SPPoint**)malloc(sizeof(SPPoint*)*2);
	if(arr==NULL)
		return NULL;
	for (i = 0; i < 2; ++i) {
		arr[i]=(SPPoint*)malloc(sizeof(SPPoint)*KD2Array[i]->size);
		if(arr[i]==NULL){
			spKDArrayDestroyPointsArr(arr,i);
			return NULL;
		}
	}
	return arr;
}

/** KDArray Split Helper Method
 *
 * free all allocation Memory in the two side of sorted2dArray
 * @param arr - array that contains right and left Sorted2Darrays.
 * @param dimension - number of rows
 * @param index - number of Sorted2Darrays we want to destroy.
 *
 */
void spKDArrayDestroySorted2DArray(int*** arr,int dimension,int index){
	int i;
	for (i = 0; i < index; ++i) {
		spKDArrayDestroySorted2Darray(arr[i],dimension);
	}
	free(arr);
}

/** KDArray Split Helper Method
 *
 * Create Sorted2DArray for each side left and right KDAray
 * @param dimension - number of rows
 * @param KD2Array - KDArray array that contains left and right Kdarray
 * @return array that contains 2 Sorted2DArray for left and right sides.
 *
 */
int*** spKDArrayCreateSorted2DArray(int dimension,SPKDArray* KD2Array){
	int i;
	int*** arr=(int***)malloc(sizeof(int**)*2);
	if(arr==NULL)
		return NULL;
	for (i = 0; i < 2; ++i) {
		arr[i]=spKDArrayCreateSorted2Darray(dimension,KD2Array[i]->size);
		if(arr[i]==NULL){
			spKDArrayDestroySorted2DArray(arr,dimension,i);
			return NULL;
		}
	}
	return arr;
}


SPKDArray* spKDArraySplit(SPKDArray kdArr, int coor) {

	int dimension,i,j,lj=0,rj=0;
	int* Xarray;
	SPKDArray* KD2Array;
	int** maps;
	SPPoint** PointsArrays;
	int*** Sorted2DArrays;
	if(!spKDArraySpliteInputValid(kdArr,coor))
		return NULL;
	dimension=spPointGetDimension(kdArr->pointsArray[0]);
	KD2Array=spKDArrayCreateSidesKDarrays();
	if(KD2Array==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		return NULL;
	}
	Xarray=spKDArrayCreateXArray(kdArr->size);
	if(Xarray==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		spKDArrayDestroySidesKDarrays(KD2Array,2);
		return NULL;
	}
	maps=spKDArrayCreateMapsArrays(kdArr->size);
	if(maps==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		spKDArrayDestroySidesKDarrays(KD2Array,2);
		spKDArrayDestroytXArray(Xarray);
		return NULL;
	}
	KD2Array=spKDArrayChooseSplitSize(KD2Array,kdArr->size);
	Xarray=spKDArrayFillXarray(Xarray,kdArr->Sorted2Darray,KD2Array[0]->size,kdArr->size,coor);
	maps=spKDarrayFillMAps(maps,kdArr->size,Xarray);
	PointsArrays=spKDArrayCreatePointsArr(KD2Array);
	if(PointsArrays==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		spKDArrayDestroyMapsArrays(maps,2);
		spKDArrayDestroySidesKDarrays(KD2Array,2);
		spKDArrayDestroytXArray(Xarray);
		return NULL;
	}
	Sorted2DArrays=spKDArrayCreateSorted2DArray(dimension,KD2Array);
	if(Sorted2DArrays==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		spKDArrayDestroyPointsArr(PointsArrays,2);
		spKDArrayDestroyMapsArrays(maps,2);
		spKDArrayDestroySidesKDarrays(KD2Array,2);
		spKDArrayDestroytXArray(Xarray);
		return NULL;
	}
	for (i = 0; i < 2; ++i) {
		KD2Array[i]->pointsArray=PointsArrays[i];
		KD2Array[i]->Sorted2Darray=Sorted2DArrays[i];
	}
	//fill left and right pointsarray
	for (i = 0; i < kdArr->size; ++i) {
		if(Xarray[i]==0){
			KD2Array[0]->pointsArray[lj]=spPointCopy(kdArr->pointsArray[i]);
			lj++;
		}
		else{
			KD2Array[1]->pointsArray[rj]=spPointCopy(kdArr->pointsArray[i]);
			rj++;
		}
	}

	//Fill A1 and A2
	for (i = 0; i < dimension; ++i) {
		lj=0;
		rj=0;
		for (j = 0; j < kdArr->size; ++j) {
			if(Xarray[kdArr->Sorted2Darray[i][j]]==0){
				KD2Array[0]->Sorted2Darray[i][lj]=maps[0][kdArr->Sorted2Darray[i][j]];
				lj++;
			}
			else{
				KD2Array[1]->Sorted2Darray[i][rj]=maps[1][kdArr->Sorted2Darray[i][j]];
				rj++;
			}
		}
	}
	// destroy Xarray and maps
	spKDArrayDestroyMapsArrays(maps,2);
	spKDArrayDestroytXArray(Xarray);
	free(PointsArrays);
	free(Sorted2DArrays);

	return KD2Array;
}

void spDestroyKD2Array(SPKDArray* KD2Array){
	int dim;
	if(KD2Array==NULL)
		return;
	dim = spPointGetDimension(KD2Array[0]->pointsArray[0]);
	spKDArrayDestroyPoints(KD2Array[0]->pointsArray,KD2Array[0]->size);
	spKDArrayDestroyPoints(KD2Array[1]->pointsArray,KD2Array[1]->size);
	spKDArrayDestroySorted2Darray(KD2Array[0]->Sorted2Darray,dim);
	spKDArrayDestroySorted2Darray(KD2Array[1]->Sorted2Darray,dim);
	spKDArrayDestroySidesKDarrays(KD2Array,2);
}


void spDestroyKDArray(SPKDArray kdArr) {
	int dim;
	if (kdArr == NULL)
		return;
	dim = spPointGetDimension(kdArr->pointsArray[0]);
	spKDArrayDestroyPoints(kdArr->pointsArray,kdArr->size);
	spKDArrayDestroySorted2Darray(kdArr->Sorted2Darray,dim);
	free(kdArr);
}

int spGetSizeKDArray(SPKDArray kdArr) {
	if(kdArr==NULL)
		return -1;
	return kdArr->size;
}

SPPoint* spGetPointsArrayKDArray(SPKDArray kdArr) {
	if(kdArr==NULL)
		return NULL;
	return kdArr->pointsArray;
}

int** spGetSorted2DarrayKDArray(SPKDArray kdArr) {
	if(kdArr==NULL)
		return NULL;
	return kdArr->Sorted2Darray;
}

int spGetSorted2DarrayIJ(SPKDArray KDArr, int i, int j) {
	assert(i>=0 && j>=0);
	if(KDArr==NULL)
		return -1;
	return KDArr->Sorted2Darray[i][j];
}
