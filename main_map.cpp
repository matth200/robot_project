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


#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000
#define MAX_TIME_ESC 500
#define COLOR_RED SDL_MapRGB(screen->format, 245,10,10)
#define COLOR_WHITE SDL_MapRGB(screen->format, 255,255,255)
#define COLOR_BLACK SDL_MapRGB(screen->format, 0,0,0)

#define FPS 40.0

struct Line{
    int x1,y1;
    int x2,y2;
};

bool saveDataInFile(const char* filename, vector<Line> &liste1, vector<Line> &liste2);
void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color);
void drawLine(SDL_Surface *screen, Line line, Uint32 color);
void setPixel(SDL_Surface *screen, int x, int y, Uint32 color);


int main(int argc, char **argv){
    cout << "DESSIN DE LA CARTE" << endl;

    vector<Line> carte, carte_red;

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
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
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

    Line line_red;
    line_red.x1 = -1;
    line_red.y1 = -1;
    line_red.x2 = -1;
    line_red.y2 = -1;
    int state_line_red = 0;


    //button
    bool status_esc = 0, status_enter = 0;

    int counter_esc = 0;
    time_point start_point_esc;

    while(continuer){
        start_point = chrono::high_resolution_clock::now();
        //on gere les events
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    continuer = false;
                    break;
                case SDL_MOUSEMOTION:
                    rectMouse.x = event.motion.x;
                    rectMouse.y = event.motion.y;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym==SDLK_RETURN){
                        if(status_enter==0){
                            //on enregistre le fichier
                            if(!saveDataInFile("../resources/map/map.level", carte, carte_red)){
                                cout << "Enregistrement fait avec succes" << endl;
                            }else{
                                cout << "Erreur d'enregistrement du fichier" << endl;
                            }

                            status_enter=1;
                        }
                    }else if(event.key.keysym.sym==SDLK_ESCAPE){
                        if(status_esc==0){
                            //on initialise les line et line_red
                            state_line = 0;
                            line.x1 = -1;
                            line.y1 = -1;
                            line.x2 = -1;
                            line.y2 = -1;

                            state_line_red = 0;
                            line_red.x1 = -1;
                            line_red.y1 = -1;
                            line_red.x2 = -1;
                            line_red.y2 = -1;

                            //on lance le chrono
                            counter_esc++;
                            if(counter_esc-1==0){
                                start_point_esc = chrono::high_resolution_clock::now();
                                //cout << "init time" << endl;
                            }
                            time_point tmp_point = chrono::high_resolution_clock::now();
                            double duration_time = chrono::duration_cast<chrono::milliseconds>(tmp_point-start_point_esc).count();
                            if(duration_time>MAX_TIME_ESC){
                                counter_esc = 1;
                            }
                            //counter_esc valeur correcte

                            //cout << counter_esc << " t: " << duration_time/1000.0 << endl;
                            if(counter_esc==2){
                                carte.clear();
                                carte_red.clear();
                                counter_esc = 0;
                            }

                            //plus de valeur correcte
                            if(duration_time>MAX_TIME_ESC){
                                counter_esc = 0;
                            }

                            status_esc=1;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if(event.key.keysym.sym==SDLK_RETURN){
                        if(status_enter==1){
                            status_enter=0;
                        }
                    }else if(event.key.keysym.sym==SDLK_ESCAPE){
                        if(status_esc==1){
                            status_esc = 0;
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT){
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
                    }else if(event.button.button==SDL_BUTTON_RIGHT){
                         switch(state_line_red){
                            case 0:
                                line_red.x1 = event.motion.x;
                                line_red.y1 = event.motion.y;
                                break;
                            case 1:
                                line_red.x2 = event.motion.x;
                                line_red.y2 = event.motion.y;
                                break;
                        }
                    }
                    break;
                
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT){
                        if(state_line==0&&line.x1!=-1&&line.y1!=-1){
                            state_line++;
                            //cout << "click 1" << endl;
                        }
                        if(state_line==1&&line.x2!=-1&&line.y2!=-1){
                            //on enregistre
                            carte.push_back(Line(line));
                            //cout << "click 2" << endl;

                            //on initialise
                            state_line = 0;
                            line.x1 = -1;
                            line.y1 = -1;
                            line.x2 = -1;
                            line.y2 = -1;
                        }
                    }else if(event.button.button == SDL_BUTTON_RIGHT){
                        if(state_line_red==0&&line_red.x1!=-1&&line_red.y1!=-1){
                            state_line_red++;
                            //cout << "click 1" << endl;
                        }
                        if(state_line_red==1&&line_red.x2!=-1&&line_red.y2!=-1){
                            //on enregistre
                            carte_red.push_back(Line(line_red));
                            //cout << "click 2" << endl;

                            //on initialise
                            state_line_red = 0;
                            line_red.x1 = -1;
                            line_red.y1 = -1;
                            line_red.x2 = -1;
                            line_red.y2 = -1;
                        }
                    }
                    break;
            }
        }
        //affichage
        SDL_FillRect(screen, NULL, COLOR_BLACK);

        //SDL_FillRect(screen, &rectMouse, SDL_MapRGB(screen->format,100,100,100));

        //ligne actuel
        if(line.x1!=-1&&line.y1!=-1){
            drawLine(screen, line.x1, line.y1, rectMouse.x, rectMouse.y, COLOR_WHITE);
        }

        //même chose mais rouge
        if(line_red.x1!=-1&&line_red.y1!=-1){
            drawLine(screen, line_red.x1, line_red.y1, rectMouse.x, rectMouse.y, COLOR_RED);
        }

        //affichage de carte
        for(int i(0);i<carte.size();i++){
            drawLine(screen, carte[i], COLOR_WHITE);
        }

        //même chose mais rouge
        for(int i(0);i<carte_red.size();i++){
            drawLine(screen, carte_red[i], COLOR_RED);
        }

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

bool saveDataInFile(const char* filename, vector<Line> &liste1, vector<Line> &liste2){
    ofstream file(filename, ios::binary);
    if(!file.is_open()){
        return true;
    }

    int cursor = 0;
    int size = liste1.size();
    file.write((const char*)(&size), sizeof(size));
    cursor+=sizeof(size);

    for(int i(0);i<liste1.size();i++){
        Line line(liste1[i]);
        file.write((const char*)(&line), sizeof(line));
        cursor+=sizeof(line);
    }

    size = liste2.size();
    file.write((const char*)(&size), sizeof(size));
    cursor+=sizeof(size);

    for(int i(0);i<liste2.size();i++){
        Line line(liste2[i]);
        file.write((const char*)(&line), sizeof(line));
        cursor+=sizeof(line);
    }


    return false;
}

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color){
    *((Uint32*)(screen->pixels) + x + y * screen->w) = color;
}


void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color){
    int deltaX = x2-x1;
    int deltaY = y2-y1;
    int h = (int)sqrt(deltaX*deltaX + deltaY*deltaY) + 1;
    if(h!=0){
        double rapportX = (double(deltaX)/h),
               rapportY = (double(deltaY)/h);
        for(int i(0);i<h;i++){
            int x = int(x1+rapportX*i),
                y =  int(y1+rapportY*i);
            setPixel(screen, x,y, color);
        }
    }
}
void drawLine(SDL_Surface *screen, Line line, Uint32 color){
    drawLine(screen, line.x1, line.y1, line.x2, line.y2, color);
}