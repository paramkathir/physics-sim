#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>

struct Ball
{
    float x, y;
    float vx, vy;
    float radius;
    float mass;
    sf::Color color;
};

sf::Color randomColor()
{
    return sf::Color(
        100 + std::rand() % 155,
        100 + std::rand() % 155,
        100 + std::rand() % 155
    );
}

void resolveBallCollision(Ball & a, Ball & b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    float minDist = a.radius + b.radius;

    if (dist >= minDist || dist == 0)
        return;

    float nx = dx / dist;
    float ny = dy / dist;

    float overlap = minDist - dist;
    float totalMass = a.mass + b.mass;
    a.x -= nx * overlap * (b.mass / totalMass);
    a.y -= ny * overlap * (b.mass / totalMass);
    b.x += nx * overlap * (a.mass / totalMass);
    b.y += ny * overlap * (a.mass / totalMass);

    float dvx = b.vx - a.vx;
    float dvy = b.vy - a.vy;
    float dvn = dvx * nx + dvy * ny;

    if (dvn > 0)
        return;

    float restitution = 0.8f;
    float impulse = -(1.f + restitution) * dvn / totalMass;

    a.vx -= impulse * b.mass * nx;
    a.vy -= impulse * b.mass * ny;
    b.vx += impulse * a.mass * nx;
    b.vy += impulse * a.mass * ny;
}

std::vector<Ball> defaultBalls()
{
    return {
        {200, 100, 150,  0,   20, 1.f,  sf::Color(100, 200, 255)},
        {400,  50, -100, 0,   25, 1.5f, sf::Color(255, 150, 100)},
        {600, 150, -80,  50,  15, 0.8f, sf::Color(150, 255, 150)},
        {300, 200, 200,  -50, 30, 2.f,  sf::Color(255, 220, 80)},
    };
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Physics Sim");
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");

    float gravity = 980.f;
    bool gravityOn = true;
    bool paused = false;
    std::srand(42);

    std::vector<Ball> balls = defaultBalls();

    bool isDragging = false;
    sf::Vector2f dragStart;

    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::G)
                    gravityOn = !gravityOn;

                if (event.key.code == sf::Keyboard::Space)
                    paused = !paused;

                if (event.key.code == sf::Keyboard::R)
                {
                    balls = defaultBalls();
                    std::srand(42);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                isDragging = true;
                dragStart = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
            }

            if (event.type == sf::Event::MouseButtonReleased &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                isDragging = false;
                sf::Vector2f dragEnd(event.mouseButton.x, event.mouseButton.y);

                float dx = dragEnd.x - dragStart.x;
                float dy = dragEnd.y - dragStart.y;
                float dragDist = std::sqrt(dx * dx + dy * dy);

                if (dragDist > 5.f)
                {
                    float radius = 15.f + std::rand() % 20;
                    float mass = radius / 15.f;

                    float launchVx = (dragStart.x - dragEnd.x) * 4.f;
                    float launchVy = (dragStart.y - dragEnd.y) * 4.f;

                    Ball b;
                    b.x = dragStart.x;
                    b.y = dragStart.y;
                    b.vx = launchVx;
                    b.vy = launchVy;
                    b.radius = radius;
                    b.mass = mass;
                    b.color = randomColor();

                    balls.push_back(b);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Right)
            {
                balls.clear();
            }
        }

        if (!paused)
        {
            for (auto & b : balls)
            {
                if (gravityOn)
                    b.vy += gravity * dt;

                b.x += b.vx * dt;
                b.y += b.vy * dt;

                if (b.x - b.radius < 0)
                {
                    b.x = b.radius;
                    b.vx *= -0.8f;
                }
                if (b.x + b.radius > 800)
                {
                    b.x = 800 - b.radius;
                    b.vx *= -0.8f;
                }
                if (b.y + b.radius > 600)
                {
                    b.y = 600 - b.radius;
                    b.vy *= -0.8f;
                }
                if (b.y - b.radius < 0)
                {
                    b.y = b.radius;
                    b.vy *= -0.8f;
                }
            }

            for (int i = 0; i < (int)balls.size(); i++)
            {
                for (int j = i + 1; j < (int)balls.size(); j++)
                {
                    resolveBallCollision(balls[i], balls[j]);
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        if (isDragging)
        {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            sf::Vertex line[] = {
                sf::Vertex(dragStart, sf::Color::White),
                sf::Vertex(sf::Vector2f(mouse), sf::Color(255, 255, 255, 100))
            };
            window.draw(line, 2, sf::Lines);
        }

        for (auto & b : balls)
        {
            sf::CircleShape shape(b.radius);
            shape.setFillColor(b.color);
            shape.setOrigin(b.radius, b.radius);
            shape.setPosition(b.x, b.y);
            window.draw(shape);
        }

        if (fontLoaded)
        {
            std::string info =
                "Click & drag to toss a ball   |   Balls: " + std::to_string(balls.size()) +
                "   G: gravity " + (gravityOn ? "ON" : "OFF") +
                "   Space: " + (paused ? "PAUSED" : "running") +
                "   R: reset   RClick: clear";

            sf::RectangleShape bar(sf::Vector2f(800, 30));
            bar.setFillColor(sf::Color(0, 0, 0, 150));
            bar.setPosition(0, 0);
            window.draw(bar);

            sf::Text text;
            text.setFont(font);
            text.setString(info);
            text.setCharacterSize(14);
            text.setFillColor(sf::Color(200, 200, 200));
            text.setPosition(10, 10);
            window.draw(text);

            if (paused)
            {
                sf::Text pauseText;
                pauseText.setFont(font);
                pauseText.setString("PAUSED");
                pauseText.setCharacterSize(48);
                pauseText.setFillColor(sf::Color(255, 255, 255, 180));
                sf::FloatRect bounds = pauseText.getLocalBounds();
                pauseText.setPosition(400 - bounds.width / 2, 270);
                window.draw(pauseText);
            }
        }

        window.display();
    }

    return 0;
}