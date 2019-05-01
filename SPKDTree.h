#include "SPConfig.h"
#include "SPBPriorityQueue.h"
#include "SPKDArray.h"

typedef struct sp_kdTree_node_t* TreeNode;

/*
 * Create a KDTree recursivly.
 * @param points - points array
 * @param size - points array length
 * @param config - the configiration file
 * @return the root node of the tree.
 */
TreeNode SPKDTreeCreate(SPPoint* points, int size, SPConfig config);

/*
 * Get the SPread of the i"th dimention
 * @param points - points array
 * @param i - dimension
 * @param size - points array length
 * @return the spread of the i"th dimension
 */
double getSpread(SPPoint* points, int i, int size);

/*
 * Choose and return the splitting dimension
 * @param points - points array
 * @param size - points array length
 * @return Define the spread of the i"th dimension to be the difference
 * 		 between the maximum and minimum of the i"th coordinate of all points
 */
int calcSplitDimension(SPPoint* points, int size);

/*
 * Choose randomly and return the splitting dimension
 * @param limit - the range number
 * @return a random number between 0 - limit
 */
int randLimit(int limit);

/*
 * Saerch and Retun the value of the median point in the points array in the coordeinate dimension.
 * @param kdArray - KDArray object
 * @param dimention - the splitting dimension
 * @return the value of the median point in the points array in the coordeinate dimension.
 *
 */
double findMedianValueOfPointsArray(SPKDArray kdArray, int dimension);

/*
 * free all allocation memory in the KDTree recursivley
 * @param root - the root of the KDTree
 */
void spDestroyTree(TreeNode root);

/*
 * Check if we arrived a leaf.
 * @param node - KDTree Node
 * @return true id it is leaf,or false otherwise
 */
bool isLeaf(TreeNode node);

/*
 * KNN Search :Search for the K Nearest Neighbors.
 * @param node - KDTree begginning with node
 * @param p - Point
 * @param size - K = size
 * @return BPriority Queue that contains elements of the clossest points to p
 */
SPBPQueue kNearestNeighborsSearch(TreeNode node,SPPoint p,int size);
