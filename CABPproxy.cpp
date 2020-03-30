#include "CABP.h"

extern "C" {

	void * cabp_create() {
		return new CABP(100, 200);
	}

	void cabp_destroy(void * cabp) {
		delete (CABP*)cabp;
	}

	int cabp_position(void * cabp, int16_t* x, int16_t* y, uint16_t* r) {
		return (int)((CABP*)cabp)->position(x, y, r);
	}

	void cabp_clear(void * cabp) {
		return ((CABP*)cabp)->clear();
	}

	int cabp_add_comm_node(void * cabp, uint16_t id, int16_t x, int16_t y, uint16_t r) {
		return ((CABP*)cabp)->addCommNode(id, x, y, r);
	}

	int cabp_add_noncomm_node(void * cabp, uint16_t id, int16_t x, int16_t y, uint16_t r) {
		return ((CABP*)cabp)->addNonCommNode(id, x, y, r);
	}

}
