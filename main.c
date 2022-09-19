#include <ncurses.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "shapes.h"
#include "zoom.h"

int ppr = 0, ppc = 0; // pixels_per_row, pixels_per_column
int rows, cols; // rows_per_window, cols_per_window
int total; // how many grids in one window
int bytes; // how much memory (bytes) needed for one window to do bitwise operations

static int w_resize = 0; // resize caused by adjusting window size
static int f_resize = 0; // resize caused by adjusting font size
static int to_finish = 0;
static int ppr_min = 5, ppr_max = 27;
static int keypress;
static useconds_t delay = 40000 ;
static struct winsize winsz;
static Donut donut = {0.0};
static Heart heart = {0.0};
static Cube cube = {0.0};
static Knot knot = {0.0};
static WINDOW *windows[4];

void clear_screen() {
    for (int i = 0; i < 4; i++) {
        delwin(windows[i]);
    }
    touchwin(stdscr);
    refresh();
}

void create_windows() {
    for (int i = 0; i < 4; i++) {
	windows[i] = newwin(rows, cols, i / 2 * (1 + rows), i % 2 * (1 + cols));
    }
    refresh();
}

void setup() {
    //if it is the first time setup being called
    if (!ppr) {
        ioctl(STDIN_FILENO, TIOCGWINSZ, &winsz);
    }
    ppr = winsz.ws_ypixel / winsz.ws_row;
    ppc = winsz.ws_xpixel / winsz.ws_col;
    rows = winsz.ws_row >> 1;
    cols = winsz.ws_col >> 1;
    total = rows * cols;
    bytes = (total + 7) >> 3;
    // objects will be roughly centred at 75% of the screen
    int range = winsz.ws_ypixel > winsz.ws_xpixel ? winsz.ws_xpixel >> 1 : winsz.ws_ypixel >> 1;
    donut.R1 = range * 0.125; // 0.375 * (1 / 3)
    donut.R2 = range * 0.25; //  0.375 * (2 / 3)
    heart.unit = range * 0.01875; // 0.375 / 20
    cube.side = range * 0.23; // between 0.375 / sqrt2 and 0.375 / sqrt3
    cube.step = 1.0 * range / 150; // total 300 points
    knot.R1 = range * 0.15; // 0.375 * 0.4
    knot.R2 = range * 0.225; // 0.375 * 0.6
    knot.R3 = knot.R1 * 0.1;
    //(2, 5)-torus knot
    knot.p = 2;
    knot.q = 5;
}

void finish() {
    clear_screen();
    curs_set(1);
    endwin();
    system(zoom_back);
    exit(0);
}

void sig_handler(int sig) {
    if (sig == SIGINT || sig == SIGQUIT || sig == SIGTSTP)
        finish();
    if (sig == SIGWINCH) {
        if (!ttyname(0))
            return;
        if (ioctl(STDIN_FILENO, TIOCGWINSZ, &winsz) == -1)
            return;
	if (keypress == 'k' || keypress == 'j') {
	    setup();
            resizeterm(winsz.ws_row, winsz.ws_col);
	    create_windows();
            f_resize = 0;
	    return;
	}
	if (ppr == winsz.ws_ypixel / winsz.ws_row) {
	    w_resize = 1;
	    return;
	}
	// font adjusted without 'k' or 'j', just quit
	to_finish = 1;
    }
}

int main(void) {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0); 
    timeout(0);
    
    signal(SIGINT, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGWINCH, sig_handler);
    signal(SIGTSTP, sig_handler);

    Donut *p0 = &donut;
    Heart *p1 = &heart;
    Cube *p2 = &cube;
    Knot *p3 = &knot;
    setup();
    create_windows();

    while (1) {
	while (to_finish) {finish();}
	while (w_resize) {
	    clear_screen();
	    setup();
	    if (winsz.ws_row >= 2 && winsz.ws_col >= 2) {
                resizeterm(winsz.ws_row, winsz.ws_col);
                create_windows();
                w_resize = 0;
	    }
	}
        while (f_resize);
        if ((keypress = wgetch(stdscr)) == ERR ) {
	    draw_donut(p0, windows[0]);
	    draw_heart(p1, windows[1]);
	    draw_cube(p2, windows[2]);
	    draw_knot(p3, windows[3]);
	    usleep(delay);
        } else {
	    switch (keypress) {
                case 'q':
		    to_finish = 1;
		    break;
		case 'j':
		    if (ppr > ppr_min) {
			f_resize = 1;
                        clear_screen();
		        system(zoom_out);
		    } 
		    break;
		case 'k':
		    if (ppr < ppr_max) {
			f_resize = 1;
                        clear_screen();
		        system(zoom_in);
		    }
	    	    break;
	    }
        }
    }
    return 0;
}
