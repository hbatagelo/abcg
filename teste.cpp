#include <curses.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void wurf(double v0, double alpha) {
  double x, y;
  double xw;
  double tx;
  double pi = 3.14159;

  alpha = alpha * (2 * pi / 360);

  xw = ((v0 * v0) * sin(2 * alpha)) / 9.81;
  printf("%lf\n", xw);

  for (x = 0; x <= xw; x++) {
    tx = x / (v0 * cos(alpha));
    y = -0.5 * 9.81 * (tx * tx) + v0 * sin(alpha) * tx;

    if ((int)y > 0) {
      move((int)x, 25 - (int)y);
      addch('*');
      refresh();
    }
  }
}

int main(int argc, char *argv[]) {
  WINDOW *mainwin;

  if ((mainwin = initscr()) == NULL) {
    fprintf(stderr, "ERROR INITIALISING NCURSES.\n");
    exit(EXIT_FAILURE);
  }

  wurf(20, 45);

  getch();

  delwin(mainwin);
  endwin();
  refresh();

  return EXIT_SUCCESS;
}