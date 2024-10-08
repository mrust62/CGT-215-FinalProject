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

void LoadTex(Texture& tex, string filename)
{
	if (!tex.loadFromFile(filename))
	{
		cout << "could not load " << filename << endl;
	}
}

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
	const int screenX = 1000;
	const int screenY = 600;
	//Create the window
	RenderWindow window(VideoMode(screenX, screenY), "Space Shooter");
	World world(Vector2f(0, 0));

	// Create bottom of screen
	PhysicsRectangle bottom;
	bottom.setSize(Vector2f(screenX, 10));
	bottom.setCenter(Vector2f((screenX / 2), (screenY + 5)));
	bottom.setStatic(true);
	world.AddPhysicsBody(bottom);

	// Create top of screen
	PhysicsRectangle top;
	top.setSize(Vector2f(screenX , 10));
	top.setCenter(Vector2f((screenX / 2), -25));
	top.setStatic(true); 
	world.AddPhysicsBody(top);

	// Create left boundry
	PhysicsRectangle left;
	left.setSize(Vector2f(10, screenY));
	left.setCenter(Vector2f(-15, (screenY / 2)));
	left.setStatic(true);
	world.AddPhysicsBody(left);

	// Create the Ship
	PhysicsSprite ship;
	Texture shipTex;
	LoadTex(shipTex, "FinalProjectImages/spaceship.jpg");
	ship.setTexture(shipTex);
	Vector2f sz = ship.getSize();
	ship.setCenter(Vector2f((screenX / 2), screenY - sz.y / 2));
	ship.setStatic(true);
	world.AddPhysicsBody(ship);

	// Create enemy ship
	PhysicsSprite enemyShip;
	Texture enemyTex;
	LoadTex(enemyTex, "FinalProjectImages/enemyShip.jpeg");
	enemyShip.setTexture(enemyTex);
	enemyShip.setCenter(Vector2f((screenX - 25), (screenY / 2)));
	enemyShip.setVelocity(Vector2f(0, .20));
	
	world.AddPhysicsBody(enemyShip);
	

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
		// Laser deleting iteself after a certain time if stuck
		lasers.DoRemovals();
		for (const auto& laser : lasers)
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
		
		// Spawn and destroy Asteroids
		if (interval >= 1000)
		{
			PhysicsSprite& asteroid = asteroids.Create();
			asteroid.setTexture(asteroidTex);
			Vector2f sz = asteroid.getSize();
			asteroid.setCenter(Vector2f(rand() % (screenX - 100), 50));
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
					world.RemovePhysicsBody(asteroid);
					asteroids.QueueRemove(asteroid);
					score += 10;
				}
			};
			interval = 0;
			
		}

		//text display
		Text scoreText;
		scoreText.setString(to_string(score));
		scoreText.setFont(fnt);
		scoreText.setPosition(Vector2f(985 - GetTextSize(scoreText).x, (screenY - 50)));
		window.draw(scoreText);
		Text healthText;
		healthText.setString("health " + to_string(health));
		healthText.setFont(fnt);
		healthText.setPosition(Vector2f(-50 + GetTextSize(healthText).x, (screenY - 50)));


		window.draw(healthText);
		window.draw(bottom);
		window.draw(top);
		window.draw(left);
		window.draw(ship);
		window.draw(enemyShip);
		window.display();
	}
	window.display(); // this is needed to see the last frame
	Text gameOverText;
	gameOverText.setString("GAME OVER");
	gameOverText.setFont(fnt);
	sz = GetTextSize(gameOverText);
	gameOverText.setPosition((screenX / 2) - (sz.x / 2), (screenY / 2) - (sz.y / 2));
	window.draw(gameOverText);
	window.display();
	while (true)
	{
		if (Keyboard::isKeyPressed(Keyboard::Enter))
		{
			exit(4);
		}
	}
}


