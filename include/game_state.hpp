#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include "texture.hpp"
#include "piece.hpp"
#include "debug.hpp"

static const char* FONT_PATH = "font/Righteous-Regular.ttf";

enum STATE
{
    STATE_UNCHANGED,
    STATE_TITLE,
    STATE_NEW_GAME,
    STATE_PAUSED,
    STATE_RESUME_GAME,
    STATE_QUIT
};

class GameState
{
public:
    GameState() = default;
    virtual STATE HandleEvent(const SDL_Event& event)
    {
        DEBUG_PRINT("ERR GameState::HandleEvent called");
        return STATE_QUIT;
    }
    virtual STATE Step(double delta_time_sec)
    {
        DEBUG_PRINT("ERR GameState::Step called");
        return STATE_QUIT;
    }
    virtual void Render()
    {
        DEBUG_PRINT("ERR GameState::Render called");
        exit(1);
    }
    virtual ~GameState() = default;

public:
    SDL_Window* m_window = NULL;
    SDL_Renderer* m_renderer = NULL;
    TTF_Font* m_font = NULL;
    std::unordered_map<std::string, SDL_Texture*> m_texture_data;
    std::unordered_map<std::string, Label> m_labels;
};


class TitleState : public GameState
{
public:
    TitleState(SDL_Window* window, SDL_Renderer* renderer);
    STATE HandleEvent(const SDL_Event& event);
    STATE Step(double delta_time_sec);
    void Render();
    ~TitleState();

private:
    Rect m_logo;
};


class InGameState : public GameState
{
public:
    InGameState(SDL_Window* window, SDL_Renderer* renderer);
    ~InGameState();
    STATE HandleEvent(const SDL_Event& event);
    STATE Step(double delta_time_sec);
    void Render();

private:
    enum SpeedChange
    {
        UP,
        DOWN,
    };
    
    enum LR_Key_State
    {
        KEY_NONE,
        KEY_LEFT,
        KEY_RIGHT
    };

private:
    bool Move(const Coordinate& movement);
    bool Move(const Coordinate& movement, const Piece& piece);
    bool CanMove(const Coordinate& movement, const Piece& piece);
    void Rotate();
    bool PlacePiece();
    bool NewPiece();
    void ChangeSpeed(SpeedChange dir);
    void CheckCompletedRow();

private:
    //game board size
    static const int COORD_LIMIT_X = 9;
    static const int COORD_LIMIT_Y = 22;

    const Coordinate m_coord_limits {COORD_LIMIT_X, COORD_LIMIT_Y};
    std::vector<Square> m_placed_squares;
    Piece m_falling_piece;
    SDL_Rect m_game_view;
    std::array<SDL_Rect, 3> m_game_border;
    int m_cube_size = 0;
    bool m_game_running = true;
    double m_time_since_down_move = 0;
    double m_fall_speed = 0.4;
    double m_fall_speed_step = 0.05;
    double m_fall_speed_minimum = 0.1;
    double m_hold_key_move_speed = 0.1;
    size_t m_lines_cleared = 0;
    Label m_score;
    LR_Key_State m_LR_key_state = KEY_NONE;
    double m_time_since_LR_move = 0;
    bool m_down_key_state = false;
    std::unordered_map<std::string, Mix_Chunk*> m_sound_effects;
};


class PausedState : public GameState
{
public:
    PausedState(SDL_Window* window, SDL_Renderer* renderer){};
    STATE HandleEvent(const SDL_Event& event){return STATE_UNCHANGED;};
    STATE Step(double delta_time_sec){return STATE_UNCHANGED;};
    void Render(){};
};
