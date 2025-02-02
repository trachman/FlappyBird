#include "ConsoleEngine.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>


// width
// Static method for computing the offset into an array
//
int Utilities::computeTheOffset(const int row, const int col, const int width)
{
    return ( row * width ) + col;
}


// Constructor
//
ConsoleEngine::ConsoleEngine(const std::wstring& title, const int width, const int height)
: m_running(false),
  m_fps(0.0),
  m_width(width),
  m_height(height),
  m_stdInput(INVALID_HANDLE_VALUE),
  m_stdOutput(INVALID_HANDLE_VALUE),
  m_outputBuffer({}),
  m_inputBuffer({}),
  m_inputCommands({}),
  m_title(title)
{
    // Nothing else to do
}


// Destructor
//
ConsoleEngine::~ConsoleEngine(void)
{
    this->flushConsole();

    m_running = false;
    m_width = 0;
    m_height = 0;
    m_stdInput = INVALID_HANDLE_VALUE;
    m_stdOutput = INVALID_HANDLE_VALUE;
    m_outputBuffer.clear();
    m_inputBuffer.clear();
    m_inputCommands.clear();
}


// gameLoop
// The main game loop.
//
int ConsoleEngine::gameLoop(void)
{
    using namespace std::chrono;

    this->onGameBegin();

    while (true)
    {
        this->resetGameState();

        high_resolution_clock timer;
        auto lastFrame = timer.now();

        while (m_running)
        {
            // Compute delta time
            auto timeNow = timer.now();
            auto deltaTimeMicro = duration_cast<microseconds>(timeNow - lastFrame).count();
            double deltaTimeSeconds = static_cast<double>(deltaTimeMicro) * 1e-6;
            lastFrame = timeNow;
            m_fps = 1.0 / deltaTimeSeconds;

            // Clear any stale data
            this->clearOutputBuffer();

            // Get user input
            if (!this->input())
            {
                std::cout << "Error getting user input." << std::endl;

                return EXIT_FAILURE;
            }

            // Update game state
            if (!this->update(deltaTimeSeconds))
            {
                std::cout << "Error updating game state." << std::endl;

                return EXIT_FAILURE;
            }

            // Render game state
            if (!this->render())
            {
                std::cout << "Error rendering." << std::endl;

                return EXIT_FAILURE;
            }
        }

        if (this->onGameEnd() != PlayAgain::YES)
        {
            break;
        }
    }

    return EXIT_SUCCESS;
}


// initializeConsole
// Initializes the console for the game.
//
bool ConsoleEngine::initializeConsole(void)
{
    // Get input handle
    m_stdInput = GetStdHandle(STD_INPUT_HANDLE);

    if (m_stdInput == INVALID_HANDLE_VALUE)
    {
        std::cout << "Unable to get STDINPUT Handle." << std::endl;

        return false;
    }

    // Get output handle
    m_stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    if (m_stdOutput == INVALID_HANDLE_VALUE)
    {
        std::cout << "Unable to get STDOUTPUT Handle." << std::endl;

        return false;
    }

    // Set the window title
    if (!SetConsoleTitle(m_title.data()))
    {
        std::cout << "Unable to set console title." << std::endl;

        return false;
    }

    // Remove the cursor
    CONSOLE_CURSOR_INFO curInfo;

    if (!GetConsoleCursorInfo(m_stdOutput, &curInfo))
    {
        std::cout << "Unable to get the console cursor information." << std::endl;

        return false;
    }

    curInfo.bVisible = false;

    if (!SetConsoleCursorInfo(m_stdOutput, &curInfo))
    {
        std::cout << "Unable to set the console cursor information." << std::endl;

        return false;
    }

    // Sleep, otherwise we run to fast to center the window
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    // Keep the same initial dimensions
    RECT windowRectangle;
    WCHAR consoleTitle[50];
    GetConsoleTitle(consoleTitle, 50);
    HWND windowHandle = FindWindow(nullptr, consoleTitle);
    GetWindowRect(windowHandle, &windowRectangle);

    // Get the screen dimensions
    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int centerWidth = screenWidth / 2;
    const int centerHeight = screenHeight / 2;

    // Calculate new top
    const int width = windowRectangle.right - windowRectangle.left;
    const int height = windowRectangle.bottom - windowRectangle.top;

    // Center the window
    SetWindowPos(
        windowHandle,
        nullptr,
        centerWidth - (width / 2),
        centerHeight - (height / 2),
        width,
        height,
        NULL);

    // Initialize buffers
    this->initializeOutputBuffer();

    this->initializeInputBuffer();

    this->flushConsole();

    return true;
}


// render
// Renders the next frame.
//
bool ConsoleEngine::render(void)
{
    if (!this->writeToConsole())
    {
        std::cout << "Unable to write to the console." << std::endl;

        return false;
    }

    return true;
}


// initializeConsole
// Returns a vector of user inputs
//
bool ConsoleEngine::input(void)
{
    unsigned long numEvents = 0;

    if (!GetNumberOfConsoleInputEvents(m_stdInput, &numEvents))
    {
        m_inputCommands.clear();
        m_inputBuffer.clear();

        std::cout << "Error getting number of console input events." << std::endl;

        return false;
    }

    if (numEvents == 0)
    {
        m_inputCommands.clear();
        m_inputBuffer.clear();

        return true;
    }

    m_inputBuffer.resize(numEvents);

    if (!ReadConsoleInput(
        m_stdInput,
        m_inputBuffer.data(),
        static_cast<DWORD>(m_inputBuffer.capacity()),
        &numEvents))
    {
        m_inputCommands.clear();
        m_inputBuffer.clear();

        std::cout << "Error reading console input." << std::endl;

        return false;
    }

    for (const auto& input : m_inputBuffer)
    {
        const auto inputEvent = this->extractInputEvent(input);

        if (inputEvent != Input::NONE)
        {
            m_inputCommands.push_back(inputEvent);
        }
    }

    return true;
}


// width
// Accessor for m_width
//
int ConsoleEngine::width(void) const
{
    return m_width;
}


// height
// Accessor for m_height
//
int ConsoleEngine::height(void) const
{
    return m_height;
}


// drawStringToBuffer
// Draws the given string at the given position.
//
void ConsoleEngine::drawStringToBuffer(
    const std::wstring& string,
    const int row,
    const int col)
{
    int i = 0;
    int offset = this->computeOffset(row, col);

    for (; i < string.length(); ++offset, ++i)
    {
        CHAR_INFO charInfo;
        charInfo.Attributes = 7;
        charInfo.Char.UnicodeChar = string.at(i);
        m_outputBuffer.at(offset) = charInfo;
    }
}


// drawStringToBuffer
// Maps a Row and Column to a position in our output buffer.
//
int ConsoleEngine::computeOffset(const int row, const int col) const
{
    return Utilities::computeTheOffset(row, col, m_width);
}


// initializeOutputBuffer
// Initializes the output buffer to empty.
//
void ConsoleEngine::initializeOutputBuffer(void)
{
    const int outputBufferReserveSize = m_width * m_height;
    m_outputBuffer.reserve(outputBufferReserveSize);
    m_outputBuffer.resize(outputBufferReserveSize);
    this->clearOutputBuffer();
}


// initializeInputBuffer
// Initializes the input buffer to empty.
//
void ConsoleEngine::initializeInputBuffer(void)
{
    constexpr size_t inputBufferReserveSize = 10;
    m_inputBuffer.reserve(inputBufferReserveSize);
    m_inputCommands.reserve(inputBufferReserveSize);
    m_inputBuffer.clear();
    m_inputCommands.clear();
}


// clearOutputBuffer
// Clears the data in the output buffer to be blank spaces.
//
void ConsoleEngine::clearOutputBuffer(void)
{
    std::for_each(
        m_outputBuffer.begin(),
        m_outputBuffer.end(),
        [](auto& charInfo) {
            charInfo.Attributes = NULL;
            charInfo.Char.UnicodeChar = L' '; });
}


// writeToConsole
// Writes to output buffer the console.
//
bool ConsoleEngine::writeToConsole(void)
{
    const short width = static_cast<short>(m_width);
    const short height = static_cast<short>(m_height);

    SMALL_RECT writeRegion;
    writeRegion.Left = 0;
    writeRegion.Top = 0;
    writeRegion.Right = width;
    writeRegion.Bottom = height;

    if (!WriteConsoleOutput(
        m_stdOutput,
        m_outputBuffer.data(),
        { width, height },
        { 0, 0 },
        &writeRegion))
    {
        return false;
    }

    return true;
}


// clearOutputBuffer
// Writes cleared data to the screen.
//
void ConsoleEngine::flushConsole(void)
{
    this->clearOutputBuffer();

    if (!this->writeToConsole())
    {
        std::cout << "Unable to flush console." << std::endl;
    }
}


// extractInputEvent
// Takes in an Input Record and maps it to our Input enum.
//
ConsoleEngine::Input ConsoleEngine::extractInputEvent(const INPUT_RECORD& input)
{
    switch (input.EventType)
    {
        case KEY_EVENT: return this->extractKeyEvent(input.Event.KeyEvent);
        default:        return ConsoleEngine::Input::NONE;
    }

    return ConsoleEngine::Input::UNDEFINED;
}


// extractKeyEvent
// Takes in an Key Event Record and maps it to our Input enum.
//
ConsoleEngine::Input ConsoleEngine::extractKeyEvent(const KEY_EVENT_RECORD& keyEvent)
{
    if (!keyEvent.bKeyDown)
    {
        return ConsoleEngine::Input::NONE;
    }

    // W, A, S, D, Q Mapping
    switch (keyEvent.uChar.UnicodeChar)
    {
        case L'q': return ConsoleEngine::Input::QUIT;
        case L'w': return ConsoleEngine::Input::UP;
        case L's': return ConsoleEngine::Input::DOWN;
        case L'a': return ConsoleEngine::Input::LEFT;
        case L'd': return ConsoleEngine::Input::RIGHT;
        default:   break;
    }

    // Arrow Key Mapping and Escape Key Mapping
    switch (keyEvent.wVirtualKeyCode)
    {
        case 27: return ConsoleEngine::Input::QUIT;
        case 38: return ConsoleEngine::Input::UP;
        case 40: return ConsoleEngine::Input::DOWN;
        case 37: return ConsoleEngine::Input::LEFT;
        case 39: return ConsoleEngine::Input::RIGHT;
        default: return ConsoleEngine::Input::NONE;
    }

    return ConsoleEngine::Input::UNDEFINED;
}
