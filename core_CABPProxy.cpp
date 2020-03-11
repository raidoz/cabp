#include "core_CABPProxy.h"
#include "CABP.h"

CABP algo(12,24);

JNIEXPORT void JNICALL Java_core_CABPProxy_clear(JNIEnv *e, jobject o) {
	algo.enableCompensating(25);
	algo.clear();
}

JNIEXPORT jint JNICALL Java_core_CABPProxy_position(JNIEnv *e, jobject o, jintArray xArr, jintArray yArr, jintArray rArr) {
	jint jx[1];
	jint jy[1];
	jint jr[1];
	int16_t x;
	int16_t y;
	uint16_t r;
	jint result;

	result = algo.position(&x, &y, &r);

	jx[0] = x;
	jy[0] = y;
	jr[0] = r;

	e->SetIntArrayRegion(xArr, 0, 1, jx);
	e->SetIntArrayRegion(yArr, 0, 1, jy);
	e->SetIntArrayRegion(rArr, 0, 1, jr);

	return result;
}

JNIEXPORT jint JNICALL Java_core_CABPProxy_addCommNode(JNIEnv *e, jobject o, jint id, jint x, jint y, jint r) {
	return algo.addCommNode(id, x, y, r);
}

JNIEXPORT jint JNICALL Java_core_CABPProxy_addNonCommNode(JNIEnv *e, jobject o, jint id, jint x, jint y, jint r) {
	return algo.addNonCommNode(id, x, y, r);
}

/*
 * Return number of points and store points in arrays if enough space
 * Return -number of points if not enough, nothing is stored
 * */
JNIEXPORT jint JNICALL Java_core_CABPProxy_getAreaPoints(JNIEnv *e, jobject o, jintArray xArr, jintArray yArr, jintArray naArr, jintArray nbArr) {
	PointSet *pset = algo.getPointSet();
	point_t *p = NULL;
	jint *x;
	jint *y;
	jint *na;
	jint *nb;
	jint i = 0;
	jsize xArrLen = e->GetArrayLength(xArr);
	jsize yArrLen = e->GetArrayLength(yArr);
	jsize naArrLen = e->GetArrayLength(naArr);
	jsize nbArrLen = e->GetArrayLength(nbArr);
	jsize len = xArrLen;
	if(yArrLen < len) len = yArrLen;
	if(naArrLen < len) len = naArrLen;
	if(nbArrLen < len) len = nbArrLen;

	if(len > 0) {
		if(len > (jsize)(pset->count())) {
			x = new jint[len];
			y = new jint[len];
			na = new jint[len];
			nb = new jint[len];

			i = 0;
			pset->startIteration();

			while((p = pset->next()) != NULL) {
				if(i >= len) break;
				x[i] = p->x;
				y[i] = p->y;
				na[i] = p->na->nodeID;
				nb[i] = p->nb->nodeID;
				i++;
			}

			e->SetIntArrayRegion(xArr, 0, i, x);
			e->SetIntArrayRegion(yArr, 0, i, y);
			e->SetIntArrayRegion(naArr, 0, i, na);
			e->SetIntArrayRegion(nbArr, 0, i, nb);

			delete [] x;
			delete [] y;
			delete [] na;
			delete [] nb;
		}
		else return -pset->count();
	}

	return i;
}

JNIEXPORT jint JNICALL Java_core_CABPProxy_getAreas(JNIEnv *e, jobject o, jintArray idArr, jintArray xArr, jintArray yArr, jintArray rArr) {
	NodeSet* set = algo.getNodeSetL();
	node_t* node = NULL;
	jint* id;
	jint* x;
	jint* y;
	jint* r;
	jint i = 0;
	jsize idArrLen = e->GetArrayLength(idArr);
	jsize xArrLen = e->GetArrayLength(xArr);
	jsize yArrLen = e->GetArrayLength(yArr);
	jsize rArrLen = e->GetArrayLength(rArr);
	jsize len = idArrLen;
	if(xArrLen < len) len = xArrLen;
	if(yArrLen < len) len = yArrLen;
	if(rArrLen < len) len = rArrLen;

	if(len > 0) {
		if(len > (jsize)(set->count())) {
			id = new jint[len];
			x = new jint[len];
			y = new jint[len];
			r = new jint[len];

			i = 0;
			set->startIteration(0);

			while((node = set->next(0)) != NULL) {
				if(i >= len) break;
				id[i] = node->nodeID;
				x[i] = node->x;
				y[i] = node->y;
				r[i] = node->r;
				i++;
			}

			e->SetIntArrayRegion(idArr, 0, i, id);
			e->SetIntArrayRegion(xArr, 0, i, x);
			e->SetIntArrayRegion(yArr, 0, i, y);
			e->SetIntArrayRegion(rArr, 0, i, r);

			delete [] id;
			delete [] x;
			delete [] y;
			delete [] r;
		}
		else return -set->count();
	}

	return i;
}
