#include "CABP.h"
#include <cstdio>

	/* Sets of nodes */
	/* Set L - able to communicate */
	/* Set K - unable to communicate */
	/* Set N - set L nodes with intersecting communication areas */
	/* Set P - !Points and nodes! describes the NLA node's area */
	/* Set M - made from K by referencing set P */

	error_t CABP::calculateIntersection(node_t *nodeN, node_t *nodeP, point_t *q1, point_t *q2) {
		/* Thanks to: http://local.wasp.uwa.edu.au/~pbourke/geometry/2circle/ */
		int32_t x2, y2;
		int32_t dx, dy, r0_2, r1_2, d_2;
		int32_t d, a, h, rx, ry;

		if(nodeN == NULL || nodeP == NULL) return FAIL;
		if(nodeN->nodeID == nodeP->nodeID) return FAIL; /* Same node */

		r0_2 = (int32_t)nodeN->r*(int32_t)nodeN->r;
		r1_2 = (int32_t)nodeP->r*(int32_t)nodeP->r;

		dx = nodeP->x - nodeN->x;
		dy = nodeP->y - nodeN->y;
		d_2 = dy*dy + dx*dx;
		d = sqrt(d_2);

		if(d > (nodeN->r + nodeP->r)) return FAIL; /* No solution. Circles do not intersect. */
		if(d < abs(nodeN->r - nodeP->r)) return FAIL; /* No solution. One circle is contained in the other. */

		/* Determine the distance from point 0 to point 2. */
		a = (r0_2 - r1_2 + d_2) / (2 * d) ;

		/* Determine the coordinates of point 2. */
		x2 = nodeN->x + (dx * a/d);
		y2 = nodeN->y + (dy * a/d);

		/* Determine the distance from point 2 to either of the intersection points. */
		h = sqrt(r0_2 - a*a);

		/* Now determine the offsets of the intersection points from point 2. */
		rx = -dy * h / d;
		ry = dx * h / d;

		q1->x = x2 + rx;
		q1->y = y2 + ry;
		q1->na = nodeN;
		q1->nb = nodeP;

		q2->x = x2 - rx;
		q2->y = y2 - ry;
		q2->na = nodeN;
		q2->nb = nodeP;

		return SUCCESS;
	}

	uint16_t CABP::distance(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
		int32_t xdiff, ydiff;
		xdiff = x1 - x2;
		ydiff = y1 - y2;
		return sqrt(xdiff*xdiff + ydiff*ydiff);
	}

	uint8_t CABP::checkIntersection(node_t* node1, node_t* node2, uint16_t* dist) {
		uint16_t d = distance(node1->x, node1->y, node2->x, node2->y);
		*dist = d; // We also return the distance to the calling function
		if(d < (node1->r + node2->r)) {
			if(d > abs(node1->r - node2->r)) return INTERSECT;
			else return INSIDE;
		}
		return OUTSIDE;
	}

	uint16_t CABP::calculateMultiplier(uint16_t r, uint16_t r2, uint16_t dist) {
		uint32_t nr = dist - r;
		uint32_t c2 = (nr * 100) / r2 - 100;
		printf("r2 = %d, nr = %d, c2 = %d\n", r2, nr, c2);
		return ((c2 / compensationMultiplier) + 1) * compensationMultiplier;
	}

	error_t CABP::createSetN() { /* Create set N */
		node_t *nodeL = NULL;
		node_t *nodeN = NULL;
		uint16_t dist = 0;
		uint16_t c = 0;
		bool intersects = TRUE;

		printf("SetL has %d nodes.\n",setL->count());
		if(setL->count() > 0) {
			setL->startIteration(0);
			setN->add(setL->next(0)); /* First one has no restrictions */
			while((nodeL = setL->next(0)) != NULL) {
				intersects = TRUE;
				setN->startIteration(0);
				while(((nodeN = setN->next(0)) != NULL) && intersects) { // intersects gets set to FALSE, if it is discovered that it does not intersect with the existing nodes
					switch(checkIntersection(nodeL, nodeN, &dist)) { // We might need the distance and since it is calculated here, let's get it
						case INTERSECT: // All good
							break;
						case INSIDE: // Ignore this node, since the set is sorted and the previous one describes the area more accurately
							printf("Area %d is inside area %d, ignoring area %d.\n", nodeN->nodeID, nodeL->nodeID, nodeL->nodeID);
							intersects = FALSE;
							break;
						case OUTSIDE: // Contradicting nodes
							printf("Areas %d and %d are too far from each other!\n", nodeN->nodeID, nodeL->nodeID);
							if(compensatingEnabled) {
								printf("Compensating...\n");
								// Let's increase the areas a bit
								// distance = r1 * c1 + r2 * c2;
								// c1 = (distance - r2 * c2)/r1;
								c = (dist * 100) / (nodeN->r + nodeL->r); // %
								c = (((c - 100) / compensationMultiplier) + 1) * compensationMultiplier; // It is possible, that if c is already an exact multiple of compensationMultiplier, then we increase the multiplier too much

								if(nodeN->c > c) {
									nodeL->c = calculateMultiplier(nodeN->r, nodeL->rm, dist);
									printf("Calculated multiplier %d for area %d.\n",nodeL->c, nodeL->nodeID);
								}
								else if(nodeL->c > c) {
									nodeN->c = calculateMultiplier(nodeL->r, nodeN->rm, dist);
									printf("Calculated multiplier %d for area %d.\n",nodeN->c, nodeN->nodeID);
								}
								else {
									nodeN->c = c;
									nodeL->c = c;
								}

								if((nodeN->c == 0) || (nodeL->c == 0)) return FAIL; // Some math problem somewhere probably, FAIL, because would cause a loop otherwise

								nodeN->r = (nodeN->rm * (100 + nodeN->c)) / 100;
								nodeL->r = (nodeL->rm * (100 + nodeL->c)) / 100;
								setN->clear();
								return ERETRY;
							}
							intersects = FALSE;
							break;
						default: // Meh?
							intersects = FALSE;
							break;
					}
				}
				if(intersects) setN->add(nodeL);
			}
			return SUCCESS;
		}
		return FAIL; /* Could not create setN, no nodes in setL */
	}

	error_t CABP::createSetP() { /* Create set P */
		point_t q1;
		point_t q2;
		node_t *nodeN;
		node_t *nodeP;

		printf("SetN has %d nodes.\n",setN->count());
		if(setN->count() >= 2) {
			setN->startIteration(0);
			if(calculateIntersection(setN->next(0),setN->next(0),&q1,&q2) == SUCCESS) { /* Take 0,1 and calculate intersection points q1,q2 */
				printf("Intersection calculated (%d,%d), (%d,%d), node %d and node %d.\n",q1.x,q1.y,q2.x,q2.y,q1.na->nodeID, q1.nb->nodeID);
				psetP->add(q1);/* Add to set P */
				psetP->add(q2);/* Add to set P */
				if(q1.na->setP > 0) setP->add(q1.na); /* Check not really needed */
				if(q1.nb->setP > 0) setP->add(q1.nb); /* Check not really needed */
			}
			else {
				return FAIL; /* These nodes should intersect */
			}
			while((nodeN = setN->next(0)) != NULL) {
				setP->startIteration(0);
				while((nodeP = setP->next(0)) != NULL) {
					if(calculateIntersection(nodeN, nodeP, &q1, &q2) == SUCCESS) {
						printf("Intersection calculated (%d,%d), (%d,%d), node %d and node %d.\n",q1.x,q1.y,q2.x,q2.y,q1.na->nodeID, q1.nb->nodeID);
						psetP->add(q1); /* setP only adds the quadruple if it fits the requirements and */
						psetP->add(q2); /* automatically removes quadruples that have become invalid */
					}
					else {
						return FAIL; /* All nodes at this point should intersect */
					}
				}
				if(nodeN->setP > 0) setP->add(nodeN);
			}
		}
		else if(setN->count() == 1) { /* Only one node, not possible to calculate intersection points */
			setN->startIteration(0);
			nodeN = setN->next(0);
			nodeN->setP = setK->count(); /* So that the only communicating node will not be removed from the set when processed with nodes from M (M<=K) */
			setP->add(nodeN); /* This is a special case, it does not have any points, but is in setP */
		}
		else {
			return FAIL; /* Not a thing to do really */
		}
		printf("SetP has %i nodes\n", setP->count());
		return SUCCESS;
	}

	error_t CABP::createSetM() {
		node_t *nodeK = NULL;
		if(setP->count() > 0) { /* Empty sets intersect with all other sets ... not needed if: Empty sets do not intersect with any other sets? */
			setK->startIteration(0);
			while((nodeK = setK->next(0)) != NULL) {
				if(setP->intersects(nodeK)) setM->add(nodeK);
			}
		}
		return SUCCESS;
	}

	error_t CABP::combineSetPwithSetM() {
		point_t q1;
		point_t q2;
		node_t * nodeM;
		node_t * nodeP;
		printf("SetM has %d nodes.\n",setM->count());
		setM->startIteration(0);
		while((nodeM = setM->next(0)) != NULL) {
			printf("SetP has %d nodes.\n",setP->count());
			setP->startIteration(0);
			while((nodeP = setP->next(0)) != NULL) {
				if(calculateIntersection(nodeM, nodeP, &q1, &q2) == SUCCESS) {
					printf("Intersection calculated (%d,%d), (%d,%d), node %d and node %d.\n",q1.x,q1.y,q2.x,q2.y,q1.na->nodeID, q1.nb->nodeID);
					psetP->add(q1); /* setP only adds the quadruple if it fits the requirements and */
					psetP->add(q2); /* automatically removes quadruples that have become invalid */
				}
				else printf("Nodes %i and %i do not have intersecting points.\n", nodeM->nodeID, nodeP->nodeID); /* This can happen with non-communicating nodes */
			}
			if(nodeM->setP > 0) setP->add(nodeM);
		}
		return SUCCESS;
	}

	error_t CABP::calculateCenterAndRadius(int16_t *xC, int16_t *yC, uint16_t *rC) {
		uint8_t i = 0;
		point_t *p = NULL;
		node_t *node = NULL;
		int32_t dx = 0;
		int32_t dy = 0;
		uint16_t d = 0;
		int32_t x = 0;
		int32_t y = 0;
		uint16_t r = 0;

		if(psetP->count() > 0) {
			psetP->startIteration();
			while((p = psetP->next()) != NULL) { /* Calculate average */
				x += p->x;
				y += p->y;
				i++;
			}
			x = x/i;
			y = y/i;

			psetP->startIteration();
			while((p = psetP->next()) != NULL) { /* Find radius */
				dx = p->x - x;
				dy = p->y - y;
				d = sqrt((dy*dy) + (dx*dx));
				if(d>r) r = d;
			}
		}
		else {
			if(setP->count() == 1) { /* Special case, the only node area also defines our node position */
				setP->startIteration(0);
				node = setP->next(0);
				x = node->x;
				y = node->y;
				r = node->r;
			}
			else return FAIL; /* Either no nodes at all or more than 1 node, which makes no sense if we don't have any points */
		}
		printf("Center: (%d, %d), radius: %d.\n",x,y,r); /* Signal */

		if(xC != NULL) *xC = x;
		if(yC != NULL) *yC = y;
		if(rC != NULL) *rC = r;

		return SUCCESS;
	}

	error_t CABP::position(int16_t *xC, int16_t *yC, uint16_t *rC) {
		point_t *p = NULL;
		node_t *node = NULL;
		bool firstStepDone = FALSE;
		if(count > 0) {
			while(!firstStepDone) {
				printf("Sorting setL\n");
				setL->startIteration(0);
				while((node = setL->next(0)) != NULL) printf("%d(%d) ", node->r, node->nodeID);
				printf("\n");
				setL->sort();
				setL->startIteration(0);
				while((node = setL->next(0)) != NULL) printf("%d(%d) ", node->r, node->nodeID);
				printf("\n");

				printf("Creating set N\n");
				switch(createSetN()) {
					case FAIL:
						firstStepDone = TRUE;
						printf("FAILED!\n");
						return FAIL;
					case SUCCESS:
						firstStepDone = TRUE;
						break;
					case ERETRY: // Try again from the top
						printf("Conflicting areas in setL, compensating and retrying...\n");
						break;
					default: // Not supposed to happen
						return FAIL;
				}
			}
			printf("Creating set P\n");
			if(createSetP() == FAIL) printf("FAILED!\n");
			printf("Creating set M\n");
			if(createSetM() == FAIL) printf("FAILED!\n");
			printf("Creating set PM\n");
			if(combineSetPwithSetM() == FAIL) printf("FAILED!\n");

			printf("Output:\n");
			printf("SetP has %d points.\n", psetP->count());
			psetP->startIteration();
			while((p = psetP->next()) != NULL) {
				printf("Point: (%d,%d)\n",p->x,p->y);
			}
			calculateCenterAndRadius(xC, yC, rC);

			return SUCCESS;
		}
		return FAIL;
	}

	error_t CABP::addCommNode(uint16_t nodeID, int16_t x, int16_t y, uint16_t r) {
		if(count < max_nodes) {
			nodes[count].nodeID = nodeID;
			nodes[count].x = x;
			nodes[count].y = y;
			nodes[count].r = r;
			nodes[count].rm = r;
			nodes[count].c = 0;
			nodes[count].set = TRUE;
			nodes[count].setP = 0;
			setL->add(&nodes[count]);
			count++;
			return SUCCESS;
		}
		return FAIL;
	}

	error_t CABP::addNonCommNode(uint16_t nodeID, int16_t x, int16_t y, uint16_t r) {
		if(count < max_nodes) {
			nodes[count].nodeID = nodeID;
			nodes[count].x = x;
			nodes[count].y = y;
			nodes[count].r = r;
			nodes[count].rm = r;
			nodes[count].c = 0;
			nodes[count].set = FALSE;
			nodes[count].setP = 0;
			setK->add(&nodes[count]);
			count++;
			return SUCCESS;
		}
		return FAIL;
	}

	void CABP::clear() {
		count = 0;
		setL->clear();
		setK->clear();
		setN->clear();
		psetP->clear(); /* Also clears setP */
		setM->clear();
	}

	PointSet* CABP::getPointSet() {
		return psetP;
	}

	NodeSet* CABP::getNodeSetL() {
		return setL;
	}

	CABP::CABP(uint8_t max_nodes, uint8_t max_points) {
		this->max_nodes = max_nodes;
		nodes = new node_t[max_nodes];

		setL = new NodeSet(max_nodes);
		setK = new NodeSet(max_nodes);
		setN = new NodeSet(max_nodes);
		setP = new NodeSet(max_nodes);
		setM = new NodeSet(max_nodes);
		psetP = new PointSet(max_points, setP);

		compensatingEnabled = FALSE;
		compensationMultiplier = 0;

		clear();
	}

	void CABP::enableCompensating(uint16_t multiplier) {
		if(multiplier > 0) {
			compensatingEnabled = TRUE;
			compensationMultiplier = multiplier;
		}
		else compensatingEnabled = FALSE;
	}

	void CABP::disableCompensating() {
		compensatingEnabled = FALSE;
	}

	CABP::~CABP() {
		delete psetP;
		delete setL;
		delete setK;
		delete setN;
		delete setP;
		delete setM;
		delete [] nodes;
	}
