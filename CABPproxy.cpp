#include "CABP.h"

CABP global_algo(100,200);

extern "C" {

	int cabp_position(int16_t* x, int16_t* y, uint16_t* r) {
		return (int)global_algo.position(x, y, r);
	}

	void cabp_clear() {
		return global_algo.clear();
	}

	int cabp_add_comm_node(uint16_t id, int16_t x, int16_t y, uint16_t r) {
		return global_algo.addCommNode(id, x, y, r);
	}

	int cabp_add_noncomm_node(uint16_t id, int16_t x, int16_t y, uint16_t r) {
		return global_algo.addNonCommNode(id, x, y, r);
	}

}
