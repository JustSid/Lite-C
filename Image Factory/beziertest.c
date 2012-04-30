#include <acknex.h>
#include <default.c>

void drawBezier(VECTOR *vec1, VECTOR *vec2, VECTOR *vec3, VECTOR *vec4)
{
	double p1[2], p2[2], p3[2], p4[2];
	double t;
	double factor;
	double position[2], tempPos[2];

	p1[0] = vec1->x;
	p1[1] = vec1->y;
	p2[0] = vec2->x;
	p2[1] = vec2->y;
	p3[0] = vec3->x;
	p3[1] = vec3->y;
	p4[0] = vec4->x;
	p4[1] = vec4->y;

	draw_line(vec1, NULL, 100);
	for(t = 0; t<=1; t+=0.04)
	{
		position[0] = 0;	position[1] = 0;

		factor = (1 - t) * (1 - t) * (1 - t);
		tempPos[0] = p1[0] * factor;	tempPos[1] = p1[1] * factor;
		position[0] += tempPos[0];	position[1] += tempPos[1];
		
		factor = 3 * (1 - t) * (1 - t) * t;
		tempPos[0] = p2[0] * factor;	tempPos[1] = p2[1] * factor;
		position[0] += tempPos[0];	position[1] += tempPos[1];
		
		factor = 3 * (1 - t) * t * t;
		tempPos[0] = p3[0] * factor;	tempPos[1] = p3[1] * factor;
		position[0] += tempPos[0];	position[1] += tempPos[1];
		
		factor = t * t * t;
		tempPos[0] = p4[0] * factor;	tempPos[1] = p4[1] * factor;
		position[0] += tempPos[0];	position[1] += tempPos[1];

		draw_line(vector((float)position[0], (float)position[1],0), COLOR_RED, 100);
	}
	draw_line(vec4, NULL, 100);
}
BMAP* testRed = "testRed.png";
PANEL *testPan =
{
	flags = SHOW;
}
MATERIAL *_ctaMat =
{
	effect = "shaders//colortoalpha.fx";
}
void main()
{
	wait(1);
	screen_color.blue = 255;
	BMAP* test = bmap_createblack(658,409,32);
	bmap_process(test, testRed, _ctaMat);
	testPan->bmap = test;
	while(1)
	{
		drawBezier(vector(10,10,0),vector(40,10,0),vector(90,50,0),vector(120,50,0));
		wait(1);
	}
}