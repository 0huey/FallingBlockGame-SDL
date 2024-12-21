#include <SDL_mixer.h>
#include <vector>
#include <unordered_map>
#include <string>

std::vector<Mix_Music*> LoadMusic();
std::unordered_map<std::string, Mix_Chunk*> LoadSoundEffects();
void DestroySoundEffects(std::unordered_map<std::string, Mix_Chunk*>& sounds);