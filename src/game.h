#pragma once
#include "grid.h"
#include "blocks.cpp"
#include <raylib.h>

class Game
{
public:
    Game();
    ~Game();
    void Draw();
    void DrawNextBlock(int x, int y);
    void HandleInput();
    void MoveBlockDown();
    void SaveScore();
    int GetHighScore();
    void Reset();
    bool gameOver;
    int score;
    int level;
    Music music;

private:
    void LoadHighScore();
    int highScore;

    void MoveBlock(int direction);

    Block GetRandomBlock();
    std::vector<Block> GetAllBlocks();
    bool IsBlockOutside();
    void RotateBlock();
    void LockBlock();
    bool BlockFits();
    void UpdateScore(int linesCleared, int moveDownPoints);
    void HardDrop();

    Grid grid;
    std::vector<Block> blocks;
    Block currentBlock;
    Block nextBlock;
    Sound rotateSound;
    Sound clearSound;
    Sound lockSound;
};
