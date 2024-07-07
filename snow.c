#include <signal.h>
#include <stdio.h>
#include <math.h>

#include "blib.c"
#include "bgui.c"

volatile sig_atomic_t signal_status = 0;
void sighandler(int s) {signal_status = s;}

int main(){
	srand(time(NULL));

	initscr();

	int ct = width*height/400;
	float points[ct][4];
	for(int i=0; i<ct; ++i){
		points[i][0] = rand() % width;
		points[i][1] = rand() % height;
		points[i][2] = (rand() % 2000 - 1000) / 700.;
		points[i][3] = (rand() % 2000 - 1000) / 700.;
	}

	signal(SIGINT, sighandler); signal_status = 0;
	while(1){
		if(signal_status == SIGINT) break;

		clear();
		for(int j=0; j<ct; ++j){
			points[j][0] += points[j][2];
			points[j][1] += points[j][3];
			if(points[j][0] < 0) points[j][0] = width-1;
			if(points[j][1] < 0) points[j][1] = height-1;
			if(points[j][0] >= width) points[j][0] = 0.;
			if(points[j][1] >= height) points[j][1] = 0.;
			point((int)points[j][0], (int)points[j][1], 1);
		}
		draw();
		sleepms(1000/40);
	}
	signal(SIGINT, SIG_DFL);

	endscr();
}
