#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include <SFML/Graphics.hpp>

class Asteroid {
public:
    Asteroid(float radius, const sf::Vector2f& position, const sf::Vector2f& velocity);

    void move(float deltaTime);
    void wrapAroundEdges(unsigned int windowWidth, unsigned int windowHeight);

    float getRadius() const;
    const sf::CircleShape& getShape() const;
    sf::FloatRect getGlobalBounds() const;

private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

#endif // ASTEROID_HPP
