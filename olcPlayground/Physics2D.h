#pragma once
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>

class Physics2D :
	public olc::PixelGameEngine
{
private:

	struct Circle
	{
		float x;
		float y;
		float dx;
		float dy;
		float radius;
		float distanceTo(Circle& circle)
		{
			return (circle.position() - position()).mag();
		}
		olc::vf2d vectorTo(Circle& circle)
		{
			return circle.position() - position();
		}
		olc::vf2d position()
		{
			return olc::vf2d(x, y);
		}
		olc::vf2d velocity()
		{
			return olc::vf2d(dx, dy);
		}
		void setPosition(olc::vf2d position)
		{
			x = position.x;
			y = position.y;
		}
		void setVelocity(olc::vf2d velocity)
		{
			dx = velocity.x;
			dy = velocity.y;
		}
	};

	struct Collision
	{
		Circle& circle1;
		Circle& circle2;
		olc::vf2d velocity1;
		olc::vf2d velocity2;
		olc::vf2d collisionN;
		float adjust;
		void collide()
		{
			adjust = circle1.radius + circle2.radius - circle1.distanceTo(circle2) + 0.1f;
			collisionN = circle1.vectorTo(circle2).norm();
			std::cout << "adjust = " << adjust << std::endl;
			std::cout << "p1 = " << circle1.position() << std::endl << "p2 = " << circle2.position() << std::endl;
			std::cout << "p1 - " << adjust * collisionN * 0.5f << std::endl << "p2 - " << adjust * collisionN * 0.5f << std::endl;
			circle1.setPosition(circle1.position() - adjust * collisionN * 0.5f);
			circle2.setPosition(circle2.position() + adjust * collisionN * 0.5f);
			std::cout << "p1' = " << circle1.position() << std::endl << "p2' = " << circle2.position() << std::endl;
			velocity1 = circle1.velocity();
			velocity2 = circle2.velocity();
			collisionN = circle1.vectorTo(circle2).norm();
			circle1.setVelocity(velocity1 - (2 * circle2.radius / (circle1.radius + circle2.radius)) * (velocity1 - velocity2).dot(collisionN) * collisionN);
			circle2.setVelocity(velocity2 - (2 * circle1.radius / (circle1.radius + circle2.radius)) * (velocity2 - velocity1).dot(collisionN) * collisionN);
			std::cout << "v1 = " << velocity1 << std::endl
				<< "v2 = " << velocity2 << std::endl
				<< "v1' = " << circle1.velocity() << std::endl
				<< "v2' = " << circle2.velocity() << std::endl
				<< "m1v1x + m2v2x = " << circle1.radius * velocity1.x + circle2.radius * velocity2.x << std::endl
				<< "m1v1x' + m2v2x' = " << circle1.radius * circle1.velocity().x + circle2.radius * circle2.velocity().x << std::endl
				<< "m1v1y + m1v2y = " << circle1.radius * velocity1.y + circle2.radius * velocity2.y << std::endl
				<< "m1v1y' + m2v2y' = " << circle1.radius * circle1.velocity().y + circle2.radius * circle2.velocity().y << std::endl << std::endl;
		}
	};
	int32_t mouseX_initial;
	int32_t mouseY_initial;
	int32_t dWheel;
	std::vector<Circle> circles;
	Circle* circleCreate;
	Circle* aCircle;
	int32_t screenWidth;
	int32_t screenHeight;
	bool enable = false;
	int collisionCount = 0;
	int shapeNum = 0;
	std::string shapeString = "Circle";
public:

	Physics2D()
	{
		sAppName = "Simple 2D Physics Engine";

	}



	bool OnUserCreate() override
	{
		screenWidth = GetDrawTargetWidth();
		screenHeight = GetDrawTargetHeight();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		if (GetKey(olc::K1).bPressed)
		{
			shapeNum = 0;
			shapeString = "Circle";
		}
		else if (GetKey(olc::K2).bPressed)
		{
			shapeNum = 1;
			shapeString = "Rectangle";
		}

		DrawString({ 1, 1 }, "Shape: " + shapeString);

		if (enable)
		{
			dWheel = -GetMouseWheel() / 120;
			circleCreate->radius += (float)dWheel;
			DrawCircle((int32_t)circleCreate->x, (int32_t)circleCreate->y, (int32_t)circleCreate->radius);
			DrawLine(circleCreate->position(), GetMousePos());
		}


		for (size_t i = 0; i < circles.size(); i++)
		{
			Circle& circle = circles[i];
			circle.x += circle.dx * fElapsedTime;
			circle.y += circle.dy * fElapsedTime;
			if (circle.x <= circle.radius || screenWidth - circle.x <= circle.radius)
			{
				circle.x = circle.radius * (circle.x < screenWidth / 2) + (screenWidth - circle.radius) * (circle.x > screenWidth / 2);
				circle.dx = -circle.dx;
			}

			if (circle.y <= circle.radius || screenHeight - circle.y <= circle.radius)
			{
				circle.y = circle.radius * (circle.y < screenHeight / 2) + (screenHeight - circle.radius) * (circle.y > screenHeight / 2);
				circle.dy = -circle.dy;
			}

			for (size_t k = i + 1; k < circles.size(); k++)
				if (circle.distanceTo(circles[k]) <= circle.radius + circles[k].radius)
				{
					std::cout << ++collisionCount << std::endl;
					Collision({ circle, circles[k] }).collide();
				}


			DrawCircle((int32_t)circle.x, (int32_t)circle.y, (int32_t)circle.radius);
			DrawLine(circle.position(), circle.position() + circle.velocity().norm() * circle.radius);
		}

		if (GetMouse(0).bPressed)
		{
			mouseX_initial = GetMouseX();
			mouseY_initial = GetMouseY();
			circleCreate = new Circle({ (float)mouseX_initial, (float)mouseY_initial, 0, 0, 10.0f });
			enable = true;
		}

		if (GetMouse(0).bReleased)
		{
			if (circleCreate != nullptr)
			{
				enable = false;
				float xf = (float)(mouseX_initial - GetMouseX());
				float yf = (float)(mouseY_initial - GetMouseY());
				circles.push_back({ (float)mouseX_initial, (float)mouseY_initial, xf, yf, circleCreate->radius });
				std::cout << mouseX_initial - GetMouseX() << std::endl << mouseY_initial - GetMouseY() << std::endl;
				DrawCircle(mouseX_initial, mouseY_initial, (int32_t)circles.back().radius);
				DrawLine(circles.back().position(), circles.back().position() + circles.back().velocity().norm() * circles.back().radius);
			}

		}

		if (GetMouse(1).bPressed)
		{
			delete circleCreate;
			circleCreate = nullptr;
			enable = false;
		}

		return true;
	}

};

int main()
{
	Physics2D engine;
	if (engine.Construct(526, 400, 2, 2))
		engine.Start();
	return 0;
}