typedef struct Donut {
  float x_rotate;
  float z_rotate;
  float R1;
  float R2;
} Donut;

typedef struct Heart {
  float x_rotate;
  float z_rotate;
  float unit;
} Heart;

void draw_donut(Donut *donut, WINDOW *win);
void draw_heart(Heart *heart, WINDOW *win);
