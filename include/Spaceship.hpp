#ifndef SPACESHIP_HPP
#define SPACESHIP_HPP

#include <SFML/Graphics.hpp>

class Spaceship {
public:
    Spaceship();

    void move(float deltaTime);
    void rotateLeft(float deltaTime);
    void rotateRight(float deltaTime);
    void wrapAroundEdges(unsigned int windowWidth, unsigned int windowHeight);

    sf::ConvexShape& getShape();
    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f getTipPosition() const; // Returns the tip position of the spaceship
    float getRotation() const;          // Returns the current rotation angle of the spaceship

private:
    sf::ConvexShape shape;
    float speed;
    float acceleration;
    float friction;
    float maxSpeed;
    float rotationSpeed;
};

#endif // SPACESHIP_HPP
