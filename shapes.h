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

typedef struct Trig {
  float cosA;
  float sinA;
  float cosB;
  float sinB;
} Trig;

typedef struct Cube {
  float x_rotate;
  float z_rotate;
  float side;
  float step;
} Cube;

typedef struct Knot {
  float y_rotate;
  float R1;
  float R2;
  float R3;
  int p;
  int q;
} Knot;

typedef struct Cone {
  float x_rotate;
  float z_rotate;
  float H;
  float r;
  float step;
} Cone;

void draw_char(uint8_t *output, char *emoji, WINDOW *win);
void draw_donut(Donut *donut, WINDOW *win);
void draw_heart(Heart *heart, WINDOW *win);
void cube_store(uint8_t *output, float x, float y, float z, Trig *trig);
void draw_cube(Cube *cube, WINDOW *win);
void draw_knot(Knot *knot, WINDOW *win);
void draw_cone(Cone *cone, WINDOW *win);
