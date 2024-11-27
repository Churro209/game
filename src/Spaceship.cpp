#include "Spaceship.hpp"

Spaceship::Spaceship()
    : speed(0), acceleration(500.0f), friction(0.99f), maxSpeed(600.0f), rotationSpeed(200.0f)
{
    shape.setPointCount(4);
    shape.setPoint(0, sf::Vector2f(-10, 20));
    shape.setPoint(1, sf::Vector2f(40, 0));
    shape.setPoint(2, sf::Vector2f(-10, -20));
    shape.setPoint(3, sf::Vector2f(0, 0));
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(1920 / 2, 1080 / 2); // Center of the window
}

void Spaceship::move(float deltaTime) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        float rad = shape.getRotation() * 3.14159265f / 180.f;
        float accelX = std::cos(rad) * acceleration;
        float accelY = std::sin(rad) * acceleration;

        speed += acceleration * deltaTime;
        shape.move(accelX * deltaTime, accelY * deltaTime);

        if (speed > maxSpeed) speed = maxSpeed;
    } else {
        speed *= friction;
    }
}

void Spaceship::rotateLeft(float deltaTime) {
    shape.rotate(-rotationSpeed * deltaTime);
}

void Spaceship::rotateRight(float deltaTime) {
    shape.rotate(rotationSpeed * deltaTime);
}

void Spaceship::wrapAroundEdges(unsigned int windowWidth, unsigned int windowHeight) {
    if (shape.getPosition().x < 0)
        shape.setPosition(windowWidth, shape.getPosition().y);
    if (shape.getPosition().x > windowWidth)
        shape.setPosition(0, shape.getPosition().y);
    if (shape.getPosition().y < 0)
        shape.setPosition(shape.getPosition().x, windowHeight);
    if (shape.getPosition().y > windowHeight)
        shape.setPosition(shape.getPosition().x, 0);
}

sf::ConvexShape& Spaceship::getShape() {
    return shape;
}

sf::FloatRect Spaceship::getGlobalBounds() const {
    return shape.getGlobalBounds();
}
