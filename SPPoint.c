/*
 * SPPoint.c
 *
 *  Created on: 01 Sep 2016
 *      Author: Yazan
 */

#include "SPPoint.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

struct sp_point_t{

	double *data;
	int dim;
	int index;

};


SPPoint spPointCreate(double* data, int dim, int index){

	if(data==NULL || dim<=0 || index<0)
		return NULL;
	SPPoint p=(SPPoint)malloc(sizeof(*p));
	if(p==NULL)
		return NULL;
	p->data=(double*)malloc(dim*sizeof(double));
	if(p->data==NULL){
		free(p);
		return NULL;
	}
	int i;
	for (i = 0; i <dim ; i++) {
		p->data[i]=data[i];
	}
	p->dim=dim;
	p->index=index;
	return p;
}

SPPoint spPointCopy(SPPoint source){

	assert (source != NULL);
	SPPoint p=(SPPoint)malloc(sizeof(*p));
	if(p!=NULL){
		p->data=(double*)malloc(source->dim*sizeof(double));
		if(p->data!=NULL){
			int i;
			for (i = 0; i <source->dim ; i++) {
				p->data[i]=source->data[i];
			}
			p->dim=source->dim;
			p->index=source->index;
			return p;
		}
		return NULL;
	}
	return NULL;
}

void spPointDestroy(SPPoint point){

	if(point!=NULL){
		if(point->data!=NULL)
			free(point->data);
		free(point);
	}
}

int spPointGetDimension(SPPoint point){

	assert (point != NULL);
	return (point->dim);
}

int spPointGetIndex(SPPoint point){
	assert (point != NULL);
	return (point->index);
}

double spPointGetAxisCoor(SPPoint point, int axis){

	assert (point!=NULL && axis < point->dim);
	return (point->data[axis]);
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q){

	assert (p!=NULL && q!=NULL && p->dim == q->dim);
	double sum=0.0;
	int i;
	for (i = 0; i < p->dim ; i++) {
		sum+=(p->data[i]-q->data[i])*(p->data[i]-q->data[i]);
	}
	return sum;
}

