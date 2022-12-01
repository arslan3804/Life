#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <windows.h>

const int W = 1500;
const int H = 1000;
const int n = 100;

const int PredatorReproductiveAge = 20;// количество процентов
const int HerbivoreReproductiveAge = 10;

const int PredatorOldAge = 100;
const int HerbivoreOldAge = 50;

const int GrassSpawnRate = 10;
const int PredatorSpawnRate = 1;
const int HerbivoreSpawnRate = 5;

const int GrassGrowingRate = 1;
const int PredatorBirthRate = 3;
const int HerbivoreBirthRate = 10;

const int PredatorHungerResistance = 35;
const int HerbivoreHungerResistance = 5;

const int DesertTime = -100;
const int DesertSpawnRate = 1;//только здесь сотая доля процента
const int DesertGrowingRate = 1;

int day = 0;
int predator_count = 0;
int herbivore_count = 0;

using namespace sf;
using namespace std;

class Animal {
	int type;
	int age;
	int hungry_days;
	bool just_went;
	bool will_die;


public:
	Animal() {
		type = 0;
		age = -1;
		hungry_days = -1;
		just_went = 0;
		will_die = 0;
	}
	Animal(int value) {
		type = value;
		if (type > 0) {
			age = 0;
			hungry_days = 0;
			just_went = 0;
			will_die = 0;
		}
		else {
			age = -1;
			hungry_days = -1;
			just_went = 0;
			will_die = 0;
		}
	}
	int GetType() {
		return type;
	}
	bool IsWent() {
		return just_went;
	}
	void Went() {
		just_went = 1;
	}
	void NextDay() {
		just_went = 0;
	}
	void Eat() {
		hungry_days = 0;
	}

	void Hungry() {
		hungry_days++;
	}
	int HungryDays() {
		return hungry_days;
	}
	int GetAge() {
		return age;
	}
	void Aging() {
		age++;
	}
	int IsDying() {
		return will_die;
	}
	void WillDie() {
		will_die = 1;
	}
};


void HerbivoreLife(Animal coords[n][n], int grass[n][n], int i, int j, int x, int y) {
	coords[i][j].Hungry();//животное голодает еще один день	
	if (grass[x][y] == 1) {
		coords[i][j].Eat();
		grass[x][y] = 0;
	}
	else if (coords[i][j].HungryDays() > HerbivoreHungerResistance) {//если животное слишком долго голодало, оно умирает
		coords[i][j] = Animal();
		herbivore_count--;
		return;
	}

	if (x != i or y != j) { // если рядом с животным есть свободное место, животное туда переходит и трава в этом месте исчезает
		coords[x][y] = coords[i][j];
		coords[i][j] = Animal();

		int r = rand() % 100; // если животное не слишком молодое и не слишком старое, то с шансом HerbivoreBirthRate рождается травоядное
		if (r < HerbivoreBirthRate and coords[x][y].GetAge() >= HerbivoreReproductiveAge and coords[x][y].GetAge() < HerbivoreOldAge) {
			coords[i][j] = Animal(1);
			herbivore_count++;
		}
	}
	coords[x][y].Went();//животное отмечается, что оно уже ходило в этом дне
	coords[x][y].Aging();//животное стареет на день
}

void PredatorLife(Animal coords[n][n], int i, int j, int x, int y) {
	coords[i][j].Hungry();//хищник голодает день

	if (coords[x][y].GetType() == 1) {
		coords[i][j].Eat();
		herbivore_count--;
	}
	
	else if (coords[i][j].HungryDays() > PredatorHungerResistance) { // если хищник долго голодал, он умирает
		coords[i][j] = Animal();
		predator_count--;
		return;
	}

	if (x != i or y != j) {//если у хищника есть рядом место, он переходит туда
		coords[x][y] = coords[i][j];
		coords[i][j] = Animal();

		int r = rand() % 100;// если хищник не слишком молодой и не слишком старый, то с шансом PredatorBirthRate рождается хищник
		if (r < PredatorBirthRate and coords[x][y].GetAge() >= PredatorReproductiveAge and coords[x][y].GetAge() < PredatorOldAge) {
			coords[i][j] = Animal(2);
			predator_count++;
		}
	}
	coords[x][y].Went();//хищник отмечается, что он уже ходил в этом дне
	coords[x][y].Aging();//хищник стареет на день
}


void Herbivore(RenderWindow& window, int x, int y) { //рисовка травоядного
	CircleShape circle(5.f);
	circle.setFillColor(Color(50, 50, 50));
	circle.setPosition(x, y);
	window.draw(circle);
}

void Predator(RenderWindow& window, int x, int y) {//рисовка хищника
	CircleShape circle(5.f);
	circle.setFillColor(Color::Red);
	circle.setPosition(x, y);
	window.draw(circle);
}

void Grass(RenderWindow& window, int x, int y) {//рисовка травы
	RectangleShape rectangle(Vector2f(9.f, 9.f));
	rectangle.setFillColor(Color::Green);
	rectangle.setPosition(x, y);
	window.draw(rectangle);
}

void Desert(RenderWindow& window, int x, int y) {//рисовка травы
	RectangleShape rectangle(Vector2f(9.f, 9.f));
	rectangle.setFillColor(Color::Yellow);
	rectangle.setPosition(x, y);
	window.draw(rectangle);
}

void Wall(RenderWindow& window, int x, int y) {//рисовка стены
	RectangleShape rectangle(Vector2f(9.f, 9.f));
	rectangle.setFillColor(Color::Blue);
	rectangle.setPosition(x, y);
	window.draw(rectangle);
}

pair<int, int> random_coords(Animal coords[n][n], int x, int y) {// выбирается случайная свободная клетка рядом, если такой нет, то возвращает клетку x y
	vector <pair<int, int>> places;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (coords[x + i][y + j].GetType() == 0) {
				places.push_back(make_pair(x + i, y + j));
			}
		}
	}
	if (places.size() == 0) {
		return make_pair(x, y);
	}
	int r = rand() % places.size();
	return places[r];
}

pair<int, int> random_grass_coords(Animal coords[n][n], int grass[n][n], int x, int y) {
	vector <pair<int, int>> places;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (coords[x + i][y + j].GetType() == 0 and grass[x + i][y + j] == 1) {
				places.push_back(make_pair(x + i, y + j));
			}
		}
	}
	if (places.size() == 0) {
		return make_pair(x, y);
	}
	int r = rand() % places.size();
	return places[r];
}

pair<int, int> random_herbivore_coords(Animal coords[n][n], int x, int y) {
	vector <pair<int, int>> places;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (coords[x + i][y + j].GetType() == 1 and coords[x + i][y + j].IsWent() == 0) {
				places.push_back(make_pair(x + i, y + j));
			}
		}
	}
	if (places.size() == 0) {
		return make_pair(x, y);
	}
	int r = rand() % places.size();
	return places[r];
}

pair<int, int> random_herbivore_die_coords(Animal coords[n][n], int x, int y) {
	vector <pair<int, int>> places;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (coords[x + i][y + j].GetType() == 1) {
				places.push_back(make_pair(x + i, y + j));
				coords[x + i][y + j].WillDie();
			}
		}
	}
	if (places.size() == 0) {
		return make_pair(x, y);
	}
	int r = rand() % places.size();
	return places[r];
}

pair<int, int> random_predator_coords(Animal coords[n][n], int x0, int y0, int x, int y) {//рядом с определенным травоядным выбирается хищник, который еще не ходил и который съест это травоядное
	vector <pair<int, int>> places;
	places.push_back(make_pair(x0, y0));
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if ((i != 0 or j != 0) and coords[x + i][y + j].GetType() == 2 and coords[x + i][y + j].IsWent() == 0) {

				pair<int, int>herbivore_die = random_herbivore_die_coords(coords, x + i, y + j);
				if (herbivore_die.first == x and herbivore_die.second == y) {
					places.push_back(make_pair(x + i, y + j));
				}
			}
		}
	}
	int r = rand() % places.size();
	return places[r];
}

pair<int, int> random_desert_growing(Animal coords[n][n], int grass[n][n], int x, int y) {//рядом с пустынным блоком выбирается место для распространения пустыни
	vector <pair<int, int>> places;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (grass[x + i][y + j] != 2 and coords[x + i][y + j].GetType() != -1) {
				places.push_back(make_pair(x + i, y + j));
			}
		}
	}
	if (places.size() == 0) {
		return make_pair(x, y);
	}
	int r = rand() % places.size();
	return places[r];
}

int main() {
	srand(time(0));
	setlocale(LC_ALL, "RUS");
	RenderWindow window(VideoMode(W, H), "Life");
	window.clear(Color::White);

	Animal coords[n][n];//поле с четырьмя типами объектов: стена, пустота(здесь может находиться трава), травоядное, хищник(здесь тоже может находиться трава) 
	int grass[n][n]{};//поле с координатами травы
	for (int i = 0; i < n; i++) { //инициализация объектов
		for (int j = 0; j < n; j++) {
			if (i == 0 or j == 0 or i == n - 1 or j == n - 1) {
				coords[i][j] = Animal(-1); // стена
			}
			else {
				int r = rand() % 100;
				if (r < HerbivoreSpawnRate) {
					coords[i][j] = Animal(1); //травоядное с шансом HerbivoreBirthRate
					herbivore_count++;
				}
				if (r > 99 - GrassSpawnRate) {
					grass[i][j] = 1; //трава с шансом GrassGrowingRate
				}
				r = rand() % 100;
				if (r < PredatorSpawnRate) {
					coords[i][j] = Animal(2); //хищник с шансом PredatorBirthRate
					predator_count++;
				}
			}
		}
	}
	bool play1 = 0;
	bool play2 = 0;

	Font font;
	font.loadFromFile("Times New Roman.ttf");
	Text text("", font, 30);
	text.setFillColor(Color::Black);
	text.setStyle(Text::Bold);
	text.setString(L"День: " + to_string(day) + L"\nХищники: " + to_string(predator_count) + L"\nТравоядные: " + to_string(herbivore_count));
	text.setPosition(1000, 100);

	while (window.isOpen()) {
		Event event;
		window.clear(Color::White);
		play2 = 0;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			if (event.type == Event::MouseButtonPressed) {
				if (play1 == 0) {
					play1 = 1;
				}
				else {
					play1 = 0;
				}
			}
			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Space) {
					play2 = 1;
				}
			}
		}
		if (play1 or play2) {
			window.clear(Color::White);
			if (day % 3 == 0) {
				for (int i = 1; i < n - 1; i++) { // жизнь травоядных
					for (int j = 1; j < n - 1; j++) {
						if (coords[i][j].GetType() == 1 and coords[i][j].IsWent() == 0) {//проверяем, чтобы тип животного равен 1(травоядное) и еще не ходило

							bool is_grass_around = 1;
							bool have_eaten = 0;
							while (is_grass_around == 1 and have_eaten == 0) {
								is_grass_around = 0;
								pair<int, int> grass_coords = random_grass_coords(coords, grass, i, j);
								int x1 = grass_coords.first, y1 = grass_coords.second;
								if (x1 != i or y1 != j) {
									is_grass_around = 1;
									pair<int, int> herbivore_coords = random_herbivore_coords(coords, x1, y1);
									int x2 = herbivore_coords.first, y2 = herbivore_coords.second;
									if (x2 == i and y2 == j) {
										have_eaten = 1;
									}
									HerbivoreLife(coords, grass, x2, y2, x1, y1);
								}
							}
							if (have_eaten == 0) {
								pair<int, int> move = random_coords(coords, i, j);//травоядное выбирает случайную клетку, чтобы туда пойти
								int x = move.first, y = move.second;
								HerbivoreLife(coords, grass, i, j, x, y);
							}

						}

						else if (coords[i][j].GetType() == 2) { //все хищники переходят на новый день
							coords[i][j].NextDay();
						}
					}
				}
			}

			else if (day % 3 == 1) {
				for (int i = 1; i < n - 1; i++) { // жизнь хищников
					for (int j = 1; j < n - 1; j++) {
						if (coords[i][j].GetType() == 2 and coords[i][j].IsWent() == 0) {//проверяем, чтобы тип животного равен 2(хищник) и еще не ходило

							bool is_herbivore_around = 1;
							bool have_eaten = 0;
							while (is_herbivore_around == 1 and have_eaten == 0) {
								is_herbivore_around = 0;
								pair<int, int> herbivore_coords = random_herbivore_die_coords(coords, i, j);
								int x1 = herbivore_coords.first, y1 = herbivore_coords.second;
								if (x1 != i or y1 != j) {
									is_herbivore_around = 1;
									pair<int, int> predator_coords = random_predator_coords(coords, i, j, x1, y1);
									int x2 = predator_coords.first, y2 = predator_coords.second;
									if (x2 == i and y2 == j) {
										have_eaten = 1;
									}
									PredatorLife(coords, x2, y2, x1, y1);
								}
							}
							if (have_eaten == 0) {
								pair<int, int> move = random_coords(coords, i, j);//хищник выбирает случайную клетку, чтобы туда пойти
								int x = move.first, y = move.second;
								PredatorLife(coords, i, j, x, y);
							}
						}
						else if (coords[i][j].GetType() == 1) {// а все травоядные переходят на новый день
							coords[i][j].NextDay();
						}

					}
				}
				for (int i = 1; i < n - 1; i++) {//если травоядное находилось рядом с хищником, то оно умрет
					for (int j = 1; j < n - 1; j++) {
						if (coords[i][j].IsDying() == 1) {
							coords[i][j] = Animal();
							herbivore_count--;
						}
					}
				}
			}

			else {
				for (int i = 1; i < n - 1; i++) { // случайное появление травы с шансом GrassSpawnRate
					for (int j = 1; j < n - 1; j++) {
						if (coords[i][j].GetType() != 1 and grass[i][j] <= 0) {//проверяем, чтобы здесь нет травы и что в этом месте не находится травоядное
							int r = rand() % 100;// с шансом в GrassGrowingRate на этом месте появляется трава
							if (r < GrassGrowingRate) {
								grass[i][j] = 1;
							}
						}
						if (grass[i][j] <= 0) {// если здесь не растет трава, то уменьшаем счетчик дней, сколько не росла трава
							grass[i][j]--;
							if (grass[i][j] < DesertTime) { // если трава не растет больше, чем DesertTime, то с шансом DesertSpawnRate на этом месте образуется пустыня
								int r = rand() % 10000;
								if (r < DesertSpawnRate) {
									grass[i][j] = 2;
								}
							}
						}
						else if (grass[i][j] == 2) {// если здесь уже пустыня
							int r = rand() % 100;
							if (r < DesertGrowingRate) {//то с шансом DesertGrowing Rate рядом образуется еще одна пустыня
								pair<int, int> move = random_desert_growing(coords, grass, i, j);
								int x = move.first, y = move.second;
								if (x != i or y != j) {
									grass[x][y] = 2;
								}
							}
						}
					}
				}
			}
			day++;

		}

	for (int i = 0; i < n; i++) {// рисуется трава, стены, травоядные и хищники
		for (int j = 0; j < n; j++) {
			int is_grass = grass[i][j];
			if (is_grass == 1) {
				Grass(window, i * 10, j * 10);
			}
			else if (is_grass == 2) {
				Desert(window, i * 10, j * 10);
			}
			int type = coords[i][j].GetType();
			if (type == -1) {
				Wall(window, i * 10, j * 10);
			}
			if (type == 1) {
				Herbivore(window, i * 10, j * 10);
			}
			if (type == 2) {
				Predator(window, i * 10, j * 10);
			}
		}
	}
			
		text.setString(L"День: " + to_string(day / 3) + L"\nХищники: " + to_string(predator_count) + L"\nТравоядные: " + to_string(herbivore_count));
		window.draw(text);
		window.display();		
	}	
	
	return 0;
}


