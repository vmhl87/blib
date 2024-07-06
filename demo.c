#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include "blib.c"

int main(){
	setlocale(LC_CTYPE, "");
	
	const int sz = 64;
	struct bgrid *grid = bgen(sz, sz);
	bres(grid);

	for(int i=0; i<sz; ++i) for(int j=0; j<sz; ++j) bset(grid, j, i, (j&1)^(i&1));

	for(int i=10; i<sz-10; ++i) for(int j=10; j<sz-10; ++j) bset(grid, j, i, 0);

	bset(grid, 30, 29, 1);
	bset(grid, 33, 29, 1);
	bset(grid, 30, 30, 1);
	bset(grid, 33, 30, 1);

	for(int i=30; i<34; ++i) bset(grid, i, 33, 1);
	bset(grid, 29, 32, 1);
	bset(grid, 34, 32, 1);

	wchar_t *out = malloc(sizeof(wchar_t)*(1+grid->width/2));
	out[grid->width/2] = 0;

	for(int i=0; i<grid->height/4; ++i){
		for(int j=0; j<grid->width/2; ++j) out[j] = bat(grid, j, i);
		printf("%ls\n", out);
	}

	free(out), bdel(grid);
}
