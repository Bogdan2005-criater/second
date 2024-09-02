//Написать функцию, с помощью которой можно будет поставить игру на паузу при нажатии клавиши ‘P’. Добавить пояснительную надпись на главный экран.


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

#define MAX_X 15
#define MAX_Y 15
#define CONTROLS 4
#define INITIAL_DELAY 250000 
#define PAUSE_KEY 'p'

typedef struct tail_t {
    int x;
    int y;
} tail_t;

typedef struct snake_t {
    int x;
    int y;
    char direction;  
    tail_t *tail;
    size_t tsize;
    int level;      
    int delay;      
} snake_t;

typedef struct {
    int x;
    int y;
} food_t;

enum control_keys { LEFT_KEY = 'a', RIGHT_KEY = 'd', UP_KEY = 'w', DOWN_KEY = 's' };

typedef struct {
    int32_t key;
    void (*action)(snake_t* snake);
} control_button;


int _kbhit(void);
int _getch(void);

struct snake_t initSnake(int x, int y, size_t tsize) {
    struct snake_t snake;
    snake.x = x;
    snake.y = y;
    snake.direction = RIGHT_KEY;  
    snake.tsize = tsize;
    snake.level = 0;              
    snake.delay = INITIAL_DELAY;  
    snake.tail = (tail_t *)malloc(sizeof(tail_t) * 100);
    for (int i = 0; i < tsize; ++i) {
        snake.tail[i].x = x + i + 1;
        snake.tail[i].y = y;
    }
    return snake;
}

void generateFood(food_t *food, snake_t *snake) {
    int valid = 0;
    while (!valid) {
        food->x = rand() % MAX_X;
        food->y = rand() % MAX_Y;
        valid = 1;
        if (food->x == snake->x && food->y == snake->y) {
            valid = 0;
        }
        for (int i = 0; i < snake->tsize; ++i) {
            if (snake->tail[i].x == food->x && snake->tail[i].y == food->y) {
                valid = 0;
                break;
            }
        }
    }
}

void printLevel(snake_t *snake) {
    printf("Level: %d\n", snake->level);
}

void printExit(snake_t *snake) {
    printf("Game Over! Final Level: %d\n", snake->level);
}

void printSnake(struct snake_t snake, food_t food) {
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
    matrix[food.x][food.y] = 'F'; 

    for (int j = 0; j < MAX_Y; ++j) {
        for (int i = 0; i < MAX_X; ++i) {
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
    printLevel(&snake);  
    printf("Press 'P' to pause the game\n"); 
}

void updateSpeed(snake_t *snake) {
    if (snake->delay > 20000) { 
        snake->delay -= 5000;   
    }
}

int checkDirection(snake_t* snake, int32_t key) {
    int dx = 0, dy = 0;
    switch (key) {
        case LEFT_KEY: dx = -1; dy = 0; break;
        case RIGHT_KEY: dx = 1; dy = 0; break;
        case UP_KEY: dx = 0; dy = -1; break;
        case DOWN_KEY: dx = 0; dy = 1; break;
        default: return 1;  
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

void move(snake_t* snake, food_t* food) {
    int dx = 0, dy = 0;
    switch (snake->direction) {
        case LEFT_KEY: dx = -1; break;
        case RIGHT_KEY: dx = 1; break;
        case UP_KEY: dy = -1; break;
        case DOWN_KEY: dy = 1; break;
    }
    if (!checkDirection(snake, snake->direction)) {
        for (int i = snake->tsize - 1; i > 0; i--) {
            snake->tail[i] = snake->tail[i - 1];
        }
        snake->tail[0].x = snake->x;
        snake->tail[0].y = snake->y;
        snake->x += dx;
        snake->y += dy;
        if (snake->x < 0) snake->x = MAX_X - 1;
        if (snake->x >= MAX_X) snake->x = 0;
        if (snake->y < 0) snake->y = MAX_Y - 1;
        if (snake->y >= MAX_Y) snake->y = 0;


        if (snake->x == food->x && snake->y == food->y) {
            snake->level++;  
            snake->tsize++; 
            updateSpeed(snake); 
            generateFood(food, snake); 
        }
    }
}

void pauseGame() {
    printf("Game paused. Press any key to continue...\n");
    while (!_kbhit()) {
        usleep(100000); 
    }
    _getch(); 
}

// Реализация функции _kbhit
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
    { LEFT_KEY, NULL },
    { RIGHT_KEY, NULL },
    { UP_KEY, NULL },
    { DOWN_KEY, NULL }
};

int main() {
    srand(time(NULL));  
    struct snake_t snake = initSnake(10, 5, 2);
    food_t food;
    generateFood(&food, &snake);
    printSnake(snake, food);

    while (1) {
        if (_kbhit()) {
            int key = _getch();
            key = tolower(key);
            if (key == PAUSE_KEY) {
                pauseGame();
            } else {
                for (int i = 0; i < CONTROLS; i++) {
                    if (key == default_controls[i].key) {
                        snake.direction = key;
                        break;
                    }
                }
            }
        }

        move(&snake, &food);
        usleep(snake.delay);  
        system("clear");
        printSnake(snake, food);
    }

    printExit(&snake);
    return 0;
}
