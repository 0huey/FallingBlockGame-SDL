# Falling Block Game

My implementation of tetr!s using the [SDL2 library](https://www.libsdl.org/)

![screenshot](img/screenshot.jpg)

## Dependencies

- [SDL2](https://github.com/libsdl-org/SDL)
- [SDL Image](https://github.com/libsdl-org/SDL_image)
- [SDL Mixer](https://github.com/libsdl-org/SDL_mixer)
- [SDL TTF](https://github.com/libsdl-org/SDL_ttf)

It is expected that these libraries will be installed one directory above the project folder

```
|__SDL2
|  |__include
|  |__lib
|     |__x64
|        |__SDL2.dll
|        |__SDL2_image.dll
|        |__SDL2_mixer.dll
|        |__SDL2_ttf.dll
|
|__FallingBlockGame-SDL
|  |__src
```

## Controls

- `W` / `Up Arrow` : Rotate
- `A and D` / `Left and Right Arrow` : Move left/right
- `S` / `Down Arrow` : Move down faster
- `F5` : Next song
