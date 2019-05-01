/*
 * SPKDTree.c
 *
 *  Created on: Aug 25, 2016
 *      Author: Naseem
 */
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "SPKDTree.h"
#include "SPPoint.h"


#define allocationError "Allocation Error\n\n"

struct sp_kdTree_node_t{
	int dim; // the splitting dimension
	double val; //The median value of the splitting dimension
	struct sp_kdTree_node_t* left;
	struct sp_kdTree_node_t* right;
	SPPoint data; // Pointer to a point (only if the current node is a leaf) otherwise this field's value is NULL.
};

TreeNode InitializationRecursion(SPKDArray kdArray, int size, int prevSplitDimension, SPConfig config);
void kNearestNeighborsSearchRecursive(TreeNode node,SPBPQueue bpq,SPPoint p);

TreeNode SPKDTreeCreate(SPPoint* points, int size, SPConfig config){
	SPKDArray kdArray = spKDArrayCreate(points, size); // Initialize the KD array.
	TreeNode node; // Will be the tree's root.
	if(kdArray == NULL)
		return NULL;
	node = InitializationRecursion(kdArray, size, -1, config);
	if(node==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		return NULL;
	}
	spDestroyKDArray(kdArray);
	return node; // root
}

TreeNode InitializationRecursion(SPKDArray kdArray, int size, int prevSplitDimension, SPConfig config){
	int splitDimension = 0, dimension=0;
	SPKDArray* splitArray;
	TreeNode newNode;
	TreeNode node;
	SPPoint* pointsArray;
	SP_CONFIG_MSG msg=SP_CONFIG_SUCCESS;
	enum splitMethod splitMeth = spConfigGetSpliteMethod(config,&msg);
	if(spGetSizeKDArray(kdArray)==1){
		node = (TreeNode)malloc(sizeof(*node));
		if(node == NULL){
			spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
			return NULL;
		}
		node->dim = -1;
		node->val = -1;
		node->left = NULL;
		node->right = NULL;
		if(isLeaf(node) == true){
			node->data = spPointCopy(spGetPointsArrayKDArray(kdArray)[0]);
			return node;
		}
		else{
			return NULL;
		}
	}
	//enum splitMethod splitMeth:
	if(splitMeth == MAX_SPREAD){
		pointsArray=spGetPointsArrayKDArray(kdArray);
		splitDimension = calcSplitDimension(pointsArray, size);
		splitArray = spKDArraySplit(kdArray, splitDimension);
	}
	else if(splitMeth == RANDOM){
		pointsArray=spGetPointsArrayKDArray(kdArray);
		dimension=spPointGetDimension(pointsArray[0]);
		splitDimension = randLimit(dimension);
		splitArray = spKDArraySplit(kdArray, splitDimension);
	}
	else{ // == INCREMENTAL //
		splitDimension = (prevSplitDimension + 1) % (spPointGetDimension(spGetPointsArrayKDArray(kdArray)[0]));
		splitArray = spKDArraySplit(kdArray, splitDimension);
	}
	// After splitting, now comes the third step in the recursive procedure:
	newNode = (TreeNode)malloc(sizeof(*newNode));
	if(newNode == NULL){
		// No need to free(node);
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		return NULL;
	}
	newNode->dim = splitDimension;
	newNode->val = findMedianValueOfPointsArray(kdArray, splitDimension);
	if(size % 2 == 0){
	newNode->left = InitializationRecursion(splitArray[0], size/2, splitDimension, config);
	newNode->right = InitializationRecursion(splitArray[1], size/2, splitDimension, config);
	}
	else{
		newNode->left = InitializationRecursion(splitArray[0], (size/2) + 1, splitDimension, config);
		newNode->right = InitializationRecursion(splitArray[1], size/2, splitDimension, config);
	}
	newNode->data = NULL;
	spDestroyKDArray(splitArray[0]);
	spDestroyKDArray(splitArray[1]);
	free(splitArray);
	return newNode;
}

void spDestroyTree(TreeNode root){
	if(root == NULL)
		return;
	if(isLeaf(root)){
		spPointDestroy(root->data);
		free(root);
		return;
	}
	spDestroyTree(root->left);
	spDestroyTree(root->right);
	free(root);
}

// Returns the index of the dimension with the highest spread.
int calcSplitDimension(SPPoint* points, int size){ // Computes the index of the split dimension, for MAX_SPREAD splitting method.
	int i, index;
	double k;
	int dim = spPointGetDimension(points[0]);
	if(points == NULL){
		return -1;
	}
	double* spreads = (double*)malloc(sizeof(double)*dim);
	if(spreads == NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		return -1;
	}
	for(i = 0; i < dim; i++){
		spreads[i] = getSpread(points, i, size); // e3ni: el spread tb3 l i-th dimension is: getSpread...
	}

	//Now we find the dimension with the highest spread:
	k = spreads[0];
	for(i = 0; i < dim; i++){
		if(spreads[i] >= k){
			index = i;
			k = spreads[i];
		}
	}
	free(spreads);
	return index;

}

// Calculates and returns the spread for dimension i. (for MAX SPREAD)
double getSpread(SPPoint* points, int i, int size){
	double max, min;
	int j;
	double coor;
	double spread;
	max = spPointGetAxisCoor(points[0], i);
	min = spPointGetAxisCoor(points[0], i);
	for(j = 0; j < size; j++){
		coor = spPointGetAxisCoor(points[j], i);
		if(coor >= max){
			max = coor;
		}
		if(coor <= min){
			min = coor;
		}

	}
	spread = max - min;
	return spread;
}


//Generates a random integer between 0 and 'limit' (inclusive):

int randLimit(int limit) {
    int returnValue = rand() % limit;
    return returnValue;
}

double findMedianValueOfPointsArray(SPKDArray kdArray, int dimension){
	double value;
	SPPoint* points = spGetPointsArrayKDArray(kdArray);
	int size=spGetSizeKDArray(kdArray);
	value = spPointGetAxisCoor(points[(size/2)],dimension);
	return value;
}

bool isLeaf(TreeNode node){
	if ((node->left == NULL) && (node->right == NULL)){
		return true;
	}
	else{
		return false;
	}
}


SPBPQueue kNearestNeighborsSearch(TreeNode node,SPPoint p,int size){
	SPBPQueue bpq;
	if(node==NULL)
		return NULL;
	bpq=spBPQueueCreate(size);
	if(bpq==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
		return NULL;
	}
	kNearestNeighborsSearchRecursive(node,bpq,p);
	if(bpq==NULL){
		spLoggerPrintError(allocationError,__FILE__,__func__,__LINE__);
	}
	return bpq;
}


void kNearestNeighborsSearchRecursive(TreeNode node,SPBPQueue bpq,SPPoint p){
	bool leftside;
	SPListElement element;
	int distance,index;
	if(node==NULL)
		return;
	if(isLeaf(node)){
		distance=spPointL2SquaredDistance(p,node->data);
		index=spPointGetIndex(node->data);
		element=spListElementCreate(index,distance);
		spBPQueueEnqueue(bpq,element);
		spListElementDestroy(element);
		return;
	}
	leftside=false;
	if(spPointGetAxisCoor(p,node->dim)<=node->val){
		kNearestNeighborsSearchRecursive(node->left,bpq,p);
		leftside=true;
	}
	else kNearestNeighborsSearchRecursive(node->right,bpq,p);


	if(!spBPQueueIsEmpty(bpq) || pow(((spPointGetAxisCoor(p,node->dim))-(node->val)),2)<spBPQueueMaxValue(bpq)){
		if(leftside)
			kNearestNeighborsSearchRecursive(node->right,bpq,p);
		else kNearestNeighborsSearchRecursive(node->left,bpq,p);
	}

}



