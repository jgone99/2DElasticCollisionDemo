#include "Physics2D.h"

/*
 * CONTROLS: 
 * Hold left mouse click to create a circle.
 * (While holding) Scroll with middle mouse to change circle radius (mass).
 * (While holding) Drag mouse to change circle starting velocity.
 */

int main()
{
	Physics2D engine;
	if (engine.Construct(526, 400, 2, 2))
		engine.Start();
	return 0;
}