#include <ncurses.h>

int main()
{
	initscr();
        noecho();
	getch();
	endwin();
	return 0;
}
