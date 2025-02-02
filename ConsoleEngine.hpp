#pragma once

#include <windows.h>
#include <string>
#include <vector>


namespace Utilities
{
    [[nodiscard]] int computeTheOffset(const int row, const int col, const int width);
}

class ConsoleEngine
{
public:
    enum class Input
    {
        UNDEFINED = -1,
        NONE      = 0,
        QUIT      = 1,
        UP        = 2,
        DOWN      = 3,
        LEFT      = 4,
        RIGHT     = 5
    };

    enum class PlayAgain
    {
        YES = 0,
        NO = 1
    };

    ConsoleEngine(const std::wstring& title, const int width, const int height);
    virtual ~ConsoleEngine(void);

    ConsoleEngine(void) = delete;
    ConsoleEngine(const ConsoleEngine& RHS) = delete;
    ConsoleEngine(ConsoleEngine&& RHS) = delete;
    ConsoleEngine& operator=(const ConsoleEngine& RHS) = delete;
    ConsoleEngine& operator=(ConsoleEngine&& RHS) = delete;

    [[nodiscard]] int gameLoop(void);
    [[nodiscard]] bool initializeConsole(void);

protected:
    [[nodiscard]] virtual bool update(const double deltaTime) = 0;
    [[nodiscard]] virtual bool render(void);
    virtual void resetGameState(void) = 0;
    virtual void onGameBegin(void) = 0;
    virtual PlayAgain onGameEnd(void) const = 0;
    [[nodiscard]] bool input(void);
    [[nodiscard]] int width(void) const;
    [[nodiscard]] int height(void) const;
    void drawStringToBuffer(const std::wstring& string, const int row, const int col);

    bool m_running;
    double m_fps;
    int m_width;
    int m_height;
    std::vector<CHAR_INFO> m_outputBuffer;
    std::vector<ConsoleEngine::Input> m_inputCommands;

private:
    void initializeOutputBuffer(void);
    void initializeInputBuffer(void);
    void clearOutputBuffer(void);
    bool writeToConsole(void);
    void flushConsole(void);
    [[nodiscard]] int computeOffset(const int row, const int col) const;
    [[nodiscard]] ConsoleEngine::Input extractInputEvent(const INPUT_RECORD& input);
    [[nodiscard]] ConsoleEngine::Input extractKeyEvent(const KEY_EVENT_RECORD& keyEvent);

    HANDLE m_stdInput;
    HANDLE m_stdOutput;
    std::vector<INPUT_RECORD> m_inputBuffer;
    const std::wstring m_title;
};
