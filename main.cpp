#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Segment {
    int x, y;
};

std::vector<Segment> snake;
int foodX = 10, foodY = 10;
enum Direction { UP, DOWN, LEFT, RIGHT };
Direction dir;
enum GameState { START, RUNNING, PAUSED, GAME_OVER };
GameState gameState = START;
const int WINDOW_WIDTH = 200;
const int WINDOW_HEIGHT = 200;
const int FIELD_WIDTH = 100;
const int FIELD_HEIGHT = 100;
int offsetX = (WINDOW_WIDTH - FIELD_WIDTH) / 2;
int offsetY = (WINDOW_HEIGHT - FIELD_HEIGHT) / 2;
// Ширина змейки и яблоко
const int SIZE = 4;

void initGame() {
    // Инициализация или реинициализация игры
    snake = {{5, 5}, {5, 6}, {5, 7}};
    dir = RIGHT;
    gameState = RUNNING;
}

void drawBorders() {
    glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2i(offsetX, offsetY);
        glVertex2i(offsetX + FIELD_WIDTH, offsetY);
        glVertex2i(offsetX + FIELD_WIDTH, offsetY + FIELD_HEIGHT);
        glVertex2i(offsetX, offsetY + FIELD_HEIGHT);
        glEnd();
}

void placeFood() {
    bool isOnSnake;
    do {
        isOnSnake = false;
        foodX = rand() % (FIELD_WIDTH / SIZE);
        foodY = rand() % (FIELD_HEIGHT / SIZE);

        for (const auto& segment : snake) {
            if (segment.x == foodX && segment.y == foodY) {
                isOnSnake = true;
                break;
            }
        }
    } while (isOnSnake);
}

void updateSnakePosition() {
    if (gameState != RUNNING) return;

    // Обновляем положение головы
    if (dir == UP)    snake[0].y++;
    if (dir == DOWN)  snake[0].y--;
    if (dir == LEFT)  snake[0].x--;
    if (dir == RIGHT) snake[0].x++;

    // Проверка границ поля
    Segment head = snake.front();
    if (head.x < 0 || head.x * SIZE + SIZE > FIELD_WIDTH || head.y < 0 || head.y * SIZE + SIZE > FIELD_HEIGHT) {
        gameState = GAME_OVER;
        return;
    }

    // Проверка на съедение яблока
    if (head.x == foodX && head.y == foodY) {
        // Добавляем новый сегмент в конец змейки
        snake.push_back({snake.back().x, snake.back().y});
        placeFood();
    } else {
        // Двигаем все сегменты, кроме головы
        for (size_t i = snake.size() - 1; i > 0; --i) {
            snake[i] = snake[i - 1];
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC для паузы
            if (gameState == RUNNING) {
                gameState = PAUSED;
            } else if (gameState == PAUSED) {
                gameState = RUNNING;
            }
        }
    
    if (gameState == START && key == '\r') { // Нажатие Enter для начала игры
        initGame();
    }

    if (gameState == RUNNING) {
        switch (key) {
            case 'w': if (dir != DOWN) dir = UP; break;
            case 's': if (dir != UP) dir = DOWN; break;
            case 'a': if (dir != RIGHT) dir = LEFT; break;
            case 'd': if (dir != LEFT) dir = RIGHT; break;
        }
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBorders();
    // Установка цвета для змейки (например, зеленого)
    glColor3f(0.0f, 1.0f, 0.0f);
    for (auto &segment : snake) {
        glRecti(segment.x * SIZE + offsetX, segment.y * SIZE + offsetY,
                        segment.x * SIZE + offsetX + SIZE, segment.y * SIZE + offsetY + SIZE);
    }

    // Установка цвета для еды (например, красного)
    glColor3f(1.0f, 0.0f, 0.0f);
    glRecti(foodX * SIZE + offsetX, foodY * SIZE + offsetY,
                foodX * SIZE + offsetX + SIZE, foodY * SIZE + offsetY + SIZE);

    glFlush();
}

void timer(int) {
    if (gameState == RUNNING) {
            updateSnakePosition();
        }
    glutPostRedisplay();
    glutTimerFunc(1000 / 10, timer, 0);
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(nullptr)));
    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Snake Game");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
