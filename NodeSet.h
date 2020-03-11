#ifndef NODESET_H_
#define NODESET_H_
#include "types.h"

typedef struct node_t {
	uint16_t nodeID;
	int16_t x;
	int16_t y;
	uint16_t r;
	uint16_t rm;
	uint16_t c;
	bool set;
	uint8_t setP; // How many intersection points with this node belong to setP
} node_t;

class NodeSet {
public:
	NodeSet(uint8_t max_nodes);

	void add(node_t *n);
	node_t* nodeId(uint16_t id);
	void removeId(uint16_t id);
	void clear();
	void sort();

	uint8_t count();

	bool intersects(node_t *n);

	void startIteration(uint8_t iter);
	node_t *next(uint8_t iter);

	virtual ~NodeSet();

private:
	uint16_t distance(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	void consolidate();
	void shell_sort();

private:
	uint8_t max_nodes;
	node_t** nodes; // An array of pointers
	uint8_t nodeCount;
	int8_t lastReturned[2];
};

#endif /*NODESET_H_*/
