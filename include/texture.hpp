#include <unordered_map>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

const SDL_Color DEFAULT_COLOR = {0xFF, 0, 0xFF, 0xFF}; // pink
const SDL_Color COLOR_BLACK = {0, 0, 0, 0xFF};
const SDL_Color COLOR_WHITE = {0xFF, 0XFF, 0XFF, 0xFF};

class Rect
{
public:
    enum TYPE
    {
        TEXTURED,
        FILLED,
        BORDER
    };

public:
    Rect() = default;
    Rect(SDL_Texture* t);
    Rect(SDL_Texture* t, int x, int y, int w, int h);
    void Center();
    void Reposition(int x, int y, bool recenter);
    void Step(double delta_time_sec);
    void Render(SDL_Renderer* renderer);

public:   
    SDL_Texture* m_texture = NULL;
    SDL_Color m_color = DEFAULT_COLOR;

    SDL_Rect m_position = {0,0,0,0};
    SDL_Rect m_texture_src = {0,0,0,0};
    double m_rotation = 0;
    SDL_Point m_rotation_center = {0,0};

    TYPE m_type = FILLED;

    SDL_RendererFlip m_flip = SDL_FLIP_NONE;

    double m_rotation_speed = 0;
};

class Label : public Rect
{
public:
    Label() = default;
    Label(SDL_Renderer* renderer, TTF_Font* font, std::string text, const SDL_Color& fg);
    Label(SDL_Renderer* renderer, TTF_Font* font, std::string text, const SDL_Color& fg, const SDL_Color& bg);
    //void SetHoverColor(const SDL_Color& fg);
    void SetHoverColor(const SDL_Color& fg, const SDL_Color& bg);
    void Hover(SDL_Renderer* renderer, TTF_Font* font);
    void Recolor(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& fg);
    void Recolor(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& fg, const SDL_Color& bg);
    void UpdateText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text);
    void Revert(SDL_Renderer* renderer, TTF_Font* font, bool force=false);
    void DestroyTexture();

private:
    void CreateTexture(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& fg);
    void CreateTextureWithBG(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& fg, const SDL_Color& bg);

private:
    std::string m_text;
    SDL_Color m_fg = DEFAULT_COLOR;
    SDL_Color m_bg = DEFAULT_COLOR;
    SDL_Color m_hover_fg = DEFAULT_COLOR;
    SDL_Color m_hover_bg = DEFAULT_COLOR;
    bool m_has_bg = false;
    bool m_has_hover_fg = false;
    bool m_has_hover_bg = false;
    bool m_recolored = false;

};

void LoadTextures(SDL_Renderer* renderer, const char* rel_path, std::unordered_map<std::string, SDL_Texture*>& out_texture_map);
void DestroyTextures(std::unordered_map<std::string, SDL_Texture*>& texture_map);
