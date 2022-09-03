#include <ncurses.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <locale.h>
#include <stdlib.h>
#define SETBIT(A,k) (A[(k)/8] |= (1 << ((k)%8)))
#define TESTBIT(A,k) (A[(k)/8] & (1 << ((k)%8)))
#define PIXELS_PER_ROW 24
#define PIXELS_PER_COL 10

int main()
{
    int cols, rows;
    float theta, phi;
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    getmaxyx(stdscr, rows, cols);
    int total = rows * cols;
    int bytes = (total + 7) / 8;
    uint8_t output[bytes]; //output bitmap
    float A = 0, B = 0; //  rotate around x-axis by A, z-axis by B
    // donut will be roughly centred at 75% of the screen
    float R1 = rows * PIXELS_PER_ROW * 1.0 / 8;
    float R2 = rows * PIXELS_PER_ROW * 2.0 / 8;

    for(;;) {
        memset(output, 0, bytes);
        float cosA = cos(A);
        float sinA = sin(A);
        float cosB = cos(B);
        float sinB = sin(B);
        for (theta = 0; theta < 6.28; theta += 0.07) {
            float costheta = cos(theta);
            float sintheta = sin(theta);
            for(phi = 0; phi < 6.28; phi += 0.02) {
                float cosphi = cos(phi);
        	float sinphi = sin(phi);
        	float circlex = R2 + R1 * costheta;
                float circley = R1 * sintheta;
        	float x = circlex*(cosB*cosphi + sinA*sinB*sinphi) - circley*cosA*sinB; 
                float y = circlex*(sinB*cosphi - sinA*cosB*sinphi) + circley*cosA*cosB;
		int proj_x = (int) (cols / 2 + x / PIXELS_PER_COL);
		int proj_y = (int) (rows / 2 - y / PIXELS_PER_ROW);
		int index = proj_x + cols * proj_y;
        	SETBIT(output, index);
            }
        }
        move(0,0);
        /* colour setting for nerd font if needed
	* start_color();			
	* init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	* attron(COLOR_PAIR(1));
	* attroff(COLOR_PAIR(1));
        * addstr("");
	*/
        for(int k = 0; k < total; k++) {
            if (TESTBIT(output, k)) {
                addstr("😂");
                k++; //depends on how many 'cursor' places the symbol takes
	    } else {
		addch(32);// ascii space
	    }
	}
        A += 0.07;
        B += 0.03;
        refresh();
        usleep(30000); //40000
    }
    endwin();
    return 0;
}
