#include <ncurses.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "shapes.h"

volatile sig_atomic_t signal_status = 0;

void setup(Args *lty[]) {
    struct winsize winsz;
    ioctl(0, TIOCGWINSZ, &winsz);
    int height, width;
    height = (winsz.ws_row < 10 ? 10 : winsz.ws_row) / 2;
    width = (winsz.ws_col < 10 ? 10 : winsz.ws_col) / 2;
    for (int i = 0; i < 4; i++) {
        lty[i] = malloc(sizeof(Args));
        lty[i]->pixels_per_row = winsz.ws_ypixel / winsz.ws_row;
        lty[i]->pixels_per_col = winsz.ws_xpixel / winsz.ws_col;
        lty[i]->x_rotate = 0;
        lty[i]->z_rotate = 0;
	lty[i]->win = newwin(height, width, i / 2 * (1 + height), i % 2 * (1 + width) );
    }
    lty[0]->win = newwin(height, width, 0, 0);
    lty[1]->win = newwin(height, width, 0, width + 1);
    lty[2]->win = newwin(height, width, height + 1, 0);
    lty[3]->win = newwin(height, width, height + 1, width + 1);
    refresh();
}

/* resize screen logic is recycled from the code found at
 * https://github.com/abishekvashok/cmatrix
 */
void resize_screen(Args *lty[]) {
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
    int height = (winsz.ws_row < 10 ? 10 : winsz.ws_row) / 2;
    int width = (winsz.ws_col < 10 ? 10 : winsz.ws_col) / 2;
    resizeterm(height * 2, width * 2);
    clear();
    for (int i = 0; i < 4; i++) {
        lty[i]->pixels_per_row = winsz.ws_ypixel / winsz.ws_row;
        lty[i]->pixels_per_col = winsz.ws_xpixel / winsz.ws_col;
        delwin(lty[i]->win);
        lty[i]->win = newwin(height, width, i / 2 * (1 + height), i % 2 * (1 + width) );
    }
    refresh();
}

void sig_handler(int s) {
    signal_status = s;
}

void finish(Args *lty[]) {
    for (int i = 0; i < 4; i++) {
        delwin(lty[i]->win);
        free(lty[i]);
    }
    curs_set(1);
    clear();
    refresh();
    endwin();
    system("xdotool key Control+Home");
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
    Args *layout[4];
    char keypress;
    setup(layout);

    while (1) {
        if (signal_status == SIGINT || signal_status == SIGQUIT || signal_status == SIGTSTP) {
            finish(layout);
            break;
        }
        if (signal_status == SIGWINCH) {
            resize_screen(layout);
            signal_status = 0;
        }
        if ((keypress = wgetch(stdscr)) == ERR) {
	    donut(layout[0]);
	    donut(layout[1]);
	    donut(layout[2]);
	    donut(layout[3]);
        } else {
	    if (keypress == 'q') {
                finish(layout);
                break;
	    }
        }
    }
    return 0;
}
