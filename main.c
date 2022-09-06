#include <ncurses.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <signal.h>
#include <pthread.h>
#include "shapes.h"

volatile sig_atomic_t signal_status = 0;

//void resize_screen() {
//    char *tty;
//    int fd = 0;
//    int result = 0;
//    struct winsize win;
//
//    tty = ttyname(0);
//    if (!tty) {
//        return;
//    }
//    result = ioctl(0, TIOCGWINSZ, &win);
//    if (result == -1) {
//        return;
//    }
//
//    COLS = win.ws_col;
//    LINES = win.ws_row;
//
//    if (LINES < 10) {
//        LINES = 10;
//    }
//    if (COLS < 10) {
//        COLS = 10;
//    }
//    resizeterm(LINES, COLS);
//}

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
            //resize_screen();
            signal_status = 0;
        }
        if ((keypress = wgetch(stdscr)) == ERR) {
            pthread_create(&thread_id, NULL, donut, (void *) args);
            pthread_join(thread_id, NULL);
        } else {
            finish();
            break;
        }
    }
    return 0;
}
