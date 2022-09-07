#include <ncurses.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include "shapes.h"

volatile sig_atomic_t signal_status = 0;

/* resize screen logic is recycled from the code found at
 * https://github.com/abishekvashok/cmatrix
 */
void resize_screen(Args *args) {
    char *tty;
    int fd = 0;
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
    args->pixels_per_row = winsz.ws_ypixel / winsz.ws_row;
    args->pixels_per_col = winsz.ws_xpixel / winsz.ws_col;
    resizeterm(winsz.ws_row < 10 ? 10 : winsz.ws_row,
		    winsz.ws_col < 10 ? 10 : winsz.ws_col);
    clear();
    refresh();
}

void sig_handler(int s) {
    signal_status = s;
}

void finish(void) {
    curs_set(1);
    clear();
    refresh();
    endwin();
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
    
    struct winsize winsz;
    ioctl(0, TIOCGWINSZ, &winsz);

    Args *args; 
    args->pixels_per_row = winsz.ws_ypixel / winsz.ws_row;
    args->pixels_per_col = winsz.ws_xpixel / winsz.ws_col;
    args->win = stdscr;
    args->x_rotate = 0;
    args->z_rotate = 0;
    pthread_t thread_id;
    char keypress;

    while (1) {
        if (signal_status == SIGINT || signal_status == SIGQUIT || signal_status == SIGTSTP) {
            finish();
            break;
        }
        if (signal_status == SIGWINCH) {
            resize_screen(args);
            signal_status = 0;
        }
        if ((keypress = wgetch(stdscr)) == ERR) {
            pthread_create(&thread_id, NULL, donut, (void *) args);
            pthread_join(thread_id, NULL);
        } else {
	    if (keypress == 'q') {
                finish();
                break;
	    }
        }
    }
    return 0;
}
