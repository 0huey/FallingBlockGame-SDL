#include "game_state.hpp"
#include "debug.hpp"

static const char* TEXTURE_PATH = "texture/title";

TitleState::TitleState(SDL_Window* window, SDL_Renderer* renderer)
{
    DEBUG_PRINT("TitleState created");

    m_window = window;
    m_renderer = renderer;

    LoadTextures(m_renderer, TEXTURE_PATH, m_texture_data);

    m_font = TTF_OpenFont(FONT_PATH, 50);

    if (m_font == NULL)
    {
        std::cerr << "Could not load font from " << FONT_PATH << std::endl;
        exit(-1);
    }

    int screen_width, screen_height;

    SDL_GetRendererOutputSize(m_renderer, &screen_width, &screen_height);

    m_logo = Rect(
        m_texture_data["logo"],
        (int)(screen_width / 2),
        (int)(screen_height / 5),
        150,
        100);

    m_logo.Center();
    m_logo.m_rotation_speed = 45;

    m_labels["start"] = Label(m_renderer, m_font, "Start", COLOR_WHITE, COLOR_BLACK);
    m_labels["start"].Reposition((int)(screen_width / 2), (int)(screen_height / 2), true);
    m_labels["start"].SetHoverColor(COLOR_BLACK, COLOR_WHITE);

    m_labels["quit"] = Label(m_renderer, m_font, "Quit", COLOR_WHITE, COLOR_BLACK);
    m_labels["quit"].Reposition(
        (int)(screen_width / 2),
        m_labels["start"].m_position.y + m_labels["start"].m_position.h * 2,
        true
    );
    m_labels["quit"].SetHoverColor(COLOR_BLACK, COLOR_WHITE);
}

STATE TitleState::HandleEvent(const SDL_Event& event)
{
    SDL_Point point = {-1, -1};

    if (event.type == SDL_MOUSEMOTION)
    {
        point.x = event.motion.x;
        point.y = event.motion.y;

        for (auto& [name, label] : m_labels)
        {
            if (SDL_PointInRect(&point, &label.m_position))
            {
                label.Hover(m_renderer, m_font);
            }
            else
            {
                label.Revert(m_renderer, m_font);
            }
        }
    }

    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        point.x = event.button.x;
        point.y = event.button.y;

        for (auto& [name, label] : m_labels)
        {
            if (SDL_PointInRect(&point, &label.m_position))
            {
                if (name == "start")
                {
                    return STATE_NEW_GAME;
                }
                else if (name == "quit")
                {
                    return STATE_QUIT;
                }
            }
        }
    }
    return STATE_UNCHANGED;
}

STATE TitleState::Step(double delta_time_sec)
{
    STATE next_state = STATE_UNCHANGED;

    m_logo.Step(delta_time_sec);

    return next_state;
}

void TitleState::Render()
{
    SDL_SetRenderDrawColor(m_renderer, COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b, COLOR_BLACK.a);
    SDL_RenderClear(m_renderer);

    m_logo.Render(m_renderer);

    for (auto& [name, texture] : m_labels)
    {
        texture.Render(m_renderer);
    }

    SDL_RenderPresent(m_renderer);
}

TitleState::~TitleState()
{
    for (auto& [name, label] : m_labels)
    {
        label.DestroyTexture();
    }

    DestroyTextures(m_texture_data);
    TTF_CloseFont(m_font);
}
