#include <stdlib.h>
#include <wchar.h>

// generate backing array with size
struct bgrid* bgen(int width, int height);

// clear backing
void bres(struct bgrid *grid);

// set point (x, y) to bool/int v
void bset(struct bgrid *grid, int x, int y, int v);

// compute braille character at x, y (in characters, not dots)
wchar_t bat(struct bgrid *grid, int x, int y);

// delete backing array
void bdel(struct bgrid *grid);

// braille grid structure
struct bgrid{
	unsigned int *grid;
	int width, height, size;
};

struct bgrid* bgen(int width, int height){
	struct bgrid *new = malloc(sizeof(struct bgrid));
	new -> width = (1+(width-1)/2)<<1;
	new -> height = (1+(height-1)/4)<<2;
	new -> size = 1+(new->width*new->height-1>>5);
	new -> grid = malloc(sizeof(unsigned int)*new->size);
	return new;
}

void bres(struct bgrid *grid){
	for(int i=0; i<grid->size; ++i) grid -> grid[i] = 0;
}

void _bwr(unsigned int *grid, int coord, int v){
	if(v) grid[coord>>5] |= 1 << (coord & 31);
	else grid[coord>>5] &= ~(1 << (coord & 31));
}

void bset(struct bgrid *grid, int x, int y, int v){
	_bwr(grid->grid, x + y*grid->width, v);
}

int _bget(unsigned int *grid, int coord){
	return (grid[coord>>5] >> (coord & 31)) & 1;
}

const int _ptmap[8] = {0, 3, 1, 4, 2, 5, 6, 7};

wchar_t bat(struct bgrid *grid, int x, int y){
	wchar_t ret = 0x2800;
	int base = (x<<1) + (y<<2)*grid->width;
	const int *p = _ptmap;

	for(int i=0; i<8; base += grid->width*(i&1), ++i)
		ret |= _bget(grid->grid, base + (i&1)) << *p, ++p;

	return ret;
}

void bdel(struct bgrid *grid){
	free(grid -> grid), free(grid);
}
