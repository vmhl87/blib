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
	size_t ptc;
	struct vec2 *pts;

	float mass, radius;

	struct vec2 p, v;
	float a, r;
};

struct vec2 screenpos(struct collider *c, size_t i){
	return vadd(c->p, vrotate(c->pts[i], c->a));
}

const float PI = 3.14159265358979;

struct vec2 screenvel(struct collider *c, size_t i){
	return vadd(c->v, vmult(vrotate(c->pts[i], c->a + PI/2.), c->r));
}

const float bounce = 0,
	  friction = .3,
	  damping = 100.;

void applyforce(struct collider *c, struct vec2 f, struct vec2 o){
	c->v = vadd(c->v, vmult(f, 1/c->mass));
	c->r += (o.x*f.y - o.y*f.x)/c->mass/c->radius/damping;
}

void iterate(struct collider *c, float dt){
	c->p = vadd(c->p, vmult(c->v, dt));
	c->a += c->r*dt;

	c->v.y += 0.07*dt;

	for(size_t i=0; i<c->ptc; ++i){
		struct vec2 p = screenpos(c, i),
					v = screenvel(c, i);

		if(p.y > height-.5)
			applyforce(c, nvec2(0, -0.07*dt*c->mass), vadd(p, vmult(c->p, -1)));

		if(p.y > height-.5 && v.y > 0){
			struct vec2 f = nvec2(v.x*-friction, -v.y*(1+bounce));
			f = vmult(f, c->mass);
			applyforce(c, f, vadd(p, vmult(c->p, -1)));
		}

		if(p.x > width-.5 && v.x > 0){
			struct vec2 f = nvec2(-v.x*(1+bounce), v.y*-friction);
			f = vmult(f, c->mass);
			applyforce(c, f, vadd(p, vmult(c->p, -1)));
		}

		if(p.y < .5 && v.y < 0){
			struct vec2 f = nvec2(v.x*-friction, -v.y*(1+bounce));
			f = vmult(f, c->mass);
			applyforce(c, f, vadd(p, vmult(c->p, -1)));
		}

		if(p.x < .5 && v.x < 0){
			struct vec2 f = nvec2(-v.x*(1+bounce), v.y*-friction);
			f = vmult(f, c->mass);
			applyforce(c, f, vadd(p, vmult(c->p, -1)));
		}
	}
}

void display(struct collider *c){
	for(size_t i=0; i<c->ptc; ++i){
		size_t o = c->ptc - 1;
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

	square.pts[0] = nvec2(-20, -1);
	square.pts[1] = nvec2(-20, 1);
	square.pts[2] = nvec2(20, 1);
	square.pts[3] = nvec2(20, -1);
	
	square.p = nvec2(width/2, height/2);
	square.v = nvec2(0, 0);
	
	square.a = 0.3;
	square.r = 0;

	square.mass = 400;
	square.radius = 12;

	struct collider s;

	s.ptc = 3;
	s.pts = malloc(sizeof(struct vec2) * 3);

	s.pts[0] = nvec2(0, -7);
	s.pts[1] = nvec2(6, 3.5);
	s.pts[2] = nvec2(-6, 3.5);
	
	s.p = nvec2(width/2+50, height/2);
	s.v = nvec2(0, 0);
	
	s.a = 0;
	s.r = -0.2;

	s.mass = 60;
	s.radius = 1;

	struct collider circle;

	circle.ptc = 32;
	circle.pts = malloc(sizeof(struct vec2) * 32);

	for(int i=0; i<32; ++i) circle.pts[i] = vrotate(nvec2(0, 8), PI/16.*i);

	circle.p = nvec2(width/2 - 50, height/2);
	circle.v = nvec2(0, 0);

	circle.a = 0;
	circle.r = 0.03;

	circle.mass = 200;
	circle.radius = 24;

	const int iterations = 10;
	const float dt = 0.1;

	signal(SIGINT, sighandler); signal_status = 0;
	while(1){
		if(signal_status == SIGINT) break;

		for(int i=0; i<iterations; ++i)
			iterate(&square, dt), iterate(&s, dt), iterate(&circle, dt);

		clear();
		display(&square);
		display(&s);
		display(&circle);
		draw();

		sleepms(1000/60);
	}
	signal(SIGINT, SIG_DFL);

	endscr();
}
