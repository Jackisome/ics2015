#include "hal.h"

#define NR_KEYS 18

enum {KEY_STATE_EMPTY, KEY_STATE_WAIT_RELEASE, KEY_STATE_RELEASE, KEY_STATE_PRESS};

/* Only the following keys are used in NEMU-PAL. */
static const int keycode_array[] = {
	K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE,
	K_RETURN, K_SPACE, K_PAGEUP, K_PAGEDOWN, K_r,
	K_a, K_d, K_e, K_w, K_q,
	K_s, K_f, K_p
};

static int key_state[NR_KEYS];
//static int l_key_state[NR_KEYS];

void
keyboard_event(void) {
	/* DONE: Fetch the scancode and update the key states. */
	
	uint32_t key_code = in_byte(0x60);
	int i;
	for(i = 0; i < NR_KEYS; i++) {
	    if((key_code & 0x7f) == keycode_array[i]) {
			if(key_code & 0x80) key_state[i] = KEY_STATE_RELEASE;
			else key_state[i] = KEY_STATE_PRESS;
			break;
	    }
	}

}

static inline int
get_keycode(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return keycode_array[index];
}

static inline int
query_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return key_state[index];
}

static inline void
release_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_WAIT_RELEASE;
}

static inline void
clear_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_EMPTY;
}

bool 
process_keys(void (*key_press_callback)(int), void (*key_release_callback)(int)) {
	cli();

	int i;
	for(i = 0; i < NR_KEYS; i++) {
	    if(query_key(i) == KEY_STATE_PRESS) {
			key_press_callback(get_keycode(i));
			release_key(i);
			sti();
			return true;
	    } else if(query_key(i) == KEY_STATE_RELEASE) {
			key_release_callback(get_keycode(i));
			clear_key(i);
			sti();
			return true;	    
		}
	}

	sti();
	return false;
}
