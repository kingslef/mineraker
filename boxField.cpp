#include "boxField.hpp"

#include <random>

std::ostream & operator<<(std::ostream & out, const BoxField::Box & box)
{
    out << "(" << box.position.x << ", " << box.position.y << ")";
    return out;
}

namespace {
    // FIXME: not the best way to do this
    const char * mines_to_text(unsigned int mines)
    {
        switch(mines) {
        case 8:
            return "8";
        case 7:
            return "7";
        case 6:
            return "6";
        case 5:
            return "5";
        case 4:
            return "4";
        case 3:
            return "3";
        case 2:
            return "2";
        case 1:
            return "1";
        case 0:
            return "";
        default:
            return "x";
        }
    }
}

sf::Text BoxField::Box::boxDigit(const std::array<sf::Color, 9> & colors,
                  const sf::Font & font) const
{
    // FIXME: set default font size somewhere
    sf::Text text(mines_to_text(mines_touching), font, 20);
    if (mines_touching < colors.size()) {
        text.setColor(colors[mines_touching]);
    }
    return text;
}

BoxField::BoxField(const char * box_texture_file,
                   const char * pressed_box_texture_file,
                   const char * mine_texture_file,
                   const char * pressed_mine_texture_file,
                   const char * flag_texture_file,
                   const char * font_file,
                   unsigned int mines, unsigned int w, unsigned int h)
{
    if (!box_texture.loadFromFile(box_texture_file)) {
        throw std::invalid_argument("Couldn't open the box texture");
    }

    if (!pressed_box_texture.loadFromFile(pressed_box_texture_file)) {
        throw std::invalid_argument("Couldn't open the pressed box "
                                    "texture");
    }

    if (!mine_texture.loadFromFile(mine_texture_file)) {
        throw std::invalid_argument("Couldn't open the mine texture");
    }

    if (!pressed_mine_texture.loadFromFile(pressed_mine_texture_file)) {
        throw std::invalid_argument("Couldn't open the pressed mine texture");
    }

    if (!flag_texture.loadFromFile(flag_texture_file)) {
        throw std::invalid_argument("Couldn't open the flag texture");
    }

    if (!digit_font.loadFromFile(font_file)) {
        throw std::invalid_argument("Couldn't open the font");
    }

    box_sprite.setTexture(box_texture);
    pressed_box_sprite.setTexture(pressed_box_texture);
    mine_sprite.setTexture(mine_texture);
    pressed_mine_sprite.setTexture(pressed_mine_texture);
    flag_sprite.setTexture(flag_texture);

    // FIXME: remove scaling
    box_sprite.setScale(sf::Vector2f(.2f, .2f));
    pressed_box_sprite.setScale(sf::Vector2f(.2f, .2f));
    mine_sprite.setScale(sf::Vector2f(.2f, .2f));
    pressed_mine_sprite.setScale(sf::Vector2f(.2f, .2f));
    flag_sprite.setScale(sf::Vector2f(.2f, .2f));

    box_height = box_sprite.getGlobalBounds().height;
    box_width = box_sprite.getGlobalBounds().width;

    width = w / box_width;
    height = h / box_height;

    std::default_random_engine generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0,1);

    for (unsigned int i = 0; i < width; i++) {
        std::vector<Box> row;
        for (unsigned int j = 0; j < height; j++) {
            // FIXME: move to a function
            bool is_mine = mines ? distribution(generator) : false;
            if (is_mine) {
                std::cout << "mine in (" << i << "," << j << ")" << std::endl;
                mines--;
            }

            row.push_back(Box(i, j, is_mine));
        }
        field.push_back(row);
    }

    calculateTouchingMines();
}

void BoxField::calculateTouchingMines()
{
    for (unsigned int i = 0; i < width; i++) {
        for (unsigned int j = 0; j < height; j++) {
            auto & box = field[i][j];
            if (box.mine) {
                continue;
            }

            for (int x = -1; x <= 1; x++) {
                if ((i == 0 && x == -1 ) || i + x >= width) {
                    continue;
                }
                for (int y = -1; y <= 1; y++) {
                    if ((j == 0 && y == - 1) || j + y >= height) {
                        continue;
                    }
                    if (field[i + x][j + y].mine) {
                        box.mines_touching++;
                    }
                }
            }
        }
    }
}

void BoxField::draw(sf::RenderWindow & window)
{
    for (auto & row : field) {
        for (auto & box : row) {
            sf::Vector2f pos(box.position.x * box_width, box.position.y * box_height);
            sf::Sprite sprite;
            if (box.mine && box.pressed) {
                sprite = pressed_mine_sprite;
            } else if (box.pressed) {
                sprite = pressed_box_sprite;
            } else if (box.marked && !box.pressed) {
                sprite = flag_sprite;
            } else {
                sprite = box_sprite;
            }

            // If game is over, show all mines
            // FIXME: this forgets marked boxes
            if (game_over) {
                if (box.mine && !box.pressed) {
                    sprite = mine_sprite;
                }
            }

            sprite.setPosition(pos);
            window.draw(sprite);

            if (box.pressed && !box.mine) {
                sf::Text text = box.boxDigit(digit_colors, digit_font);
                // Text is not quite centered so it has to be moved a little
                text.setPosition(pos - sf::Vector2f(-4, 3));
                window.draw(text);
            }
        }
    }
}

const sf::Vector2u BoxField::calculatePressedBox(const sf::Vector2u & position) const
{
    /**
     * Calculate to which box is pressed with given position. E.g. if position.x
     * is somewhere between [0,1), then pressed box should be in first column.
     */
    unsigned int x = position.x / box_width
        + (position.x % box_width ? 1 : 0)
        - 1;
    unsigned int y = position.y / box_height
        + (position.y % box_height ? 1 : 0)
        - 1;

    return sf::Vector2u(x, y);
}

void BoxField::mark(const sf::Vector2u & position)
{
    if (!game_over) {
        const sf::Vector2u & box_pos = calculatePressedBox(position);

        std::cout << "box: (" << box_pos.x << "," << box_pos.y << ")" << std::endl;

        // This shouldn't be possible
        if (box_pos.x >= width || box_pos.y >= height) {
            throw std::invalid_argument("Invalid press location");
        }

        auto & box = field[box_pos.x][box_pos.y];
        box.marked = !box.marked;
    }
}

void BoxField::pressAdjacent(int x, int y)
{
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    auto & box = field[x][y];
    if (box.mine || box.pressed) {
        return;
    }

    box.pressed = true;

    /* Try to press all adjacent boxes, but not diagonally adjacent. */
    pressAdjacent(x - 1, y);
    pressAdjacent(x, y - 1);
    pressAdjacent(x + 1, y);
    pressAdjacent(x, y + 1);
}

void BoxField::press(const sf::Vector2u & position)
{
    if (!game_over) {
        const sf::Vector2u & box_pos = calculatePressedBox(position);

        std::cout << "box: (" << box_pos.x << "," << box_pos.y << ")" << std::endl;

        // This shouldn't be possible
        if (box_pos.x >= width || box_pos.y >= height) {
            throw std::invalid_argument("Invalid press location");
        }

        auto & box = field[box_pos.x][box_pos.y];

        if (!box.mine) {
            pressAdjacent(box_pos.x, box_pos.y);
        } else {
            box.pressed = true;
            game_over = true;
        }
    }
}

void BoxField::reset()
{
    if (game_over) {
        for (auto & row : field) {
            for (auto & box : row) {
                box.pressed = false;
                box.marked = false;
            }
        }

        game_over = false;
    }
}

bool BoxField::isOver() const
{
    return game_over;
}

std::ostream & operator<<(std::ostream & out, const BoxField & boxField)
{
    for (auto & row : boxField.field) {
        for (auto & box : row) {
            out << box << ", ";
        }
        out << std::endl;
    }

    return out;
}
