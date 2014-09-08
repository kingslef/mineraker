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

BoxField::BoxField(unsigned int mines, unsigned int w, unsigned int h)
    : mines_amount(mines)
{
    if (!box_texture.loadFromFile("textures/box.png")) {
        throw std::invalid_argument("Couldn't open the box texture");
    }

    if (!pressed_box_texture.loadFromFile("textures/box_pressed.png")) {
        throw std::invalid_argument("Couldn't open the pressed box "
                                    "texture");
    }

    if (!mine_texture.loadFromFile("textures/mine.png")) {
        throw std::invalid_argument("Couldn't open the mine texture");
    }

    if (!pressed_mine_texture.loadFromFile("textures/mine_pressed.png")) {
        throw std::invalid_argument("Couldn't open the pressed mine texture");
    }

    if (!flagged_mine_texture.loadFromFile("textures/mine_flagged.png")) {
        throw std::invalid_argument("Couldn't open the flagged mine texture");
    }

    if (!flag_texture.loadFromFile("textures/flag.png")) {
        throw std::invalid_argument("Couldn't open the flag texture");
    }

    if (!digit_font.loadFromFile("textures/arial.ttf")) {
        throw std::invalid_argument("Couldn't open the font");
    }

    box_sprite.setTexture(box_texture);
    pressed_box_sprite.setTexture(pressed_box_texture);
    mine_sprite.setTexture(mine_texture);
    pressed_mine_sprite.setTexture(pressed_mine_texture);
    flagged_mine_sprite.setTexture(flagged_mine_texture);
    flag_sprite.setTexture(flag_texture);

    // FIXME: remove scaling
    box_sprite.setScale(sf::Vector2f(.2f, .2f));
    pressed_box_sprite.setScale(sf::Vector2f(.2f, .2f));
    mine_sprite.setScale(sf::Vector2f(.2f, .2f));
    pressed_mine_sprite.setScale(sf::Vector2f(.2f, .2f));
    flagged_mine_sprite.setScale(sf::Vector2f(.2f, .2f));
    flag_sprite.setScale(sf::Vector2f(.2f, .2f));

    box_height = box_sprite.getGlobalBounds().height;
    box_width = box_sprite.getGlobalBounds().width;

    width = w / box_width;
    height = h / box_height;

    for (unsigned int i = 0; i < width; i++) {
        std::vector<Box> row;
        for (unsigned int j = 0; j < height; j++) {
            row.push_back(Box(i, j));
        }
        field.push_back(row);
    }
}

void BoxField::setupMines()
{
    std::default_random_engine generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0,1);
    std::uniform_int_distribution<int> distribution_per_row(0,3);

    unsigned int mines_set = 0;

    /* Try to divide roughly same amount of mines for each row so first rows are
     * not so packed. */
    unsigned int mines_per_row = mines_amount / field.size()
        + (mines_amount % field.size() ? 1 : 0);

    for (auto & row : field) {
        unsigned int mines_set_per_row = 0;

        /* Random variable when setting mines per row so player cannot assume
         * that every row has same amount of mines.
         */
        unsigned int mines_per_row_rand = mines_per_row
            + distribution_per_row(generator);

        for (auto & box : row) {
            if (box.pressed || mines_set_per_row == mines_per_row_rand) {
                continue;
            }

            bool is_mine = mines_set != mines_amount ?
                distribution(generator) : false;
            if (is_mine) {
                box.mine = true;
                mines_set++;
                mines_set_per_row++;
            }
        }
    }

    is_mines_set = true;
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
            } else if (box.flagged && !box.pressed) {
                sprite = flag_sprite;
            } else {
                sprite = box_sprite;
            }

            // If game is over, show all mines
            if (game_over) {
                if (box.mine && !box.pressed) {
                    if (box.flagged) {
                        sprite = flagged_mine_sprite;
                    } else {
                        sprite = mine_sprite;
                    }
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
        box.flagged = !box.flagged;
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

        if (!is_mines_set) {
            /* If mines are not set, this is the first press so we can setup the
             * mines now. If we would have done it earlier, game could be over
             * on first press which isn't very nice.
             */
            // FIXME: hack
            box.pressed = true;
            setupMines();
            box.pressed = false;
            calculateTouchingMines();
        }

        if (!box.mine) {
            pressAdjacent(box_pos.x, box_pos.y);
        } else {
            box.pressed = true;
            game_over = true;
        }
    }
}

bool BoxField::checkIfWon()
{
    for (auto & row : field) {
        for (auto & box : row) {
            if (box.mine && !box.flagged) {
                std::cout << "found unflagged mine" << std::endl;
                return false;
            }

            if (!box.mine && !box.pressed) {
                std::cout << "found unpressed box" << std::endl;
                return false;
            }
        }
    }

    std::cout << "All mines are flagged and boxes without mines are pressed!" << std::endl;
    game_over = true;
    return true;
}

void BoxField::reset()
{
    if (game_over) {
        for (auto & row : field) {
            for (auto & box : row) {
                box.pressed = false;
                box.flagged = false;
                box.mine = false;
                box.mines_touching = 0;
            }
        }

        is_mines_set = false;
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
