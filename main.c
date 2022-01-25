#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>

#define WIDTH 60
#define HEIGHT 30

typedef enum {UP, DOWN, LEFT, RIGHT} snakeDirection; //перечисление для направления змеи

typedef struct //описание одной ячейки змеи
{
    int x, y;
} Body;

typedef struct //описание змеи
{
    Body body[WIDTH * HEIGHT - 1];
    int length;
    snakeDirection direction;
} Snake;
Snake snake;

const int DELAY = 10; //задержка
bool inGame = true;
Body* head = NULL; //указатель на голову змеи
Body apple;
int applesEaten = 0; //кол-во съеденных яблок

char map[HEIGHT][WIDTH + 1]; //карта

//отчистка карты
void clearMap()
{
    memset(map[0], '#', WIDTH);
    map[0][WIDTH] = '\n';

    for (int i = 1; i < HEIGHT - 1; i++)
    {
        memset(map[i], 249, WIDTH-1);
        map[i][0] = '#';
        map[i][WIDTH-1] = '#';
        map[i][WIDTH] = '\n';
    }
    memset(map[HEIGHT-1], '#', WIDTH);
}

//установка курсора в нужную позицию. Нужно для отчистки экрана без мерцания
void setCur(int x, int y)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(handle, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(handle, &info);

    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(handle, coord);
}

//отрисовка карты
void showMap()
{
    setCur(0, 0);
    puts(map);
    printf("apples: %d", applesEaten);
}

//инициализация змеи
void initSnake()
{
    snake.direction = RIGHT;
    snake.length = 3;

    snake.body[0].x = 15;
    snake.body[0].y = 15;

    snake.body[1].x = 14;
    snake.body[1].y = 15;

    snake.body[2].x = 13;
    snake.body[2].y = 15;

    head = &snake.body[0];
}

//помещение объектов на карту
void putOnMap()
{
    switch (snake.direction)
    {
    case UP:
        map[head->y][head->x] = 30;
        break;
    case DOWN:
        map[head->y][head->x] = 31;
        break;
    case LEFT:
        map[head->y][head->x] = 17;
        break;
    case RIGHT:
        map[head->y][head->x] = 16;
        break;
    }

    for (int i = 1; i < snake.length; i++)
        map[snake.body[i].y][snake.body[i].x] = 'o';

    map[apple.y][apple.x] = 'a';
}

//проверка коллизии. Возращает true если произошло столкновение
bool collison(int x, int y, snakeDirection dir)
{
    bool success;

    int dx = 0;
    int dy = 0;

    switch(dir)
    {
    case UP:
        dy = -1;
        break;
    case DOWN:
        dy = 1;
        break;
    case LEFT:
        dx = -1;
        break;
    case RIGHT:
        dx = 1;
        break;
    }

    if (map[head->y + dy][head->x + dx] == '#' || map[head->y + dy][head->x + dx] == 'o')
        success = true;
    else
        success = false;
    return success;
}

//передвижение змеи
void moveSnake()
{
    for (int i = snake.length - 1; i > 0; i--)
        snake.body[i] = snake.body[i-1];

    switch (snake.direction)
    {
    case UP:
        if ( collison(head->x, head->y - 1, snake.direction) )
            inGame = false;
        else
            head->y--;
        break;

    case DOWN:
        if ( collison(head->x, head->y + 1, snake.direction) )
            inGame = false;
        else
            head->y++;
        break;

    case LEFT:
        if ( collison(head->x - 1, head->y, snake.direction) )
            inGame = false;
        else
            head->x--;
        break;

    case RIGHT:
        if ( collison(head->x + 1, head->y, snake.direction) )
            inGame = false;
        else
            head->x++;
        break;
    }
}

//управление
void control()
{
    if (GetAsyncKeyState(VK_UP) < 0 && snake.direction != DOWN) snake.direction = UP;
    else if (GetAsyncKeyState(VK_DOWN) < 0 && snake.direction != UP) snake.direction = DOWN;
    else if (GetAsyncKeyState(VK_LEFT) < 0 && snake.direction != RIGHT) snake.direction = LEFT;
    else if (GetAsyncKeyState(VK_RIGHT) < 0 && snake.direction != LEFT) snake.direction = RIGHT;
    if (GetAsyncKeyState(VK_ESCAPE) < 0) inGame = false;
}

//генерация яблока
void generateApple()
{
    srand(time(NULL));
    while(true)
    {
        apple.x = rand() % WIDTH;
        apple.y = rand() % HEIGHT;
        if(map[apple.y][apple.x] != '#' && map[apple.y][apple.x] != 'o')
            break;
    }
}

void addNewBody();

//функция для проверки съедения яблока
void appleEaten()
{
    if (apple.x == head->x && apple.y == head->y)
    {
        applesEaten++;
        generateApple();
        addNewBody();
    }
}

//увеличение змеи
void addNewBody()
{
    snake.length++;

    if (snake.body[snake.length-2].x < snake.body[snake.length-3].x)
    {
        snake.body[snake.length-1].x = snake.body[snake.length-2].x - 1;
        snake.body[snake.length-1].y = snake.body[snake.length-2].y;
    }
    else if (snake.body[snake.length-2].x > snake.body[snake.length-3].x)
    {
        snake.body[snake.length-1].x = snake.body[snake.length-2].x + 1;
        snake.body[snake.length-1].y = snake.body[snake.length-2].y;
    }
    else if (snake.body[snake.length-2].y > snake.body[snake.length-3].y)
    {
        snake.body[snake.length-1].y = snake.body[snake.length-2].y + 1;
        snake.body[snake.length-1].x = snake.body[snake.length-2].x;
    }
     else if (snake.body[snake.length-2].y < snake.body[snake.length-3].y)
    {
        snake.body[snake.length-1].y = snake.body[snake.length-2].y - 1;
        snake.body[snake.length-1].x = snake.body[snake.length-2].x;
    }
}

int main()
{
    clearMap();
    initSnake();
    generateApple();
    showMap();
    setCur(0, HEIGHT);
    printf("press enter to start\n");
    getch();
    system("cls");
    int counter = 0;

    do
    {
        clearMap();
        putOnMap();
        control();
        if (counter >= DELAY)
        {
            moveSnake();
            counter = 0;
        }
        appleEaten();
        showMap();
        counter++;
        Sleep(1);

    }
    while(inGame);
    getch();
    return 0;
}
