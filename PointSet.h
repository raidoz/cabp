#ifndef POINTSET_H_
#define POINTSET_H_
#include "types.h"
#include "NodeSet.h"

typedef struct point_t {
	int16_t x;
	int16_t y;
	node_t *na;
	node_t *nb;
} point_t;

class PointSet {
public:
	PointSet(uint8_t max_points, NodeSet *set);

	void add(point_t &p);
	void clear();
	uint8_t count();

	void startIteration();
	point_t *next();

	virtual ~PointSet();

private:
	bool intersects(node_t *n);
	bool isWithinRangeOfSet(point_t  *p);
	bool isWithinRangeOfNode(node_t *n, point_t *p);
	uint16_t distance(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

private:
	uint8_t max_points;
	NodeSet *set;
	point_t *points;
	bool *free;

	uint8_t pointCount;
	int8_t lastReturned;
};

#endif /*POINTSET_H_*/
