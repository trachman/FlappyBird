#pragma once

#include "ConsoleEngine.hpp"


class FlappyBird : public ConsoleEngine
{
public:
    // Deleted Special Member Functions
    //
    FlappyBird(void) = delete;
    FlappyBird(const FlappyBird& RHS) = delete;
    FlappyBird(FlappyBird&& RHS) = delete;
    FlappyBird& operator=(const FlappyBird& RHS) = delete;
    FlappyBird& operator=(FlappyBird&& RHS) = delete;

    // Constructor
    //
    FlappyBird(const std::wstring& title, const int width, const int height);

    // Destructor
    //
    virtual ~FlappyBird(void) = default;

private:
    // Virtual Methods
    //
    [[nodiscard]] bool update(const double deltaTime) override;
    [[nodiscard]] bool render(void) override;
    void resetGameState(void) override;
    void onGameBegin(void) override;
    [[nodiscard]] PlayAgain onGameEnd(void) const override;

    // Handle Input Events
    // 
    void handleInputEvents(void);

    // Draw FPS to Screen
    //
    void drawFPSToOutputBuffer(void);

    // Draw Score to Screen
    //
    void drawScoreToOutputBuffer(void);

    // Private Data Variables
    //
    size_t m_score;
};
