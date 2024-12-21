#include <vector>
#include <array>
#include <string>

struct Coordinate
{
    int x;
    int y;

    bool operator==(const Coordinate& c)
    {
        if (x == c.x && y == c.y)
        {
            return true;
        }
        return false;
    }

    void operator+=(const Coordinate& c)
    {
        x += c.x;
        y += c.y;
    }

    void operator*=(int n)
    {
        x *= n;
        y *= n;
    }
};

static const Coordinate MOVEMENT_NULL = {0,0};
static const Coordinate MOVEMENT_UP = {0,-1};
static const Coordinate MOVEMENT_DOWN = {0,1};
static const Coordinate MOVEMENT_LEFT = {-1,0};
static const Coordinate MOVEMENT_RIGHT = {1,0};

struct Square
{
    Coordinate coord;
    std::string color;
};

const size_t SQUARES_PER_PIECE = 4;

enum PIECE_TYPE
{
    SQUARE,
    LINE,
    L_RIGHT,
    L_LEFT,
    T,
    Z_RIGHT,
    Z_LEFT,
    LENGTH
};

class Piece
{
public:
    Piece() = default;
    Piece(PIECE_TYPE t, int game_width);
    void Move(const Coordinate& movement);
    void Rotate(const Coordinate& coord_limits);
    std::vector<Square> GetSquares();
    
public:
    PIECE_TYPE m_type;
    size_t m_rotation_index = 0;
    std::string m_color;
    std::array<Coordinate, SQUARES_PER_PIECE> m_coords;
};