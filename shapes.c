#include <ncurses.h>
#include <math.h>
#include <string.h>
#include "shapes.h"

#define SETBIT(A,k) (A[(k)/8] |= (1 << ((k)%8)))
#define TESTBIT(A,k) (A[(k)/8] & (1 << ((k)%8)))
extern int ppr, ppc, rows, cols, total, bytes;

void draw_char(uint8_t *output, char *emoji, WINDOW *win) {
    wmove(win, 0, 0);
    for(int k = 0; k < total; k++) {
        if (TESTBIT(output, k)) {
            waddstr(win, emoji);
            k++; //depends on how many 'cursor' places the symbol takes
        } else {
            waddch(win, 32);// ascii space
        }
    }
    wrefresh(win);
}

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
    draw_char(output, "🍩", win);
    donut->x_rotate += 0.07;
    donut->z_rotate += 0.03;
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
    draw_char(output, "❤ ", win);
    heart->x_rotate += 0.07;
    heart->z_rotate -= 0.03;
}

void map_store(uint8_t *output, float _x, float _y, float _z, Trig *trig) {
    float x = _x * trig->cosB + _y * trig->sinB;
    float y = trig->cosA * (_y * trig->cosB - _x * trig->sinB) + _z * trig->sinA;
    float z = -trig->sinA * (_y * trig->cosB - _x * trig->sinB) + _z * trig->cosA;
    x += 0.2239 * z;
    y += 0.4471 * z;
    int proj_x = (int) ((cols >> 1) + x / ppc);
    int proj_y = (int) ((rows >> 1) - y / ppr);
    int index = proj_x + cols * proj_y;
    SETBIT(output, index);
}

void draw_cube(Cube *cube, WINDOW *win) {
    uint8_t output[bytes]; //output bitmap
    float half = cube->side;
    Trig trig;
    trig.cosA = cos(cube->x_rotate);
    trig.sinA = sin(cube->x_rotate);
    trig.cosB = cos(cube->z_rotate);
    trig.sinB = sin(cube->z_rotate);
    memset(output, 0, bytes);
    for (float x = -half; x < half; x += cube->step) {
        for (float z = -half; z < half; z += cube->step) {
          map_store(output, x, -half, z, &trig);
          map_store(output, x, half, z, &trig);
          map_store(output, -half, x, z, &trig);
          map_store(output, half, x, z, &trig);
          map_store(output, x, -z, -half, &trig);
          map_store(output, x, -z, half, &trig);
        }
    }
    draw_char(output, "🧊", win);
    cube->x_rotate -= 0.07;
    cube->z_rotate += 0.03;
}

void draw_knot(Knot *knot, WINDOW *win) {
    uint8_t output[bytes]; //output bitmap
    memset(output, 0, bytes);
    float cosy = cos(knot->y_rotate);
    float siny = sin(knot->y_rotate);
    for (float u = 0; u < 6.28; u += 0.02) {
        float hu = knot->R3 * cos(u);
        float vu = knot->R3 * sin(u);
        for (float t = 0; t < 6.28; t += 0.01) {
	    float phi = knot->q * t;
	    float theta = knot->p * t;
	    float cosphi = cos(phi);
	    float sinphi = sin(phi);
	    float costheta = cos(theta);
	    float sintheta = sin(theta);
	    float c1 = knot->R2 + knot->R1 * cosphi;
	    float c2 = knot->q * knot->R1;
	    float k1 = c1 * cos(theta);
	    float k2 = c1 * sin(theta);
	    float k3 = knot->R1 * sin(phi);
	    float n1 = costheta * cosphi;
	    float n2 = sintheta * cosphi;
	    float n3 = sinphi;
	    float _t1 = -c2 * sinphi * costheta - knot->p * c1 * sintheta;
	    float _t2 = -c2 * sinphi * sintheta + knot->p * c1 * costheta;
	    float _t3 = c2 * cosphi;
	    float length = sqrt(_t1 * _t1 + _t2 * _t2 + _t3 * _t3);
	    float t1 = _t1 / length;
	    float t2 = _t2 / length;
	    float t3 = _t3 / length;
	    float b1 = t2 * n3 - n2 * t3;
	    float b2 = n1 * t3 - t1 * n3;
	    float b3 = t1 * n2 - n1 * t2;
	    float _x = k1 + hu * n1 + vu * b1;
	    float _y = k2 + hu * n2 + vu * b2;
	    float _z = k3 + hu * n3 + vu * b3;
	    float x = cosy * _x - siny * _z;
            int proj_x = (int) ((cols >> 1) + x / ppc);
            int proj_y = (int) ((rows >> 1) - _y / ppr);
            int index = proj_x + cols * proj_y;
            SETBIT(output, index);
        }
    }
    draw_char(output, "🌺", win);
    knot->y_rotate += 0.03;
}
