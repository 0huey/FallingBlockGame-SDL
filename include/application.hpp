#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include "game_state.hpp"

class Application
{
public:
    Application();
    ~Application();

private:
    int m_screen_width = 640;
    int m_screen_height = 1000;
    SDL_Window* m_window = NULL;
    SDL_Renderer* m_renderer = NULL;
    GameState* m_active_state = NULL;
    GameState* m_saved_state = NULL;
    Uint64 m_last_frame_ticks = 0;
    bool m_should_quit = false;
    std::vector<Mix_Music*> m_music;
    size_t m_music_index = 0;

private:
    void MainLoop();
    void ChangeState(STATE new_state);
    void DeleteState(GameState*& state);
    bool HandleEvent(const SDL_Event& event);
};
