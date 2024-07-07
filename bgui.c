#ifndef BGUI_EXISTS
#define BGUI_EXISTS

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <locale.h>
#include <stdio.h>
#include <time.h>

#include "blib.c"

// initialize / cleanup braille interface
void initscr();
void endscr();

// drawing utilities - set (x, y) to v [0 or 1]
void point(int x, int y, int v);
// line from (x0, y0) <-> (x1, y1)
void line(int x0, int y0, int x1, int y1);
// clear screen
void clear();
// write to terminal
void draw();

// utilities - pause for t milliseconds
void sleepms(int t);

int width, height;

void get_dimensions(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	height = w.ws_row*4, width = w.ws_col*2;
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

	get_dimensions();

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

void line(int x0, int y0, int x1, int y1){
	int dx = abs(x1 - x0),
		sx = x0 < x1 ? 1 : -1,
		dy = -abs(y1 - y0),
		sy = y0 < y1 ? 1 : -1,
		err = dx + dy;

	while(1){
		bset(screen, x0, y0, 1);
		if(x0 == x1 && y0 == y1) break;
		int e2 = 2 * err;
		if(e2 >= dy){
			if(x0 == x1) break;
			err += dy, x0 += sx;
		}
		if(e2 <= dx){
			if(y0 == y1) break;
			err += dx, y0 += sy;
		}
	}
}

void draw(){
	wchar_t out[1+height/4*width/2], *p = out;
	
	for(int i=0; i<height/4; ++i)
		for(int j=0; j<width/2; ++j)
			*p = bchar(screen, j, i), ++p;
	*p = 0;

	printf("\033[0;0H%ls", out);
	fflush(stdout);
}

void endscr(){
	bfree(screen);

	printf("\033c");
	fflush(stdout);
	set_echo(1);
}

#endif
