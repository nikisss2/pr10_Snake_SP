#include <iostream>
#include <string>
#include <fstream>
#include <conio.h>
#include <windows.h>

struct Segment
{
    int x;
    int y;
    Segment* next;
};

struct Record
{
    char name[20];
    int score;
};

class Snake
{
public:
    Segment* head;
    char board[20][20];

    int foodX;
    int foodY;
    int dirX;
    int dirY;
    int score;

    Snake()
    {
        head = nullptr;
        foodX = 0;
        foodY = 0;
        dirX = 1;
        dirY = 0;
        score = 0;
    }

    void ClearSnake()
    {
        Segment* current = head;
        while (current != nullptr)
        {
            Segment* temp = current->next;
            delete current;
            current = temp;
        }
        head = nullptr;
    }

    bool Move(int posX, int posY)
    {
        Segment* newHead = new Segment;
        newHead->x = head->x + posX;
        newHead->y = head->y + posY;
        newHead->next = head;
        head = newHead;

        //границы
        if (head->x <= 0 || head->x >= 19 || head->y <= 0 || head->y >= 19)
        {
            return false;
        }

        //столкновение с собой
        Segment* check = head->next;
        while (check != nullptr)
        {
            if (check->x == head->x && check->y == head->y)
            {
                return false;
            }
            check = check->next;
        }

        //если съели
        if (head->x == foodX && head->y == foodY)
        {
            score += 10;

            foodX = 1 + rand() % 18;
            foodY = 1 + rand() % 18;
        }
        else
        {
            Segment* current = head;
            while (current->next->next != nullptr)
            {
                current = current->next;
            }

            delete current->next;
            current->next = nullptr;
        }

        return true;
    }

    void StartGame(const int sizeX, const int sizeY)
    {
        ClearSnake();
        score = 0;
        dirX = 1;
        dirY = 0;

        for (int i = 0; i < sizeX; i++)
        {
            for (int j = 0; j < sizeY; j++)
            {
                board[i][j] = ' ';
            }
        }

        //еда
        foodX = 1 + rand() % (sizeX - 2);
        foodY = 1 + rand() % (sizeY - 2);

        //начальная змейка
        head = new Segment;
        head->x = sizeX / 2;
        head->y = sizeY / 2;

        head->next = new Segment;
        head->next->x = (sizeX / 2) - 1;
        head->next->y = sizeY / 2;

        head->next->next = new Segment;
        head->next->next->x = (sizeX / 2) - 2;
        head->next->next->y = sizeY / 2;
        head->next->next->next = nullptr;
    }

    void PrintBoard(int sizeX, int sizeY)
    {
        //очищаем поле
        for (int y = 0; y < sizeY; y++)
        {
            for (int x = 0; x < sizeX; x++)
            {
                board[y][x] = ' ';
            }
        }

        for (int x = 0; x < sizeX; x++)
        {
            board[0][x] = '#';
            board[sizeY - 1][x] = '#';
        }
        for (int y = 0; y < sizeY; y++)
        {
            board[y][0] = '#';
            board[y][sizeX - 1] = '#';
        }

        board[foodY][foodX] = 'X';


        Segment* current = head;
        while (current != nullptr)
        {
            board[current->y][current->x] = 'O';
            current = current->next;
        }

        std::cout << "Счет: " << score << std::endl;
        std::cout << "W A S D-движение. Q-выход в меню" << std::endl;

        for (int y = 0; y < sizeY; y++)
        {
            for (int x = 0; x < sizeX; x++)
            {
                std::cout << board[y][x] << " ";
            }
            std::cout << std::endl;
        }
    }

    void SaveGame()
    {
        std::ofstream file("save.bin", std::ios::binary);
        if (!file)
        {
            std::cout << "Ошибка сохранения" << std::endl;
            return;
        }

        file.write((char*)&score, sizeof(score));
        file.write((char*)&dirX, sizeof(dirX));
        file.write((char*)&dirY, sizeof(dirY));

        file.write((char*)&foodX, sizeof(foodX));
        file.write((char*)&foodY, sizeof(foodY));

        int length = 0;
        Segment* current = head;
        while (current != nullptr)
        {
            length++;
            current = current->next;
        }
        file.write((char*)&length, sizeof(length));

        current = head;
        while (current != nullptr)
        {
            file.write((char*)&current->x, sizeof(current->x));
            file.write((char*)&current->y, sizeof(current->y));
            current = current->next;
        }

        file.close();
        std::cout << "Игра успешно сохранена" << std::endl;
    }

    //загрузка
    bool LoadGame()
    {
        std::ifstream file("save.bin", std::ios::binary);
        if (!file)
        {
            std::cout << "Файл сохранения не найден" << std::endl;
            Sleep(800);
            return false;
        }

        ClearSnake();

        file.read((char*)&score, sizeof(score));
        file.read((char*)&dirX, sizeof(dirX));
        file.read((char*)&dirY, sizeof(dirY));
        file.read((char*)&foodX, sizeof(foodX));
        file.read((char*)&foodY, sizeof(foodY));

        int length = 0;
        file.read((char*)&length, sizeof(length));

        Segment* last = nullptr;
        for (int i = 0; i < length; i++)
        {
            int segX = 0;
            int segY = 0;
            file.read((char*)&segX, sizeof(segX));
            file.read((char*)&segY, sizeof(segY));

            Segment* newSeg = new Segment;
            newSeg->x = segX;
            newSeg->y = segY;
            newSeg->next = nullptr;

            if (head == nullptr)
            {
                head = newSeg;
                last = newSeg;
            }
            else
            {
                last->next = newSeg;
                last = newSeg;
            }
        }

        file.close();
        return true;
    }
};

void AddRecord(int score)
{
    Record rec;
    std::cout << "Введите ваше имя для таблицы рекордов: ";
    std::cin >> rec.name;
    rec.score = score;

    std::ofstream file("records.bin", std::ios::binary | std::ios::app);
    if (file)
    {
        file.write((char*)&rec, sizeof(Record));
        file.close();
    }
}

void ShowRecords()
{
    system("cls");
    std::ifstream file("records.bin", std::ios::binary);
    if (!file)
    {
        std::cout << "Рекордов пока нет" << std::endl;
    }
    else
    {
        Record rec;
        int i = 1;
        while (file.read((char*)&rec, sizeof(Record)))
        {
            std::cout << i << ". " << rec.name << " - " << rec.score << std::endl;
            i++;
        }
        file.close();
    }

    std::cout << "\nНажмите любую клавишу для выхода в меню";
    _getch();
}

void PlayGame(Snake& snake)
{
    while (true)
    {
        if (_kbhit())
        {
            char key = _getch();

            if ((key == 'w' || key == 'W') && snake.dirY != 1)
            {
                snake.dirX = 0;
                snake.dirY = -1;
            }
            else if ((key == 's' || key == 'S') && snake.dirY != -1)
            {
                snake.dirX = 0;
                snake.dirY = 1;
            }
            else if ((key == 'a' || key == 'A') && snake.dirX != 1)
            {
                snake.dirX = -1;
                snake.dirY = 0;
            }
            else if ((key == 'd' || key == 'D') && snake.dirX != -1)
            {
                snake.dirX = 1;
                snake.dirY = 0;
            }
            else if (key == 'q' || key == 'Q')
            {
                snake.SaveGame();
                break;
            }
        }

        //шаг
        if (!snake.Move(snake.dirX, snake.dirY))
        {
            system("cls");
            std::cout << "Вы проиграли! Итоговый счет: " << snake.score << std::endl;
            AddRecord(snake.score);
            break;
        }

        system("cls");
        snake.PrintBoard(20, 20);

        //задержка движения
        Sleep(200);
    }
}

int main()
{
    srand(time(NULL));
    setlocale(NULL, "Russian");
    

    const int height = 20;
    const int width = 20;
    int choice = 0;

    Snake snake;

    while (true)
    {
        system("cls");
        std::cout << "1. Новая игра" << std::endl;
        std::cout << "2. Загрузить игру" << std::endl;
        std::cout << "3. Таблица рекордов" << std::endl;
        std::cout << "4. Выход" << std::endl;
        std::cout << "Выберите пункт: ";
        std::cin >> choice;

        if (choice == 1)
        {
            snake.StartGame(width, height);
            PlayGame(snake);
        }
        else if (choice == 2)
        {
            if (snake.LoadGame())
            {
                PlayGame(snake);
            }
        }
        else if (choice == 3)
        {
            ShowRecords();
        }
        else if (choice == 4)
        {
            std::cout << "Выход из игры" << std::endl;
            break;
        }
        else
        {
            std::cout << "Некорректный ввод" << std::endl;
        }
    }

    return 0;
}