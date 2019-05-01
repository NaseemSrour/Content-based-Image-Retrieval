/*
 * main_aux.h
 *
 *  Created on: 08 Sep 2016
 *      Author: Yazan
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SPLogger.h"
#include "SPConfig.h"
#include "SPPoint.h"
#include "SPKDTree.h"
#include "SPBPriorityQueue.h"

/*
 * Choose Logger Level and Create the Logger
 * @param config - the onfigration file
 * @return SP_LOGGER_SUCCESS on success.
 */
SP_LOGGER_MSG CheckLoggerLevelAndCreateLogger(SPConfig config);

/* Put the features of all images in one points array and build KDTree
 * @param config - configiratin file
 * @param featurePoints - points(features) array
 * @param numOfAllFeats - number of all the features
 * @param numOfeatsArray - array contains number of features for each image
 * @return new KDTree
 *
 */
TreeNode BuildKDTree(SPConfig config,SPPoint** featurePoints,int numOfAllFeats,int* numOfFeatsArray);

/*
 * Get the index of the max value in the array
 * @param array - integer array
 * @param size - array length
 * @return the index of the max value in the array
 */
int GetMaxIndexInArray(int* array,int size);

/*
 * For each point of the query Image do KNNSearch in the KDTree
 * @param config - configiratin file
 * @param numOfQueryFeats - number of the features in the query Image
 * @param root - the root of the KDTree
 * @param queryFeaturePoints - the points (features) of the query Image
 *
 * @return array contains the number of appearence for each Image in every single search
 */
int* KnnSearchAndFillNumOfAppearenceForEachImg(SPConfig config,int numOfQueryFeats,TreeNode root,SPPoint* queryFeaturePoints);

/*
 * Fill tha data in the images file Feats,for each image Fill:(example)
 * FeatsNumber= 100
 * ImageIndex= 0
 * Dimension= 20
 * FeatureArray: data (rows and columns)
 *
 * @param imagePathFeat - the path of the new file we want
 * 	 to creat (the same as the image but with suffix .feats)
 * @param index - index of the image
 * @param points Array - array that contains the features(points)
 * @param numOfFeats - points array length
 * @return true in succes, and false otherwise
 *
 */
bool spFillFeatsFiles(char* imagePathFeat,int index,SPPoint* pointsArray,int numOfFeats);

/*
 * Get the Data from the files (.feats) and fill it in points array for each image
 * @param config - configiratin file
 * @param numOfAllFeats - sum of all features
 * @param numOfFeatsArray - array that contains number of features for each image
 * @return array that contains features(points) array for each image
 */
SPPoint** GetFeaturesFromFeatsFiles(SPConfig config,int* numOfAllFeats,int* numOfFeatsArray);

/*
 * destroy all allocated memory in the images features array that contains feature array for every image
 * @param imagesFeaturePointsArray - images features array that contains feature array for every image
 * @param size - number of features array we want to destroy
 */
void spDestroyimagesFeaturePointsArray(SPPoint** imagesFeaturePointsArray,int size);

#endif /* MAIN_AUX_H_ */
