#include "audio.hpp"
#include <filesystem>
#include "debug.hpp"

static const char* MUSIC_PATH = "audio/music";
static const char* SOUND_EFFECT_PATH = "audio/effects";

std::vector<Mix_Music*> LoadMusic()
{
    std::vector<Mix_Music*> music;

    if (!std::filesystem::is_directory(MUSIC_PATH))
    {
        ERROR_PRINT("could not find music directory " << MUSIC_PATH);
        exit(-1);
    }

    std::filesystem::directory_iterator directory(MUSIC_PATH);

    for (std::filesystem::directory_entry file : directory)
    {
        if (!file.is_regular_file())
        {
            continue;
        }

        if (file.path().extension() != ".mp3")
        {
            continue;
        }

        std::string file_path = file.path().string();

        size_t backslash_pos = file_path.find("\\");

        while (backslash_pos != std::string::npos)
        {
            file_path.replace(backslash_pos, 1, "/");
            backslash_pos = file_path.find("\\");
        }

        Mix_Music* music_file = Mix_LoadMUS(file_path.c_str());

        if (music_file == NULL)
        {
            ERROR_PRINT("Error loading music: " << file_path << " Mix error: " << Mix_GetError());
            continue;
        }

        DEBUG_PRINT("INFO: Loaded music: " << file_path);

        music.push_back(music_file);
    }

    return music;
}

std::unordered_map<std::string, Mix_Chunk*> LoadSoundEffects()
{
    std::unordered_map<std::string, Mix_Chunk*> effects;

    if (!std::filesystem::is_directory(SOUND_EFFECT_PATH))
    {
        ERROR_PRINT("could not find sound effect directory " << SOUND_EFFECT_PATH);
        exit(-1);
    }

    std::filesystem::directory_iterator directory(SOUND_EFFECT_PATH);

    for (std::filesystem::directory_entry file : directory)
    {
        if (!file.is_regular_file())
        {
            continue;
        }

        if (file.path().extension() != ".mp3")
        {
            continue;
        }

        std::string file_path = file.path().string();

        size_t backslash_pos = file_path.find("\\");

        while (backslash_pos != std::string::npos)
        {
            file_path.replace(backslash_pos, 1, "/");
            backslash_pos = file_path.find("\\");
        }

        Mix_Chunk* sound_file = Mix_LoadWAV(file_path.c_str());

        if (sound_file == NULL)
        {
            ERROR_PRINT("Error loading sound effect: " << file_path << " Mix error: " << Mix_GetError());
            continue;
        }

        std::string file_name = file.path().stem().string();

        DEBUG_PRINT("INFO: Loaded sound effect: " << file_name);

        effects[file_name] = sound_file;
    }

    return effects;
}

void DestroySoundEffects(std::unordered_map<std::string, Mix_Chunk*>& sounds)
{
    for (auto& [key, val] : sounds)
    {
        Mix_FreeChunk(val);
    }
}