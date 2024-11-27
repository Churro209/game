#include "Bullet.hpp"
#include <cmath>

Bullet::Bullet(const sf::Vector2f& position, float angle)
    : angle(angle)
{
    shape.setRadius(5);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(5, 5); // Center the bullet
    shape.setPosition(position);
}

void Bullet::move(float deltaTime, float speed) {
    float rad = angle * 3.14159265f / 180.f;
    float velocityX = std::cos(rad) * speed;
    float velocityY = std::sin(rad) * speed;
    shape.move(velocityX * deltaTime, velocityY * deltaTime);
}

bool Bullet::isOutOfBounds(unsigned int windowWidth, unsigned int windowHeight) const {
    sf::Vector2f position = shape.getPosition();
    return position.x < 0 || position.x > windowWidth || position.y < 0 || position.y > windowHeight;
}

const sf::CircleShape& Bullet::getShape() const {
    return shape;
}
