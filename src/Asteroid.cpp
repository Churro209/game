#include "Asteroid.hpp"

Asteroid::Asteroid(float radius, const sf::Vector2f& position, const sf::Vector2f& velocity)
    : velocity(velocity)
{
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(radius, radius);
    shape.setPosition(position);
}

void Asteroid::move(float deltaTime) {
    shape.move(velocity * deltaTime);
}

void Asteroid::wrapAroundEdges(unsigned int windowWidth, unsigned int windowHeight) {
    sf::Vector2f position = shape.getPosition();
    if (position.x < 0) shape.setPosition(windowWidth, position.y);
    if (position.x > windowWidth) shape.setPosition(0, position.y);
    if (position.y < 0) shape.setPosition(position.x, windowHeight);
    if (position.y > windowHeight) shape.setPosition(position.x, 0);
}

float Asteroid::getRadius() const {
    return shape.getRadius();
}

const sf::CircleShape& Asteroid::getShape() const {
    return shape;
}

sf::FloatRect Asteroid::getGlobalBounds() const {
    return shape.getGlobalBounds();
}
