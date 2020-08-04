#ifndef CABP_H_
#define CABP_H_
#include "NodeSet.h"
#include "PointSet.h"

class CABP {
public:
	CABP(uint8_t max_nodes, uint8_t max_points);
	void enableCompensating(uint32_t multiplier);
	void disableCompensating();

	error_t addCommNode(uint16_t nodeID, int32_t x, int32_t y, uint32_t r);
	error_t addNonCommNode(uint16_t nodeID, int32_t x, int32_t y, uint32_t r);

	error_t position(int32_t *xC, int32_t *yC, uint32_t *rC);

	PointSet *getPointSet();
	NodeSet *getNodeSetL();

	void clear();

	virtual ~CABP();

private:
	// Tasks:
	error_t createSetN();
	error_t createSetP();
	error_t createSetM();
	error_t combineSetPwithSetM();
	error_t calculateCenterAndRadius(int32_t *xC, int32_t *yC, uint32_t *rC);

	// Functions:
	uint32_t distance(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
	uint8_t checkIntersection(node_t* node1, node_t* node2, uint32_t* dist);

	error_t calculateIntersection(node_t *nodeN, node_t *nodeP, point_t *q1, point_t *q2);

	uint32_t calculateMultiplier(uint32_t r, uint32_t r2, uint32_t dist);

	enum {
		INTERSECT,
		OUTSIDE,
		INSIDE
	};

private:
	uint8_t max_nodes;
	node_t *nodes;
	uint8_t count;

	bool compensatingEnabled;
	uint8_t compensationMultiplier;

	NodeSet *setL;//Set L - able to communicate
	NodeSet *setK;//Set K - unable to communicate
	NodeSet *setN;//Set N - set L nodes with intersecting communication areas
	NodeSet *setP;//Set P - describes the NLA node's area
	PointSet *psetP;
	NodeSet *setM;

};

#endif /*CABP_H_*/
