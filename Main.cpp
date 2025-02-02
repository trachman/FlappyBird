#include "FlappyBird.h"


// Main method
//
int main(void)
{
    constexpr int WIDTH = 120;
    constexpr int HEIGHT = 30;
    const std::wstring gameTitle = L"Flappy Bird";
    FlappyBird flappyBird(gameTitle, WIDTH, HEIGHT);

    if (!flappyBird.initializeConsole())
    {
        return EXIT_FAILURE;
    }

    return flappyBird.gameLoop();
}