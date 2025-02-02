#include "FlappyBird.h"

#include <cmath>


// Constructor
//
FlappyBird::FlappyBird(const std::wstring& title, const int width, const int height)
: m_score(0),
  ConsoleEngine(title, width, height)
{
    this->resetGameState();
}


// Update
//
bool FlappyBird::update(const double deltaTime)
{
    this->handleInputEvents();

    // TODO
    // Physics, movements, etc.

    return true;
}


// Render
//
bool FlappyBird::render(void)
{
    this->drawFPSToOutputBuffer();
    this->drawScoreToOutputBuffer();

    // TODO
    // Draw Pipes to buffer
    // Draw Bird to buffer

    return this->ConsoleEngine::render();
}


// Reset Game State
//
void FlappyBird::resetGameState(void)
{
    m_score = 0;
    m_running = true;
}


// On Game Begin
//
void FlappyBird::onGameBegin(void)
{
    // Landing page.
    const std::wstring welcomeMessage = L"Welcome to Flappy Bird!";
    const std::wstring instructions = L"Press the spacebar to jump.";
    const std::wstring quitInstructions = L"Press 'q' or 'ESC' to quit.";
    const std::wstring playInstructions = L"Press any of the above keys to play.";
    const std::wstring credits = L"Developed by Tristan Rachman :)";
    this->drawStringToBuffer(welcomeMessage, 11, 40);
    this->drawStringToBuffer(instructions, 12, 40);
    this->drawStringToBuffer(quitInstructions, 13, 40);
    this->drawStringToBuffer(playInstructions, 14, 40);
    this->drawStringToBuffer(credits, 17, 40);

    while (true)
    {
        if (!this->input())
        {
            break;
        }

        if (!this->ConsoleEngine::render())
        {
            break;
        }

        if (!m_inputCommands.empty())
        {
            break;
        }
    }
}


// On Game End
//
ConsoleEngine::PlayAgain FlappyBird::onGameEnd(void) const
{
    std::wstring scoreString = L"Your score was: " + std::to_wstring(m_score)
                             + L"\nYou wanna play again?";

    constexpr int YES_INT = 6;
    const int response = MessageBox(
        nullptr,
        scoreString.data(),
        TEXT("Uh oh!"),
        MB_YESNO);

    return response == YES_INT ? PlayAgain::YES : PlayAgain::NO;
}


// Handle Input Events
//
void FlappyBird::handleInputEvents(void)
{
    for (const Input input : m_inputCommands)
    {
        if (input == Input::QUIT || input == Input::UNDEFINED)
        {
            // Break out of the game loop
            m_running = false;
            break;
        }
    }
}


// Draw FPS to Screen
//
void FlappyBird::drawFPSToOutputBuffer(void)
{
    const int fpsInt = static_cast<int>(std::ceil(m_fps));
    const std::wstring fpsStr = L"FPS: " + std::to_wstring(fpsInt);

    this->drawStringToBuffer(fpsStr, 0, 0);
}


// Draw Score to Screen
//
void FlappyBird::drawScoreToOutputBuffer(void)
{
    const std::wstring scoreStr = L"Score: " + std::to_wstring(m_score);

    this->drawStringToBuffer(scoreStr, 1, 0);
}
