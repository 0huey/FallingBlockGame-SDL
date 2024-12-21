#include "game_state.hpp"
#include "debug.hpp"
#include "utility.hpp"
#include "audio.hpp"
#include <format>

static const char* TEXTURE_PATH = "texture/game";

InGameState::InGameState(SDL_Window* window, SDL_Renderer* renderer)
{
    DEBUG_PRINT("InGameState created");

    m_window = window;
    m_renderer = renderer;
    LoadTextures(m_renderer, TEXTURE_PATH, m_texture_data);
    m_sound_effects = LoadSoundEffects();

    m_font = TTF_OpenFont(FONT_PATH, 50);

    if (m_font == NULL)
    {
        std::cerr << "Could not load font from " << FONT_PATH << std::endl;
        exit(-1);
    }

    int screen_width, screen_height;
    SDL_GetRendererOutputSize(m_renderer, &screen_width, &screen_height);

    m_game_view.w = (int)(screen_width * 0.65);
    m_game_view.y = (int)(screen_height * .10);
    m_cube_size = (int)(m_game_view.w / m_coord_limits.x);
    m_game_view.w = m_cube_size * m_coord_limits.x;
    m_game_view.h = m_cube_size * m_coord_limits.y;

    if ((m_game_view.y + m_game_view.h) > (screen_height - 5))
    {
        m_game_view.h = screen_height - m_game_view.y - 5;
        m_cube_size = (int)(m_game_view.h / m_coord_limits.y);
        m_game_view.h = m_cube_size * m_coord_limits.y; 
        m_game_view.w = m_cube_size * m_coord_limits.x;
    }

    m_game_view.x = (int)(screen_width / 2) - (int)(m_game_view.w / 2);

    for (int i = 0; i < m_game_border.size(); i++)
    {
        m_game_border[i].x = m_game_view.x - 1 * i - 1;
        m_game_border[i].y = m_game_view.y - 1 * i - 1;
        m_game_border[i].w = m_game_view.w + 2 * i + 2;
        m_game_border[i].h = m_game_view.h + 2 * i + 2;
    }
    
    m_score = Label(m_renderer, m_font, "Lines: 0", COLOR_WHITE);
    m_score.Reposition(5, 5, false);

    NewPiece();
}

STATE InGameState::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN && !event.key.repeat)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_w:
        case SDLK_UP:
            Rotate();
            break;

        case SDLK_a:
        case SDLK_LEFT:
            m_LR_key_state = KEY_LEFT;
            break;

        case SDLK_d:
        case SDLK_RIGHT:
            m_LR_key_state = KEY_RIGHT;
            break;

        case SDLK_s:
        case SDLK_DOWN:
            m_down_key_state = true;
            break;

        case SDLK_F1:
            ChangeSpeed(SpeedChange::UP);
            break;

        case SDLK_F2:
            ChangeSpeed(SpeedChange::DOWN);
            break;
        }
    }

    else if (event.type == SDL_KEYUP)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_a:
        case SDLK_LEFT:
            if (m_LR_key_state == KEY_LEFT)
                m_LR_key_state = KEY_NONE;
            break;

        case SDLK_d:
        case SDLK_RIGHT:
            if (m_LR_key_state == KEY_RIGHT)
                m_LR_key_state = KEY_NONE;
            break;

        case SDLK_s:
        case SDLK_DOWN:
            m_down_key_state = false;
            break;
        }
    }

    return STATE_UNCHANGED;
}

STATE InGameState::Step(double delta_time_sec)
{
    if (!m_game_running)
    {
        return STATE_UNCHANGED;
    }

    m_time_since_down_move += delta_time_sec;
    m_time_since_LR_move += delta_time_sec;

    if (m_LR_key_state != KEY_NONE && m_time_since_LR_move > m_hold_key_move_speed)
    {
        m_time_since_LR_move = 0;

        switch (m_LR_key_state)
        {
        case KEY_LEFT:
            Move(MOVEMENT_LEFT);
            break;
        case KEY_RIGHT:
            Move(MOVEMENT_RIGHT);
            break;
        }
    }

    bool should_auto_fall = (m_time_since_down_move > m_fall_speed);
    bool should_hold_down_fall = (m_down_key_state && m_time_since_down_move > m_hold_key_move_speed);

    if (should_auto_fall || should_hold_down_fall)
    {
        bool moved_down = Move(MOVEMENT_DOWN);

        if (moved_down)
        {
            DEBUG_PRINT("DEBUG: move down time since move: " << m_time_since_down_move);
            m_time_since_down_move = 0;
        }
        else if (m_time_since_down_move > m_fall_speed * 1.5)
        {
            DEBUG_PRINT("DEBUG: place piece time since move: " << m_time_since_down_move);
            m_time_since_down_move = 0;

            if (!PlacePiece())
            {
                m_game_running = false;
            }

            size_t previous_lines = m_lines_cleared;
            CheckCompletedRow();

            bool crossed_10 = (m_lines_cleared / 10) > (previous_lines / 10);

            DEBUG_PRINT("INFO: lines cleared: " << m_lines_cleared);

            if (crossed_10)
            {
                ChangeSpeed(DOWN);
            }
        }
    }

    return STATE_UNCHANGED;
}

void InGameState::ChangeSpeed(SpeedChange dir)
{
    if (dir == UP)
    {
        m_fall_speed += m_fall_speed_step;
        DEBUG_PRINT("DEBUG: increased fall speed to " << m_fall_speed);
    }
    else if (dir == DOWN)
    {
        m_fall_speed -= m_fall_speed_step;

        if (m_fall_speed <= m_fall_speed_minimum)
        {
            m_fall_speed = m_fall_speed_minimum;
        }
        DEBUG_PRINT("DEBUG: decreased fall speed to " << m_fall_speed);
    }
}

void InGameState::CheckCompletedRow()
{
    std::array<int, COORD_LIMIT_Y> row_counts = {0};
    
    for (const Square& sq : m_placed_squares)
    {
        row_counts[sq.coord.y]++;
    }

    std::vector<Square> remaining_squares;

    for (const Square& sq : m_placed_squares)
    {
        if (row_counts[sq.coord.y] < COORD_LIMIT_X)
        {
            remaining_squares.push_back(sq);
        }
    }

    int rows_lowered = 0;

    for (int i = (int)row_counts.size() - 1; i >= 0; i--)
    {
        if (row_counts[i] >= COORD_LIMIT_X)
        {
            for (Square& sq : remaining_squares)
            {
                if (sq.coord.y < i + rows_lowered)
                {
                    sq.coord += MOVEMENT_DOWN;
                }
            }
            rows_lowered++;
        }
    }

    m_lines_cleared += rows_lowered;
    m_placed_squares = remaining_squares;

    if (rows_lowered > 0)
    {
        m_score.UpdateText(m_renderer, m_font, std::format("Lines: {}", m_lines_cleared));
    }

    if (rows_lowered == 1)
    {
        Mix_PlayChannel(-1, m_sound_effects["smash1"], 0);
    }
    else if (rows_lowered > 1)
    {
        Mix_PlayChannel(-1, m_sound_effects["smash2"], 0);
    }
}

bool InGameState::Move(const Coordinate& movement)
{
    return Move(movement, m_falling_piece);
}

bool InGameState::Move(const Coordinate& movement, const Piece& piece)
{
    if (CanMove(movement, piece))
    {
        m_falling_piece.Move(movement);
        return true;
    }
    return false;
}

bool InGameState::CanMove(const Coordinate& movement, const Piece& piece)
{
    for (const Coordinate& coord : piece.m_coords)
    {
        Coordinate new_pos = coord;
        new_pos += movement;

        if (new_pos.x < 0 || new_pos.x >= m_coord_limits.x ||
            new_pos.y < 0 || new_pos.y >= m_coord_limits.y)
        {
            return false;
        }

        for (const Square& placed : m_placed_squares)
        {
            if (placed.coord == new_pos)
            {
                return false;
            }
        }
    }
    return true;
}

void InGameState::Rotate()
{
    Piece new_piece = m_falling_piece;
    new_piece.Rotate(m_coord_limits);   

    if (CanMove(MOVEMENT_NULL, new_piece))
    {
        m_falling_piece = new_piece;
    }
    else if (CanMove(MOVEMENT_LEFT, new_piece))
    {
        new_piece.Move(MOVEMENT_LEFT);
        m_falling_piece = new_piece;
    }
    else if (CanMove(MOVEMENT_RIGHT, new_piece))
    {
        new_piece.Move(MOVEMENT_RIGHT);
        m_falling_piece = new_piece;
    }
    else if (CanMove(MOVEMENT_UP, new_piece))
    {
        new_piece.Move(MOVEMENT_UP);
        m_falling_piece = new_piece;
    }
}

bool InGameState::PlacePiece()
{
    for (Square sq : m_falling_piece.GetSquares())
    {
        m_placed_squares.push_back(sq);
    }

    if (Mix_PlayChannel(-1, m_sound_effects["drop1"], 0) == -1)
    {
        DEBUG_PRINT("ERROR: could not play sound 'drop1'. Mix_Error: " << Mix_GetError());
    }

    return NewPiece();
}

bool InGameState::NewPiece()
{
    PIECE_TYPE new_piece = (PIECE_TYPE)Random(0, PIECE_TYPE::LENGTH - 1);

    m_falling_piece = Piece(new_piece, COORD_LIMIT_X);

    // test if the player is blocked out
    return CanMove(MOVEMENT_NULL, m_falling_piece);
}

void InGameState::Render()
{
    SDL_Rect cube = {0,0,m_cube_size,m_cube_size};

    SDL_RenderSetViewport(m_renderer, NULL);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(m_renderer);

    SDL_RenderSetViewport(m_renderer, &m_game_view);

    for (const Square& sq : m_falling_piece.GetSquares())
    {
        cube.x = sq.coord.x * m_cube_size;
        cube.y = sq.coord.y * m_cube_size;

        SDL_RenderCopy(m_renderer, m_texture_data[sq.color], NULL, &cube);
    }

    for (const Square& sq : m_placed_squares)
    {
        cube.x = sq.coord.x * m_cube_size;
        cube.y = sq.coord.y * m_cube_size;

        SDL_RenderCopy(m_renderer, m_texture_data[sq.color], NULL, &cube);
    }

    SDL_SetRenderDrawColor(m_renderer, 0, 33, 120, 0xFF);
    SDL_RenderSetViewport(m_renderer, NULL);

    for (const SDL_Rect& border : m_game_border)
    {
        SDL_RenderDrawRect(m_renderer, &border);
    }

    m_score.Render(m_renderer);

    SDL_RenderPresent(m_renderer);
}

InGameState::~InGameState()
{
    DestroyTextures(m_texture_data);
}