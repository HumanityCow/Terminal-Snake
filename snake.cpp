#include <algorithm>
#include <chrono>
#include <cstring>
#include <deque>
#include <iostream>
#include <thread>

class Segment {
private:
  int row;
  int col;

public:
  Segment(int r, int c) {
    row = r;
    col = c;
  }

  void setRow(int r) { row = r; }

  void setCol(int c) { col = c; }

  int getRow() { return row; }

  int getCol() { return col; }

  int getPos() { return row * 60 + col; }
};

class Apple {
private:
  int pos;

public:
  Apple(std::deque<Segment> snake) { pos = generatePos(snake); }

  int generatePos(std::deque<Segment> snake) {
    do {
      pos = rand() % 30 * 60 + rand() % 60;
    } while (std::any_of(snake.begin(), snake.end(),
                         [this](Segment s) { return pos == s.getPos(); }));
    return pos;
  }

  int getPos() { return pos; }
};

void drawGame(std::deque<Segment> snake, Apple apple, int applesEaten) {
  int pos = 0;

  std::cout << "W - Up\t\tS - Down\tA - Left\tD - Right" << "\r\n";
  std::cout << " IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII "
            << "\r\n";
  for (int row = 0; row < 30; row++) {
    std::cout << '|';
    for (int col = 0; col < 60; col++) {
      if (std::any_of(snake.begin(), snake.end(),
                      [=](Segment s) { return pos == s.getPos(); }))
        std::cout << 'X';
      else if (pos == apple.getPos())
        std::cout << 'O';
      else
        std::cout << ' ';
      pos++;
    }
    std::cout << '|' << "\r\n";
  }
  std::cout << " IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII "
            << "\r\n";
  std::cout << "Apples Eaten: " << applesEaten << "\r\n";
}

int prevDirection;

void getDirection(int &direction) {
  char input;

  while (true) {
    input = getchar();
    std::cout << "\b \b";

    switch (input) {
    case 'w':
      if (prevDirection != 2)
        direction = 0;
      break;

    case 'a':
      if (prevDirection != 3)
        direction = 1;
      break;

    case 's':
      if (prevDirection != 0)
        direction = 2;
      break;

    case 'd':
      if (prevDirection != 1)
        direction = 3;
    }
  }
}

std::deque<Segment> moveSnake(std::deque<Segment> snake, int direction) {
  prevDirection = direction;

  for (int i = snake.size() - 1; i > 0; i--) {
    snake[i].setRow(snake[i - 1].getRow());
    snake[i].setCol(snake[i - 1].getCol());
  }
  switch (direction) {
  case 0:
    snake[0].setRow(snake[0].getRow() - 1);
    break;

  case 1:
    snake[0].setCol(snake[0].getCol() - 1);
    break;

  case 2:
    snake[0].setRow(snake[0].getRow() + 1);
    break;

  case 3:
    snake[0].setCol(snake[0].getCol() + 1);
  }
  return snake;
}

bool isAlive(std::deque<Segment> snake) {
  if (snake[0].getRow() > 29 || snake[0].getRow() < 0 ||
      snake[0].getCol() > 59 || snake[0].getCol() < 0)
    return false;
  else
    for (int i = 1; i < snake.size(); i++) {
      if (snake[i].getPos() == snake[0].getPos())
        return false;
    }
  return true;
}

int main() {

  using clock = std::chrono::steady_clock;
  using frames = std::chrono::duration<int, std::ratio<1, 10>>;

  srand((unsigned)time(NULL));

  std::deque<Segment> snake;
  snake.push_back(Segment(14, 29));
  Apple apple(snake);
  int applesEaten = 0;
  bool appleConsumed = false;
  int direction = 2;
  int temp[2];
  int lastSegmentPos[2];

  std::cout << "\e[?25l";

  std::thread input_thread(getDirection, std::ref(direction));
  input_thread.detach();

  system("stty raw");

  while (isAlive(snake)) {
    auto start = clock::now();

    system("clear");
    drawGame(snake, apple, applesEaten);
    if (snake.at(0).getPos() == apple.getPos()) {
      apple.generatePos(snake);
      applesEaten++;
      appleConsumed = true;
      temp[0] = snake.back().getRow();
      temp[1] = snake.back().getCol();
      memcpy(lastSegmentPos, temp, 2 * sizeof(*temp));
    }
    snake = moveSnake(snake, direction);
    if (appleConsumed) {
      snake.push_back(Segment(lastSegmentPos[0], lastSegmentPos[1]));
      appleConsumed = false;
    }

    auto finish = clock::now();
    if (finish - start < frames(1))
      std::this_thread::sleep_until(start + frames(1));
  }

  system("stty cooked");
  std::cout << "\e[?25h";
  return 0;
}
