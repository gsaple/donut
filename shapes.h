typedef struct Donut {
  float cosz;
  float sinz;
  float cosx;
  float sinx;
  float R1;
  float R2;
} Donut;

typedef struct Heart {
  float cosz;
  float sinz;
  float cosx;
  float sinx;
  float unit;
} Heart;

typedef struct Cube {
  float cosz;
  float sinz;
  float cosx;
  float sinx;
  float side;
  float step;
} Cube;

typedef struct Knot {
  float cosy;
  float siny;
  float R1;
  float R2;
  float R3;
  int p;
  int q;
} Knot;

typedef struct Cone {
  float cosz;
  float sinz;
  float cosx;
  float sinx;
  float H;
  float r;
  float step;
} Cone;

void draw_donut(Donut *donut, WINDOW *win);
void draw_heart(Heart *heart, WINDOW *win);
void draw_cube(Cube *cube, WINDOW *win);
void draw_knot(Knot *knot, WINDOW *win);
void draw_cone(Cone *cone, WINDOW *win);
