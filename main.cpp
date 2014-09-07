#include "boxField.hpp"

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Minerake",
                            sf::Style::Titlebar | sf::Style::Close);

    sf::Texture texture;
    if (!texture.loadFromFile("box.png"))
    {
        return -1;
    }

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(sf::Vector2f(0.2f, .2f));

    BoxField boxField("box.png", "box_pressed.png", "mine.png",
                      "mine_pressed.png", 250,
                      window.getSize().x / sprite.getGlobalBounds().width,
                      window.getSize().y / sprite.getGlobalBounds().height);

    window.setVerticalSyncEnabled(true);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    std::cout << "the left button was pressed on ("
                              << event.mouseButton.x << ","
                              << event.mouseButton.y << ")" << std::endl;

                    boxField.press(sf::Vector2u(event.mouseButton.x,
                                                event.mouseButton.y));
                }
                break;
            default:
                break;
            }
        }

        window.clear();

        boxField.draw(window);

        window.display();
    }

    return 0;
}
