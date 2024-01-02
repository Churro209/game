#include <SFML/Graphics.hpp>

int main()
{
    auto window = sf::RenderWindow{ { 1920u, 1080u }, "CMake SFML Project" };
    window.setFramerateLimit(144);
    //Space Ship
   sf::ConvexShape spaceship;
    spaceship.setPointCount(4);
    spaceship.setPoint(0, sf::Vector2f(-10, 20));  // Top point
    spaceship.setPoint(1, sf::Vector2f(40, 0));  // Bottom-right point
    spaceship.setPoint(2, sf::Vector2f(-10,-20)); // Bottom-left point
    spaceship.setPoint(3, sf::Vector2f(0, 0)); // Bottom-left point
    spaceship.setFillColor(sf::Color::Red);
    spaceship.setPosition(900, 500);

    //world conditions
    float speed = 0;
    float Initalspeed = 0.0f;
    float acceleration = 400.0f;
    float deceleration = 200.0f;
    float maxSpeed = 600.0f;
    float rotationSpeed = 200.0f;
    float bulletSpeed = 800.0f;

    //clocks
    sf::Clock clock;
    sf::Clock bulletClock;
    
    //bullet
    float bulletDelay = 0.5f; // Bullet delay in seconds

   
    while (window.isOpen())
    {
        sf::Time deltaTime = clock.restart();
        float seconds = deltaTime.asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Spaceship movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            float rad = spaceship.getRotation() * 3.14159265f / 180.f;
            float accelX = std::cos(rad) * acceleration;
            float accelY = std::sin(rad) * acceleration;

            speed += acceleration * seconds;

            spaceship.move(accelX * seconds, accelY * seconds);

            if (speed > maxSpeed)
                speed = maxSpeed;
        }
        else
        {
            if (speed > 0)
                speed -= deceleration * seconds;
            else if (speed < 0)
                speed += deceleration * seconds;

            if (std::abs(speed) < deceleration * seconds)
                speed = 0;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            spaceship.rotate(-rotationSpeed * seconds);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){ 
            spaceship.rotate(rotationSpeed * seconds);
        }

        // Shooting logic

        window.clear();
        window.draw(spaceship);
        window.display();
    }
}