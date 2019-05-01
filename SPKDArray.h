/*
 * SPKDArray.h
 *
 *  Created on: 01 Sep 2016
 *      Author: Yazan
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_


#include "SPPoint.h"
#include "SPLogger.h"


typedef struct sp_KDArray_t* SPKDArray;
typedef struct sp_point_helper_sort* SPPointHelperSort;

/*Initializes the kd-array with the data given by arr
 *Allocates a new KDarray object in the memory
 *@param arr - points array
 *@param size - length of the array
 *@return
 	 new KDArray object
*/
SPKDArray spKDArrayCreate(SPPoint* arr, int size);


/*returns the value of the cell ij from the sorted2DArray
 *@param KDArray
 *@param index i
 *@param index j
 *@return
 *		the value of the cell ij from the sorted2DArray
 */
int spGetSorted2DarrayIJ(SPKDArray KDArr,int i,int j);


/* Returns two kd-arrays (kdLeft, kdRight) such that the first âŒˆð�’�/ð�Ÿ�âŒ‰  points with
 *  respect to the coordinate coor are in kdLeft
 *  ,and the rest of the points are in kdRight.
 *
 *	@param kdArr - KDArray
 *	@param coor - coordinate
 *	@return
 *		array that the fisrt element is the left KDArray,
 *		and the seconed element is the right KDArray.
 *
 */

SPKDArray* spKDArraySplit(SPKDArray kdArr, int coor);


/*free all allocates memory from the given KDArray Object
 * @param kdArr - KDArray object
 *
 */
void spDestroyKDArray(SPKDArray kdArr);

/**
 *free all allocates memory from the given KDArray Array Object
 *@param KD2Array - SPKDArray Array (two KDArray, left and right)
 *
 */
void spDestroyKD2Array(SPKDArray* KD2Array);

/**
 * Get KDArray Size.
 * @param kdArr - KDArray object
 * @return
 *  KDArr size if (kdArr!=NULL) or -1 otherwise.
 *
 */
int spGetSizeKDArray(SPKDArray kdArr);

/**
 * Get points array from KDArray.
 * @param kdArr - KDArray object
 * @return
 *  points array if (kdArr!=NULL) or NULL otherwise.
 *
 */
SPPoint* spGetPointsArrayKDArray(SPKDArray kdArr);

/**
 * Get Sorted2Darray from KDArray.
 * @param kdArr - KDArray object
 * @return
 * Sorted2Darray if (kdArr!=NULL) or NULL otherwise.
 *
 */
int** spGetSorted2DarrayKDArray(SPKDArray kdArr);

/**
 * free all allocates memory from the given SPPoint points array beginning
 *  from the fist point to points[size-1] .
 * @param points - array contains points
 * @param size - number of points that we want to destroy (0 - size)
 *
 */
void spKDArrayDestroyPoints(SPPoint* points, int size);



#endif /* SPKDARRAY_H_ */
