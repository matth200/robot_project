#include <fstream>
#include <iostream>
#include <cmath>

//random librairy
#include <cstdlib>
#include <ctime>

//management time
#include <chrono>
#include <thread>

//SDL
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include <vector>

using namespace std;
typedef chrono::high_resolution_clock::time_point time_point;


#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define OUTSCREEN_W 500

#define FPS 40.0

struct Line{
    int x1,y1;
    int x2,y2;
};

bool saveDataInFile(const char* filename, vector<Line> &liste);
void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2);
void drawLine(SDL_Surface *screen, Line line);
void setPixel(SDL_Surface *screen, int x, int y, Uint32 color);


int main(int argc, char **argv){
    cout << "DESSIN DE LA CARTE" << endl;

    vector<Line> carte;

    srand(time(NULL));

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Erreur de lancement de la sdl: " << SDL_GetError() << endl;
        return 1;
    }

    if( TTF_Init() < 0){
        cout << "Erreur de lancement de SDL_ttf: " << TTF_GetError() << endl;
        return 1;
    }

    atexit(SDL_Quit);
    atexit(TTF_Quit);

    TTF_Font *police = TTF_OpenFont("../resources/fonts/pixel_font.ttf", 16);

    SDL_Surface *screen = NULL;
    screen = SDL_SetVideoMode(SCREEN_WIDTH+OUTSCREEN_W, SCREEN_HEIGHT, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if(screen==NULL){
        cout << "Erreur d'initialisation de la fenetre d'affichage" << endl;
        return 1;
    }

    SDL_WM_SetCaption("DESSIN DE LA MAP",NULL);

    //main loop
    SDL_Event event;
    bool continuer = true;
    time_point start_point, end_point;
    double duration = 0;
    double fpsActuel = 0;

    //on gere la souris
    SDL_Rect rectMouse;
    rectMouse.x = 0;
    rectMouse.y = 0;
    rectMouse.w = 10;
    rectMouse.h = 10;

    //on gere la ligne en cours
    Line line;
    line.x1 = -1;
    line.y1 = -1;
    line.x2 = -1;
    line.y2 = -1;
    int state_line = 0;

    while(continuer){
        start_point = chrono::high_resolution_clock::now();
        //on gere les events
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    continuer = false;
                    break;
                case SDL_MOUSEMOTION:
                    rectMouse.x = event.motion.x-5;
                    rectMouse.y = event.motion.y-5;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    switch(state_line){
                        case 0:
                            line.x1 = event.motion.x;
                            line.y1 = event.motion.y;
                            break;
                        case 1:
                            line.x2 = event.motion.x;
                            line.y2 = event.motion.y;
                            break;
                    }
                    break;
                
                case SDL_MOUSEBUTTONUP:
                    if(state_line==0&&line.x1!=-1&&line.y1!=-1){
                        state_line++;
                        cout << "click 1" << endl;
                    }
                    if(state_line==1&&line.x2!=-1&&line.y2!=-1){
                        //on enregistre
                        carte.push_back(Line(line));
                        cout << "click 2" << endl;

                        //on initialise
                        state_line = 0;
                        line.x1 = -1;
                        line.y1 = -1;
                        line.x2 = -1;
                        line.y2 = -1;
                    }
                    break;
            }
        }
        //affichage
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));

        SDL_FillRect(screen, &rectMouse, SDL_MapRGB(screen->format,100,100,100));

        if(line.x1!=-1&&line.y1!=-1){
            drawLine(screen, line.x1, line.y1, rectMouse.x, rectMouse.y);
        }
        drawLine(screen, 0,0,500,500);

        SDL_Flip(screen);
        //on gere les fps
        end_point = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end_point-start_point).count();
        if(duration<1000.0/FPS){
            this_thread::sleep_for(chrono::milliseconds((unsigned int)(1000.0/FPS-duration)));
        }
        fpsActuel = 1000.0/(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()-start_point)).count();
    }

    TTF_CloseFont(police);
    return 0;
}

bool saveDataInFile(const char* filename, vector<Line> &liste){
    ofstream file("resources/map/carte.level", ios::binary);

    int cursor = 0;
    int size = liste.size();
    file.write((const char*)(&size), sizeof(size));
    cursor+=sizeof(size);

    for(int i(0);i<liste.size();i++){
        Line line(liste[i]);
        file.write((const char*)(&line), sizeof(line));
        cursor+=sizeof(Line);
    }
    return false;
}

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color){
    *((Uint32*)(screen->pixels) + x + y * screen->w) = color;
}


void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2){
    int deltaX = x2-x1;
    int deltaY = y2-y1;
    int h = (int)sqrt(deltaX*deltaX + deltaY*deltaY) + 1;
    if(h!=0){
        double rapportX = (double(deltaX)/h),
               rapportY = (double(deltaY)/h);
        for(int i(0);i<h;i++){
            int x = int(x1+rapportX*i),
                y =  int(y1+rapportY*i);
            setPixel(screen, x,y, SDL_MapRGB(screen->format, 255,255,255));
        }
    }
}
void drawLine(SDL_Surface *screen, Line line){
    drawLine(screen, line.x1, line.y1, line.x2, line.y2);
}