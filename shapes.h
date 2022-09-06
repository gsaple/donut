void *donut(void *args);

typedef struct Args {
  WINDOW *win;
  int pixels_per_row;
  int pixels_per_col;
  float x_rotate;
  float z_rotate;
} Args;
