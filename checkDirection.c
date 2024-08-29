//Написать функцию, которая будет проверять корректность выбранного направления. Змейка не может наступать на хвост, поэтому необходимо запретить:
//— перемещение справа-налево (при движении RIGHT нажатие стрелки влево),
//— перемещение сверху-вниз (при движении UP нажатие стрелки вниз),
//— перемещение слева-направо (при движении LEFT нажатие стрелки вправо),
//— перемещение снизу-вверх (при движении DOWN нажатие стрелки вверх).

//Функция должна иметь вид:
//int checkDirection(snake_t* snake, int32_t key).


#include <stdint.h>

int checkDirection(snake_t* snake, int32_t key) {
    int dx = 0, dy = 0;

    switch (key) {
        case 'a':  // LEFT
            dx = -1; dy = 0;
            break;
        case 'd':  // RIGHT
            dx = 1; dy = 0;
            break;
        case 'w':  // UP
            dx = 0; dy = -1;
            break;
        case 's':  // DOWN
            dx = 0; dy = 1;
            break;
        default:
            return 1;  
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
