// CGT 215 FinalProject.cpp : 
// This file contains the 'main' function. 
// Program execution begins and ends there.
// Written by Matthew Rust, 11/28/2022

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <random>

using namespace std;
using namespace sf;
using namespace sfp;

//load textures
void LoadTex(Texture& tex, string filename)
{
	if (!tex.loadFromFile(filename))
	{
		cout << "could not load " << filename << endl;
	}
}

// moves the ship
void MoveShip(PhysicsSprite& crossbow, int elapsedMS)
{
	if (Keyboard::isKeyPressed(Keyboard::Right))
	{
		Vector2f newPos(crossbow.getCenter());
		newPos.x = newPos.x + (0.5 * elapsedMS);
		crossbow.setCenter(newPos);
	}

	if (Keyboard::isKeyPressed(Keyboard::Left))
	{
		Vector2f newPos(crossbow.getCenter());
		newPos.x = newPos.x - (0.5 * elapsedMS);
		crossbow.setCenter(newPos);
	}

	if (Keyboard::isKeyPressed(Keyboard::Up))
	{
		Vector2f newPos(crossbow.getCenter());
		newPos.y = newPos.y - (0.5 * elapsedMS);
		crossbow.setCenter(newPos);
	}

	if (Keyboard::isKeyPressed(Keyboard::Down))
	{
		Vector2f newPos(crossbow.getCenter());
		newPos.y = newPos.y + (0.5 * elapsedMS);
		crossbow.setCenter(newPos);
	}
}

// for determining laser start location
Vector2f GetShipCenter(PhysicsSprite ship) {
	Vector2f center = ship.getCenter();
	return Vector2f(center.x, center.y);
}

// for displaying text
Vector2f GetTextSize(Text text) {
	FloatRect r = text.getGlobalBounds();
	return Vector2f(r.width, r.height);
}


int main()
{
	int score = 0;
	int health = 3;

	//Create the window
	RenderWindow window(VideoMode(1000, 600), "Space Shooter");
	World world(Vector2f(0, 0));

	// Create bottom of screen
	PhysicsRectangle bottom;
	bottom.setSize(Vector2f(1000, 10));
	bottom.setCenter(Vector2f(400, 605));
	bottom.setStatic(true);
	world.AddPhysicsBody(bottom);

	// Create top of screen
	PhysicsRectangle top;
	top.setSize(Vector2f(1000, 10));
	top.setCenter(Vector2f(400, -5));
	top.setStatic(true);
	world.AddPhysicsBody(top);

	// Create right wall 
	PhysicsRectangle right;
	right.setSize(Vector2f(10, 600));
	right.setCenter(Vector2f(1005, 300));
	right.setStatic(true);
	world.AddPhysicsBody(right);

	// Create the Ship
	PhysicsSprite ship;
	Texture shipTex;
	LoadTex(shipTex, "FinalProjectImages/spaceship.jpg");
	ship.setTexture(shipTex);
	Vector2f sz = ship.getSize();
	ship.setCenter(Vector2f(400, 600 - sz.y / 2));
	ship.setStatic(true);
	world.AddPhysicsBody(ship);

	// Create the laser
	/*
	PhysicsSprite laser;
	Texture laserTex;
	LoadTex(laserTex, "FinalProjectImages/laser.png");
	laser.setTexture(laserTex);
	bool drawingLaser = false;
	*/
	

	// Create the asteroids
	Texture asteroidTex;
	LoadTex(asteroidTex, "FinalProjectImages/asteroid.jpg");
	PhysicsShapeList<PhysicsSprite> asteroids;

	laser.onCollision =
		[&drawingLaser, &ship, &laser, &world, &asteroids]
	(PhysicsBodyCollisionResult result)
	{
		if (result.object2 == ship)
		{
			cout << "hits ship";
			drawingLaser = false;
			world.RemovePhysicsBody(laser);
		}
		cout << "hits something?";
	};

	// Colission detections
	top.onCollision = [&drawingLaser, &world, &laser](PhysicsBodyCollisionResult result)
	{
		drawingLaser = false;
		world.RemovePhysicsBody(laser);
	};

	// loads font
	Font fnt;
	if (!fnt.loadFromFile("Halloween Sweet.ttf"))
	{
		cout << "could not load font." << endl;
		exit(3);
	}

	Clock clock;
	Time lastTime(clock.getElapsedTime());
	long interval = 0;
	while (health > 0)
	{
		// calculate MS since the last frame
		Time currentTime(clock.getElapsedTime());
		Time deltaTime(currentTime - lastTime);
		int deltaTimeMS(deltaTime.asMilliseconds());
		
		if (deltaTimeMS > 10)
		{
			interval += deltaTimeMS;
			world.UpdatePhysics(deltaTimeMS);
			lastTime = currentTime;
			MoveShip(ship, deltaTimeMS);
			if (Keyboard::isKeyPressed(Keyboard::Space) && !(drawingLaser))
			{
				drawingLaser = true;
				laser.setCenter(Vector2f(GetShipCenter(ship).x, GetShipCenter(ship).y - 45));
				laser.setVelocity(Vector2f(0, -1));
				world.AddPhysicsBody(laser);
			}
		}
		window.clear(Color(0, 0, 0));
		// Laser delting iteself after a certain time if stuck
		if (laser.getVelocity().y != -1)
		{
			drawingLaser = false;
			world.RemovePhysicsBody(laser);
		}
		if (drawingLaser)
		{
			window.draw(laser);
		}
		asteroids.DoRemovals();
		for (PhysicsShape& asteroid : asteroids) {
			window.draw((PhysicsSprite&)asteroid);
		}
		if (interval >= 1000)
		{
			PhysicsSprite& asteroid = asteroids.Create();
			asteroid.setTexture(asteroidTex);
			Vector2f sz = asteroid.getSize();
			asteroid.setCenter(Vector2f(rand() % 1000, 50));
			asteroid.setVelocity(Vector2f(0, 0.25));
			world.AddPhysicsBody(asteroid);
			asteroid.onCollision =
				[&drawingLaser, &world, &laser, &asteroid, &asteroids, &score, &bottom, &health, &ship]
			(PhysicsBodyCollisionResult result)
			{
				if (result.object2 == laser)
				{
					//cout << "hits asteroid";
					//drawingLaser = false;
					world.RemovePhysicsBody(laser);
					world.RemovePhysicsBody(asteroid);
					asteroids.QueueRemove(asteroid);
					score += 10;
				}
				if (result.object2 == bottom)
				{
					world.RemovePhysicsBody(asteroid);
					asteroids.QueueRemove(asteroid);
					health -= 1;
				}
				if (result.object2 == ship)
				{
					world.RemovePhysicsBody(asteroid);
					asteroids.QueueRemove(asteroid);
					health -= 1;
				}
			};
			interval = 0;
			
		}

		
		

		//text display
		Text scoreText;
		scoreText.setString(to_string(score));
		scoreText.setFont(fnt);
		scoreText.setPosition(Vector2f(985 - GetTextSize(scoreText).x, 550));
		window.draw(scoreText);
		Text healthText;
		healthText.setString("health " + to_string(health));
		healthText.setFont(fnt);
		healthText.setPosition(Vector2f(-50 + GetTextSize(healthText).x, 550));


		window.draw(healthText);
		window.draw(bottom);
		window.draw(right);
		window.draw(ship);
		window.display();
	}
	return 0;
}


