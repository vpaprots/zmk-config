
typedef int (*draw_one_pixel)(int position, int* setR, int* setG, int* setB, draw_one_pixel next); //int? int32?

draw_one_pixel animations[4]; //macro for length? = {static initialization?}

// define matrix? row+col vs position
// [ 1  5   9  -1]
// [ 2  6  10  16]
// [ -1 -1 11  17]

int prepare_next_buffer(/* buf */) {
	wait(REFRESH|BUFFER_FREE);
	clear_events();
	//buf  =next_buf;
	for (int p = 0; p< cfg->pixels; p++) {
		int r = 0, g = 0, b = 0;
		for (int f = 0; f<4; f++) { //macro for length
			if (0<animations[f](p, &r, &g, &b)) {
				break;
			}
		}
		write_to_buf(buf, p, r, g, b);
	}
	set_event(BUF_READY);
}

K_TIMER_DEFINE(frame_timer, NULL, NULL);

int draw_pixels (){
	// clear_events();
	// dispatch
	// sleep(80);
	// event(buf_ready);
	// wait(buf_ready)
	char* cur_buf = cfg->buf1;
	while(1) { //can disable?
		k_timer_start(&frame_timer, K_USEC(80), K_NO_WAIT);
		int events = k_event_wait(&my_event, 0xFFF, false, K_FOREVER);
		prepare_next_buffer(cur_buf);
		k_timer_status_sync(&frame_timer);
		dispatch_buffer(cur_buf);
	}
}


void sample_timer() {
	
	// ...
	/* do first protocol operation */
	// ...

	/* start one shot timer that expires after 500 ms */
	k_timer_start(&my_sync_timer, K_MSEC(500), K_NO_WAIT);

	/* do other work */
	// ...

	/* ensure timer has expired (waiting for expiry, if necessary) */
	k_timer_status_sync(&my_sync_timer);
	/* do second protocol operation */
}

void init() {
	// create work queue
}

int animation() {

}

int backglow() {

}

int bqstatus() {

}

int key_press_animation() {

}