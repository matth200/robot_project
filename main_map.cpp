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
#define COLOR_GREEN SDL_MapRGB(screen->format, 10,220,10)
#define COLOR_PURPLE SDL_MapRGB(screen->format, 230, 10, 230)

#define FPS 40.0

struct Line{
    int x1,y1;
    int x2,y2;
};

struct LineD{
    double x1, y1;
    double x2, y2;
};

struct Pos{
    int x, y;
};

bool saveDataInFile(const char* filename, vector<Line> &liste1, vector<Line> &liste2, vector<Line> &liste3, vector<Pos> &points);
bool loadDataInFile(const char *filename, vector<Line> &liste1, vector<Line> &liste2, vector<Line> &liste3, vector<Pos> &points);
void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color);
void drawLine(SDL_Surface *screen, Line line, Uint32 color);
void setPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawCross(SDL_Surface *screen, int x, int y, Uint32 color);


int main(int argc, char **argv){
    cout << "DESSIN DE LA CARTE" << endl;

    vector<Line> carte, carte_red, carte_green;

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


    //on gere les positions
    vector<Pos> points;


    if(argc==2){
        cout << "On récupére une ancienne carte à " << argv[1] << endl;
        if(loadDataInFile(argv[1], carte, carte_red, carte_green, points)){
            cout << "Chargé avec succées" << endl;
        }else{
            cout << "Erreur le fichier n'existe pas ou n'est pas compatible..." << endl;
        }
    }

    //on gere la vue
    Pos viewPos;
    viewPos.x = 0;
    viewPos.y = 0;


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

    Line line_green;
    line_green.x1 = -1;
    line_green.y1 = -1;
    line_green.x2 = -1;
    line_green.y2 = -1;
    int state_line_green = 0;

    //button
    bool status_esc = 0, status_enter = 0, state_space  = 0, state_u = 0;
    bool status_ctrl = false;

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
                    if(status_ctrl){
                        viewPos.x += event.motion.x - rectMouse.x;
                        viewPos.y += event.motion.y - rectMouse.y;
                        if(viewPos.x>0){
                            viewPos.x = 0;
                        }
                        if(viewPos.y>0){
                            viewPos.y = 0;
                        }
                    }
                    rectMouse.x = event.motion.x;
                    rectMouse.y = event.motion.y;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym==SDLK_m){
                        //on initialise la vue
                        viewPos.x = 0;
                        viewPos.y = 0;
                    }else if(event.key.keysym.sym==SDLK_RETURN){
                        if(status_enter==0){
                            //on enregistre le fichier
                            if(saveDataInFile("../resources/map/map.level", carte, carte_red, carte_green, points)){
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

                            state_line_green = 0;
                            line_green.x1 = -1;
                            line_green.y1 = -1;
                            line_green.x2 = -1;
                            line_green.y2 = -1;

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
                                carte_green.clear();
                                points.clear();
                                counter_esc = 0;
                            }

                            //plus de valeur correcte
                            if(duration_time>MAX_TIME_ESC){
                                counter_esc = 0;
                            }

                            status_esc=1;
                        }
                    }else if(event.key.keysym.sym==SDLK_SPACE){
                        state_space=1;
                    }else if(event.key.keysym.sym==SDLK_u){
                        state_u = 1;
                    }else if(event.key.keysym.sym==SDLK_LCTRL){
                        if(!status_ctrl){
                            status_ctrl = true;
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
                    }else if(event.key.keysym.sym==SDLK_SPACE){
                        state_space = 0;
                    }else if(event.key.keysym.sym==SDLK_u){
                        state_u = 0;
                    }
                    else if(event.key.keysym.sym==SDLK_LCTRL){
                        status_ctrl = false;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT){
                        //line green
                        if(state_space){
                            switch(state_line_green){
                                case 0:
                                    line_green.x1 = event.motion.x-viewPos.x;
                                    line_green.y1 = event.motion.y-viewPos.y;
                                    break;
                                case 1:
                                    line_green.x2 = event.motion.x-viewPos.x;
                                    line_green.y2 = event.motion.y-viewPos.y;
                                    break;
                            }
                        //line white
                        }else if(state_u){
                            Pos pos;
                            pos.x = event.motion.x-viewPos.x;
                            pos.y = event.motion.y-viewPos.y;
                            points.push_back(pos);
                            state_u = 0;
                        }
                        else{
                            switch(state_line){
                                case 0:
                                    line.x1 = event.motion.x-viewPos.x;
                                    line.y1 = event.motion.y-viewPos.y;
                                    break;
                                case 1:
                                    line.x2 = event.motion.x-viewPos.x;
                                    line.y2 = event.motion.y-viewPos.y;
                                    break;
                            }
                        }
                    }else if(event.button.button==SDL_BUTTON_RIGHT){
                         switch(state_line_red){
                            case 0:
                                line_red.x1 = event.motion.x-viewPos.x;
                                line_red.y1 = event.motion.y-viewPos.y;
                                break;
                            case 1:
                                line_red.x2 = event.motion.x-viewPos.x;
                                line_red.y2 = event.motion.y-viewPos.y;
                                break;
                        }
                    }
                    break;
                
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT){
                        //white line
                        if(!state_space&&state_line==0&&line.x1!=-1&&line.y1!=-1){
                            state_line++;
                            //cout << "click 1" << endl;
                        }
                        if(!state_space&&state_line==1&&line.x2!=-1&&line.y2!=-1){
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

                        //green_line
                        if(state_space&&state_line_green==0&&line_green.x1!=-1&&line_green.y1!=-1){
                            state_line_green++;
                            //cout << "click 1" << endl;
                        }
                        if(state_space&&state_line_green==1&&line_green.x2!=-1&&line_green.y2!=-1){
                            //on enregistre
                            carte_green.push_back(Line(line_green));
                            //cout << "click 2" << endl;

                            //on initialise
                            state_line_green = 0;
                            line_green.x1 = -1;
                            line_green.y1 = -1;
                            line_green.x2 = -1;
                            line_green.y2 = -1;
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
        //drawCross(screen, rectMouse.x, rectMouse.y, COLOR_PURPLE);

        //ligne actuel
        if(line.x1!=-1&&line.y1!=-1){
            drawLine(screen, line.x1+viewPos.x, line.y1+viewPos.y, rectMouse.x, rectMouse.y, COLOR_WHITE);
        }

        //même chose mais rouge
        if(line_red.x1!=-1&&line_red.y1!=-1){
            drawLine(screen, line_red.x1+viewPos.x, line_red.y1+viewPos.y, rectMouse.x, rectMouse.y, COLOR_RED);
        }

        //même chose mais vert
        if(line_green.x1!=-1&&line_green.y1!=-1){
            drawLine(screen, line_green.x1+viewPos.x, line_green.y1+viewPos.y, rectMouse.x, rectMouse.y, COLOR_GREEN);
        }

        //affichage de carte
        for(int i(0);i<carte.size();i++){
            Line line = carte[i];

            drawLine(screen, line.x1+viewPos.x, line.y1+viewPos.y, line.x2+viewPos.x, line.y2+viewPos.y, COLOR_WHITE);
        }

        //même chose mais rouge
        for(int i(0);i<carte_red.size();i++){
            Line line = carte_red[i];
            drawLine(screen, line.x1+viewPos.x, line.y1+viewPos.y, line.x2+viewPos.x, line.y2+viewPos.y, COLOR_RED);
        }

        //même chose mais vert
        for(int i(0);i<carte_green.size();i++){
            Line line = carte_green[i];
            drawLine(screen, line.x1+viewPos.x, line.y1+viewPos.y, line.x2+viewPos.x, line.y2+viewPos.y, COLOR_GREEN);
        }

        //affichage des points
        for(int i(0);i<points.size();i++){
            Pos pos = points[i];
            drawCross(screen, pos.x+viewPos.x, pos.y+viewPos.y, COLOR_PURPLE);
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

bool loadDataInFile(const char *filename, vector<Line> &liste1, vector<Line> &liste2, vector<Line> &liste3, vector<Pos> &points){
     ifstream file(filename, ios::binary);
    if(!file.is_open()){
        return false;
    }

    Line line;
    int size=0;
    //white lines
    file.read((char*)&size,sizeof(size));
    for(int i(0);i<size;i++){
        file.read((char*)&line, sizeof(line));
        liste1.push_back(Line(line));
    }
    //red lines
    file.read((char*)&size, sizeof(size));
    for(int i(0);i<size;i++){
        file.read((char*)&line, sizeof(line));
        liste2.push_back(Line(line));
    }
    //green lines
    file.read((char*)&size, sizeof(size));
    for(int i(0);i<size;i++){
        file.read((char*)&line, sizeof(line));
        liste3.push_back(Line(line));
    }
    //purple points
    file.read((char*)&size, sizeof(size));
    Pos pos;
    for(int i(0);i<size;i++){
        file.read((char*)&pos, sizeof(pos));
        points.push_back(Pos(pos));
    }
    return true;
}

bool saveDataInFile(const char* filename, vector<Line> &liste1, vector<Line> &liste2, vector<Line> &liste3, vector<Pos> &points){
    ofstream file(filename, ios::binary);
    if(!file.is_open()){
        return false;
    }

    //white lines
    int cursor = 0;
    int size = liste1.size();
    file.write((const char*)(&size), sizeof(size));
    cursor+=sizeof(size);
    for(int i(0);i<liste1.size();i++){
        Line line(liste1[i]);
        file.write((const char*)(&line), sizeof(line));
        cursor+=sizeof(line);
    }

    //red lines
    size = liste2.size();
    file.write((const char*)(&size), sizeof(size));
    cursor+=sizeof(size);
    for(int i(0);i<liste2.size();i++){
        Line line(liste2[i]);
        file.write((const char*)(&line), sizeof(line));
        cursor+=sizeof(line);
    } 

    //green lines
    size = liste3.size();
    file.write((const char*)(&size), sizeof(size));
    cursor+=sizeof(size);

    for(int i(0);i<liste3.size();i++){
        Line line(liste3[i]);
        file.write((const char*)(&line), sizeof(line));
        cursor+=sizeof(line);
    } 
    //purple points
    size = points.size();
    file.write((const char*)(&size), sizeof(size));
    cursor+=sizeof(size);

    for(int i(0);i<points.size();i++){
        Pos pos(points[i]);
        file.write((const char*)(&pos), sizeof(pos));
        cursor+=sizeof(pos);
    }
    return true;
}

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color){
    if(screen->w>x&&x>=0&&screen->h>y&&y>=0){
        *((Uint32*)(screen->pixels) + x + y * screen->w) = color;
    }
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

void drawCross(SDL_Surface *screen, int x, int y, Uint32 color){
    Pos pos;
    pos.x = x;
    pos.y = y;
    Line line1, line2;
    line1.x1 = pos.x-10;
    line1.y1 = pos.y-10;
    line1.x2 = pos.x+10;
    line1.y2 = pos.y+10;

    line2.x1 = pos.x+10;
    line2.y1 = pos.y-10;
    line2.x2 = pos.x-10;
    line2.y2 = pos.y+10;

    drawLine(screen, line1, color);
    drawLine(screen, line2, color);
}