#ifndef INCLUDE_LIVE_TERMINAL
#define INCLUDE_LIVE_TERMINAL

#include <iostream>
#include <chrono>

#define ERASE_LINE "\e[2K\r\e[1A"
#define LIVE_COLOR_RED "\e[31m"
#define COLOR_NONE "\e[0m"

typedef std::chrono::high_resolution_clock::time_point time_point;

class LoadBar {
public:
  LoadBar(int max, int size = 30);
  void show(int a);
  double getSpeed() const;
private:
  double _speed;
  int _size, _max;
  time_point _lastTime;
};

#endif
