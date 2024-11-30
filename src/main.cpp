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
    MAIN_MENU,
    PLAYING,
    GAME_OVER
};

// Serial port setup
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

// Read serial input as a single byte
std::optional<uint8_t> readSerialInput(std::optional<sp_port*>& serialPort) {
    if (serialPort) {
        uint8_t buffer;
        int bytesRead = sp_nonblocking_read(*serialPort, &buffer, 1);
        if (bytesRead == 1) {
            return buffer;
        }
    }
    return std::nullopt;
}

void displayMenu(sf::RenderWindow& window, sf::Text& loadingText, sf::Text& statusText) {
    window.clear();
    window.draw(loadingText);
    window.draw(statusText);
    window.display();
}

void resetGame(Spaceship& spaceship, std::vector<Bullet>& bullets, std::vector<Asteroid>& asteroids, const int maxAsteroids, const float asteroidSpeed) {
    spaceship.getShape().setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    spaceship.getShape().setRotation(0);

    bullets.clear();
    asteroids.clear();

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

    sf::Font font;
    if (!font.loadFromFile("../assets/OpenSans-VariableFont_wdth,wght.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    sf::Text loadingText("Loading...\nConnecting to Controller", font, 50);
    loadingText.setFillColor(sf::Color::White);
    loadingText.setPosition(WINDOW_WIDTH / 2 - loadingText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 - 200);

    sf::Text statusText("Controller Status: Not Connected", font, 50);
    statusText.setFillColor(sf::Color::Red);
    statusText.setPosition(WINDOW_WIDTH / 2 - statusText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2);

    sf::Text gameOverText("Game Over\nPress Enter to Restart\nPress Escape to Quit", font, 50);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition(WINDOW_WIDTH / 2 - gameOverText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 - 100);

    std::optional<sp_port*> serialPort = setupSerial("/dev/tty.usbmodem2103", 9600);
    if (serialPort) {
        statusText.setString("Controller Status: Connected");
        statusText.setFillColor(sf::Color::Green);
    }

    Spaceship spaceship;
    std::vector<Bullet> bullets;
    std::vector<Asteroid> asteroids;
    sf::Clock bulletClock;

    const float bulletDelay = 0.5f;
    const float bulletSpeed = 900.0f;
    const int maxAsteroids = 10;
    const float asteroidSpeed = 100.0f;

    resetGame(spaceship, bullets, asteroids, maxAsteroids, asteroidSpeed);

    sf::Clock clock;
    sf::Clock actionClock;
    const float actionInterval = 0.05f; // Minimum interval between actions (50ms)

    GameState gameState = serialPort ? PLAYING : MAIN_MENU;

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

        if (gameState == MAIN_MENU) {
            displayMenu(window, loadingText, statusText);

            if (serialPort) {
                gameState = PLAYING;
            } else {
                serialPort = setupSerial("/dev/tty.usbmodem2103", 9600);
                if (serialPort) {
                    statusText.setString("Controller Status: Connected");
                    statusText.setFillColor(sf::Color::Green);
                }
            }
        }

        if (gameState == PLAYING) {
            if (serialPort) {
                auto serialInputOpt = readSerialInput(serialPort);

                while (serialInputOpt.has_value()) {
                    uint8_t serialInput = serialInputOpt.value();
                    bool rotateLeft = (serialInput == 0x0E);
                    bool moveForward = (serialInput == 0x0D);
                    bool rotateRight = (serialInput == 0x0B);
                    bool shoot = (serialInput == 0x07);

                    // Handle movement and rotation
                    if (actionClock.getElapsedTime().asSeconds() > actionInterval) {
                        if (rotateLeft && !rotateRight) {
                            spaceship.rotateLeft(seconds);
                        }
                        if (rotateRight && !rotateLeft) {
                            spaceship.rotateRight(seconds);
                        }
                        if (moveForward) {
                            spaceship.move(seconds);
                        }
                        actionClock.restart();
                    }

                    // Handle shooting independently
                    if (shoot && bulletClock.getElapsedTime() > sf::seconds(bulletDelay)) {
                        sf::Vector2f shipTip = spaceship.getTipPosition();
                        bullets.emplace_back(shipTip, spaceship.getRotation());
                        bulletClock.restart();
                    }

                    serialInputOpt = readSerialInput(serialPort);
                }
            }

            spaceship.wrapAroundEdges(WINDOW_WIDTH, WINDOW_HEIGHT);

            for (auto& bullet : bullets) {
                bullet.move(seconds, bulletSpeed);
            }
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& bullet) {
                return bullet.isOutOfBounds(WINDOW_WIDTH, WINDOW_HEIGHT);
            }), bullets.end());

            for (auto& asteroid : asteroids) {
                asteroid.move(seconds);
                asteroid.wrapAroundEdges(WINDOW_WIDTH, WINDOW_HEIGHT);
            }

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
