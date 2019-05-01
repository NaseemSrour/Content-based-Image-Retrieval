#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
#include "SPList.h"
/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new Bounded Priority Queue .
 *
 *This function creates a new Bounded Priority Queue with a maximum size
 *that contains an empty list.
 *
 *@param maximum size of the Bounded Priority Queue
 * @return
 * 	NULL - If allocations failed.
 * 	A new Bounded Priority Queue in case of success.
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of target Bounded Priority Queue.
 *
 * The new copy will contain all the elements from the source Queue in the same
 * order.
 *
 * @param list The target Queue to copy
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A Queue containing the same elements with same order as list otherwise.
 *
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * QueueDestroy: Deallocates an existing list. Clears all elements by using the
 * stored free function.
 *
 * @param Queue Target list to be deallocated. If Queue is NULL nothing will be
 * done
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from target Queue. The state of the current element will not be defined afterwards.
 *
 * The elements are deallocated using the stored freeing function
 * @param Queue Target Queue to remove all element from
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in a Queue.
 *
 * @param Queue The target Queue which size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the list.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * Returns the maximum number of elements that the Queue can contain.
 *
 * @param Queue The target Queue which size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the maximum number of elements that the Queue can contain.
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Inserts a NEW COPY element to the queue.
 * A bounded priority queue is similar to a regular minimum
 * priority queue, except that there is a fixed upper bound
 * on the number of elements that can be stored in the BPQ
 *
 * if the Queue is FULL and the new element value is higher than all of the
 * elemnts in the Queue it will not be added.
 * if the Queue is FULL and the new element has been added in the right place
 * then we have to remove the last element which is the highest value element.
 * otherwise we isert the elemnt normly in the right place.
 *
 * @param Queue The target Queue which we want to insert a new element to it.
 * @param List element that we want to insert to the Queue.
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if tha target is NULL OR the element is NULL.
 * SP_BPQUEUE_FULL if target Queue is FULL and element has the highest value.
 * SP_BPQUEUE_OUT_OF_MEMORY if an allocation failed
 * otherwise SP_BPQUEUE_SUCCESS.
 *
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * Removes the element with the lowest value.
 *
 * @param Queue The target Queue which we want to remove its element with the lowest value.
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if the target is NULL.
 * SP_BPQUEUE_EMPTY if the target Queue is Empty.
 * otherwise SP_BPQUEUE_SUCCESS.
 *
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * Returns a NEW COPY of the element with the lowest value in the Queue.
 *
 * @param Queue The target Queue which we want to return a new copy of the element
 * with the lowest value.
 * @return
 * NULL if the target Queue is NULL.
 * otherwise a NEW COPY of the element with the lowest value.
 *
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * Returns a NEW COPY of the element with the highest value in the Queue.
 *
 * @param Queue The target Queue which we want to return a new copy of the element
 * with the highest value.
 * @return
 * NULL if the target Queue is NULL OR there is no elements in the Queue.
 * otherwise a NEW COPY of the element with the highest value.
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * Returns the minimum value in the Queue.
 *
 * @param Queue The target Queue which we want to return the lowest value.
 * @return
 * -1 if a NULL pointer was sent OR there is no elements in the Queue.
 * Otherwise the lowest value in the Queue.
 *
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * Returns the highest value in the Queue.
 *
 * @param Queue The target Queue which we want to return the highest value.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the highest value in the Queue.
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * Returns true if the queue is empty.
 *
 * @param Queue The target Queue which we want to check if it is empty.
 * @return
 * Null if NULL pointer was sent.
 * True if the Queue is empty.
 * otherwise Flase if the Queue is not empty.
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * Returns true if the queue is Full.
 *
 * @param Queue The target Queue which we want to check if it is Full.
 * @return
 * Null if NULL pointer was sent.
 * True if the Queue is Full.
 * otherwise Flase if the Queue is not Full.
 */
bool spBPQueueIsFull(SPBPQueue source);

SPList spBPQueueGetList(SPBPQueue source);

#endif
