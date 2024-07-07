#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <locale.h>
#include <stdio.h>
#include <time.h>

#include "blib.c"

int width, height;

int dim_init = 0;

void get_dimensions(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	height = w.ws_row*4, width = w.ws_col*2;

	dim_init = 1;
}

void sleepms(int t){
	struct timespec ts;
	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;
	nanosleep(&ts, &ts);
}

void set_echo(int s){
	struct termios term;
	tcgetattr(fileno(stdin), &term);
	
	if(s){
		term.c_lflag |= ECHO;
		tcsetattr(fileno(stdin), 0, &term);
		printf("\033[?25h");
		fflush(stdout);
	}else{
		term.c_lflag &= ~ECHO;
		tcsetattr(fileno(stdin), 0, &term);
		printf("\033[?25l");
		fflush(stdout);
	}
}

struct bgrid *screen;

void initscr(){
	setlocale(LC_CTYPE, "");

	if(!dim_init) get_dimensions();

	screen = balloc(width, height);
	bclear(screen);

	printf("\033c");
	fflush(stdout);

	set_echo(0);
}

void clear(){
	bclear(screen);
}

void point(int x, int y, int v){
	bset(screen, x, y, v);
}

void draw(){
	printf("\033[0;0H");
	
	for(int i=0; i<height/4; ++i)
		for(int j=0; j<width/2; ++j)
			printf("%lc", bchar(screen, j, i));

	fflush(stdout);
}

void endscr(){
	bfree(screen);

	printf("\033c");
	fflush(stdout);
	set_echo(1);
}
