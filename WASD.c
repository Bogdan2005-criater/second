//Добавить возможность управления змейкой клавишами WSAD (вне зависимости от раскладки и регистра).
//Зависимости от регистра в соответствии с таблицей.

//Для решения предлагается сделать массив кодов управления struct control_buttons default_controls[CONTROLS]. CONTROLS – определяем количество элементов массива.
//В необходимых функциях в цикле необходимо сравнивать с каждым типом управления в цикле: for (int i = 0; i < CONTROLS; i++)



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdint.h>

#define MAX_X 15
#define MAX_Y 15
#define CONTROLS 4

typedef struct tail_t {
    int x;
    int y;
} tail_t;

typedef struct snake_t {
    int x;
    int y;
    tail_t *tail;
    size_t tsize;
} snake_t;

enum control_keys { LEFT_KEY = 'a', RIGHT_KEY = 'd', UP_KEY = 'w', DOWN_KEY = 's' };

typedef struct {
    int32_t key;
    void (*action)(snake_t* snake);
} control_button;

struct snake_t initSnake(int x, int y, size_t tsize) {
    struct snake_t snake;
    snake.x = x;
    snake.y = y;
    snake.tsize = tsize;
    snake.tail = (tail_t *)malloc(sizeof(tail_t) * 100);
    for (int i = 0; i < tsize; ++i) {
        snake.tail[i].x = x + i + 1;
        snake.tail[i].y = y;
    }
    return snake;
}

void printSnake(struct snake_t snake) {
    char matrix[MAX_X][MAX_Y];
    for (int i = 0; i < MAX_X; ++i) {
        for (int j = 0; j < MAX_Y; ++j) {
            matrix[i][j] = ' ';
        }
    }
    matrix[snake.x][snake.y] = '@';
    for (int i = 0; i < snake.tsize; ++i) {
        matrix[snake.tail[i].x][snake.tail[i].y] = '*';
    }
    for (int j = 0; j < MAX_Y; ++j) {
        for (int i = 0; i < MAX_X; ++i) {
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
}

int checkDirection(snake_t* snake, int32_t key) {
    int dx = 0, dy = 0;
    switch (key) {
        case LEFT_KEY: dx = -1; dy = 0; break;
        case RIGHT_KEY: dx = 1; dy = 0; break;
        case UP_KEY: dx = 0; dy = -1; break;
        case DOWN_KEY: dx = 0; dy = 1; break;
        default: return 1;  // Неверная клавиша
    }
    int newX = snake->x + dx;
    int newY = snake->y + dy;
    if (newX < 0) newX = MAX_X - 1;
    if (newX >= MAX_X) newX = 0;
    if (newY < 0) newY = MAX_Y - 1;
    if (newY >= MAX_Y) newY = 0;
    for (int i = 0; i < snake->tsize; ++i) {
        if (snake->tail[i].x == newX && snake->tail[i].y == newY) {
            return 1;  
        }
    }
    return 0;  
}

void moveLeft(snake_t* snake) {
    if (!checkDirection(snake, LEFT_KEY)) {
        for (int i = snake->tsize - 1; i > 0; i--) {
            snake->tail[i] = snake->tail[i - 1];
        }
        snake->tail[0].x = snake->x;
        snake->tail[0].y = snake->y;
        snake->x -= 1;
        if (snake->x < 0) snake->x = MAX_X - 1;
    }
}

void moveRight(snake_t* snake) {
    if (!checkDirection(snake, RIGHT_KEY)) {
        for (int i = snake->tsize - 1; i > 0; i--) {
            snake->tail[i] = snake->tail[i - 1];
        }
        snake->tail[0].x = snake->x;
        snake->tail[0].y = snake->y;
        snake->x += 1;
        if (snake->x >= MAX_X) snake->x = 0;
    }
}

void moveUp(snake_t* snake) {
    if (!checkDirection(snake, UP_KEY)) {
        for (int i = snake->tsize - 1; i > 0; i--) {
            snake->tail[i] = snake->tail[i - 1];
        }
        snake->tail[0].x = snake->x;
        snake->tail[0].y = snake->y;
        snake->y -= 1;
        if (snake->y < 0) snake->y = MAX_Y - 1;
    }
}

void moveDown(snake_t* snake) {
    if (!checkDirection(snake, DOWN_KEY)) {
        for (int i = snake->tsize - 1; i > 0; i--) {
            snake->tail[i] = snake->tail[i - 1];
        }
        snake->tail[0].x = snake->x;
        snake->tail[0].y = snake->y;
        snake->y += 1;
        if (snake->y >= MAX_Y) snake->y = 0;
    }
}

int _kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int _getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

control_button default_controls[CONTROLS] = {
    { LEFT_KEY, moveLeft },
    { RIGHT_KEY, moveRight },
    { UP_KEY, moveUp },
    { DOWN_KEY, moveDown }
};

int main() {
    struct snake_t snake = initSnake(10, 5, 2);
    printSnake(snake);

    while (1) {
        if (_kbhit()) {
            int key = _getch();
            key = tolower(key);
            for (int i = 0; i < CONTROLS; i++) {
                if (key == default_controls[i].key) {
                    default_controls[i].action(&snake);
                    break;
                }
            }
        }

        sleep(1);
        system("clear"); 
        printSnake(snake);
    }

    return 0;
}
