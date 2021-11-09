#ifndef RAYLIB_H
#include <raylib.h>
#endif
#include <stdlib.h>
#include <raymath.h>
#include <math.h>

#define WIDTH 1280
#define HEIGHT 720
#define N_CIRCLES 5
#define RADIUS_CIRCLES 50

typedef struct circle{
	Vector2 pos;
	Vector2 v;
	float radius;
} Circle;

Circle *circles[255];
int circle_cnt = 0;
float speed_mult = 1.0F;
float circle_speed = 50.0F;
int paused = 0;
int debug = 0;
int square_size = 10;
int selected_circle = -1;

float
sample_pos(int x, int y)
{
	float value = 0;
	Vector2 pos = (Vector2) {x, y};
	for (int i = 0; i < circle_cnt; ++i) {
		float dst = Vector2Distance(circles[i]->pos, pos);
		value += (circles[i]->radius * circles[i]->radius)/(dst*dst);
	}

	return value;
}

short
get_num(float a, float b, float c, float d)
{
	return ((a > 1.0F) << 3) +
		((b > 1.0F) << 2) +
		((c > 1.0F) << 1) +
		(d > 1.0F);
}

float
get_lerp(float a, float b, int square_size)
{
	return Lerp(0, square_size, (1.0F - a)/(b - a));
}

void
add_circle(void)
{
	if (circle_cnt == 255) return;
	Circle *ptr = malloc(sizeof(Circle));
	ptr->pos = (Vector2) {WIDTH / 2, HEIGHT / 2};
	ptr->v = Vector2Rotate(Vector2One(), (float)rand() / (float)(RAND_MAX/360.0F));
	ptr->radius = RADIUS_CIRCLES;
	circles[circle_cnt] = ptr;
	circle_cnt++;
}

void
update_circles(void)
{
	for (int i = 0; i < circle_cnt; ++i) {
		circles[i]->pos = Vector2Add(circles[i]->pos, Vector2Scale(circles[i]->v, GetFrameTime()*circle_speed*speed_mult));
		if (circles[i]->pos.x + circles[i]->radius > WIDTH || circles[i]->pos.x - circles[i]->radius < 0)
			circles[i]->v.x = -circles[i]->v.x;

		if (circles[i]->pos.y + circles[i]->radius > HEIGHT || circles[i]->pos.y - circles[i]->radius < 0)
			circles[i]->v.y = -circles[i]->v.y;

		if (circles[i]->pos.x + circles[i]->radius > WIDTH + 20)
			circles[i]->pos.x = WIDTH - circles[i]->radius;
		if (circles[i]->pos.x - circles[i]->radius < -20)
			circles[i]->pos.x = circles[i]->radius;
		if (circles[i]->pos.y + circles[i]->radius > HEIGHT + 20)
			circles[i]->pos.y = HEIGHT - circles[i]->radius;
		if (circles[i]->pos.y - circles[i]->radius < -20)
			circles[i]->pos.y = circles[i]->radius;
	}
}

void
march_squares()
{
	float a, b, c, d;
	float thickness = 1.0F;
	for (int x = 0; x < WIDTH; x += square_size) {
		for (int y = 0; y < HEIGHT; y += square_size) {
			a = sample_pos(x, y);
			b = sample_pos(x + square_size, y);
			c = sample_pos(x + square_size, y + square_size);
			d = sample_pos(x, y + square_size);
			switch (get_num(a, b, c, d)) {
				case 0b0001:
				case 0b1110:
					DrawLineEx((Vector2){x, y + get_lerp(a, d, square_size)}, (Vector2){x + get_lerp(d, c, square_size), y + square_size}, thickness, RED);
					break;
				case 0b0010:
				case 0b1101:
					DrawLineEx((Vector2){x + get_lerp(d, c, square_size), y + square_size}, (Vector2){x + square_size, y + get_lerp(b, c, square_size)}, thickness, RED);
					break;
				case 0b0011:
				case 0b1100:
					DrawLineEx((Vector2){x, y + get_lerp(a, d, square_size)}, (Vector2){x + square_size, y + get_lerp(b, c, square_size)}, thickness, RED);
					break;
				case 0b0100:
				case 0b1011:
					DrawLineEx((Vector2){x + get_lerp(a, b, square_size), y}, (Vector2){x + square_size, y + get_lerp(b, c, square_size)}, thickness, RED);
					break;
				case 0b0101:
					if (sample_pos(x + square_size/2, y + square_size/2) > 1) {
						DrawLineEx((Vector2){x + get_lerp(a, b, square_size), y}, (Vector2){x, y + get_lerp(a, d, square_size)}, thickness, RED);
						DrawLineEx((Vector2){x + get_lerp(c, d, square_size), y + square_size}, (Vector2){x + square_size, y + get_lerp(b, c, square_size)}, thickness, RED);
					} else {
						DrawLineEx((Vector2){x, y + get_lerp(a, d, square_size)}, (Vector2){x + get_lerp(c, d, square_size), y + square_size}, thickness, RED);
						DrawLineEx((Vector2){x + get_lerp(a, b, square_size), y}, (Vector2){x + square_size, y + get_lerp(b, c, square_size)}, thickness, RED);
					}
					break;
				case 0b0110:
				case 0b1001:
					DrawLineEx((Vector2){x + get_lerp(a, b, square_size), y}, (Vector2){x + get_lerp(d, c, square_size), y + square_size}, thickness, RED);
					break;
				case 0b0111:
				case 0b1000:
					DrawLineEx((Vector2){x + get_lerp(a, b, square_size), y}, (Vector2){x, y + get_lerp(a, d, square_size)}, thickness, RED);
					break;
				case 0b1010:
					if (sample_pos(x + square_size/2, y + square_size/2) > 1) {
						DrawLineEx((Vector2){x, y + get_lerp(a, d, square_size)}, (Vector2){x + get_lerp(c, d, square_size), y + square_size}, thickness, RED);
						DrawLineEx((Vector2){x + get_lerp(a, b, square_size), y}, (Vector2){x + square_size, y + get_lerp(b, c, square_size)}, thickness, RED);
					} else {
						DrawLineEx((Vector2){x + get_lerp(a, b, square_size), y}, (Vector2){x, y + get_lerp(a, d, square_size)}, thickness, RED);
						DrawLineEx((Vector2){x + get_lerp(c, d, square_size), y + square_size}, (Vector2){x + square_size, y + get_lerp(b, c, square_size)}, thickness, RED);
					}
					break;
			}
		}
	}
}

void
handle_keys(void)
{
	switch (GetKeyPressed()) {
		case KEY_SPACE:
			add_circle();
			break;
		case KEY_UP:
			speed_mult *= 2;
			break;
		case KEY_DOWN:
			speed_mult /= 2;
			break;
		case KEY_R:
			for (int i = 0; i < circle_cnt; ++i)
				free(circles[i]);
			circle_cnt = 0;
			break;
		case KEY_P:
			paused = !paused;
			break;
		case KEY_D:
			debug = !debug;
			break;
		case KEY_C:
			square_size += 5;
			break;
		case KEY_V:
			if (square_size > 5)
				square_size -= 5;
			break;
	}
}

void
render_circles(void)
{
	for (int i = 0; i < circle_cnt; ++i)
		DrawCircleV(circles[i]->pos, circles[i]->radius, LIGHTGRAY);
}

void
draw_pixels(void)
{
	for (int x = 0; x < WIDTH; ++x)
		for (int y = 0; y < HEIGHT; ++y)
			if (sample_pos(x, y) > 1)
				DrawPixel(x, y, RED);
}

void
draw_overlay()
{
	DrawFPS(10, 10);
	DrawText(TextFormat("circle_cnt: %d", circle_cnt), 10, 30, 20, DARKGRAY);
	DrawText(TextFormat("speed mult: %.2f", speed_mult), 10, 50, 20, DARKGRAY);
	DrawText(TextFormat("square size: %d", square_size), 10, 70, 20, DARKGRAY);
}

void
render(void)
{
	BeginDrawing();
		ClearBackground(RAYWHITE);
		if (debug)
			render_circles();
		march_squares();
		draw_overlay();
	EndDrawing();
}

void
handle_mouse(void)
{
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		if (selected_circle != -1) {
			circles[selected_circle]->pos = GetMousePosition();
		} else {
			for (int i = 0; i < circle_cnt; ++i) {
				if (CheckCollisionPointCircle(GetMousePosition(), circles[i]->pos, circles[i]->radius)) {
					selected_circle = i;
					break;
				}
			}
		}
	} else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && selected_circle != -1) {
		if (!paused)
			circles[selected_circle]->v = GetMouseDelta();

		selected_circle = -1;
	}
}

int
main(void)
{
	InitWindow(WIDTH, HEIGHT, "Marching squares");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		handle_keys();
		if (!paused)
			update_circles();
		handle_mouse();
		render();
	}

	CloseWindow();
	return EXIT_SUCCESS;
}
