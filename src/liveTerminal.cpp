#include "liveTerminal.h"
using namespace std;

LoadBar::LoadBar(int max, int nbrSymbol):_speed(0),_size(nbrSymbol),_max(max)
{
    _lastTime = chrono::high_resolution_clock::now();
}

void LoadBar::show(int a)
{
	if(a!=0)
  {
    cout << ERASE_LINE;
    time_point time_now = chrono::high_resolution_clock::now();
    //_speed = 1000.0/chrono::duration_cast<chrono::milliseconds>(time_now-_lastTime).count();
		if(chrono::duration_cast<chrono::seconds>(time_now-_lastTime).count()!=0)
			_speed = (double)a/chrono::duration_cast<chrono::seconds>(time_now-_lastTime).count();
		else
			_speed = 1;
    //_lastTime = time_now;
  }

	//proportionalité
	 int b = a/double(_max)*_size;

	//affichage
	cout << LIVE_COLOR_RED << "loading :" << COLOR_NONE << " [";
	for(int i(0);i<_size-1;i++)
	{
		if(i<b)
			cout << "#";
		else
			cout << " ";
	}
	cout << "] (" << a+1 <<"/"<<_max<< ") ";
  //speed
  if(_speed!=0)
    cout << int((_max-a)/_speed)/60 << "m " << int((_max-a)/_speed)%60 << "s restantes";
  cout << endl;
}

double LoadBar::getSpeed() const
{
  return _speed;
}
