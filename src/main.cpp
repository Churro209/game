#include <SFML/Graphics.hpp>
#include "Bullet.hpp"
#include "Asteroid.hpp"
#include "Spaceship.hpp"
#include <libserialport.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>

constexpr unsigned int WINDOW_WIDTH = 1920;
constexpr unsigned int WINDOW_HEIGHT = 1080;

enum GameState {
    PLAYING,
    GAME_OVER
};

std::optional<sp_port*> setupSerial(const char* portName, int baudrate) {
    sp_port* serialPort;
    if (sp_get_port_by_name(portName, &serialPort) != SP_OK) {
        return std::nullopt;
    }

    if (sp_open(serialPort, SP_MODE_READ) != SP_OK) {
        sp_free_port(serialPort);
        return std::nullopt;
    }

    if (sp_set_baudrate(serialPort, baudrate) != SP_OK) {
        sp_close(serialPort);
        sp_free_port(serialPort);
        return std::nullopt;
    }

    sp_set_bits(serialPort, 8);
    sp_set_parity(serialPort, SP_PARITY_NONE);
    sp_set_stopbits(serialPort, 1);
    sp_set_flowcontrol(serialPort, SP_FLOWCONTROL_NONE);

    return serialPort;
}

std::string readSerialInput(std::optional<sp_port*>& serialPort) {
    if (serialPort) {
        char buffer[16];
        int bytesRead = sp_nonblocking_read(*serialPort, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0'; // Null-terminate the string
            return std::string(buffer);
        }
    }
    return "";
}

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

    sf::RenderWindow window({WINDOW_WIDTH, WINDOW_HEIGHT}, "Asteroids");
    window.setFramerateLimit(60);

    Spaceship spaceship;
    std::vector<Bullet> bullets;
    sf::Clock bulletClock;
    float bulletDelay = 0.5f;
    float bulletSpeed = 900.0f;

    std::vector<Asteroid> asteroids;
    const int maxAsteroids = 10;
    const float asteroidSpeed = 100.0f;

    resetGame(spaceship, bullets, asteroids, maxAsteroids, asteroidSpeed);

    sf::Font font;
    if (!font.loadFromFile("../assets/OpenSans-VariableFont_wdth,wght.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    sf::Text gameOverText("Game Over\nPress Enter to Restart\nPress Escape to Quit", font, 50);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition(WINDOW_WIDTH / 2 - gameOverText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 - 100);

    std::optional<sp_port*> serialPort = setupSerial("/dev/ttyACM0", 9600);

    sf::Clock clock;
    GameState gameState = PLAYING;

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
                        resetGame(spaceship, bullets, asteroids, maxAsteroids, asteroidSpeed);
                        gameState = PLAYING;
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                }
            }
        }

        if (gameState == PLAYING) {
            std::string serialInput = serialPort ? readSerialInput(serialPort) : "";

            // Spaceship movement
            if (serialInput.find("LEFT") != std::string::npos) {
                spaceship.rotateLeft(seconds);
            } else if (serialInput.find("RIGHT") != std::string::npos) {
                spaceship.rotateRight(seconds);
            } else {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) spaceship.rotateLeft(seconds);
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) spaceship.rotateRight(seconds);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                spaceship.move(seconds);
            }
            spaceship.wrapAroundEdges(WINDOW_WIDTH, WINDOW_HEIGHT);

            // Shooting bullets
            if ((serialInput.find("SHOOT") != std::string::npos || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) &&
                bulletClock.getElapsedTime() > sf::seconds(bulletDelay)) {
                sf::Vector2f shipTip = spaceship.getTipPosition();
                bullets.emplace_back(shipTip, spaceship.getRotation());
                bulletClock.restart();
            }

            // Update bullets
            for (auto& bullet : bullets) {
                bullet.move(seconds, bulletSpeed);
            }
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& bullet) {
                return bullet.isOutOfBounds(WINDOW_WIDTH, WINDOW_HEIGHT);
            }), bullets.end());

            // Update asteroids
            for (auto& asteroid : asteroids) {
                asteroid.move(seconds);
                asteroid.wrapAroundEdges(WINDOW_WIDTH, WINDOW_HEIGHT);
            }

            // Check collisions
            for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
                bool bulletHit = false;

                for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                    if (bulletIt->getShape().getGlobalBounds().intersects(asteroidIt->getGlobalBounds())) {
                        bulletHit = true;

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

                if (bulletHit) {
                    bulletIt = bullets.erase(bulletIt);
                } else {
                    ++bulletIt;
                }
            }

            for (const auto& asteroid : asteroids) {
                if (spaceship.getGlobalBounds().intersects(asteroid.getGlobalBounds())) {
                    gameState = GAME_OVER;
                    break;
                }
            }

            window.clear();
            window.draw(spaceship.getShape());
            for (const auto& bullet : bullets) window.draw(bullet.getShape());
            for (const auto& asteroid : asteroids) window.draw(asteroid.getShape());
            window.display();
        }

        if (gameState == GAME_OVER) {
            window.clear();
            window.draw(gameOverText);
            window.display();
        }
    }

    if (serialPort) {
        sp_close(*serialPort);
        sp_free_port(*serialPort);
    }

    return 0;
}
