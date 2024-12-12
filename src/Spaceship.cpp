#include "Spaceship.hpp"
#include <cmath>

// Define PI as a constant for readability
constexpr float PI = 3.14159265f;

Spaceship::Spaceship()
    : speed(0), acceleration(500.0f), friction(0.99f), maxSpeed(600.0f), rotationSpeed(275.0f)
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
        float rad = shape.getRotation() * PI / 180.f;
        sf::Vector2f direction(std::cos(rad), std::sin(rad));

        speed += acceleration * deltaTime;
        if (speed > maxSpeed)
            speed = maxSpeed;

        shape.move(direction * speed * deltaTime);
        speed *= friction;
        if (std::abs(speed) < 0.01f) // Stop completely when very slow
            speed = 0;
    }


void Spaceship::rotateLeft(float deltaTime) {
    shape.rotate(-rotationSpeed * deltaTime);
}

void Spaceship::rotateRight(float deltaTime) {
    shape.rotate(rotationSpeed * deltaTime);
}

void Spaceship::wrapAroundEdges(unsigned int windowWidth, unsigned int windowHeight) {
    sf::Vector2f position = shape.getPosition();

    if (position.x < 0) position.x = windowWidth;
    if (position.x > windowWidth) position.x = 0;
    if (position.y < 0) position.y = windowHeight;
    if (position.y > windowHeight) position.y = 0;

    shape.setPosition(position);
}

sf::ConvexShape& Spaceship::getShape() {
    return shape;
}

sf::FloatRect Spaceship::getGlobalBounds() const {
    return shape.getGlobalBounds();
}
sf::Vector2f Spaceship::getTipPosition() const {
    // Get the transformed position of the tip of the spaceship
    return shape.getTransform().transformPoint(shape.getPoint(0));
}

float Spaceship::getRotation() const {
    // Return the current rotation angle of the spaceship
    return shape.getRotation();
}