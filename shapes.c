#include <ncurses.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "shapes.h"

#define SETBIT(A,k) (A[(k)/8] |= (1 << ((k)%8)))
#define TESTBIT(A,k) (A[(k)/8] & (1 << ((k)%8)))

void donut(Args *args) {
    WINDOW *win = args->win;
    int ppr = args->pixels_per_row;
    int ppc = args->pixels_per_col;
    float xr = args->x_rotate;
    float zr = args->z_rotate;
    int cols, rows;
    getmaxyx(win, rows, cols);
    int total = rows * cols;
    int bytes = (total + 7) / 8;
    uint8_t output[bytes]; //output bitmap
	
    // donut will be roughly centred at 75% of the screen
    float R1 = rows * ppr * 1.0 / 8;
    float R2 = rows * ppr * 2.0 / 8;

    memset(output, 0, bytes);
    float cosA = cos(xr);
    float sinA = sin(xr);
    float cosB = cos(zr);
    float sinB = sin(zr);
    for (float theta = 0; theta < 6.28; theta += 0.07) {
        float costheta = cos(theta);
        float sintheta = sin(theta);
        for(float phi = 0; phi < 6.28; phi += 0.02) {
            float cosphi = cos(phi);
    	    float sinphi = sin(phi);
    	    float circlex = R2 + R1 * costheta;
            float circley = R1 * sintheta;
    	    float x = circlex*(cosB*cosphi + sinA*sinB*sinphi) - circley*cosA*sinB; 
            float y = circlex*(sinB*cosphi - sinA*cosB*sinphi) + circley*cosA*cosB;
    	    int proj_x = (int) (cols / 2 + x / ppc);
    	    int proj_y = (int) (rows / 2 - y / ppr);
    	    int index = proj_x + cols * proj_y;
    	    SETBIT(output, index);
	}
    }
    wmove(win, 0, 0);
    for(int k = 0; k < total; k++) {
        if (TESTBIT(output, k)) {
            waddstr(win, "🍩");
            k++; //depends on how many 'cursor' places the symbol takes
        } else {
            waddch(win, 32);// ascii space
        }
    }
    args->x_rotate += 0.07;
    args->z_rotate += 0.03;
    wrefresh(win);
    usleep(5000);
}
