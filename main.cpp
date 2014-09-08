#include "boxField.hpp"

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Mineraker",
                            sf::Style::Titlebar | sf::Style::Close);

    BoxField boxField(20, window.getSize().x, window.getSize().y);

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
                    boxField.press(sf::Vector2u(event.mouseButton.x,
                                                event.mouseButton.y));
                    boxField.checkIfWon();
                }
                else if (event.mouseButton.button == sf::Mouse::Right)
                {
                    if (boxField.isOver()) {
                        boxField.reset();
                    } else {
                        boxField.mark(sf::Vector2u(event.mouseButton.x,
                                                   event.mouseButton.y));
                        boxField.checkIfWon();
                    }
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
