#include "application.hpp"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "audio.hpp"
#include "utility.hpp"
#include "debug.hpp"

Application::Application()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        std::cerr << "SDL IMG could not initialize! SDL_Error: " << IMG_GetError() << std::endl;
        return;
    }

    if (TTF_Init() != 0)
    {
        std::cerr << "SDL TTF could not initialize! SDL_Error: " << TTF_GetError() << std::endl;
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "SDL Mixer could not initialize! SDL_Error: " << Mix_GetError() << std::endl;
        return;
    }

    m_window = SDL_CreateWindow(
        "Falling Block Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_screen_width,
        m_screen_height,
        SDL_WINDOW_SHOWN);

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    //SDL_SetWindowResizable(m_window, SDL_TRUE);

    m_music = LoadMusic();
    if (m_music.size() == 0)
    {
        ERROR_PRINT("could not load any music");
    }

    if (m_music.size() > 0)
    {
        m_music_index = (size_t)Random() % m_music.size();
        Mix_FadeInMusic(m_music[m_music_index], 0, 500);
    }

    ChangeState(STATE_TITLE);

    m_last_frame_ticks = SDL_GetTicks64();

    MainLoop();
}

void Application::MainLoop()
{
    STATE new_state;
    SDL_Event event;

    while (!m_should_quit)
    {
        new_state = STATE_UNCHANGED;

        bool event_returned = (bool)SDL_PollEvent(&event);

        if (event_returned)
        {
            if (HandleEvent(event))
            {
                continue;
            }

            new_state = m_active_state->HandleEvent(event);
            ChangeState(new_state);
            continue;
        }

        double delta_time_sec = (double)(SDL_GetTicks64() - m_last_frame_ticks) / 1000;

        m_last_frame_ticks = SDL_GetTicks64();

        new_state = m_active_state->Step(delta_time_sec);

        if (new_state != STATE_UNCHANGED)
        {
            ChangeState(new_state);
            continue;
        }

        m_active_state->Render();
        //SDL_Delay(10);

        if (m_music.size() > 0 && !Mix_PlayingMusic())
        {
            m_music_index = (m_music_index + 1) % m_music.size();
            Mix_FadeInMusic(m_music[m_music_index], 0, 500);
        }
    }
}

void Application::ChangeState(STATE new_state)
{
    switch (new_state)
    {
    case STATE_TITLE:
        DeleteState(m_active_state);
        m_active_state = new TitleState(m_window, m_renderer);
        break;

    case STATE_NEW_GAME:
        DeleteState(m_active_state);
        DeleteState(m_saved_state);
        m_active_state = new InGameState(m_window, m_renderer);
        break;

    case STATE_PAUSED:
        DeleteState(m_saved_state);
        m_saved_state = m_active_state;
        m_active_state = new PausedState(m_window, m_renderer);
        break;

    case STATE_RESUME_GAME:
        DeleteState(m_active_state);
        if (m_saved_state != NULL)
        {
            m_active_state = m_saved_state;
            m_saved_state = NULL;
        }
        else
        {
            m_active_state = new InGameState(m_window, m_renderer);
        }
        break;

    case STATE_QUIT:
        m_should_quit = true;
    }
}

void Application::DeleteState(GameState*& state)
{
    if (state != NULL)
    {
        delete state;
        state = NULL;
    }
}

bool Application::HandleEvent(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_QUIT:
        DEBUG_PRINT("SDL_QUIT Recieved");
        m_should_quit = true;
        return true;

    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_F5)
        {
            Mix_FadeOutMusic(500);
            return true;
        }
    }

    return false;
}

Application::~Application()
{
    DeleteState(m_active_state);
    DeleteState(m_saved_state);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

