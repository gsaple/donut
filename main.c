#include <ncurses.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "shapes.h"
#include "zoom.h"

static int to_draw = 1;
static int to_resize = 0;

int ppr = 0, ppc = 0; // pixels_per_row, pixels_per_column
int old_ppr = 0;
int ppr_min = 6, ppr_max = 27;
int rows, cols; // rows_per_window, cols_per_window
int total; // how many grids in one window
int bytes; // how much memory (bytes) needed for one window to do bitwise operations
Donut donut = {0.0};
WINDOW *windows[4];
int keypress;

void setup() {
    //if it is the first time setup being called
    if (!ppr) {
        struct winsize winsz;
        ioctl(STDIN_FILENO, TIOCGWINSZ, &winsz);
        ppr = winsz.ws_ypixel / winsz.ws_row;
        old_ppr = ppr;
        ppc = winsz.ws_xpixel / winsz.ws_col;
        rows = winsz.ws_row >> 1;
        cols = winsz.ws_col >> 1;
        total = rows * cols;
        bytes = (total + 7) / 8;
    }
    // donut will be roughly centred at 75% of the screen
    donut.R1 = rows * ppr * 1.0 / 8;
    donut.R2 = rows * ppr * 2.0 / 8;

    for (int i = 0; i < 4; i++) {
	windows[i] = newwin(rows, cols, i / 2 * (1 + rows), i % 2 * (1 + cols) );
    }
    refresh();
}

void clear_screen() {
    for (int i = 0; i < 4; i++) {
        delwin(windows[i]);
    }
    touchwin(stdscr);
    refresh();
}

/* some of the admin logic is recycled from the code found at
 * https://github.com/abishekvashok/cmatrix
 */
void resize_screen() {
    char *tty;
    int result = 0;
    struct winsize winsz;
    tty = ttyname(0);
    if (!tty) {
        return;
    }
    result = ioctl(STDIN_FILENO, TIOCGWINSZ, &winsz);
    if (result == -1) {
        return;
    }
    ppr = winsz.ws_ypixel / winsz.ws_row;

    if (keypress == ERR) {
        // corner case: directly zoom in or zoom out without using 'j' or 'k'
        if (ppr != old_ppr) {
            ppr = old_ppr;
            return;
	}
	// should be when window is resized by frame size, not by font size
        clear_screen();
    }
    old_ppr = ppr;
    ppc = winsz.ws_xpixel / winsz.ws_col;
    rows = winsz.ws_row >> 1;
    cols = winsz.ws_col >> 1;
    total = rows * cols;
    bytes = (total + 7) / 8;
    resizeterm(winsz.ws_row, winsz.ws_col);
    setup();
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
        resize_screen();
        to_resize = 0;
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
    setup();

    while (1) {
        while (to_resize);
        if ((keypress = wgetch(stdscr)) == ERR ) {
	    draw_donut(p0, windows[0]);
	    draw_donut(p0, windows[1]);
	    draw_donut(p0, windows[2]);
	    draw_donut(p0, windows[3]);
        } else {
	    switch (keypress) {
                case 'q':
		    finish();
		    break;
		case 'j':
		    if (ppr > ppr_min) {
			to_resize = 1;
                        clear_screen();
		        system(zoom_out);
		    } 
		    break;
		case 'k':
		    if (ppr < ppr_max) {
			to_resize = 1;
                        clear_screen();
		        system(zoom_in);
		    }
	    	    break;
	    }
        }
    }
    return 0;
}
