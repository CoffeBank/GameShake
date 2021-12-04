#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <locale.h>
#include <windows.h>

//введем переменную отвечающие за начало и конец игры
int stopgame;

//определим константы ширины и высоты
const int width = 60;
const int height = 20;

//координаты начального положения головы змейки
int x, y;

//перечисление для константы управления
enum eDirection
{
	STOP = 0, //змейка стоит на месте
	RIGHT,    //двигается в право
	LEFT,     //лего
	UP,       //верх
	DOWN      //вниз
};

//присвоим перечислению переменную dir
enum eDirection dir;

//переменная для обозначения лучшего результата пользователя
int bestscore = 0;

//присвоим координаты положения бонуса
int FX = 0;
int FY = 0;

//координаты хвоста по икс и по игреку в качестве массивов  
int XX[100], YY[100];

//колличество элементов в хвосте
int quantity = 0;

//присвоим значение количеству очков в игре начиная с нуля
int score = 0;

void gotoxy(int x, int y)
{
  static HANDLE h = NULL;  
  if(!h)
    h = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD c = { x, y };  
  SetConsoleCursorPosition(h,c);
}

//функция отвечающая за логику игры
void logic()
{
	//переменная для предыдущего значения икс или игрека (первое звено хвоста)
	//изначально устанавливается на 0 координат
	int prevX = XX[0];
	int prevY = YY[0];
	//второе звено хвоста
	int prev2X, prev2Y;
	//начальную координату установим на положение головы змеи
	XX[0] = x;
	YY[0] = y;
	//алгоритм обработки всего хвоста по координатам змейки
	for (int i = 1; i < quantity; i++)
	{
		prev2X = XX[i];
		prev2Y = YY[i];
		XX[i] = prevX;
		YY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}
	//switch перемещает голову змейки в зависимости от принятой клавиши
	switch (dir)
	{
	case LEFT:
		x--;
		break;
	case RIGHT:
		x++;
		break;
	case UP:
		y--;
		break;
	case DOWN:
		y++;
		break;
	}

	//if (x > width || x < 0 || y > height || y < 0) для режима где змейка погибает об препятствие

	//для прохождения змейки с противоположенной стороны стены при столкновении
	if (x > width - 1)
		x = 0;
	if (x < 0)
		x = width - 1;
	if (y > height-1)
		y = 0;
	if (y < 0)
		y = height; 

	//цикл для прекращения игры в случае попадания головы змейки в область тела
	for (int i = 0; i < quantity; i++)
	{
		if (XX[i] == x && YY[i] == y)
		{
			//остановка основного цикла игры
			stopgame = 1;
			//if для записи в файл нового рекорда, если пользователь его (рекорд) побил
			if (score > bestscore)
			{
				//почти стандартный алгоритм записи в файл "best"
				FILE *fp;
				char name[] = "C:\\mass\\best.txt";
				fp = fopen(name, "r+");
				fseek(fp, 0, SEEK_SET);
				fprintf_s(fp, "%d", score);
				fclose(fp);
			}
			//ставим игру на паузу
			system("PAUSE");
			return 0;
		}
			

	}
	//Если голова будет на координате фрукта, то бонус появится в другом месте 
	if (x == FX && y == FY)
	{
		//случайное переопределение положение координат бонуса
		FX = rand() % width;
		FY = rand() % height;
		//при этом хвост удлиниться на единицу
		quantity++;
		//а количество очков увеличится кратно 10
		score += 10;
		
	}
}

//функция setup включается один раз и определяет все переменные необходимые при запуске игры
void setup()
{
	//считывание с файла значения лучшего результата пользователя
	FILE *fp;
	char name[] = "C:\\mass\\best.txt";
	if ((fp = fopen(name, "r")) == NULL)
	{
		printf("Не удалось открыть файл");
		system("pause");
		return 0;
	}
	fscanf_s(fp, "%d", &bestscore);
	fclose(fp);

	//переменным координатам бонуса присваиваются случайные значения до границ поля
	FX = rand() % width;
	FY = rand() % height;

	//константу обозначаем нулю, чтобы цикл работал
	stopgame = 0;
	//змейка будет стоять до тех пор, пока пользователь не нажмет любую из клавиш управления
	dir = STOP;
	//пусть голова будет расположена на середине поля
	x = width / 2 - 1;
	y = height / 2 - 1;

	//рисуем верхнюю границу 
	for (int i = 0; i <= width+1; i++)
		printf("#");
	printf("\n");
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j <= width; j++)
		{
			if (j == 0 || j == width)
				printf("#");
		}
		printf("\n");
	}
	gotoxy(0,21);
	//строим нижнюю границу
	for (int i = 0; i <= width + 1; i++)
		printf("#");

	//вывод значении нынешних очков и лучшего результата
	//printf("\nscore:%d", score);
	printf("\nbestscore:%d", bestscore);
	gotoxy(0, 1);
}

//функция отвечающая за обновление терминала и его рисовку
void draw()
{
	//строим боковые границы, не включаю первую и последнюю строку
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j <= width; j++)
		{
			/*
			if (j == 0 || j == width)
				printf("#");
			*/
			//рисовка головы змейки
			if (i == y && j == x)
				printf("0");
			//рисовка бонуса на случайной точке
			else if (i == FY && j == FX)
				printf("+");
			else
			{
				//необходимая переменная для того чтобы при рисовании звенья компилятор не ставил пробел
				int plus = 0;
				//алгоритм нарисования звеньей змейки
				for (int k = 0; k < quantity; k++)
				{
					if (XX[k] == j && YY[k] == i)
					{
						printf("o");
						plus=1;
					}
						
				}
				//если змейка не увеличилась, то ставиться пробел
				if (plus != 1)
					printf(" ");
			}
				
		}
		printf("\n");
	}
	gotoxy(0, 1);
}

//функция принимающая значения пользователя
void input()
{
    //цикл if будет возвращать правду или действие в зависимости от того, нажал ли пользователь какую либо клавишу
	//с его помощью змейка будет постоянно двигаться в одном направлении при одном нажатии клавиши
	if (_kbhit())
	{
		//switch определяет какие кнопки принимает терминал для управления игрой
		//getch считывает клавиши
		switch (getch())
		{
		case '4':
			dir = LEFT;
			break;
		case '6':
			dir = RIGHT;
			break;
		case '8':
			dir = UP;
			break;
		case '2':
			dir = DOWN;
			break;
		case 'x':
			stopgame = 1;
			break;
		}
	}
}

//точка входа программы реализована из функции setup определяющая начало игры и большого обновляющегося цикла 
//который будет продолжаться до тех пор, пока не наступит конец игры (stopgame будет равен 1)
int main()
{
	setup();
	while (stopgame != 1)
	{
		draw();
		input();
		logic();
	}
}
