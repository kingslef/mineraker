#ifndef BOXFIELD_HPP
#define BOXFIELD_HPP

#include <SFML/Graphics.hpp>

#include <stdexcept>
#include <vector>
#include <iostream>

class BoxField {

public:
    BoxField(const char * box_texture_file,
             const char * pressed_box_texture_file,
             const char * mine_texture_file,
             unsigned int mines,
             unsigned int width,
             unsigned int height);

    void draw(sf::RenderWindow & window);

    void press(const sf::Vector2u & position);

    friend std::ostream & operator<<(std::ostream & out, const BoxField & field);

    struct Box {
        Box(const sf::Vector2u & pos, bool is_mine=false)
            : position(pos), mine(is_mine) {}
        Box(unsigned int x, unsigned int y, bool is_mine=false)
            : position(sf::Vector2u(x, y)), mine(is_mine) {}

        friend std::ostream & operator<<(std::ostream & out, const Box & box);

        sf::Vector2u position;
        bool pressed = false;
        bool mine = false;
    };

private:
    sf::Sprite box_sprite;
    sf::Sprite pressed_box_sprite;
    sf::Sprite mine_sprite;

    sf::Texture box_texture;
    sf::Texture pressed_box_texture;
    sf::Texture mine_texture;

    std::vector<std::vector<BoxField::Box>> field;
};

#endif
