#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#define coord(x, y) (x + (y)*sz)
#define block(x, y) (grid[coord(x, y)/32])
#define idx(x, y) (1 << (coord(x, y) & 31))
#define at(x, y) (block(x, y) & idx(x, y))
#define set(x, y, v) if(v){block(x, y) |= idx(x, y);}else{block(x, y) = ~((~block(x, y)) | idx(x, y));}

int main(){
	setlocale(LC_CTYPE, "");
	
	const int sz = 64, map[4][2] = {{1, 8}, {2, 16}, {4, 32}, {64, 128}};

	unsigned int *grid = malloc(sizeof(int)*(1+((sz*sz-1)/32)));
	for(int *p=grid, i=0; i<1+((sz*sz-1)/32); ++i, ++p) *p = 0;

	for(int i=0; i<sz; ++i) for(int j=0; j<sz; ++j) set(j, i, (j&1)^(i&1));

	for(int i=10; i<sz-10; ++i) for(int j=10; j<sz-10; ++j) set(j, i, 0);

	set(30, 29, 1);
	set(33, 29, 1);
	set(30, 30, 1);
	set(33, 30, 1);

	for(int i=30; i<34; ++i) set(i, 33, 1);
	set(29, 32, 1);
	set(34, 32, 1);

	wchar_t *out = malloc(1+sizeof(wchar_t)*(1+sz/2)*sz/4), *p = out;

	for(int i=0; i<sz/4; ++i){
		for(int j=0; j<sz/2; ++j){
			*p = 0x2800;
			for(int c=0; c<2; ++c) for(int r=0; r<4; ++r)
				if(at(j*2+c, i*4+r)) *p += map[r][c];
			++p;
		}
		*p = '\n', ++p;
	}

	*p = 0;

	wprintf(L"%ls", out);

	free(grid), free(out);
}
