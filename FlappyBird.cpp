#include "FlappyBird.h"


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
    // TODO
    m_running = false;

    return true;
}


// Render
//
bool FlappyBird::render(void)
{
    // TODO
    m_running = false;

    return true;
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
