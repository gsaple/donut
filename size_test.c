#include <sys/ioctl.h>
#include <stdio.h>
#include <signal.h>
static void sig_handler(int sig) {
  if (SIGWINCH == sig) {
    struct winsize winsz;
    ioctl(0, TIOCGWINSZ, &winsz);
    printf("current terminal window has: %d rows, %d columns.\n", 
		    winsz.ws_row, winsz.ws_col);
    printf("%d rows use %d pixels vertically, %d columns uses %d pixels horizontally.\n", 
		    winsz.ws_row, winsz.ws_ypixel, winsz.ws_col, winsz.ws_xpixel);
    printf("hence pixels per row: %d, pixels per col: %d\n\n", 
		    winsz.ws_ypixel/winsz.ws_row, winsz.ws_xpixel/winsz.ws_col);
  }
}

int main(int argc, char **argv) {
  signal(SIGWINCH, sig_handler);
  while (1) {} // control c to exit
}
