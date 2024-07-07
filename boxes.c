#include <signal.h>
#include <stdio.h>
#include <math.h>

#include "blib.c"
#include "bgui.c"

// keyboardinterrupt handler

volatile sig_atomic_t signal_status = 0;
void sighandler(int s) {signal_status = s;}

// vector math

struct vec2{
	float x, y;
};

struct vec2 nvec2(float x, float y){
	struct vec2 ret;
	ret.x = x, ret.y = y;
	return ret;
}

struct vec2 vadd(struct vec2 a, struct vec2 b){
	return nvec2(a.x+b.x, a.y+b.y);
}

struct vec2 vmult(struct vec2 a, float f){
	return nvec2(a.x*f, a.y*f);
}

float vmag(struct vec2 v){
	return sqrtf(v.x*v.x + v.y*v.y);
}

struct vec2 vrotate(struct vec2 v, float r){
	return nvec2(v.x*cos(r) - v.y*sin(r), v.x*sin(r) + v.y*cos(r));
}

// collider utils

struct collider{
	unsigned int ptc;
	struct vec2 *pts;

	float mass;

	struct vec2 p, v;
	float a, r;
};

struct vec2 screenpos(struct collider *c, unsigned int i){
	return vadd(c->p, vrotate(c->pts[i], c->a));
}

const float PI = 3.14159265;

struct vec2 screenvel(struct collider *c, unsigned int i){
	return vadd(c->v, vmult(vrotate(c->pts[i], c->a + PI/2.), c->r));
}

const float bounce = 0.1,
	  friction = 0.5,
	  rot_inert = 3.;

void applyforce(struct collider *c, struct vec2 f, struct vec2 o){
	c->v = vadd(c->v, f);
	c->r += (o.x*f.y - o.y*f.x) / rot_inert / c->mass;
}

void iterate(struct collider *c, float dt){
	c->v.y += 0.07*dt;

	c->p = vadd(c->p, vmult(c->v, dt));
	c->a += c->r*dt;

	for(int i=0; i<c->ptc; ++i){
		struct vec2 p = screenpos(c, i),
					v = screenvel(c, i);

		if(p.y > height-2.5 && v.y > 0){
			c->p.y += height-2.5 - p.y;
			struct vec2 f = nvec2(v.x*-friction, -v.y*(1+bounce));
			applyforce(c, f, vadd(p, vmult(c->p, -1)));
		}
	}
}

void display(struct collider *c){
	for(unsigned int i=0; i<c->ptc; ++i){
		int o = c->ptc - 1;
		if(i) o = i-1;
		
		struct vec2 p1 = screenpos(c, i),
					p2 = screenpos(c, o);

		line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
	}

	point((int)c->p.x, (int)c->p.y, 1);
}

// main

int main(){
	initscr();

	struct collider square;

	square.ptc = 4;
	square.pts = malloc(sizeof(struct vec2) * 4);

	square.pts[0] = nvec2(-10, -10);
	square.pts[1] = nvec2(-10, 10);
	square.pts[2] = nvec2(10, 10);
	square.pts[3] = nvec2(10, -10);
	
	square.p = nvec2(width/2, height/2);
	square.v = nvec2(0, 0);
	
	square.a = 0.3;
	square.r = 0;

	square.mass = 100;

	struct collider s;

	s.ptc = 3;
	s.pts = malloc(sizeof(struct vec2) * 3);

	s.pts[0] = nvec2(0, -7);
	s.pts[1] = nvec2(6, 3.5);
	s.pts[2] = nvec2(-6, 3.5);
	
	s.p = nvec2(width/2+50, height/2);
	s.v = nvec2(0, 0);
	
	s.a = 0.3;
	s.r = 0;

	s.mass = 30;

	signal(SIGINT, sighandler); signal_status = 0;
	while(1){
		if(signal_status == SIGINT) break;

		for(int i=0; i<10; ++i)
			iterate(&square, 0.1), iterate(&s, 0.1);

		clear();
		display(&square);
		display(&s);
		draw();

		sleepms(1000/60);
	}
	signal(SIGINT, SIG_DFL);

	endscr();
}
