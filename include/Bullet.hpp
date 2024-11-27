#ifndef BULLET_HPP
#define BULLET_HPP

#include <SFML/Graphics.hpp>

class Bullet {
public:
    Bullet(const sf::Vector2f& position, float angle);

    void move(float deltaTime, float speed);
    bool isOutOfBounds(unsigned int windowWidth, unsigned int windowHeight) const;

    const sf::CircleShape& getShape() const;

private:
    sf::CircleShape shape;
    float angle;
};

#endif // BULLET_HPP
