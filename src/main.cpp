#include <SFML/Graphics.hpp>
#include "Bullet.hpp"
#include "Asteroid.hpp"
#include "Spaceship.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

constexpr unsigned int WINDOW_WIDTH = 1920;
constexpr unsigned int WINDOW_HEIGHT = 1080;

enum GameState {
    PLAYING,
    GAME_OVER
};

void resetGame(Spaceship& spaceship, std::vector<Bullet>& bullets, std::vector<Asteroid>& asteroids, const int maxAsteroids, const float asteroidSpeed) {
    // Reset spaceship
    spaceship.getShape().setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    spaceship.getShape().setRotation(0);

    // Clear bullets and asteroids
    bullets.clear();
    asteroids.clear();

    // Spawn new asteroids
    for (int i = 0; i < maxAsteroids; ++i) {
        float radius = 30 + std::rand() % 20;
        sf::Vector2f position(std::rand() % WINDOW_WIDTH, std::rand() % WINDOW_HEIGHT);
        sf::Vector2f velocity((std::rand() % 200 - 100) / 100.0f * asteroidSpeed,
                              (std::rand() % 200 - 100) / 100.0f * asteroidSpeed);
        asteroids.emplace_back(radius, position, velocity);
    }
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Create window
    sf::RenderWindow window({WINDOW_WIDTH, WINDOW_HEIGHT}, "Asteroids");
    window.setFramerateLimit(144);

    // Initialize spaceship
    Spaceship spaceship;

    // Initialize bullets and asteroids
    std::vector<Bullet> bullets;
    sf::Clock bulletClock;
    float bulletDelay = 0.5f;
    float bulletSpeed = 900.0f;

    std::vector<Asteroid> asteroids;
    const int maxAsteroids = 10;
    const float asteroidSpeed = 100.0f;

    resetGame(spaceship, bullets, asteroids, maxAsteroids, asteroidSpeed);

    // Initialize font and game over text
    sf::Font font;
    if (!font.loadFromFile("../assets/OpenSans-VariableFont_wdth,wght.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    sf::Text gameOverText("Game Over\nPress Enter to Play Again\nPress Escape to Quit", font, 50);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition(WINDOW_WIDTH / 2 - gameOverText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 - 100);

    sf::Clock clock;
    GameState gameState = PLAYING;

    // Main game loop
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        float seconds = deltaTime.asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameState == GAME_OVER) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        // Restart the game
                        resetGame(spaceship, bullets, asteroids, maxAsteroids, asteroidSpeed);
                        gameState = PLAYING;
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                }
            }
        }

        if (gameState == PLAYING) {
            // Spaceship controls
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) spaceship.rotateLeft(seconds);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) spaceship.rotateRight(seconds);
            spaceship.move(seconds);
            spaceship.wrapAroundEdges(WINDOW_WIDTH, WINDOW_HEIGHT);

            // Shooting bullets
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bulletClock.getElapsedTime() > sf::seconds(bulletDelay)) {
                sf::Vector2f shipTip = spaceship.getShape().getTransform().transformPoint(spaceship.getShape().getPoint(1));
                bullets.emplace_back(shipTip, spaceship.getShape().getRotation());
                bulletClock.restart();
            }

            // Bullet movement and collision detection
            for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
                bulletIt->move(seconds, bulletSpeed);

                bool bulletHit = false;
                for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end(); ) {
                    if (bulletIt->getShape().getGlobalBounds().intersects(asteroidIt->getGlobalBounds())) {
                        bulletHit = true;

                        // Break asteroid into smaller pieces
                        if (asteroidIt->getRadius() > 20) {
                            float newRadius = asteroidIt->getRadius() / 2.0f;

                            for (int i = 0; i < 2; ++i) {
                                sf::Vector2f position = asteroidIt->getShape().getPosition();
                                sf::Vector2f velocity((std::rand() % 200 - 100) / 100.0f * asteroidSpeed,
                                                      (std::rand() % 200 - 100) / 100.0f * asteroidSpeed);
                                asteroids.emplace_back(newRadius, position, velocity);
                            }
                        }

                        asteroidIt = asteroids.erase(asteroidIt);
                    } else {
                        ++asteroidIt;
                    }
                }

                if (bulletHit || bulletIt->isOutOfBounds(WINDOW_WIDTH, WINDOW_HEIGHT)) {
                    bulletIt = bullets.erase(bulletIt);
                } else {
                    ++bulletIt;
                }
            }

            // Asteroid movement
            for (auto& asteroid : asteroids) {
                asteroid.move(seconds);
                asteroid.wrapAroundEdges(WINDOW_WIDTH, WINDOW_HEIGHT);
            }

            // Check spaceship collision with asteroids
            for (const auto& asteroid : asteroids) {
                if (spaceship.getGlobalBounds().intersects(asteroid.getGlobalBounds())) {
                    gameState = GAME_OVER;
                    break;
                }
            }
        }

        // Rendering
        window.clear();

        if (gameState == PLAYING) {
            window.draw(spaceship.getShape());
            for (const auto& bullet : bullets) window.draw(bullet.getShape());
            for (const auto& asteroid : asteroids) window.draw(asteroid.getShape());
        } else if (gameState == GAME_OVER) {
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}
