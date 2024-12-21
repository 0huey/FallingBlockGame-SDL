#ifdef _WINDOWS
//to avoid showing a console window when running in "release" with /subsystem:windows
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void main(void);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    //avoid "unreferenced param" warnings
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;
    main();
}
#endif

#define SDL_MAIN_HANDLED
#include "application.hpp"

void main(void)
{
    Application app;
}
