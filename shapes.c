#include <ncurses.h>
#include <math.h>
#include <string.h>
#include "shapes.h"

#define SETBIT(A,k) (A[(k)/8] |= (1 << ((k)%8)))
#define TESTBIT(A,k) (A[(k)/8] & (1 << ((k)%8)))
extern int ppr, ppc, rows, cols, total, bytes;

void draw_donut(Donut *donut, WINDOW *win) {
    uint8_t output[bytes]; //output bitmap
    memset(output, 0, bytes);
    float cosA = cos(donut->x_rotate);
    float sinA = sin(donut->x_rotate);
    float cosB = cos(donut->z_rotate);
    float sinB = sin(donut->z_rotate);
    for (float theta = 0; theta < 6.28; theta += 0.07) {
        float costheta = cos(theta);
        float sintheta = sin(theta);
        for(float phi = 0; phi < 6.28; phi += 0.02) {
            float cosphi = cos(phi);
    	    float sinphi = sin(phi);
            float circlex = donut->R2 + donut->R1 * costheta;
            float circley = donut->R1 * sintheta;
    	    float x = circlex*(cosB*cosphi + sinA*sinB*sinphi) - circley*cosA*sinB; 
            float y = circlex*(sinB*cosphi - sinA*cosB*sinphi) + circley*cosA*cosB;
            int proj_x = (int) ((cols >> 1) + x / ppc);
            int proj_y = (int) ((rows >> 1) - y / ppr);
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
    donut->x_rotate += 0.07;
    donut->z_rotate += 0.03;
    wrefresh(win);
}

/*Julia's parametric heart surface equation is used here*/
void draw_heart(Heart *heart, WINDOW *win) {
    uint8_t output[bytes]; //output bitmap
    memset(output, 0, bytes);
    float cosA = cos(heart->x_rotate);
    float sinA = sin(heart->x_rotate);
    float cosB = cos(heart->z_rotate);
    float sinB = sin(heart->z_rotate);
    for (float u = 0; u < 6.28; u += 0.02) {
        float cosu = cos(u);
        float cos2u = cos(2 * u);
        float cos3u = cos(3 * u);
        float sinu = sin(u);
        float sin3u = sin(u * 3);
        for(float v = 0; v < 3.14; v += 0.02) {
            float cosv = cos(v);
            float sinv = sin(v);
	    float _x = heart->unit * sinv * (15 * sinu - 4 * sin3u);
	    float _y = heart->unit * sinv * (15 * cosu - 5 * cos2u - 2 * cos3u - cos2u);
	    float _z = heart->unit * 10 * cosv;
	    float x = _x * cosB + _y * sinB;
	    float y = cosA * (_y * cosB - _x * sinB) + _z * sinA;
            int proj_x = (int) ((cols >> 1) + x / ppc);
            int proj_y = (int) ((rows >> 1) - y / ppr);
            int index = proj_x + cols * proj_y;
            SETBIT(output, index);
	}
    }
    wmove(win, 0, 0);
    for(int k = 0; k < total; k++) {
        if (TESTBIT(output, k)) {
            waddstr(win, "❤ ");
            k++; //depends on how many 'cursor' places the symbol takes
        } else {
            waddch(win, 32);// ascii space
        }
    }
    heart->x_rotate += 0.07;
    heart->z_rotate -= 0.03;
    wrefresh(win);
}
