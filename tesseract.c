#include <signal.h>
#include <math.h>

#include "blib.c"
#include "bgui.c"

volatile sig_atomic_t status = 0;
void sighandler(int s) {status = s;}

struct vec2{
	float x, y;
};

struct vec3{
	float x, y, z;
};

struct vec2 nvec2(float x, float y){
	struct vec2 ret;
	ret.x = x, ret.y = y;
	return ret;
}

struct vec3 nvec3(float x, float y, float z){
	struct vec3 ret;
	ret.x = x, ret.y = y, ret.z = z;
	return ret;
}

struct vec3 vmult(struct vec3 v, float f){
	return nvec3(v.x*f, v.y*f, v.z*f);
}

struct vec3 vadd(struct vec3 a, struct vec3 b){
	return nvec3(a.x+b.x, a.y+b.y, a.z+b.z);
}

struct vec3 vcross(struct vec3 a, struct vec3 b){
	return nvec3(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

struct vec3 vnorm(struct vec3 v){
	float mag = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	return nvec3(v.x/mag, v.y/mag, v.z/mag);
}

float vdot(struct vec3 a, struct vec3 b){
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

struct vec2 project(struct vec3 p, struct vec3 c){
	struct vec3 fwd = vnorm(vmult(c, -1.)),
				side = vcross(fwd, nvec3(0, 1, 0)),
				up = vcross(fwd, side);

	float px = vdot(side, p),
		  py = vdot(up, p),
		  pz = vdot(fwd, vadd(p, vmult(c, -1.)));

	return nvec2(px/pz, py/pz);
}

void line(int x0, int y0, int x1, int y1){
	int dx = abs(x1 - x0),
		sx = x0 < x1 ? 1 : -1,
		dy = -abs(y1 - y0),
		sy = y0 < y1 ? 1 : -1,
		err = dx + dy;

	while(1){
		point(x0, y0, 1);
		if(x0 == x1 && y0 == y1) break;
		int e2 = 2 * err;
		if(e2 >= dy){
			if(x0 == x1) break;
			err += dy;
			x0 += sx;
		}
		if(e2 <= dx){
			if(y0 == y1) break;
			err += dx;
			y0 += sy;
		}
	}
}

int main(){
	initscr();

	float t = 100;

	signal(SIGINT, sighandler), status = 0;
	while(1){
		if(status == SIGINT) break;

		clear();

		float x = t/40;
		struct vec3 camera = nvec3(cos(x)*10*.8, sin(x/2)*4*.8, sin(x)*10*.8);

		int path[32][3] = {
			{-2,2,2},{-2,2,-2},{2,2,-2},{2,2,2},
			{2,-2,2},{2,-2,-2},{-2,-2,-2},{-2,-2,2},
			{-1,-1,1},{-1,1,1},{-1,1,-1},{-1,-1,-1},
			{1,-1,-1},{1,1,-1},{1,1,1},{1,-1,1},
			{1,-1,-1},{2,-2,-2},{2,2,-2},{1,1,-1},
			{-1,1,-1},{-2,2,-2},{-2,-2,-2},{-1,-1,-1},
			{-1,-1,1},{1,-1,1},{2,-2,2},{-2,-2,2},
			{-2,2,2},{2,2,2},{1,1,1},{-1,1,1}
		};

		for(int i=0; i<32; ++i){
			int o = (i+1)%32;

			struct vec2 p1 = project(nvec3(path[i][0], path[i][1], path[i][2]), camera),
						p2 = project(nvec3(path[o][0], path[o][1], path[o][2]), camera);

			p1.x *= (float)fmin(height, width), p1.y *= (float)fmin(height, width);
			p1.x += (float)width/2, p1.y += (float)height/2;
			p2.x *= (float)fmin(height, width), p2.y *= (float)fmin(height, width);
			p2.x += (float)width/2, p2.y += (float)height/2;

			line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		}

		draw();

		sleepms(1000/40);

		++t;
	}
	signal(SIGINT, SIG_DFL);

	endscr();
}
