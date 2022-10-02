#include <ncurses.h>
#include <string.h>
#include "shapes.h"

#define SETBIT(A,k) (A[(k)/8] |= (1 << ((k)%8)))
#define TESTBIT(A,k) (A[(k)/8] & (1 << ((k)%8)))
extern int ppr, ppc, rows, cols, total, bytes;

/* only for drawing cube */
typedef struct Trig {
  float cosx;
  float sinx;
  float cosz;
  float sinz;
} Trig;

/* https://www.a1k0n.net/2021/01/13/optimizing-donut.html */
#define R(t,x,y) \
  _ = x; \
  x -= t*y; \
  y += t*_; \
  _ = (3-x*x-y*y)/2; \
  x *= _; \
  y *= _;

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
    float _;
    uint8_t output[bytes]; //output bitmap
    memset(output, 0, bytes);
    float cosz = donut->cosz;
    float sinz = donut->sinz;
    float cosx = donut->cosx;
    float sinx = donut->sinx;
    float costheta = 1.0;
    float sintheta = 0.0;
    for (int j = 0; j < 90; j++) {
        float cosphi = 1.0;
        float sinphi = 0.0;
        for(int i = 0; i < 314; i++) {
            float circlex = donut->R2 + donut->R1 * costheta;
            float circley = donut->R1 * sintheta;
            float x = circlex*(cosz*cosphi + sinx*sinz*sinphi) - circley*cosx*sinz;
            float y = circlex*(sinz*cosphi - sinx*cosz*sinphi) + circley*cosx*cosz;
            int proj_x = (int) ((cols >> 1) + x / ppc);
            int proj_y = (int) ((rows >> 1) - y / ppr);
    	    int index = proj_x + cols * proj_y;
    	    SETBIT(output, index);
	    R(0.02, cosphi, sinphi)
	}
	R(0.07, costheta, sintheta)
    }
    draw_char(output, "🍩", win);
    R(0.07, donut->cosx, donut->sinx)
    R(0.03, donut->cosz, donut->sinz)
}

/*Julia's parametric heart surface equation is used here*/
void draw_heart(Heart *heart, WINDOW *win) {
    float _;
    uint8_t output[bytes]; //output bitmap
    memset(output, 0, bytes);
    float cosz = heart->cosz;
    float sinz = heart->sinz;
    float cosx = heart->cosx;
    float sinx = heart->sinx;
    float cosu = 1.0;
    float sinu = 0.0;
    float cos3u = 1.0;
    float sin3u = 0.0;
    for (int i = 0; i < 314; i++) {
        float cos2u = 2 * cosu * cosu - 1;
        float cosv = 1.0;
        float sinv = 0.0;
        for(int j = 0; j < 157; j++) {
	    float _x = heart->unit * sinv * (15 * sinu - 4 * sin3u);
	    float _y = heart->unit * sinv * (15 * cosu - 5 * cos2u - 2 * cos3u - cos2u);
	    float _z = heart->unit * 10 * cosv;
	    float x = _x * cosz + _y * sinz;
	    float y = cosx * (_y * cosz - _x * sinz) + _z * sinx;
            int proj_x = (int) ((cols >> 1) + x / ppc);
            int proj_y = (int) ((rows >> 1) - y / ppr);
            int index = proj_x + cols * proj_y;
            SETBIT(output, index);
	    R(0.02, cosv, sinv)
	}
	R(0.02, cosu, sinu)
	R(0.06, cos3u, sin3u)
    }
    draw_char(output, "💞", win);
    R(0.07, heart->cosx, heart->sinx)
    R(-0.03, heart->cosz, heart->sinz);
}

void cube_store(uint8_t *output, float _x, float _y, float _z, Trig *trig) {
    float x = _x * trig->cosz + _y * trig->sinz;
    float y = trig->cosx * (_y * trig->cosz - _x * trig->sinz) + _z * trig->sinx;
    float z = -trig->sinx * (_y * trig->cosz - _x * trig->sinz) + _z * trig->cosx;
    x += 0.2239 * z;
    y += 0.4471 * z;
    int proj_x = (int) ((cols >> 1) + x / ppc);
    int proj_y = (int) ((rows >> 1) - y / ppr);
    int index = proj_x + cols * proj_y;
    SETBIT(output, index);
}

void draw_cube(Cube *cube, WINDOW *win) {
    float _;
    uint8_t output[bytes]; //output bitmap
    float half = cube->side;
    Trig trig;
    trig.cosx = cube->cosx;
    trig.sinx = cube->sinx;
    trig.cosz = cube->cosz;
    trig.sinz = cube->sinz;
    memset(output, 0, bytes);
    for (float x = -half; x < half; x += cube->step) {
        for (float z = -half; z < half; z += cube->step) {
          cube_store(output, x, -half, z, &trig);
          cube_store(output, x, half, z, &trig);
          cube_store(output, -half, x, z, &trig);
          cube_store(output, half, x, z, &trig);
          cube_store(output, x, -z, -half, &trig);
          cube_store(output, x, -z, half, &trig);
        }
    }
    draw_char(output, "🧊", win);
    R(-0.07, cube->cosx, cube->sinx)
    R(0.03, cube->cosz, cube->sinz)
}

/* https://en.wikipedia.org/wiki/Fast_inverse_square_root */
float Q_rsqrt(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;
    x2 = number * 0.5F;
    y  = number;
    i  = * (long *) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - (i >> 1);             // what the fuck?
    y  = * (float *) &i;
    y  = y * (threehalfs - (x2 * y * y));   // 1st iteration
    return y;
}

void draw_knot(Knot *knot, WINDOW *win) {
    float _;
    uint8_t output[bytes]; //output bitmap
    memset(output, 0, bytes);
    float cosy = knot->cosy;
    float siny = knot->siny;
    float cosu = 1.0;
    float sinu = 0.0;
    /* p, q should not be too large here
     * angle works when tanx ~= x */
    float phi_angle = knot->q * 0.01;
    float theta_angle = knot->p * 0.01;
    for (int i = 0; i < 314; i++) {
        float hu = knot->R3 * cosu;
        float vu = knot->R3 * sinu;
        float cosphi = 1.0;
        float sinphi = 0.0;
        float costheta = 1.0;
        float sintheta = 0.0;
        for (int j = 0; j < 628; j++) {
	    float c1 = knot->R2 + knot->R1 * cosphi;
	    float c2 = knot->q * knot->R1;
	    float k1 = c1 * costheta;
	    float k2 = c1 * sintheta;
	    float k3 = knot->R1 * sinphi;
	    float n1 = costheta * cosphi;
	    float n2 = sintheta * cosphi;
	    float n3 = sinphi;
	    float _t1 = -c2 * sinphi * costheta - knot->p * c1 * sintheta;
	    float _t2 = -c2 * sinphi * sintheta + knot->p * c1 * costheta;
	    float _t3 = c2 * cosphi;
	    float r_length = Q_rsqrt(_t1 * _t1 + _t2 * _t2 + _t3 * _t3);
	    float t1 = _t1 * r_length;
	    float t2 = _t2 * r_length;
	    float t3 = _t3 * r_length;
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
	    R(phi_angle, cosphi, sinphi)
	    R(theta_angle, costheta, sintheta)
        }
	R(0.02, cosu, sinu)
    }
    draw_char(output, "🌺", win);
    R(0.03, knot->cosy, knot->siny)
}

void draw_cone(Cone *cone, WINDOW *win) {
    float _;
    uint8_t output[bytes]; //output bitmap
    memset(output, 0, bytes);
    float cosz = cone->cosz;
    float sinz = cone->sinz;
    float cosx = cone->cosx;
    float sinx = cone->sinx;
    float H = cone->H;
    float r = cone->r;
    float cosu = 1.0;
    float sinu = 0.0;
    for (int i = 0; i < 314; i++) {
        for (float h = -H; h < H; h += cone->step) {
            float _x = h / H * r * cosu;
            float _z = h / H * r * sinu;
            float x = _x * cosz + h * sinz;
            float y = cosx * (h * cosz - _x * sinz) + _z * sinx;
            int proj_x = (int) ((cols >> 1) + x / ppc);
            int proj_y = (int) ((rows >> 1) - y / ppr);
            int index = proj_x + cols * proj_y;
            SETBIT(output, index);
	}
	R(0.02, cosu, sinu)
    }
    draw_char(output, "🥦", win);
    R(-0.07, cone->cosx, cone->sinx)
    R(0.03, cone->cosz, cone->sinz)
}
