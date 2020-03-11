#include "PointSet.h"
#include <cstdio>

	uint16_t PointSet::distance(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
		int32_t xdiff, ydiff;
		xdiff = x1 - x2;
		ydiff = y1 - y2;
		return sqrt(xdiff*xdiff + ydiff*ydiff);
	}

	bool PointSet::isWithinRangeOfNode(node_t *n, point_t *p) { /* On the line or closer */
		if(n == NULL || p == NULL) return FALSE;
		if(n->nodeID == p->na->nodeID) return TRUE; /* To avoid the odd chance that distance is 1 unit larger than radius because of integer math inaccuracies */
		if(n->nodeID == p->nb->nodeID) return TRUE; /* To avoid the odd chance that distance is 1 unit larger than radius because of integer math inaccuracies */
		if(distance(n->x, n->y, p->x, p->y) <= n->r) return TRUE;
		return FALSE;
	}

	bool PointSet::isWithinRangeOfSet(point_t  *p) {
		node_t *node = NULL;
		set->startIteration(1);
		while((node = set->next(1)) != NULL) {
			if(node->set && !isWithinRangeOfNode(node, p)) {
				printf("Point (%d,%d) na=%i, nb=%i not in range of %i\n", p->x, p->y, p->na->nodeID, p->nb->nodeID, node->nodeID);
				return FALSE; /* Checking only nodes that can communicate */
			}
		}
		return TRUE;
	}

	void PointSet::add(point_t &p) {
		uint8_t i;
		if(pointCount < max_points) { /* We have room to add points */
			if(pointCount < 2 || isWithinRangeOfSet(&p)) { /* Add point to set */
				for(i=0;i<max_points;i++) {
					if(free[i]) { // Add
						printf("Adding point (%d,%d) na=%i, nb=%i\n", p.x, p.y, p.na->nodeID, p.nb->nodeID);
						free[i] = FALSE;
						points[i] = p;
						pointCount++;
						p.na->setP++; /* How many intersection points this set has in P */
						p.nb->setP++; /* How many intersection points this set has in P */
						break;
					}
				}
				/* Re-check set */
				if(pointCount > 2) { /* Must have 3 or more nodes for re-checking */
					for(i=0;i<max_points;i++) {
						if(!free[i]) {
							if(p.na == points[i].na) continue; /*  Must not check against point's own node - not in range, but on the line */
							if((p.na->set && !isWithinRangeOfNode(p.na, &(points[i]))) || /* Remove if communicating node and point IS NOT within range of it */
									(!p.na->set && isWithinRangeOfNode(p.na, &(points[i]))) ) { /* Remove if NON-communicating node and point IS within range of it */
								printf("Removing point (%d,%d) na=%d, nb=%d\n", points[i].x, points[i].y, points[i].na->nodeID, points[i].nb->nodeID);
								points[i].na->setP--;
								if(points[i].na->setP <= 0) {
									printf("Removing %i from set P nodeset\n", points[i].na->nodeID);
									set->removeId(points[i].na->nodeID);
								}
								points[i].nb->setP--;
								if(points[i].nb->setP <= 0) {
									printf("Removing %i from set P nodeset\n", points[i].nb->nodeID);
									set->removeId(points[i].nb->nodeID);
								}
								free[i] = TRUE;
								pointCount--;
							}
						}
					}
				}
			}
		}
	}

	uint8_t PointSet::count() {
		return pointCount;
	}

	void PointSet::startIteration() {
		lastReturned = -1;
	}

	point_t *PointSet::next() {
		uint8_t i;
		for(i=lastReturned+1;i<max_points;i++) {
			if(!free[i]) {
				lastReturned = i;
				return &(points[i]);
			}
		}
		return NULL;
	}

	void PointSet::clear() {
		uint8_t i;
		for(i=0;i<max_points;i++) free[i] = TRUE;
		pointCount = 0;
		set->clear();
	}

	PointSet::PointSet(uint8_t max_points, NodeSet *set) {
		this->max_points = max_points;
		points = new point_t[max_points];
		free = new bool[max_points];
		this->set = set;
		clear();
	}

	PointSet::~PointSet() {
		delete [] points;
		delete [] free;
	}
