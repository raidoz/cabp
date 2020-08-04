#include "NodeSet.h"
#include <cstdio>

	void NodeSet::shell_sort() {
		uint8_t i, j, increment;
		node_t *temp;
		increment = nodeCount / 2;

		while (increment > 0) {
			for (i = increment; i < nodeCount; i++) {
				j = i;
				temp = nodes[i];
				while ((j >= increment) && (nodes[j-increment]->r > temp->r)) {
					nodes[j] = nodes[j - increment];
					j = j - increment;
				}
				nodes[j] = temp;
			}

			if (increment == 2) increment = 1;
			else increment = (uint8_t) (increment / 2.2);
		}
	}

	void NodeSet::consolidate() {
		int8_t i;
		int8_t j = max_nodes - 1;

		for(i=0;i<max_nodes;i++) { // Looking for holes left to right
			if(nodes[i] == NULL) {
				for(;j>=0;j--) { // Looking for nodes right to left
					if(j < i) return; // Everything > j is NULL
					if(nodes[j] != NULL) {
						nodes[i] = nodes[j];
						nodes[j] = NULL;
						break;
					}
				}
			}
		}
	}

	void NodeSet::sort() {
		consolidate();
		shell_sort();
	}

	uint16_t NodeSet::distance(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
		int32_t xdiff, ydiff;
		xdiff = x1 - x2;
		ydiff = y1 - y2;
		return sqrt(xdiff*xdiff + ydiff*ydiff);
	}

	void NodeSet::removeId(uint16_t id) {
		uint8_t i;
		for(i=0;i<max_nodes;i++) {
			if(nodes[i] != NULL) {
				if(nodes[i]->nodeID == id) {
					printf("NodeSet: removing node %d.\n", nodes[i]->nodeID);
					nodes[i] = NULL;
					nodeCount--; //Decrease count
					return;
				}
			}
		}
		printf("NodeSet: Wanted to remove %d, but didn't find it.\n", id);
	}

	void NodeSet::add(node_t *n) {
		uint8_t i;
		int8_t firstFree = -1;
		if(n == NULL) return;
		for(i=0;i<max_nodes;i++) {
			if(nodes[i] != NULL) {
				if(nodes[i]->nodeID == n->nodeID) return; // Already in the set
			}
			else if(firstFree < 0) firstFree = i;
		}
		if(nodeCount < max_nodes) {
			if(firstFree >= 0) {
				nodes[firstFree] = n;
				nodeCount++;
			}
		}
	}

	bool NodeSet::intersects(node_t *n) {
		uint8_t i = 0;
		uint8_t analyzed = 0;
		uint32_t d;
		if(n == NULL) return FALSE;
		for(i=0;i<max_nodes;i++) {
			if(analyzed >= nodeCount) break; // Rest of the array is empty
			if(nodes[i] != NULL) {
				analyzed++;
				d = distance(n->x, n->y, nodes[i]->x, nodes[i]->y);
				if( !( (d < (nodes[i]->r + n->r)) && (d > abs((int64_t)(nodes[i]->r) - (int64_t)(n->r))) ) ) {
					return FALSE;
				}
			}
		}
		return TRUE;
	}

	void NodeSet::clear() {
		uint8_t i;
		for(i=0;i<max_nodes;i++) nodes[i] = NULL;
		nodeCount = 0;
	}

	uint8_t NodeSet::count() {
		return nodeCount;
	}

	void NodeSet::startIteration(uint8_t iter) {
		lastReturned[iter] = -1;
	}

	node_t *NodeSet::next(uint8_t iter) {
		uint8_t i;
		for(i=lastReturned[iter]+1;i<max_nodes;i++) {
			if(nodes[i] != NULL) {
				lastReturned[iter] = i;
				return nodes[i];
			}
		}
		return NULL;
	}

	node_t *NodeSet::nodeId(uint16_t id) {
		uint8_t i;
		for(i=0;i<max_nodes;i++) {
			if(nodes[i] != NULL) {
				if(nodes[i]->nodeID == id) {
					return nodes[i];
				}
			}
		}
		return NULL;
	}

	NodeSet::NodeSet(uint8_t max_nodes) {
		this->max_nodes = max_nodes;
		nodes = new node_t*[max_nodes];
		clear();
	}

	NodeSet::~NodeSet() {
		delete [] nodes;
	}
