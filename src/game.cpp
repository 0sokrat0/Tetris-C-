#include "game.h"
#include "utils.h"  // Подключаем utils.h
#include <random>
#include <fstream>

Game::Game() {
    grid = Grid();
    blocks = GetAllBlocks();
    currentBlock = GetRandomBlock();
    nextBlock = GetRandomBlock();
    gameOver = false;
    score = 0;
    level = 1;

    InitAudioDevice();
    music = LoadMusicStream("Sounds/music.mp3");
    PlayMusicStream(music);
    rotateSound = LoadSound("Sounds/rotate.mp3");
    clearSound = LoadSound("Sounds/clear.mp3");
    lockSound = LoadSound("Sounds/lock.mp3");

    LoadHighScore();  // Загружаем рекорд при инициализации
}

Game::~Game() {
    UnloadSound(rotateSound);
    UnloadSound(clearSound);
    UnloadSound(lockSound);
    UnloadMusicStream(music);
    CloseAudioDevice();
}

void Game::UpdateMusic() {
    UpdateMusicStream(music);
}



void Game::Reset() {
    grid.Initialize();
    blocks = GetAllBlocks();
    currentBlock = GetRandomBlock();
    nextBlock = GetRandomBlock();
    score = 0;
    level = 1;
    gameOver = false;  // Сбрасываем состояние игры
}

void Game::SaveScore() {
    std::ifstream recordFile("highscore.txt");
    int highScore = 0;

    if (recordFile.is_open()) {
        recordFile >> highScore;
        recordFile.close();
    }

    if (score > highScore) {
        std::ofstream outFile("highscore.txt");
        if (outFile.is_open()) {
            outFile << score;
            outFile.close();
        }
    }
}

void Game::LoadHighScore() {
    std::ifstream recordFile("highscore.txt");
    highScore = 0;  // Устанавливаем значение по умолчанию

    if (recordFile.is_open()) {
        recordFile >> highScore;
        recordFile.close();
    }
}

int Game::GetHighScore() {
    return highScore;
}

Block Game::GetRandomBlock() {
    if (blocks.empty()) {
        blocks = GetAllBlocks();
    }
    int randomIndex = rand() % blocks.size();
    Block block = blocks[randomIndex];
    blocks.erase(blocks.begin() + randomIndex);
    return block;
}

std::vector<Block> Game::GetAllBlocks() {
    return {IBlock(), JBlock(), LBlock(), OBlock(), SBlock(), TBlock(), ZBlock()};
}

void Game::Draw() {
    grid.Draw();
    currentBlock.Draw(11, 11);
}

void Game::DrawNextBlock(int x, int y) {
    nextBlock.Draw(x + 10, y + 0);
}

void Game::HandleInput() {
    // Используем таймеры для контроля чувствительности
    static double lastMoveTimeLR = 0;
    static double lastMoveTimeDown = 0;

    if (IsKeyDown(KEY_LEFT)) {
        if (EventTriggered(0.15, lastMoveTimeLR)) {
            MoveBlock(-1);
        }
    }
    if (IsKeyDown(KEY_RIGHT)) {
        if (EventTriggered(0.15, lastMoveTimeLR)) {
            MoveBlock(1);
        }
    }
    if (IsKeyDown(KEY_DOWN)) {
        if (EventTriggered(0.05, lastMoveTimeDown)) {
            MoveBlockDown();
            UpdateScore(0, 1);
        }
    }
    if (IsKeyPressed(KEY_UP)) {
        RotateBlock();
    }
    if (IsKeyPressed(KEY_SPACE)) {
        HardDrop();
    }
}

void Game::MoveBlock(int direction) {
    if (!gameOver) {
        currentBlock.Move(0, direction);
        if (IsBlockOutside() || !BlockFits()) {
            currentBlock.Move(0, -direction);
        }
    }
}

void Game::MoveBlockDown() {
    if (!gameOver) {
        currentBlock.Move(1, 0);
        if (IsBlockOutside() || !BlockFits()) {
            currentBlock.Move(-1, 0);
            LockBlock();
        }
    }
}

void Game::HardDrop() {
    while (!IsBlockOutside() && BlockFits()) {
        currentBlock.Move(1, 0);
        score += 2;
    }
    currentBlock.Move(-1, 0);
    LockBlock();
}

bool Game::IsBlockOutside() {
    std::vector<Position> tiles = currentBlock.GetCellPositions();
    for (const auto& item : tiles) {
        if (grid.IsCellOutside(item.row, item.column)) {
            return true;
        }
    }
    return false;
}

void Game::RotateBlock() {
    if (!gameOver) {
        currentBlock.Rotate();
        if (IsBlockOutside() || !BlockFits()) {
            currentBlock.UndoRotation();
        } else {
            PlaySound(rotateSound);
        }
    }
}

void Game::LockBlock() {
    std::vector<Position> tiles = currentBlock.GetCellPositions();
    for (const auto& item : tiles) {
        grid.grid[item.row][item.column] = currentBlock.id;
    }

    PlaySound(lockSound);

    currentBlock = nextBlock;
    nextBlock = GetRandomBlock();

    if (!BlockFits()) {
        gameOver = true;
    }

    int rowsCleared = grid.ClearFullRows();
    if (rowsCleared > 0) {
        PlaySound(clearSound);
        UpdateScore(rowsCleared, 0);
    }
}

bool Game::BlockFits() {
    std::vector<Position> tiles = currentBlock.GetCellPositions();
    for (const auto& item : tiles) {
        if (!grid.IsCellEmpty(item.row, item.column)) {
            return false;
        }
    }
    return true;
}

void Game::UpdateScore(int linesCleared, int moveDownPoints) {
    static const std::vector<int> pointsPerLine = {0, 100, 300, 500, 800};
    if (linesCleared >= 1 && linesCleared <= 4) {
        score += pointsPerLine[linesCleared] * level;
    }
    score += moveDownPoints;

    // Обновление уровня каждые 1000 очков
    level = score / 1000 + 1;
}
