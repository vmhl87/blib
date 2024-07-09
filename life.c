#include <signal.h>
#include <locale.h>
#include <stdio.h>

#include "blib.c"
#include "bgui.c"

volatile sig_atomic_t signal_status = 0;
void sighandler(int s) {signal_status = s;}

int main(){
	srand(time(NULL));

	get_dimensions();

	int new[width][height], old[width][height];
	for(int i=0; i<width; ++i) for(int j=0; j<height; ++j)
		old[i][j] = rand() & 1;

	initscr();

	signal(SIGINT, sighandler), signal_status = 0;

	while(1){
		if(signal_status == SIGINT) break;

		for(int x=0; x<width; ++x) for(int y=0; y<height; ++y){
			int n = -old[x][y];
			for(int a=-1; a<2; ++a) for(int b=-1; b<2; ++b)
				n += old[(x+a+width)%width][(y+b+height)%height];
			if(old[x][y]) new[x][y] = n == 2 || n == 3;
			else new[x][y] = n == 3;
		}

		for(int x=0; x<width; ++x) for(int y=0; y<height; ++y)
			old[x][y] = new[x][y];

		clear();
		for(int x=0; x<width; ++x) for(int y=0; y<height; ++y)
			point(x, y, old[x][y]);
		draw();

		sleepms(1000/30);
	}

	signal(SIGINT, SIG_DFL);

	endscr();
}
