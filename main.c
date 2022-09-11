#include <ncurses.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "shapes.h"
#include "zoom.h"

/* global variables */
volatile sig_atomic_t signal_status = 0;
int ppr, ppc; // pixels_per_row, pixels_per_column
int rows, cols; // rows_per_window, cols_per_window
int total; // how many grids in one window
int bytes; // how much memory (bytes) needed for one window to do bitwise operations
Donut donut = {0.0};
WINDOW *windows[4];

void setup() {
    struct winsize winsz;
    ioctl(0, TIOCGWINSZ, &winsz);
    ppr = winsz.ws_ypixel / winsz.ws_row;
    ppc = winsz.ws_xpixel / winsz.ws_col;
    rows = winsz.ws_row >> 1;
    cols = winsz.ws_col >> 1;
    total = rows * cols;
    bytes = (total + 7) / 8;
    //int height, width;
    //height = (winsz.ws_row < 10 ? 10 : winsz.ws_row) / 2;
    //width = (winsz.ws_col < 10 ? 10 : winsz.ws_col) / 2;
    for (int i = 0; i < 4; i++) {
	windows[i] = newwin(rows, cols, i / 2 * (1 + rows), i % 2 * (1 + cols) );
    }
    // donut will be roughly centred at 75% of the screen
    donut.R1 = rows * ppr * 1.0 / 8;
    donut.R2 = rows * ppr * 2.0 / 8;
    refresh();
}

/* resize screen logic is recycled from the code found at
 * https://github.com/abishekvashok/cmatrix
 */
//void resize_screen(Args *lty[]) {
//    char *tty;
//    int result = 0;
//    struct winsize winsz;
//    tty = ttyname(0);
//    if (!tty) {
//        return;
//    }
//    result = ioctl(STDIN_FILENO, TIOCGWINSZ, &winsz);
//    if (result == -1) {
//        return;
//    }
//    int height = (winsz.ws_row < 10 ? 10 : winsz.ws_row) / 2;
//    int width = (winsz.ws_col < 10 ? 10 : winsz.ws_col) / 2;
//    resizeterm(height * 2, width * 2);
//    clear();
//    refresh();
//    for (int i = 0; i < 4; i++) {
//        lty[i]->pixels_per_row = winsz.ws_ypixel / winsz.ws_row;
//        lty[i]->pixels_per_col = winsz.ws_xpixel / winsz.ws_col;
//        delwin(lty[i]->win);
//        lty[i]->win = newwin(height, width, i / 2 * (1 + height), i % 2 * (1 + width) );
//    }
//}

void sig_handler(int s) {
    signal_status = s;
}

void finish() {
    for (int i = 0; i < 4; i++) {
        delwin(windows[i]);
    }
    curs_set(1);
    clear();
    refresh();
    endwin();
    system(zoom_back);
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

    char keypress;
    setup();

    while (1) {
        if (signal_status == SIGINT || signal_status == SIGQUIT || signal_status == SIGTSTP) {
            finish();
            break;
        }
        if (signal_status == SIGWINCH) {
            //resize_screen(layout);
            signal_status = 0;
        }
        if ((keypress = wgetch(stdscr)) == ERR) {
        //if ((keypress = wgetch(stdscr)) == 'a') {
	    draw_donut(p0, windows[0]);
	    draw_donut(p0, windows[1]);
	    draw_donut(p0, windows[2]);
	    draw_donut(p0, windows[3]);
        } else {
	    switch (keypress) {
                case 'q':
		    goto exit_loop;
                    break;
		case 'j':
		    clear();
		    refresh();
		    system(zoom_out);
		    break;
		case 'k':
		    clear();
		    refresh();
		    system(zoom_in);
		    break;
	    }
        }
    }
    exit_loop:
        finish();
    return 0;
}
