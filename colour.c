#include "colour.h"

#define PATH_MOD(t) {if(t>=PATHLEN) t-=PATHLEN;}

/* Colour fadey functions */
void glColour(Colour *c) {
	glColor4dv(c);
}
Colour fade(Colour *c,int i) {
	Colour out = {
		c->r,
		c->g,
		c->b,
		c->a*((double)i)/((double)PATHLEN)
	};
	return out;
}
Colour randColour() {
	double r = 1.0-0.5*(((double)rand())/((double)RAND_MAX)),
	g = 1.0-0.5*(((double)rand())/((double)RAND_MAX)),
	b = 1.0-0.5*(((double)rand())/((double)RAND_MAX)),
	a = 1.0;
	Colour out = {r,g,b,a};
	return out;
	/*
	Colour c;
	c.r = 1.0;
	c.g = 0.0;
	c.b = 0.0;
	c.a = 1.0;
	return c;
	*/
}