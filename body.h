#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "vector.h"
#include "colour.h"

#ifndef BODY_H
#define BODY_H 1

#define G 1e-8
#define PATH_MOD(t) {if(t>=PATHLEN) t-=PATHLEN;}

typedef struct {
	Vector *point;
	size_t pos;
} Path;

typedef struct {
	Vector position;
	Vector velocity;
	Vector acceleration;
	double mass;
	Path path;
	Colour colour;
} Body;

void startPath(Body*);
void drawPath(Body*,Vector*,int);
void endPath(Body*);
void traverse(Body*,void (*start)(Body*),void (*cb)(Body*,Vector*,int),void (*end)(Body*));

Body newBody(Vector, Vector, double);
double newt(double, double);
bool collide(Body*, Body*, int, int);
Vector move(Body*, Body*, int, int);
void circle(Vector*, double, Colour*);

#endif