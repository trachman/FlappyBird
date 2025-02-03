#include "FlappyBird.h"

#include <cmath>
#include <iostream>
#include <string>


// Constructor
//
FlappyBird::FlappyBird(const std::wstring& title, const int width, const int height)
: m_score(0),
  m_verticalVelocity(0.0),
  m_rowDouble(0.0),
  m_jump(false),
  m_row(0),
  m_col(0),
  m_pipes({}),
  m_jumpVelocity(0.0),
  m_pipeVelocity(0.0),
  m_gravity(0.0),
  ConsoleEngine(title, width, height)
{
    this->resetGameState();
}


// Update
//
bool FlappyBird::update(const double deltaTime)
{
    this->handleInputEvents();

    this->updatePhysics(deltaTime);

    if (m_row < 0 || m_row >= this->height())
    {
        m_running = false;
    }

    for (auto& pipe : m_pipes)
    {
        pipe.updatePosition(deltaTime);

        // Update score
        if ((!pipe.m_scoreTracked) && ((pipe.m_col + pipe.m_width) <= m_col))
        {
            ++m_score;
            pipe.m_scoreTracked = true;
        }
    }

    // Remove pipes that are output of field of view
    std::vector<size_t> pipesToErase;
    pipesToErase.reserve(m_pipes.size());

    for (size_t i = 0; i < m_pipes.size(); ++i)
    {
        if (m_pipes.at(i).m_col < 5)
        {
            pipesToErase.push_back(i);
        }
    }

    for (const size_t i : pipesToErase)
    {
        // Erase this one
        m_pipes.erase(m_pipes.begin() + i);

        // Choose random gap size:  [5, 10]
        const int randomGapSize = rand() % (10 - 5 + 1) + 5;

        // Choose random gap start: [2, 18]
        const int randomGapStart = rand() % (18 - 2 + 1) + 2;

        // Append a new one
        Pipe newPipe;
        newPipe.m_velocity = m_pipeVelocity;
        newPipe.m_colPosition = m_pipes.back().m_col + newPipe.m_width + 15;
        newPipe.m_col = static_cast<int>(std::round(newPipe.m_colPosition));
        newPipe.m_gapSize = randomGapSize;
        newPipe.m_gapStartRow = randomGapStart;
        m_pipes.emplace_back(std::move(newPipe));
    }

    return true;
}


// Render
//
bool FlappyBird::render(void)
{
    // Draw Pipes to buffer
    for (const auto& pipe : m_pipes)
    {
        bool isHit = false;
        pipe.drawToOutputBuffer(m_outputBuffer, m_row, m_col, isHit);

        if (isHit)
        {
            m_running = false;
        }
    }

    // Draw Bird to buffer
    CHAR_INFO bird;
    bird.Attributes = 7;
    bird.Char.UnicodeChar = 0x2588;

    int offset = ( m_row * this->width() ) + m_col;

    if (offset >= 0 && offset < m_outputBuffer.size())
    {
        m_outputBuffer.at(offset) = bird;
    }

    // Overlay these last
    this->drawFPSToOutputBuffer();
    this->drawVelocityToOutputBuffer();
    this->drawScoreToOutputBuffer();

    return this->ConsoleEngine::render();
}


// Reset Game State
//
void FlappyBird::resetGameState(void)
{
    m_score = 0;
    m_verticalVelocity = 0.0;
    m_row = this->height() / 2;
    m_rowDouble = static_cast<double>(m_row);
    m_col = 25;
    m_jump = false;
    m_running = true;

    m_pipes.clear();
    m_pipes.reserve(5);
    for (size_t i = 0; i < 8; ++i)
    {
        // Choose random gap size:  [5, 10]
        const int randomGapSize = rand() % (10 - 5 + 1) + 5;

        // Choose random gap start: [2, 18]
        const int randomGapStart = rand() % (18 - 2 + 1) + 2;

        Pipe newPipe;
        newPipe.m_velocity = m_pipeVelocity;
        newPipe.m_colPosition = (this->width() / 2) + (i * (newPipe.m_width + 15));
        newPipe.m_col = static_cast<int>(std::round(newPipe.m_colPosition));
        newPipe.m_gapSize = randomGapSize;
        newPipe.m_gapStartRow = randomGapStart;
        m_pipes.emplace_back(std::move(newPipe));
    }
}


// On Game Begin
//
void FlappyBird::onGameBegin(void)
{
    auto isNumber = [](const std::string& str) -> bool
    {
        std::string::const_iterator it = str.begin();
        while (it != str.end() && std::isdigit(*it)) ++it;
        return !str.empty() && it == str.end();
    };

    std::cout << "Build your own gameplay experience!" << std::endl;

    std::string jumpVelocity;
    std::cout << "Choose your jump velocity (15 default): ";
    std::getline(std::cin, jumpVelocity);

    if (!isNumber(jumpVelocity))
    {
        m_jumpVelocity = 15.0;
    }
    else
    {
        m_jumpVelocity = std::stod(jumpVelocity);
    }

    std::string pipeVelocity;
    std::cout << "Choose how fast the pipes move (15 default): ";
    std::getline(std::cin, pipeVelocity);

    if (!isNumber(pipeVelocity))
    {
        m_pipeVelocity = 15.0;
    }
    else
    {
        m_pipeVelocity = std::stod(pipeVelocity);
    }

    std::string gravity;
    std::cout << "Choose your affect of gravity (40 default): ";
    std::getline(std::cin, gravity);

    if (!isNumber(gravity))
    {
        m_gravity = 40.0;
    }
    else
    {
        m_gravity = std::stod(gravity);
    }

    // Landing page.
    const std::wstring welcomeMessage   = L"Welcome to Flappy Bird!";
    const std::wstring instructions     = L"Press the spacebar to jump.";
    const std::wstring quitInstructions = L"Press 'q' or 'ESC' to quit.";
    const std::wstring playInstructions = L"Press any of the above keys to play.";
    const std::wstring credits          = L"Developed by Tristan Rachman :)";
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


// Update Physics
// 
void FlappyBird::updatePhysics(const double deltaTime)
{
    if (m_jump)
    {
        m_verticalVelocity = m_jumpVelocity;
    }

    m_verticalVelocity = m_verticalVelocity - (m_gravity * deltaTime);

    m_rowDouble -= (m_verticalVelocity * deltaTime);

    m_row = static_cast<int>(std::round(m_rowDouble));
}


// Handle Input Events
//
void FlappyBird::handleInputEvents(void)
{
    m_jump = false;

    for (const Input input : m_inputCommands)
    {
        if (input == Input::QUIT || input == Input::UNDEFINED)
        {
            // Break out of the game loop
            m_running = false;
            break;
        }
        else if (input == Input::JUMP)
        {
            m_jump = true;
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

    this->drawStringToBuffer(scoreStr, 2, 0);
}


// Draw Velocity to Screen
//
void FlappyBird::drawVelocityToOutputBuffer(void)
{
    const std::wstring velStr = L"Velocity: " + std::to_wstring(m_verticalVelocity);

    this->drawStringToBuffer(velStr, 1, 0);
}


// Update Pipe position
//
void Pipe::updatePosition(const double deltaTime)
{
    m_colPosition -= (m_velocity * deltaTime);
    m_col = static_cast<int>(std::round(m_colPosition));
}


// Draw pipe to output buffer
//
void Pipe::drawToOutputBuffer(
    std::vector<CHAR_INFO>& outputBuffer,
    const int birdRow,
    const int birdCol,
    bool& isHit) const
{
    constexpr double WIDTH = 120;
    constexpr double HEIGHT = 30;

    if (m_col < 0 || m_col >= WIDTH - m_width)
    {
        return; // Do not draw this
    }

    // Draw the pipe
    //
    for (int row = 0; row < m_gapStartRow; ++row)
    {
        for (int col = m_col; col < m_col + m_width; ++col)
        {
            CHAR_INFO charInfo;
            charInfo.Attributes = FOREGROUND_GREEN;
            charInfo.Char.UnicodeChar = 0x2588;

            const int offset = Utilities::computeTheOffset(row, col, WIDTH);

            if (birdRow == row && birdCol == col)
            {
                isHit = true;
            }

            if (row == m_gapStartRow - 1 && col == m_col)
            {
                outputBuffer.at(offset - 1) = charInfo;
            }
            else if (row == m_gapStartRow - 1 && col == m_col + m_width - 1)
            {
                outputBuffer.at(offset + 1) = charInfo;
            }

            outputBuffer.at(offset) = charInfo;
        }
    }

    for (int row = m_gapStartRow + m_gapSize; row < HEIGHT; ++row)
    {
        for (int col = m_col; col < m_col + m_width; ++col)
        {
            CHAR_INFO charInfo;
            charInfo.Attributes = FOREGROUND_GREEN;
            charInfo.Char.UnicodeChar = 0x2588;

            const int offset = Utilities::computeTheOffset(row, col, WIDTH);

            if (birdRow == row && birdCol == col)
            {
                isHit = true;
            }

            if (row == m_gapStartRow + m_gapSize && col == m_col)
            {
                outputBuffer.at(offset - 1) = charInfo;
            }
            else if (row == m_gapStartRow + m_gapSize && col == m_col + m_width - 1)
            {
                outputBuffer.at(offset + 1) = charInfo;
            }

            outputBuffer.at(offset) = charInfo;
        }
    }
}
