#ifndef BOXFIELD_HPP
#define BOXFIELD_HPP

#include <SFML/Graphics.hpp>

#include <stdexcept>
#include <vector>
#include <iostream>
#include <array>

class BoxField {

public:
    BoxField(unsigned int mines, unsigned int width, unsigned int height);

    void draw(sf::RenderWindow & window);
    void press(const sf::Vector2u & position);
    void mark(const sf::Vector2u & position);
    void reset();
    bool isOver() const;
    bool checkIfWon();

    struct Box {
        Box(const sf::Vector2u & pos, bool is_mine=false)
            : position(pos), mine(is_mine) {}
        Box(unsigned int x, unsigned int y, bool is_mine=false)
            : position(sf::Vector2u(x, y)), mine(is_mine) {}

        sf::Text boxDigit(const std::array<sf::Color, 9> & colors,
                          const sf::Font & font) const;

        sf::Vector2u position;
        bool pressed = false;
        bool mine = false;
        bool flagged = false;
        unsigned int mines_touching = 0;
    };

private:

    const sf::Vector2u calculatePressedBox(const sf::Vector2u & position) const;
    void pressAdjacent(int x, int y);
    void setupMines();
    void calculateTouchingMines();

    sf::Sprite box_sprite;
    sf::Sprite pressed_box_sprite;
    sf::Sprite mine_sprite;
    sf::Sprite pressed_mine_sprite;
    sf::Sprite flagged_mine_sprite;
    sf::Sprite flag_sprite;

    sf::Texture box_texture;
    sf::Texture pressed_box_texture;
    sf::Texture mine_texture;
    sf::Texture pressed_mine_texture;
    sf::Texture flagged_mine_texture;
    sf::Texture flag_texture;

    unsigned int mines_amount;

    sf::Font digit_font;
    std::array<sf::Color, 9> digit_colors = { {
            sf::Color::Black,
            sf::Color::Green,
            sf::Color::Blue,
            sf::Color::Yellow,
            sf::Color::Magenta,
            sf::Color::Red,
            sf::Color::Black,
            sf::Color::Black,
            sf::Color::Black
        }
    };

    std::size_t width;
    std::size_t height;

    std::size_t box_width;
    std::size_t box_height;

    std::vector<std::vector<BoxField::Box>> field;

    bool game_over = false;
    bool is_mines_set = false;
};

#endif
