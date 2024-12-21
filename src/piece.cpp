#include "piece.hpp"

static const size_t NUM_ROT_SQUARE = 1;
static const size_t NUM_ROT_LINE = 2;
static const size_t NUM_ROT_L_RIGHT = 4;
static const size_t NUM_ROT_L_LEFT = 4;
static const size_t NUM_ROT_T = 4;
static const size_t NUM_ROT_Z_RIGHT = 2;
static const size_t NUM_ROT_Z_LEFT = 2;

static const std::array<std::array<Coordinate, SQUARES_PER_PIECE>, NUM_ROT_SQUARE> ROTATIONS_SQUARE =
{{{
    { {0,0}, {1,0}, {0,1}, {1,1} }
}}};

static const std::array<std::array<Coordinate, SQUARES_PER_PIECE>, NUM_ROT_LINE> ROTATIONS_LINE =
{{
    {{ {-1,0}, {0,0}, {1,0}, {2,0} }},
    {{ {0,-1}, {0,0}, {0,1}, {0,2} }}
}};

static const std::array<std::array<Coordinate, SQUARES_PER_PIECE>, NUM_ROT_L_RIGHT> ROTATIONS_L_RIGHT = 
{{
    {{ {0,0}, {1,0}, {2,0}, {0,1} }},
    {{ {0,0}, {0,1}, {0,2}, {-1,0} }},
    {{ {0,0}, {-1,0}, {-2,0}, {0,-1} }},
    {{ {0,0}, {0,-1}, {0,-2}, {1,0} }}
}};

static const std::array<std::array<Coordinate, SQUARES_PER_PIECE>, NUM_ROT_L_LEFT> ROTATIONS_L_LEFT =
{{
    {{ {0,0}, {-1,0}, {-2,0}, {0,1} }},
    {{ {0,0}, {0,-1}, {0,-2}, {-1,0} }},
    {{ {0,0}, {1,0}, {2,0}, {0,-1} }},
    {{ {0,0}, {0,1}, {0,2}, {1,0} }}
}};

static const std::array<std::array<Coordinate, SQUARES_PER_PIECE>, NUM_ROT_T> ROTATIONS_T =
{{
    {{ {-1,0}, {0,0}, {1,0}, {0,1} }},
    {{ {0,-1}, {0,0}, {0,1}, {-1,0} }},
    {{ {1,0}, {0,0}, {-1,0}, {0,-1} }},
    {{ {0,1}, {0,0}, {0,-1}, {1,0} }}
}};

static const std::array<std::array<Coordinate, SQUARES_PER_PIECE>, NUM_ROT_Z_RIGHT> ROTATIONS_Z_RIGHT =
{{
    {{ {-1,0}, {0,0}, {0,1}, {1,1} }},
    {{ {0,-1}, {0,0}, {-1,0}, {-1,1} }}
}};

static const std::array<std::array<Coordinate, SQUARES_PER_PIECE>, NUM_ROT_Z_LEFT> ROTATIONS_Z_LEFT =
{{
    {{ {-1,1}, {0,1}, {0,0}, {1,0} }},
    {{ {-1,-1}, {-1,0}, {0,0}, {0,1} }}
}};

Piece::Piece(PIECE_TYPE t, int game_width)
{
    m_type = t;
    m_rotation_index = 0;

    switch (m_type)
    {
    case SQUARE:
        m_coords = ROTATIONS_SQUARE[m_rotation_index];
        m_color = "yellow";
        break;

    case LINE:
        m_coords = ROTATIONS_LINE[m_rotation_index];
        m_color = "cyan";
        break;
        
    case L_RIGHT:
        m_coords = ROTATIONS_L_RIGHT[m_rotation_index];
        m_color = "orange";
        break;

    case L_LEFT:
        m_coords = ROTATIONS_L_LEFT[m_rotation_index];
        m_color = "blue";
        break;

    case T:
        m_coords = ROTATIONS_T[m_rotation_index];
        m_color = "purple";
        break;

    case Z_RIGHT:
        m_coords = ROTATIONS_Z_RIGHT[m_rotation_index];
        m_color = "green";
        break;

    case Z_LEFT:
        m_coords = ROTATIONS_Z_LEFT[m_rotation_index];
        m_color = "red";
        break;
    }

    Coordinate movement = MOVEMENT_RIGHT;
    movement *= (game_width / 2);

    Move(movement);
}

void Piece::Move(const Coordinate& movement)
{
    for (Coordinate& coord : m_coords)
    {
        coord += movement;
    }
}

void Piece::Rotate(const Coordinate& coord_limits)
{
    std::array<Coordinate, SQUARES_PER_PIECE> current_rot;
    std::array<Coordinate, SQUARES_PER_PIECE> next_rot;

    size_t current_rot_index = m_rotation_index;
    m_rotation_index++;
    
    switch (m_type)
    {
    case SQUARE:
        if (m_rotation_index >= ROTATIONS_SQUARE.size())
        {
            m_rotation_index = 0;
        }
        current_rot = ROTATIONS_SQUARE[current_rot_index];
        next_rot = ROTATIONS_SQUARE[m_rotation_index];
        break;
    case LINE:
        if (m_rotation_index >= ROTATIONS_LINE.size())
        {
            m_rotation_index = 0;
        }
        current_rot = ROTATIONS_LINE[current_rot_index];
        next_rot = ROTATIONS_LINE[m_rotation_index];
        break;

    case L_RIGHT:
        if (m_rotation_index >= ROTATIONS_L_RIGHT.size())
        {
            m_rotation_index = 0;
        }
        current_rot = ROTATIONS_L_RIGHT[current_rot_index];
        next_rot = ROTATIONS_L_RIGHT[m_rotation_index];
        break;

    case L_LEFT:
        if (m_rotation_index >= ROTATIONS_L_LEFT.size())
        {
            m_rotation_index = 0;
        }
        current_rot = ROTATIONS_L_LEFT[current_rot_index];
        next_rot = ROTATIONS_L_LEFT[m_rotation_index];
        break;

    case T:
        if (m_rotation_index >= ROTATIONS_T.size())
        {
            m_rotation_index = 0;
        }
        current_rot = ROTATIONS_T[current_rot_index];
        next_rot = ROTATIONS_T[m_rotation_index];
        break;

    case Z_RIGHT:
        if (m_rotation_index >= ROTATIONS_Z_RIGHT.size())
        {
            m_rotation_index = 0;
        }
        current_rot = ROTATIONS_Z_RIGHT[current_rot_index];
        next_rot = ROTATIONS_Z_RIGHT[m_rotation_index];
        break;

    case Z_LEFT:
        if (m_rotation_index >= ROTATIONS_Z_LEFT.size())
        {
            m_rotation_index = 0;
        }
        current_rot = ROTATIONS_Z_LEFT[current_rot_index];
        next_rot = ROTATIONS_Z_LEFT[m_rotation_index];
        break;
    }

    for (size_t i = 0; i < SQUARES_PER_PIECE; i++)
    {
        Coordinate offset
        {
            .x = m_coords[i].x - current_rot[i].x,
            .y = m_coords[i].y - current_rot[i].y
        };

        m_coords[i].x = next_rot[i].x + offset.x;
        m_coords[i].y = next_rot[i].y + offset.y;
    }

    bool in_bounds = false;

    while (!in_bounds)
    {
        in_bounds = true;

        for (Coordinate& c : m_coords)
        {
            if (c.x < 0)
            {
                Move(MOVEMENT_RIGHT);
                in_bounds = false;
                break;
            }

            if (c.x >= coord_limits.x)
            {
                Move(MOVEMENT_LEFT);
                in_bounds = false;
                break;
            }

            if (c.y < 0)
            {
                Move(MOVEMENT_DOWN);
                in_bounds = false;
                break;
            }

            if (c.y >= coord_limits.y)
            {
                Move(MOVEMENT_UP);
                in_bounds = false;
                break;
            }
        }
    }
}

std::vector<Square> Piece::GetSquares()
{
    std::vector<Square> squares;

    for (const Coordinate& coord : m_coords)
    {
        Square sq;
        sq.color = m_color;
        sq.coord = coord;
        squares.push_back(sq);
    }

    return squares;
}