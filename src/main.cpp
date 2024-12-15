#include <raylib.h>
#include "game.h"
#include "colors.h"
#include "utils.h" 
#include <iostream>
#include <fstream>
#include <cmath>

//Константы
double lastUpdateTime = 0;
double lastMoveTime = 0; 
bool isPaused = false;
bool isDarkTheme = true;
float fadeAlpha = 0.0f;
bool gameStarted = false;

//Структура с темами
struct Theme {
    Color background;
    Color text;
    Color highlight;
};


Theme darkTheme = {{20, 30, 50, 255}, WHITE, LIME};
Theme lightTheme = {RAYWHITE, BLACK, SKYBLUE};

bool DrawThemeButton(Font font, bool &isDarkTheme, float &themeTransition) {
    Rectangle button = {360, 520, 120, 50}; // Параметры кнопки

    // Проверка наведения мыши и клика
    bool hovered = CheckCollisionPointRec(GetMousePosition(), button);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    // Определяем цвет кнопки
    Color buttonColor = hovered ? SKYBLUE : LIGHTGRAY;

    // Рисуем кнопку
    DrawRectangleRounded(button, 0.3, 6, buttonColor);

    // Текст кнопки
    const char *buttonText = "Theme";
    Vector2 textSize = MeasureTextEx(font, buttonText, 20, 2);

    // Ручное смещение текста
    float textX = button.x + 15; // Сдвиг текста на 10 пикселей вправо от левого края кнопки
    float textY = button.y + (button.height - textSize.y) / 4; // Вертикальное центрирование

    // Рисуем текст с учётом смещения
    DrawTextEx(font, buttonText, {textX, textY}, 30, 2, BLACK);

    // Обработка клика
    if (clicked) {
        isDarkTheme = !isDarkTheme; // Переключаем тему
        themeTransition = 0.0f;    // Начинаем анимацию перехода
    }

    return clicked;
}


void AnimateThemeTransition(const Theme &from, const Theme &to, float transition) {
    Color background = {
        (unsigned char)(from.background.r + transition * (to.background.r - from.background.r)),
        (unsigned char)(from.background.g + transition * (to.background.g - from.background.g)),
        (unsigned char)(from.background.b + transition * (to.background.b - from.background.b)),
        255
    };
    ClearBackground(background);
}


void DrawStartScreen(Font font) {
    ClearBackground(BLACK);
    DrawTextEx(font, "Press Space to Start", {50, 270}, 40, 2, WHITE);
}

// Сохранение рекорда в файл
void SaveHighScore(int score) {
    std::ofstream outFile("highscore.txt");
    if (outFile.is_open()) {
        outFile << score;
        outFile.close();
    }
}

void DrawScore(Font font, int score, const Theme &theme) {
    DrawTextEx(font, "Score", {350, 15}, 38, 2, theme.highlight);
    char scoreText[10];
    sprintf(scoreText, "%d", score);
    Vector2 scoreSize = MeasureTextEx(font, scoreText, 38, 2);
    DrawTextEx(font, scoreText, {350 + (130 - scoreSize.x) / 2, 65}, 38, 2, theme.text);
}

void DrawHighScore(Font font, int highScore, const Theme &theme) {
    DrawTextEx(font, "High Score", {330, 110}, 30, 2, theme.highlight);
    char highScoreText[10];
    sprintf(highScoreText, "%d", highScore);
    Vector2 highScoreSize = MeasureTextEx(font, highScoreText, 38, 2);
    DrawTextEx(font, highScoreText, {350 + (130 - highScoreSize.x) / 2, 150}, 38, 2, theme.text);
}


int LoadHighScore() {
    std::ifstream inFile("highscore.txt");
    int highScore = 0;
    if (inFile.is_open()) {
        inFile >> highScore;
        inFile.close();
    }
    return highScore;
}


void ResetGame(Game &game, bool &scoreSaved, float &fadeAlpha) {
    game.Reset();
    scoreSaved = false;
    fadeAlpha = 0.0f;
}


void DrawPauseAnimation(Font font) {
    static float pauseAlpha = 0.0f;
    pauseAlpha = fmod(pauseAlpha + 0.05f, 1.0f);
    Color pauseColor = Fade(WHITE, pauseAlpha);
    Vector2 textSize = MeasureTextEx(font, "PAUSED", 60, 6);
    DrawTextEx(font, "PAUSED", {250 - textSize.x / 2, 270}, 60, 6, pauseColor);
}


int main() {
    InitWindow(500, 620, "Tetris");
    SetTargetFPS(60);

    Font font = LoadFontEx("monogram.ttf", 64, 0, 0);
    Game game = Game();
    bool scoreSaved = false;
    Sound newRecordSound = LoadSound("Sounds/new_record.mp3");
    game.UpdateMusic();
   

    bool isDarkTheme = true;
    float themeTransition = 1.0f;
    int highScore = LoadHighScore(); 

    while (!WindowShouldClose()) {

       game.UpdateMusic();

        if (!gameStarted) {
            if (IsKeyPressed(KEY_SPACE)) {
                gameStarted = true;
            }

            BeginDrawing();
            DrawStartScreen(font);
            EndDrawing();
        } else {
            if (IsKeyPressed(KEY_P)) {
                isPaused = !isPaused;
            }

            if (game.gameOver && IsKeyPressed(KEY_ENTER)) {
                ResetGame(game, scoreSaved, fadeAlpha);
            }

            if (themeTransition < 1.0f) {
                themeTransition += 0.02f;
            }

            Theme currentTheme = isDarkTheme ? darkTheme : lightTheme;
            Theme nextTheme = isDarkTheme ? lightTheme : darkTheme;

            if (!isPaused && !game.gameOver) {
                game.HandleInput();  

                double speed = 0.5 - (game.level - 1) * 0.05;  
                if (speed < 0.1) speed = 0.1;

                if (EventTriggered(speed, lastUpdateTime)) {
                    game.MoveBlockDown();
                }
            }

            BeginDrawing();
            AnimateThemeTransition(currentTheme, nextTheme, themeTransition);

            DrawScore(font, game.score, nextTheme);
            DrawHighScore(font, highScore, nextTheme);
            game.Draw();

            DrawThemeButton(font, isDarkTheme, themeTransition);

            DrawTextEx(font, "Next", {370, 200}, 30, 2, nextTheme.highlight);
            DrawRectangleLines(340, 240, 150, 130, nextTheme.text);
            game.DrawNextBlock(265, 265);

            if (isPaused) {
                DrawRectangle(0, 0, 500, 620, Fade(BLACK, 0.5f));
                DrawPauseAnimation(font);
            }

            if (game.gameOver) {
                if (fadeAlpha < 1.0f) {
                    fadeAlpha += 0.01f;
                }

                DrawRectangle(0, 0, 500, 620, Fade(BLACK, fadeAlpha));

                static float alpha = 0.0f;
                alpha = fmod(alpha + 0.01f, 1.0f);
                Color gameOverColor = Fade(RED, alpha);

               
                DrawTextEx(font, "GAME OVER", {80, 280}, 60, 6, gameOverColor);
                DrawTextEx(font, "Press ENTER to Restart", {80, 340}, 30, 2, LIGHTGRAY);

                if (!scoreSaved) {
                    if (game.score > highScore) {
                        PlaySound(newRecordSound);
                        SaveHighScore(game.score);
                        highScore = game.score; 
                    }
                    scoreSaved = true;
                }
            }

            EndDrawing();
        }
    }

    UnloadSound(newRecordSound);
    CloseWindow();
    return 0;
}
