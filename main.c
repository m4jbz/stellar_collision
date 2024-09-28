#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"

#define WIDTH 800 // panel width
#define HEIGHT 600 // panel height
#define H_WIDTH WIDTH/2.0f // half width
#define H_HEIGHT HEIGHT/2.0f // half height
#define PART_COUNT 20*1024 // amount of particles
#define RADIUS 50.0f 
#define HOLLOW_PURPLE (Color) { 192, 0, 192, 255 }

typedef struct {
	Vector2 pos; // position
	Vector2 vel;
	Color color;
} Particle;

// generates all particles initial position
Particle gen_particles(Particle particle, Vector2 center, Color color);
Vector2 attract(Vector2 pos, Vector2 vel, Vector2 center_pos, float value);
Vector2 get_normal(Vector2 pos, Vector2 otherPos);
Vector2 do_friction(Vector2 vel, float amount);
Vector2 move(Vector2 pos, Vector2 vel, int mode);
float get_dist(Vector2 pos, Vector2 otherPos);
// free all mememory
void free_all(Particle** p, Particle* c);
// main draw function
void draw(Particle** p, Particle* c);
// verifies if malloc fails
int mem_is_safe(Particle* p);

int main()
{
	Particle** particles = malloc(2 * sizeof(Particle*));
	Particle* centers = malloc(2 * sizeof(Particle));
	
	for (int i = 0; i < 2; ++i)
	{
		particles[i] = malloc(PART_COUNT * sizeof(Particle));
		if (!mem_is_safe(particles[i])) return 1;
	}

	if (!mem_is_safe(centers)) return 1;

	centers[0].pos = (Vector2) { 100.0f, H_HEIGHT };
	centers[1].pos = (Vector2) { WIDTH - 100.0f, H_HEIGHT };
	centers[0].color = BLUE;
	centers[1].color = RED;
	
	for (int i = 0; i < PART_COUNT; ++i)
	{
		particles[0][i] = gen_particles(particles[0][i], centers[0].pos, BLUE);
		particles[1][i] = gen_particles(particles[1][i], centers[1].pos, RED);
	}

	draw(particles, centers);

	free_all(particles, centers);

	return 0;
}

Particle gen_particles(Particle particle, Vector2 center, Color color)
{
	float angle = ((float)rand() / RAND_MAX) * 2.0f * PI;
	float distance = ((float)rand() / RAND_MAX) * RADIUS;

	particle.pos.x = center.x + distance * cos(angle);
	particle.pos.y = center.y + distance * sin(angle);
    particle.vel.x = GetRandomValue(-100, 100) / 10000.f;
    particle.vel.y = GetRandomValue(-100, 100) / 10000.f;
	particle.color = color;

	return particle;
}

Vector2 do_friction(Vector2 vel, float amount)
{
	vel.x *= amount;
	vel.y *= amount;

	return vel;
}

float get_dist(Vector2 pos, Vector2 otherPos) {
    const float dx = pos.x - otherPos.x;
    const float dy = pos.y - otherPos.y;

    return sqrt((dx*dx) + (dy*dy));
}

Vector2 get_normal(Vector2 pos, Vector2 otherPos) {
    float dist = get_dist(pos, otherPos);

    if (dist == 0.0f) dist = 1;

    const float dx = pos.x - otherPos.x;
    const float dy = pos.y - otherPos.y;
    Vector2 normal = (Vector2){dx*(1/dist), dy*(1/dist)};

    return normal;
}

Vector2 attract(Vector2 pos, Vector2 vel, Vector2 center_pos, float value)
{
    float dist = fmax(get_dist(pos, center_pos), value);
    Vector2 normal = get_normal(pos, center_pos);

    vel.x -= normal.x/dist;
    vel.y -= normal.y/dist;

	return vel;
}

Vector2 move(Vector2 pos, Vector2 vel, int mode) {
	if (mode == 1)
		pos.x += vel.x-0.05f;

	if (mode == 0)
		pos.x += vel.x+0.05f;

	pos.y += vel.y;

    if (pos.x < 0) pos.x += WIDTH;
    if (pos.x >= WIDTH) pos.x -= WIDTH;
    if (pos.y < 0) pos.y += WIDTH;
    if (pos.y >= WIDTH) pos.y -= WIDTH;

	return pos;
}

void draw(Particle** p, Particle* c)
{
	InitWindow(WIDTH, HEIGHT, "Window");

	SetTargetFPS(100);

	int flag = 1;
//	int frameNumber = 1;

	while (!WindowShouldClose())
	{
		for (int i = 0; i < PART_COUNT; ++i)
		{
			p[0][i].vel = attract(p[0][i].pos, p[0][i].vel, c[0].pos, 0.2f);
			p[1][i].vel = attract(p[1][i].pos, p[1][i].vel, c[1].pos, 0.2f);

			p[0][i].vel = do_friction(p[0][i].vel, 0.9f);
			p[1][i].vel = do_friction(p[1][i].vel, 0.9f);

			p[0][i].pos = move(p[0][i].pos, p[0][i].vel, 1);
			p[1][i].pos = move(p[1][i].pos, p[1][i].vel, 0);
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (int i = 0; i < PART_COUNT; ++i)
		{
			DrawPixelV(p[0][i].pos, p[0][i].color);
			DrawPixelV(p[1][i].pos, p[1][i].color);
		}

		for (int i = 0; i < 2; ++i)
			DrawPixelV(c[i].pos, c[i].color);
		
		if (flag)
		{
			for (int i = 0; i < PART_COUNT; ++i)
			{
				p[0][i].pos.x += 2;
				p[1][i].pos.x -= 2;
			}

			c[0].pos.x += 2;
			c[1].pos.x -= 2;
		}

		if (c[0].pos.x+10.0f == c[1].pos.x-10.0f)
		{
			c[0].pos.x += 10.0f;
			c[1].pos.x -= 10.0f;

			for (int i = 0; i < PART_COUNT; ++i)
			{
				DrawPixelV(p[0][i].pos, HOLLOW_PURPLE);
				DrawPixelV(p[1][i].pos, HOLLOW_PURPLE);
			}
			flag = 0;

			for (int i = 0; i < PART_COUNT; ++i)
			{
				p[0][i].vel = attract(p[0][i].pos, p[0][i].vel, c[0].pos, 0.15f);
				p[1][i].vel = attract(p[1][i].pos, p[1][i].vel, c[1].pos, 0.15f);

				p[0][i].vel = do_friction(p[0][i].vel, 0.99f);
				p[1][i].vel = do_friction(p[1][i].vel, 0.99f);

				p[0][i].pos = move(p[0][i].pos, p[0][i].vel, 0);
				p[1][i].pos = move(p[1][i].pos, p[1][i].vel, 1);
			}

			c[0].pos.x -= 10.0f;
			c[1].pos.x += 10.0f;
		}

		if (H_WIDTH - c[0].pos.x < RADIUS && flag)
		{
			for (int i = 0; i < PART_COUNT; ++i)
			{
				if (p[0][i].pos.x > c[0].pos.x)
					p[0][i].pos = move(p[0][i].pos, p[0][i].vel, 1);

				if (p[1][i].pos.x < c[1].pos.x)
					p[1][i].pos = move(p[1][i].pos, p[1][i].vel, 0);
			}
		}

//		char filename[64];
//		snprintf(filename, 64, "screenshot_%04d.png", frameNumber);
//		TakeScreenshot(filename);  // Saves the current frame as an image
//		frameNumber++;

		EndDrawing();
	}

	CloseWindow();
}

int mem_is_safe(Particle* p)
{
	if (p == NULL)
		return 0;

	return 1;
}

void free_all(Particle** p, Particle* c)
{
	for (int i = 0; i < 2; ++i)
		free(p[i]);

	free(c);
}
