#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>

#define WIDTH 1600
#define HEIGHT 900
#define PI 3.14159


class Bullet
{
public:
	float direction;
	int size = 4;
	float speed = 1000.f;
	sf::CircleShape body;
	Bullet(std::pair<int, int> pos, float dir)
	{
		direction = dir;
		body.setRadius(size);
		body.setPosition(pos.first, pos.second);
		body.setFillColor(sf::Color(0, 0, 255));
	}
};


class Ship
{
public:
	float direction;
	int size = 40;
	float speed = 280.f;
	sf::CircleShape body;
	Ship(std::pair<int, int> pos, float dir)
	{
		direction = dir;
		body.setRadius(size);
		body.setPosition(pos.first, pos.second);
		body.setFillColor(color);
	}
private:
	sf::Color color = sf::Color(255, 0, 0);
};


class Enemy
{
public:
	float direction;
	float speed;
	int size;
	sf::CircleShape body;
	Enemy(std::pair<int, int> pos, float dir, int s, float spd)
	{
		body.setRadius(s);
		body.setFillColor(sf::Color(0, 255, 0));
		body.setPosition(pos.first, pos.second);
		size = s;
		direction = dir;
		speed = spd;
	}
};

int random(int min, int max) {
	return min + rand() % (max - min + 1);
}


int main()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT, 32), "Space shooter",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);


	Ship ship({500, 500}, 0);

	sf::Font font;
	if (!font.loadFromFile("resources/sansation.ttf"))
		return EXIT_FAILURE;
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(40);
	text.setPosition(sf::Vector2f(20, 20));
	text.setFillColor(sf::Color(255, 255, 255));


	std::vector<Enemy> enemys;
	std::vector<Bullet> bullets;
	sf::Color background(0, 0, 0);

	bool shoot = false;
	bool lose = false;
	int counter = 0;

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}
			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Space) && shoot)
			{
				shoot = false;
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space) && !shoot)
			{
				shoot = true;
				float x = ship.body.getPosition().x + ship.size / 2.f;
				float y = ship.body.getPosition().y + ship.size / 2.f;
				bullets.push_back(Bullet({x, y}, ship.direction));
			}

		}

		float deltaTime = clock.restart().asSeconds();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			ship.direction += (-250 * deltaTime);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			ship.direction += (250 * deltaTime);
		}
		float factor = ship.speed * deltaTime;
		ship.body.move(std::cos(ship.direction * PI / 180) * factor, std::sin(ship.direction * PI / 180) * factor);

		if (random(0, 20) == 0)
		{
			int side = random(0, 3);
			int x, y, dir, size, speed;
			if (side == 0)
			{
				x = random(-50, WIDTH + 50);
				y = -50;
				dir = random(45, 135);
			}
			else if (side == 1)
			{
				x = WIDTH + 50;
				y = random(-50, HEIGHT + 50);
				dir = random(135, 225);
			}
			else if (side == 2)
			{
				x = random(-50, WIDTH + 50);
				y = HEIGHT + 50;
				dir = random(315, 405);
			}
			else
			{
				x = -50;
				y = random(-50, HEIGHT + 50);
				dir = random(225, 315);
			}
			size = random(10, 25);
			speed = random(250, 450);
			enemys.push_back(Enemy({x, y}, dir, size, speed));
		}
		int size = ship.size;
		int x = ship.body.getPosition().x + size / 2.f;
		int y = ship.body.getPosition().y + size / 2.f;
		if (x < size || x > WIDTH - size || y < size || y > HEIGHT - size) lose = true;
		int i = 0;
		while (i < bullets.size())
		{
			float factor = bullets[i].speed * deltaTime;
			bullets[i].body.move(std::cos(bullets[i].direction * PI / 180)
				* factor, std::sin(bullets[i].direction * PI / 180) * factor);
			float x = bullets[i].body.getPosition().x / 2;
			float y = bullets[i].body.getPosition().y / 2;
			int size = bullets[i].size;
			if (x < -size || x > WIDTH + size || y < -size || y > HEIGHT + size)
			{
				auto iter = bullets.cbegin();
				bullets.erase(iter + i);
			}
			else i++;
		}
		i = 0;
		while (i < enemys.size())
		{
			float factor = enemys[i].speed * deltaTime;
			enemys[i].body.move(std::cos(enemys[i].direction * PI / 180)
				* factor, std::sin(enemys[i].direction * PI / 180) * factor);
			float size = enemys[i].size;
			float x = enemys[i].body.getPosition().x + size / 2;
			float y = enemys[i].body.getPosition().y + size / 2;
			if (x < -100 || x > WIDTH + 100 || y < -100 || y > HEIGHT + 100)
			{
				auto iter = enemys.cbegin();
				enemys.erase(iter + i);
			}
			else
			{
				bool check = true;
				int ship_size = ship.size;
				float ship_x = ship.body.getPosition().x + ship_size / 2.f;
				float ship_y = ship.body.getPosition().y + ship_size / 2.f;
				if (sqrt((x - ship_x) * (x - ship_x) + (y - ship_y) * (y - ship_y))
					<= (size + ship_size))
				{
					lose = true;
				}
				else
				{
					for (int j = 0; j < bullets.size(); j++)
					{
						int bull_size = bullets[j].size;
						float bull_x = bullets[j].body.getPosition().x + bull_size / 2.f;
						float bull_y = bullets[j].body.getPosition().y + bull_size / 2.f;
						if (sqrt((x - bull_x) * (x - bull_x) + (y - bull_y) * (y - bull_y))
							<= (size + bull_size))
						{
							auto iter = enemys.cbegin();
							enemys.erase(iter + i);
							auto iter1 = bullets.cbegin();
							bullets.erase(iter1 + j);
							check = false;
							counter += 1;
						}
					}
				}
				if (check) i++;
			}
		}

		text.setString("Score: " + std::to_string(counter));

		window.clear(background);
		window.draw(ship.body);
		for (int i = 0; i < enemys.size(); i++)
			window.draw(enemys[i].body);
		for (int i = 0; i < bullets.size(); i++)
			window.draw(bullets[i].body);
		window.draw(text);
		window.display();
		if (lose)
		{
			window.close();
			break;
		}
	}

	return 0;
}
