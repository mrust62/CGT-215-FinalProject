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

	// So the enemy Ship bounces when reaching the bottom of the screen
	PhysicsRectangle bottomBounce;
	bottomBounce.setSize(Vector2f(60, 10));
	bottomBounce.setCenter(Vector2f(975, 600));
	bottomBounce.setStatic(true);
	world.AddPhysicsBody(bottomBounce);


	// Create top of screen
	PhysicsRectangle top;
	top.setSize(Vector2f(1050, 10));
	top.setCenter(Vector2f(400, -25));
	top.setStatic(true);
	world.AddPhysicsBody(top);
	// So the enemy Ship bounces when reaching the top of the screen
	PhysicsRectangle topBounce;
	topBounce.setSize(Vector2f(60, 10));
	topBounce.setCenter(Vector2f(975, 0));
	topBounce.setStatic(true);
	world.AddPhysicsBody(topBounce);

	// Create left boundry
	PhysicsRectangle left;
	left.setSize(Vector2f(10, 600));
	left.setCenter(Vector2f(-15, 300));
	left.setStatic(true);
	world.AddPhysicsBody(left);

	// Create the Ship
	PhysicsSprite ship;
	Texture shipTex;
	LoadTex(shipTex, "FinalProjectImages/spaceship.jpg");
	ship.setTexture(shipTex);
	Vector2f sz = ship.getSize();
	ship.setCenter(Vector2f(400, 600 - sz.y / 2));
	ship.setStatic(true);
	world.AddPhysicsBody(ship);

	// Create enemy ship
	PhysicsSprite enemyShip;
	Texture enemyTex;
	LoadTex(enemyTex, "FinalProjectImages/enemyShip.jpeg");
	enemyShip.setTexture(enemyTex);
	enemyShip.setCenter(Vector2f(975, 300));
	enemyShip.setVelocity(Vector2f(0, .20));
	//enemyShip.setStatic(true);
	world.AddPhysicsBody(enemyShip);
	//PhysicsShapeList<PhysicsSprite> enemyShips;

	// Create lasers
	Texture laserTex;
	LoadTex(laserTex, "FinalProjectImages/laser.png");
	PhysicsShapeList<PhysicsSprite> lasers;
	PhysicsShapeList<PhysicsSprite> enemyLasers;

	// Create the asteroids
	Texture asteroidTex;
	LoadTex(asteroidTex, "FinalProjectImages/asteroid.jpg");
	PhysicsShapeList<PhysicsSprite> asteroids;

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
	long laserInterval = 0;
	long enemyShoot = 0;
	//bool enemyShipSpawn = false;
	while (health > 0)
	{
		// calculate MS since the last frame
		Time currentTime(clock.getElapsedTime());
		Time deltaTime(currentTime - lastTime);
		int deltaTimeMS(deltaTime.asMilliseconds());
		if (deltaTimeMS > 10)
		{
			interval += deltaTimeMS;
			laserInterval += deltaTimeMS;
			enemyShoot += deltaTimeMS;

			world.UpdatePhysics(deltaTimeMS);
			lastTime = currentTime;
			MoveShip(ship, deltaTimeMS);
			if (Keyboard::isKeyPressed(Keyboard::Space) && laserInterval >= 100)
			{
				PhysicsSprite& laser = lasers.Create();
				laser.setTexture(laserTex);
				laser.setCenter(Vector2f(GetShipCenter(ship).x, GetShipCenter(ship).y - 45));
				laser.setVelocity(Vector2f(0, -1));

				laser.onCollision =
					[&ship, &lasers, &laser, &world, &asteroids, &health, &top, &bottom]
				(PhysicsBodyCollisionResult result)
				{
					if (result.object2 == ship)
					{
						cout << "hits ship";
						world.RemovePhysicsBody(laser);
						lasers.QueueRemove(laser);
						health -= 1;
					}
					else if (result.object2 == top) 
					{
						world.RemovePhysicsBody(laser);
						lasers.QueueRemove(laser);
					}
					else if (result.object2 == bottom)
					{
						world.RemovePhysicsBody(laser);
						lasers.QueueRemove(laser);
					}
					else
					{
						world.RemovePhysicsBody(laser);
						lasers.QueueRemove(laser);
					}
				};

				world.AddPhysicsBody(laser);
				laserInterval = 0;
			}

			
		}
		//enemy shooting
		if (enemyShoot >= 1500)
		{
			enemyShoot = 0;
			PhysicsSprite& enemyLaser = enemyLasers.Create();
			enemyLaser.setTexture(laserTex);
			Vector2f sz = enemyLaser.getSize();
			enemyLaser.setCenter(Vector2f(GetShipCenter(enemyShip).x - 100, GetShipCenter(enemyShip).y));
			enemyLaser.setVelocity(Vector2f(-0.75, 0));
			world.AddPhysicsBody(enemyLaser);
			enemyLaser.onCollision =
				[&ship, &enemyLasers, &enemyLaser, &world, &asteroids, &health, &top, &left]
			(PhysicsBodyCollisionResult result)
			{
				if (result.object2 == ship)
				{
					cout << "hits ship";
					world.RemovePhysicsBody(enemyLaser);
					enemyLasers.QueueRemove(enemyLaser);
					health -= 1;
				}
				else if (result.object2 == left)
				{
					world.RemovePhysicsBody(enemyLaser);
					enemyLasers.QueueRemove(enemyLaser);
				}
				else
				{
					world.RemovePhysicsBody(enemyLaser);
					enemyLasers.QueueRemove(enemyLaser);
				}
			};
		}
		window.clear(Color(0, 0, 0));
		// Laser delting iteself after a certain time if stuck
		lasers.DoRemovals();
		for (PhysicsShape& laser : lasers)
		{
			window.draw((PhysicsSprite&)laser);
		}
		asteroids.DoRemovals();
		for (PhysicsShape& asteroid : asteroids) {
			window.draw((PhysicsSprite&)asteroid);
		}
		enemyLasers.DoRemovals();
		for (PhysicsShape& enemyLaser : enemyLasers)
		{
			window.draw((PhysicsSprite&)enemyLaser);
		}
		/*
		enemyShips.DoRemovals();
		for (PhysicsShape& enemyShip : enemyShips) {
			window.draw((PhysicsSprite&)enemyShip);
		}
		*/
		// Spawn and destroy Asteroids
		if (interval >= 1000)
		{
			PhysicsSprite& asteroid = asteroids.Create();
			asteroid.setTexture(asteroidTex);
			Vector2f sz = asteroid.getSize();
			asteroid.setCenter(Vector2f(rand() % 900, 50));
			asteroid.setVelocity(Vector2f(0, 0.25));
			world.AddPhysicsBody(asteroid);
			asteroid.onCollision =
				[&world, &asteroid, &asteroids, &score, &bottom, &health, &ship]
			(PhysicsBodyCollisionResult result)
			{
				
				if (result.object2 == bottom)
				{
					world.RemovePhysicsBody(asteroid);
					asteroids.QueueRemove(asteroid);
					health -= 1;
				}
				else if (result.object2 == ship)
				{
					world.RemovePhysicsBody(asteroid);
					asteroids.QueueRemove(asteroid);
					health -= 1;
				}
				else 
				{
					//cout << "hits asteroid";
					//drawingLaser = false;
					world.RemovePhysicsBody(asteroid);
					asteroids.QueueRemove(asteroid);
					score += 10;
				}
			};
			interval = 0;
			
		}

		// Spawn and destroy enemy ships
		/*
		if (enemySpawn == 5000 && enemyShipSpawn == false)
		{
			PhysicsSprite& enemyShip = enemyShips.Create();
			enemyShip.setTexture(enemyTex);
			enemyShip.setCenter(Vector2f(950, rand() % 600));
			world.AddPhysicsBody(enemyShip);
			enemyShipSpawn == true;
			enemyShip.onCollision =
				[&world, &enemyShip, &enemyShips, &score, &bottom, &health, &enemyShipSpawn]
			(PhysicsBodyCollisionResult result)
			{
				if (result.object2 == bottom)
				{
					world.RemovePhysicsBody(enemyShip);
					enemyShips.QueueRemove(enemyShip);
					enemyShipSpawn == false;
				}
				else
				{
					world.RemovePhysicsBody(enemyShip);
					enemyShips.QueueRemove(enemyShip);
					enemyShipSpawn == false;
				}
			};
			enemySpawn = 0;
		}
		*/
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
		window.draw(top);
		window.draw(left);
		window.draw(ship);
		window.draw(enemyShip);
		window.display();
	}
	return 0;
}


