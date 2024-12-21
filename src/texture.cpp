#include <SDL_image.h>
#include <filesystem>
#include <cmath>
#include "debug.hpp"
#include "texture.hpp"

Rect::Rect(SDL_Texture* t)
{
    m_type = TEXTURED;
    m_texture = t;
}

Rect::Rect(SDL_Texture* t, int x, int y, int w, int h)
{
    m_type = TEXTURED;
    m_texture = t;
    m_position = {x, y, w, h};
}

/*Center on the current x,y*/
void Rect::Center()
{
    m_position.x -= (m_position.w >> 1);
    m_position.y -= (m_position.h >> 1);
}

void Rect::Reposition(int x, int y, bool recenter)
{
    m_position.x = x;
    m_position.y = y;
    if (recenter)
    {
        Center();
    }
}

void Rect::Step(double delta_time_sec)
{
    m_rotation += m_rotation_speed * delta_time_sec;

    if (std::abs(m_rotation) > 360)
    {
        m_rotation /= 360;
    }
}

void Rect::Render(SDL_Renderer* renderer)
{
    if (renderer == NULL)
    {
        DEBUG_PRINT("NULL SDL_Renderer* passed to Rectangle::Render");
        return;
    }

    int success = 0;

    if (m_type == FILLED)
    {
        SDL_SetRenderDrawColor(renderer, m_color.r, m_color.b, m_color.g, 0xFF);
        success = SDL_RenderFillRect(renderer, &m_position);
    }

    else if (m_type == BORDER)
    {
        SDL_SetRenderDrawColor(renderer, m_color.r, m_color.b, m_color.g, 0xFF);
        success = SDL_RenderDrawRect(renderer, &m_position);
    }

    else if (m_type == TEXTURED && m_texture == NULL)
    {
        DEBUG_PRINT("TEXTURED Rectangle object has NULL SDL_Texture*");
        SDL_SetRenderDrawColor(renderer, DEFAULT_COLOR.r, DEFAULT_COLOR.g, DEFAULT_COLOR.b, 0xFF);
        success = SDL_RenderFillRect(renderer, &m_position);
    }

    else if (m_type == TEXTURED)
    {
        SDL_Rect* src_rect = NULL;
        if (!SDL_RectEmpty(&m_texture_src))
        {
            src_rect = &m_texture_src;
        }

        SDL_Point* rotation_point = NULL;
        if (m_rotation_center.x != 0 || m_rotation_center.y != 0)
        {
            rotation_point = &m_rotation_center;
        }

        success = SDL_RenderCopyEx(
            renderer,
            m_texture,
            src_rect,
            &m_position,
            m_rotation,
            rotation_point,
            m_flip);
    }

    if (success != 0)
    {
        DEBUG_PRINT("Render Rectangle error: " << success << " " << SDL_GetError());
    }
}

Label::Label(SDL_Renderer* renderer, TTF_Font* font, std::string text, const SDL_Color& fg)
{
    m_type = TEXTURED;
    m_fg = fg;
    m_has_bg = false;
    UpdateText(renderer, font, text);
}

Label::Label(SDL_Renderer* renderer, TTF_Font* font, std::string text, const SDL_Color& fg, const SDL_Color& bg)
{
    m_type = TEXTURED;
    m_fg = fg;
    m_bg = bg;
    m_has_bg = true;
    UpdateText(renderer, font, text);
}

void Label::SetHoverColor(const SDL_Color& fg, const SDL_Color& bg)
{
    m_has_hover_fg = true;
    m_has_hover_bg = true;
    m_hover_fg = fg;
    m_hover_bg = bg;
}

void Label::Hover(SDL_Renderer* renderer, TTF_Font* font)
{
    if (m_has_hover_fg && !m_has_hover_bg)
    {
        Recolor(renderer, font, m_hover_fg);
    }
    else if (m_has_hover_fg && m_has_hover_bg)
    {
        Recolor(renderer, font, m_hover_fg, m_hover_bg);
    }
}

void Label::Recolor(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& fg)
{
    m_recolored = true;
    CreateTexture(renderer, font, fg);
}

void Label::Recolor(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& fg, const SDL_Color& bg)
{
    m_recolored = true;
    CreateTextureWithBG(renderer, font, fg, bg);
}

void Label::UpdateText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text)
{
    m_text = text;
    TTF_SizeText(font, m_text.c_str(), &m_position.w, &m_position.h);
    Revert(renderer, font, true);
}

void Label::Revert(SDL_Renderer* renderer, TTF_Font* font, bool force)
{
    if (!m_recolored && !force)
    {
        return;
    }

    m_recolored = false;

    if (m_has_bg)
    {
        CreateTextureWithBG(renderer, font, m_fg, m_bg);
    }
    else
    {
        CreateTexture(renderer, font, m_fg);
    }
}

void Label::CreateTexture(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& fg)
{
    DestroyTexture();

    SDL_Surface* surface = TTF_RenderText_Solid(font, m_text.c_str(), fg);
    if (surface == NULL)
    {
        DEBUG_PRINT("Error creating surface from text: " << m_text);
        return;
    }

    m_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (m_texture == NULL)
    {
        DEBUG_PRINT("Error texture from surface, text: " << m_text);
    }

    SDL_FreeSurface(surface);
    TTF_SizeText(font, m_text.c_str(), &m_position.w, &m_position.h);
}

void Label::CreateTextureWithBG(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& fg, const SDL_Color& bg)
{
    DestroyTexture();

    SDL_Surface* surface = TTF_RenderText_Shaded(font, m_text.c_str(), fg, bg);

    if (surface == NULL)
    {
        DEBUG_PRINT("Error creating surface from text: " << m_text);
        return;
    }

    m_texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (m_texture == NULL)
    {
        DEBUG_PRINT("Error texture from surface, text: " << m_text);
    }

    SDL_FreeSurface(surface);
}

void Label::DestroyTexture()
{
    if (m_texture != NULL)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = NULL;
    }
}

void LoadTextures(SDL_Renderer* renderer, const char* rel_path, std::unordered_map<std::string, SDL_Texture*>& out_texture_map)
{
    if (!std::filesystem::is_directory(rel_path))
    {
        ERROR_PRINT("could not find texture directory " << rel_path);
        exit(-1);
    }

    std::filesystem::directory_iterator directory(rel_path);

    for (std::filesystem::directory_entry file : directory)
    {
        if (!file.is_regular_file())
        {
            continue;
        }

        std::string file_name_lower;

        for (char letter : file.path().stem().string())
        {
            file_name_lower.push_back((char)std::tolower(letter));
        }

        if (out_texture_map.contains(file_name_lower))
        {
            ERROR_PRINT("ERROR: duplicate texture name " << file_name_lower);
            exit(-1);
        }

        std::string file_path = file.path().string();

        size_t backslash_pos = file_path.find("\\");

        while (backslash_pos != std::string::npos)
        {
            file_path.replace(backslash_pos, 1, "/");
            backslash_pos = file_path.find("\\");
        }

        SDL_Texture* tex = IMG_LoadTexture(renderer, file_path.c_str());

        if (tex == NULL)
        {
            ERROR_PRINT("ERROR: cannot load texture " << file.path() << " with error " << IMG_GetError());
            exit(-1);
        }

        out_texture_map[file_name_lower] = tex;

        DEBUG_PRINT("INFO: loaded texture: " << file_name_lower);
    }
}

void DestroyTextures(std::unordered_map<std::string, SDL_Texture*>& texture_map)
{
    for (const auto& [key, val] : texture_map)
    {
        SDL_DestroyTexture(val);
    }
}
