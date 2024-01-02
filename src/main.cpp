#include <SFML/Graphics.hpp>
#include <vector>

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
    float acceleration = 500.0f;
    float friction= .99f;
    float maxSpeed = 600.0f;
    float rotationSpeed = 200.0f;

    //clocks
    sf::Clock clock;
    sf::Clock bulletClock;
    
    //bullet
    float bulletDelay = 0.5f; // Bullet delay in seconds
    std::vector<sf::CircleShape> bullets;
    std::vector<float>bulletAngles;
    float bulletSpeed = 900.0f;
    int bulletCount = 0; 
    
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
             speed *= friction;
            float rad = spaceship.getRotation() * 3.14159265f / 180.f;
            float accelX = std::cos(rad) *speed;
            float accelY = std::sin(rad) *speed;
             spaceship.move(accelX * seconds, accelY * seconds);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            spaceship.rotate(-rotationSpeed * seconds);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){ 
            spaceship.rotate(rotationSpeed * seconds);
        }

        // Shooting logic
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bulletClock.getElapsedTime() > sf::seconds(bulletDelay)) {
            sf::CircleShape bullet(5);
            bullet.setFillColor(sf::Color::White);

            // Set the initial position of the bullet to the tip of the ship
            sf::Vector2f shipTipPosition = spaceship.getTransform().transformPoint(spaceship.getPoint(1));
            bullet.setPosition(shipTipPosition);

            bullets.push_back(bullet);
            bulletAngles.push_back(spaceship.getRotation());

                // Reset the bullet count when it reaches the end of the vector
                if (++bulletCount >= bullets.size()) 
                 bulletCount = 0;
            

            // Reset the bullet clock
            bulletClock.restart();
        }

      
        window.clear();
        window.draw(spaceship);
        
        //bullet rendering
        for(sf::CircleShape i : bullets){ 
            window.draw(i);
        }
        auto bulletIter = bullets.begin();
        auto angleIter = bulletAngles.begin();

        for (auto iterator = bullets.begin(); iterator != bullets.end();) {
        float rad = bulletAngles[iterator - bullets.begin()] * 3.14159265f / 180.f;
        float accelX = std::cos(rad) * bulletSpeed;
        float accelY = std::sin(rad) * bulletSpeed;
        iterator->move(accelX * seconds, accelY * seconds);

        // Check if the bullet is out of bounds
        if (iterator->getPosition().x < 0 || iterator->getPosition().x > 1920) {
        iterator = bullets.erase(iterator);
        bulletAngles.erase(bulletAngles.begin() + (iterator - bullets.begin()));
        } else {
        ++iterator;
        }//end bullet rendering
}


        window.display();
    }
    return 0;
}