#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <emscripten.h>

typedef struct {
    double x;           
    double y;           
    int isMoving;               
} Particle;

Particle *particles;
int n;
int width;
int height;
double length;
double r;

EM_JS(void, draw_particle, 
    (double x, double y, double r, int isMoving), {
        drawParticle(x, y, r, isMoving);
    }
)

double randomNum() {
    return (double)rand() / (double)RAND_MAX;
}

EMSCRIPTEN_KEEPALIVE
void initParticles(int amount) {
    n = amount;
    particles = malloc(sizeof(Particle) * n);
    for (int i=0; i < n; i++) {
        particles[i].x = randomNum() * width;
        particles[i].y = randomNum() * height;
        particles[i].isMoving = 1;
    }
}

EMSCRIPTEN_KEEPALIVE
void reInitParticles(int amount) {
    n = amount;
    particles = realloc(particles, sizeof(Particle) * n);
    for (int i=0; i < n; i++) {
        particles[i].x = randomNum() * width;
        particles[i].y = randomNum() * height;
        particles[i].isMoving = 1;

    }
}

EMSCRIPTEN_KEEPALIVE
void freeMemory() {
    free(particles);
}

void doCollision(Particle* p, int i) {
    for (int j = 0; j < n; j++) {
        if (i == j) continue;  
        if ( sqrt(pow(p->x - particles[j].x, 2) + pow(p->y - particles[j].y, 2)) <= 2*r ) {
            p->isMoving = 0;
       }
    }
}

EMSCRIPTEN_KEEPALIVE
void releaseAll_wasm() {
    for (int i = 0; i < n; i++) particles[i].isMoving = 1;
}

void randomMove(Particle* p) {
    double v = randomNum() * 2 * M_PI;
    p->x += length * cos(v);
    p->y += length * sin(v);
}

void updateParticle(Particle* p, int i) {
    // move
    if (p->isMoving) randomMove(p);
    
    // wall collision
    if (p->x >= width) {
        p->x = width - r;
        p->isMoving = 0;
    } else if (p->x <= r) {
        p->x = r;
        p->isMoving = 0;
    }  
    if (p->y >= height) {
        p->y = height - r;
        p->isMoving = 0;
    } else if (p->y <= r) {
        p->y = r;
        p->isMoving = 0;
    }

    // particle collision
    doCollision(p, i);

    // draw particle
    draw_particle(p->x, p->y, r, p->isMoving);
}

EMSCRIPTEN_KEEPALIVE
void update() {
    for (int i = 0; i < n; i++) {
        updateParticle(&particles[i], i);
    }
}

EMSCRIPTEN_KEEPALIVE
void setVariables(int w, int h, double l, double rad) {
    width = w;
    height = h;
    length = l;
    r = rad;
}

int main() {
    return 0;
}