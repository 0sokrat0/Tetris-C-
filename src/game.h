#pragma once
#include "grid.h"
#include "blocks.cpp" // Меняем с blocks.cpp на blocks.h
#include <raylib.h>

class Game
{
public:
    Game();                       // Конструктор
    ~Game();                      // Деструктор
    void Draw();                  // Отрисовка текущего блока и сетки
    void DrawNextBlock(int x, int y); // Отрисовка следующего блока
    void HandleInput();           // Обработка ввода игрока
    void MoveBlockDown();         // Перемещение блока вниз
    void SaveScore();             // Сохранение текущего счёта
    int GetHighScore();           // Получение рекорда
    void Reset();                 // Сброс состояния игры
    void UpdateMusic();           // Обновление музыкального потока
    bool gameOver;                // Флаг окончания игры
    int score;                    // Текущий счёт
    int level;                    // Текущий уровень

private:
    void LoadHighScore();         // Загрузка рекорда из файла
    int highScore;                // Рекорд

    void MoveBlock(int direction); // Перемещение блока влево/вправо
    Block GetRandomBlock();       // Получение случайного блока
    std::vector<Block> GetAllBlocks(); // Получение всех типов блоков
    bool IsBlockOutside();        // Проверка выхода блока за границы
    void RotateBlock();           // Поворот текущего блока
    void LockBlock();             // Закрепление текущего блока
    bool BlockFits();             // Проверка, подходит ли блок на позицию
    void UpdateScore(int linesCleared, int moveDownPoints); // Обновление счёта
    void HardDrop();              // Ускоренное падение блока

    Grid grid;                    // Игровое поле
    std::vector<Block> blocks;    // Список доступных блоков
    Block currentBlock;           // Текущий блок
    Block nextBlock;              // Следующий блок
    Sound rotateSound;            // Звук поворота блока
    Sound clearSound;             // Звук очистки линии
    Sound lockSound;              // Звук закрепления блока
    Music music;                  // Фоновая музыка
};
