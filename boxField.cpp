#include "boxField.hpp"

#include <random>

std::ostream & operator<<(std::ostream & out, const BoxField::Box & box)
{
    out << "(" << box.position.x << ", " << box.position.y << ")";
    return out;
}

BoxField::BoxField(const char * box_texture_file,
                   const char * pressed_box_texture_file,
                   const char * mine_texture_file,
                   unsigned int mines,
                   unsigned int w,
                   unsigned int h) : width(w), height(h)
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

    box_sprite.setTexture(box_texture);
    pressed_box_sprite.setTexture(pressed_box_texture);
    mine_sprite.setTexture(mine_texture);

    // FIXME: remove scaling
    box_sprite.setScale(sf::Vector2f(.2f, .2f));
    pressed_box_sprite.setScale(sf::Vector2f(.2f, .2f));
    mine_sprite.setScale(sf::Vector2f(.2f, .2f));

    box_height = box_sprite.getGlobalBounds().height;
    box_width = box_sprite.getGlobalBounds().width;

    std::default_random_engine generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0,1);

    for (unsigned int i = 0; i < width; i++) {
        std::vector<Box> row;
        for (unsigned int j = 0; j < height; j++) {
            bool is_mine = mines ? distribution(generator) : false;
            if (is_mine) {
                std::cout << "mine in (" << i << "," << j << ")" << std::endl;
                mines--;
            }

            row.push_back(Box(i, j, is_mine));
        }
        field.push_back(row);
    }
}

void BoxField::draw(sf::RenderWindow & window)
{
    for (auto & row : field) {
        for (auto & box : row) {
            sf::Vector2f pos(box.position.x * box_width, box.position.y * box_height);
            sf::Sprite sprite;
            if (box.mine && box.pressed) {
                sprite = mine_sprite;
            } else if (box.pressed) {
                sprite = pressed_box_sprite;
            } else {
                sprite = box_sprite;
            }

            sprite.setPosition(pos);
            window.draw(sprite);
        }
    }
}

void BoxField::press(const sf::Vector2u & position)
{
    /*
     * Calculate to which box is pressed with given position. E.g. if position.x
     * is somewhere between [0,1), then pressed box should be in first column.
     */
    unsigned int x = position.x / box_width
        + (position.x % box_width ? 1 : 0)
        - 1;
    unsigned int y = position.y / box_height
        + (position.y % box_height ? 1 : 0)
        - 1;

    std::cout << "box: (" << x << "," << y << ")" << std::endl;

    // This shouldn't be possible
    if (x >= width || y >= height) {
        throw std::invalid_argument("Invalid press location");
    }

    auto & box = field[x][y];

    //box.pressed = true;

    if (!box.mine) {
        // If user didn't press a mine, press all adjacent non-mine boxes.
        // FIXME: ugly
        for (int j = y; j >= 0; j--) {
            auto & adj_box = field[x][j];
            if (adj_box.mine || adj_box.pressed) {
                break;
            }
            adj_box.pressed = true;


            for (int i = x + 1; i < width; i++) {
                auto & adj_box = field[i][j];
                if (adj_box.mine || adj_box.pressed) {
                    break;
                }
                adj_box.pressed = true;
            }

            for (int i = x - 1; i >= 0; i--) {
                auto & adj_box = field[i][j];
                if (adj_box.mine || adj_box.pressed) {
                    break;
                }
                adj_box.pressed = true;
            }
        }

        for (int j = y + 1; j < height; j++) {
            auto & adj_box = field[x][j];
            if (adj_box.mine || adj_box.pressed) {
                break;
            }
            adj_box.pressed = true;

            for (int i = x + 1; i < width; i++) {
                auto & adj_box = field[i][j];
                if (adj_box.mine || adj_box.pressed) {
                    break;
                }
                adj_box.pressed = true;
            }

            for (int i = x - 1; i >= 0; i--) {
                auto & adj_box = field[i][j];
                if (adj_box.mine || adj_box.pressed) {
                    break;
                }
                adj_box.pressed = true;
            }
        }
    } else {
        box.pressed = true;
    }
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
