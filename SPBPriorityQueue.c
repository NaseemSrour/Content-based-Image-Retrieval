/*
 * SPBPriorityQueue.c
 *
 *  Created on: 17 Mei 2016
 *      Author: Yazan
 */

#include "SPBPriorityQueue.h"
#include <stdlib.h>
#include <assert.h>

struct sp_bp_queue_t{
	int size;
	int maxSize;
	SPList list;
};

SPBPQueue spBPQueueCreate(int maxSize){

	if(maxSize<=0)
		return NULL;

	SPBPQueue q=(SPBPQueue)malloc(sizeof(*q));
	if(q==NULL)
		return NULL;
	q->list=spListCreate();
	if(q->list==NULL){
		free(q);
		return NULL;
	}

	q->size=0;
	q->maxSize=maxSize;
	return q;
}

void spBPQueueDestroy(SPBPQueue source){

	if(source==NULL)
		return;
	spListDestroy(source->list);
	free(source);
}

void spBPQueueClear(SPBPQueue source){

	if(source==NULL)
		return;
	spListClear(source->list);
	source->size=0;
}

int spBPQueueSize(SPBPQueue source){

	if(source==NULL)
		return -1;
	return source->size;
}

int spBPQueueGetMaxSize(SPBPQueue source){

	if(source==NULL)
		return -1;
	return source->maxSize;
}

bool spBPQueueIsEmpty(SPBPQueue source){

	assert (source!=NULL);
	if(source->size==0)
		return true;
	return false;
}

bool spBPQueueIsFull(SPBPQueue source){

	assert (source!=NULL);
	return(source->size==source->maxSize);

}

double spBPQueueMinValue(SPBPQueue source){

	if(source==NULL)
		return -1.0;
	SPListElement element=spBPQueuePeek(source);
	if(element==NULL)
		return -1.0;
	double x=spListElementGetValue(element);
	spListElementDestroy(element);
	return x;
}

double spBPQueueMaxValue(SPBPQueue source){

	if(source==NULL)
		return -1.0;
	SPListElement element=spBPQueuePeekLast(source);
	if(element==NULL)
		return -1.0;
	double x=spListElementGetValue(element);
	spListElementDestroy(element);
	return x;
}

SPBPQueue spBPQueueCopy(SPBPQueue source){

	if(source==NULL)
		return NULL;

	SPBPQueue copiedQ=spBPQueueCreate(source->maxSize);
	if(copiedQ==NULL)
		return NULL;
	spListDestroy(copiedQ->list);
	copiedQ->list=spListCopy(source->list);
	if(copiedQ->list==NULL){
		free(copiedQ);
		return NULL;
	}
	copiedQ->size=source->size;
	return copiedQ;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element){

	if(source==NULL || element==NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;
	if(spBPQueueIsFull(source)){
		if(spListElementGetValue(element) > spBPQueueMaxValue(source))
			return SP_BPQUEUE_FULL;
	}

	SPListElement copiedElement=spListElementCopy(element);
	if(copiedElement==NULL)
		return SP_BPQUEUE_OUT_OF_MEMORY;
	SP_LIST_MSG msg;

	if(spBPQueueIsEmpty(source)){
		msg=spListInsertFirst(source->list,copiedElement);
		if(msg==SP_LIST_NULL_ARGUMENT ){
			spListElementDestroy(copiedElement);
			return SP_BPQUEUE_INVALID_ARGUMENT;
		}
		else if(msg==SP_LIST_OUT_OF_MEMORY){
			spListElementDestroy(copiedElement);
			return SP_BPQUEUE_OUT_OF_MEMORY;
		}
		source->size++;
		spListElementDestroy(copiedElement);
		return SP_BPQUEUE_SUCCESS;
	}

	SPListElement currentElement=spListGetFirst(source->list);
	if(currentElement==NULL)
		return SP_BPQUEUE_EMPTY;
	while(spListElementCompare(element,currentElement)>=0){
		currentElement=spListGetNext(source->list);
		if(currentElement==NULL)
			break;
	}
	if(currentElement==NULL){
		currentElement=spListGetLast(source->list);
		msg=spListInsertLast(source->list,copiedElement);
		if(msg==SP_LIST_NULL_ARGUMENT ){
			spListElementDestroy(copiedElement);
			return SP_BPQUEUE_INVALID_ARGUMENT;
		}
		else if(msg==SP_LIST_OUT_OF_MEMORY){
			spListElementDestroy(copiedElement);
			return SP_BPQUEUE_OUT_OF_MEMORY;
		}
		source->size++;
		spListElementDestroy(copiedElement);
		return SP_BPQUEUE_SUCCESS;
	}
	msg=spListInsertBeforeCurrent(source->list,copiedElement);
	if(msg==SP_LIST_NULL_ARGUMENT || msg==SP_LIST_INVALID_CURRENT){
		spListElementDestroy(copiedElement);
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	else if(msg==SP_LIST_OUT_OF_MEMORY){
		spListElementDestroy(copiedElement);
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	source->size++;
	if(source->size>source->maxSize){
		currentElement=spListGetLast(source->list);
		spListRemoveCurrent(source->list);
		source->size--;
	}
	spListElementDestroy(copiedElement);
	return SP_BPQUEUE_SUCCESS;
}


SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source){

	if(source==NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;
	if(source->size==0)
		return SP_BPQUEUE_EMPTY;
	SPListElement currentElement=spListGetFirst(source->list);
	if(currentElement==NULL)
		return SP_BPQUEUE_EMPTY;
	if(spListRemoveCurrent(source->list)!=SP_LIST_SUCCESS){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	source->size--;
	return SP_BPQUEUE_SUCCESS;
}

SPListElement spBPQueuePeek(SPBPQueue source){

	if(source==NULL)
		return NULL;
	if(source->size==0)
		return NULL;
	SPListElement firstElement=spListGetFirst(source->list);
	if(firstElement==NULL)
		return NULL;
	SPListElement element=spListElementCopy(firstElement);
	if(element==NULL){
		return NULL;
	}
	return element;
}

SPListElement spBPQueuePeekLast(SPBPQueue source){

	if(source==NULL)
		return NULL;
	if(source->size==0)
		return NULL;
	SPListElement element=NULL;
	SPListElement lastElement=spListGetLast(source->list);
	if(lastElement==NULL)
		return NULL;
	element=spListElementCopy(lastElement);
	if(element==NULL){
		return NULL;
	}
	return element;
}

SPList spBPQueueGetList(SPBPQueue source){
	if(source==NULL)
		return NULL;
	return source->list;
}



