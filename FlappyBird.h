#pragma once

#include "ConsoleEngine.hpp"


struct Point
{
    int m_col = 0;
    int m_row = 0;
};


struct Pipe
{
    static constexpr int m_width = 4;
    double m_velocity = 15.0; // units per second

    double m_colPosition = 0.0;

    int m_col = 0;         // Will decrement from the end each second
    int m_gapSize = 10;    // How big the gap is
    int m_gapStartRow = 7; // The row the gap starts relative to the top
    bool m_scoreTracked = false;

    void updatePosition(const double deltaTime);
    void drawToOutputBuffer(
        std::vector<CHAR_INFO>& outputBuffer,
        const int birdRow,
        const int birtCol,
        bool& isHit) const;
};


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

    // Update Physics
    // 
    void updatePhysics(const double deltaTime);

    // Handle Input Events
    // 
    void handleInputEvents(void);

    // Draw FPS to Screen
    //
    void drawFPSToOutputBuffer(void);

    // Draw Score to Screen
    //
    void drawScoreToOutputBuffer(void);

    // Draw Velocity to Screen
    //
    void drawVelocityToOutputBuffer(void);

    // Private Data Variables
    //
    size_t m_score;
    double m_verticalVelocity;
    double m_rowDouble;
    bool m_jump;
    int m_row;
    int m_col;
    std::vector<Pipe> m_pipes;

    // Configurable parameters
    double m_jumpVelocity;
    double m_pipeVelocity;
    double m_gravity;
};
