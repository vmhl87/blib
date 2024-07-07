#include <signal.h>
#include <stdio.h>

#include "blib.c"
#include "bgui.c"

volatile sig_atomic_t signal_status = 0;
void sighandler(int s) {signal_status = s;}

int main(){
	initscr();

	float pt[4] = {0., height/2., 2., -2.};

	signal(SIGINT, sighandler); signal_status = 0;
	while(1){
		if(signal_status == SIGINT) break;

		pt[3] += 0.1;

		for(int i=0; i<2; ++i) pt[i] += pt[i+2];

		if(pt[0] < 0.) pt[0] = 0., pt[2] *= -.8;
		if(pt[1] < 0.) pt[1] = 0., pt[3] *= -.8;

		if(pt[0] > width-1.) pt[0] = width-1., pt[2] *= -.8;
		if(pt[1] > height-1.) pt[1] = height-1., pt[3] *= -.8, pt[2] *= .8;

		clear();
		point((int)pt[0], (int)pt[1], 1);
		draw();
		sleepms(1000/40);
	}
	signal(SIGINT, SIG_DFL);

	endscr();
}
