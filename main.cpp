#include "common.h"
#include "cmath"
#include "vector"

bool Init();
void CleanUp();
void Run();
void Draw();
void Connect(int offSet, int i, int j, vector<vector<double>> pps);
void Rots();
void Setup();
void ProjDs();
vector<vector<double>> MultMatrixs(vector<vector<double>> mat1, vector<vector<double>> mat2);
double ScaleNum(double n, double minN, double maxN, double min, double max);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int screenWidth = 500;
int screenHeight = 500;
int cornerSize = 5;
int lineSize = 3;
double ang = 0;
double dist = 2;
double w;

vector<vector<double>> points;
vector<vector<double>> rotx;
vector<vector<double>> roty;
vector<vector<double>> rotz;
vector<vector<double>> rotxy;
vector<vector<double>> rotzz;
vector<vector<double>> rotxz;
vector<vector<double>> projection2D;
vector<vector<double>> projection3D;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));
    
    Setup();
    Rots();

    while (gameLoop)
    {   
        Draw();
        SDL_RenderPresent(renderer);
        ang+=.01;
        pos.x = 0;
        pos.y = 0;
        pos.w = screenWidth;
        pos.h = screenHeight;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &pos);
        
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    default:
                        break;
                }
            }
        }
    }
}

void Draw(){
    vector<vector<double>> ppps;
    vector<vector<double>> pps;
    vector<vector<double>> xyz;
    vector<vector<double>> xyzz;
    vector<double> temp;
    Rots();
    for(int i = 0; i < points.size(); i++){
        xyzz.clear();
        temp.clear();
        for(int j = 0; j < points[i].size(); j++){
            temp.push_back(points[i][j]);
            xyzz.push_back(temp);
            temp.clear();
        }
        vector<vector<double>> rotated = MultMatrixs(rotxy, xyzz);
        //rotated = MultMatrixs(rotzz, rotated);
        rotated = MultMatrixs(rotxz, rotated);
        w = 1 - (dist / rotated[rotated.size() - 1][0]);
        ProjDs();
        vector<vector<double>> xyzs = MultMatrixs(projection3D, rotated);
        for(int j = 0; j < xyzs.size(); j++){
            temp.push_back(xyzs[j][0]);
        }
        ppps.push_back(temp);

        xyz.clear();
        temp.clear();
        for(int j = 0; j < ppps[i].size(); j++){
            temp.push_back(ppps[i][j]);
            xyz.push_back(temp);
            temp.clear();
        }
        //vector<vector<double>>rotated = MultMatrixs(roty, xyz);
        //rotated = MultMatrixs(rotx, rotated);
        // rotated = MultMatrixs(rotz, rotated);
        vector<vector<double>> xys = MultMatrixs(projection2D, xyz);

        for(int j = 0; j < xys.size(); j++){
            temp.push_back(xys[j][0]);
        }
        pps.push_back(temp);

        pos.x = pps[i][0] * 10 + screenWidth / 2;
        pos.y = pps[i][1] * 10 + screenHeight / 2;
        pos.w = cornerSize;
        pos.h = cornerSize;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
    // for(int i = 0; i < ppps.size(); i++){
    //     xyz.clear();
    //     temp.clear();
        
    //     for(int j = 0; j < ppps[i].size(); j++){
    //         temp.push_back(ppps[i][j]);
    //         xyz.push_back(temp);
    //         temp.clear();
    //     }
    //     // vector<vector<double>> rotated = MultMatrixs(roty, xyz);
    //     // rotated = MultMatrixs(rotx, rotated);
    //     // rotated = MultMatrixs(rotz, rotated);
    //     vector<vector<double>> xys = MultMatrixs(projection2D, xyz);

    //     for(int j = 0; j < xys.size(); j++){
    //         temp.push_back(xys[j][0]);
    //     }
    //     pps.push_back(temp);

    //     pos.x = pps[i][0] * 10 + screenWidth / 2;
    //     pos.y = pps[i][1] * 10 + screenHeight / 2;
    //     pos.w = cornerSize;
    //     pos.h = cornerSize;
    //     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //     SDL_RenderFillRect(renderer, &pos);
    // }
    
    for (int i = 0; i < 4; i++) {
        Connect(0, i, (i + 1) % 4, pps);
        Connect(0, i + 4, ((i + 1) % 4) + 4, pps);
        Connect(0, i, i + 4, pps);
    }

    for (int i = 0; i < 4; i++) {
        Connect(8, i, (i + 1) % 4, pps);
        Connect(8, i + 4, ((i + 1) % 4) + 4, pps);
        Connect(8, i, i + 4, pps);
    }
    // Connect(0, 0, 14, pps);
    // Connect(0, 1, 15, pps);
    // Connect(0, 2, 12, pps);
    // Connect(0, 3, 13, pps);
    // Connect(0, 4, 10, pps);
    // Connect(0, 5, 11, pps);
    // Connect(0, 6, 8, pps);
    // Connect(0, 7, 9, pps);
    for (int i = 0; i < 8; i++) {
        Connect(0, i, i + 8, pps);
    }
}

void Connect(int offSet, int i, int j, vector<vector<double>> pps){
    int ix = pps[i + offSet][0] * 10 + screenWidth / 2;
    int iy = pps[i + offSet][1] * 10 + screenHeight / 2;
    int jx = pps[j + offSet][0] * 10 + screenWidth / 2;
    int jy = pps[j + offSet][1] * 10 + screenHeight / 2;
    double slope = static_cast<double>((jy - iy)) / (jx - ix);
    if(jx > ix && abs(slope) < 40){
        for(double x = jx; x >= ix; x-=.1){
            pos.x = x + (cornerSize - lineSize) / 2;
            pos.y = slope * x + slope * (-1 * jx) + jy + (cornerSize - lineSize) / 2;
            pos.w = lineSize;
            pos.h = lineSize;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
    else if(jx < ix && abs(slope) < 40){
        for(double x = jx; x <= ix; x+=.1){
            pos.x = x + (cornerSize - lineSize) / 2;
            pos.y = slope * x + slope * (-1 * jx) + jy + (cornerSize - lineSize) / 2;
            pos.w = lineSize;
            pos.h = lineSize;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
    else{
        if(jy < iy){
            for(double y = jy; y != iy; y+=1){
                pos.x = jx + (cornerSize - lineSize) / 2;
                pos.y = y + (cornerSize - lineSize) / 2;
                pos.w = lineSize;
                pos.h = lineSize;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
        else if(jy > iy){
            for(double y = jy; y != iy; y-=1){
                pos.x = jx + (cornerSize - lineSize) / 2;
                pos.y = y + (cornerSize - lineSize) / 2;
                pos.w = lineSize;
                pos.h = lineSize;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
    }
}

vector<vector<double>> MultMatrixs(vector<vector<double>> mat1, vector<vector<double>> mat2){
    vector<vector<double>> result;
    vector<double> temp;
    double a = 0;
    for(int j = 0; j < mat1.size(); j++){
        for(int k = 0; k < mat2[0].size(); k++){
            for(int i = 0; i < mat1[j].size(); i++){
                a+= mat1[j][i] * mat2[i][k];
            }
            temp.push_back(a);
            a = 0;
        }
        result.push_back(temp);
        temp.clear();
    }
    return result;
}

void Setup(){
    vector<double> temp1;
    temp1.clear();
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    temp1.push_back(.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(.5);
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    temp1.push_back(.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(.5);
    temp1.push_back(.5);
    temp1.push_back(-.5);
    temp1.push_back(.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(-.5);
    temp1.push_back(.5);
    temp1.push_back(-.5);
    temp1.push_back(.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    temp1.push_back(.5);
    temp1.push_back(.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(.5);
    temp1.push_back(-.5);
    temp1.push_back(.5);
    temp1.push_back(.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(.5);
    temp1.push_back(.5);
    temp1.push_back(.5);
    temp1.push_back(.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(-.5);
    temp1.push_back(.5);
    temp1.push_back(.5);
    temp1.push_back(.5);
    points.push_back(temp1);
    temp1.clear();

    temp1.push_back(-.5);
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(.5);
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(.5);
    temp1.push_back(.5);
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(-.5);
    temp1.push_back(.5);
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(-.5);
    temp1.push_back(-.5);
    temp1.push_back(.5);
    temp1.push_back(-.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(.5);
    temp1.push_back(-.5);
    temp1.push_back(.5);
    temp1.push_back(-.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(.5);
    temp1.push_back(.5);
    temp1.push_back(.5);
    temp1.push_back(-.5);
    points.push_back(temp1);
    temp1.clear();
    temp1.push_back(-.5);
    temp1.push_back(.5);
    temp1.push_back(.5);
    temp1.push_back(-.5);
    points.push_back(temp1);
    temp1.clear();

    // temp1.push_back(w);
    // temp1.push_back(0);
    // temp1.push_back(0);
    // projection2D.push_back(temp1);
    // temp1.clear();
    // temp1.push_back(0);
    // temp1.push_back(w);
    // temp1.push_back(0);
    // projection2D.push_back(temp1);
    // temp1.clear();
}

void ProjDs(){
    vector<double> temp1;
    projection2D.clear();
    temp1.push_back(w);
    temp1.push_back(0);
    temp1.push_back(0);
    projection2D.push_back(temp1);
    temp1.clear();
    temp1.push_back(0);
    temp1.push_back(w);
    temp1.push_back(0);
    projection2D.push_back(temp1);
    temp1.clear();

    projection3D.clear();
    temp1.push_back(w);
    temp1.push_back(0);
    temp1.push_back(0);
    temp1.push_back(0);
    projection3D.push_back(temp1);
    temp1.clear();
    temp1.push_back(0);
    temp1.push_back(w);
    temp1.push_back(0);
    temp1.push_back(0);
    projection3D.push_back(temp1);
    temp1.clear();
    temp1.push_back(0);
    temp1.push_back(0);
    temp1.push_back(w);
    temp1.push_back(0);
    projection3D.push_back(temp1);
    temp1.clear();
}

void Rots(){
    vector<double> temp;
    rotx.clear();
    roty.clear();
    rotz.clear();
    rotxy.clear();
    rotzz.clear();
    rotxz.clear();

    temp.clear();
    temp.push_back(1);
    temp.push_back(0);
    temp.push_back(0);
    rotx.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(cos(ang));
    temp.push_back(-1*sin(ang));
    rotx.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(sin(ang));
    temp.push_back(cos(ang));
    rotx.push_back(temp);
    temp.clear();

    temp.push_back(cos(ang));
    temp.push_back(0);
    temp.push_back(sin(ang));
    roty.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(1);
    temp.push_back(0);
    roty.push_back(temp);
    temp.clear();
    temp.push_back(-1*sin(ang));
    temp.push_back(0);
    temp.push_back(cos(ang));
    roty.push_back(temp);
    temp.clear();

    temp.push_back(cos(ang));
    temp.push_back(-1*sin(ang));
    temp.push_back(0);
    rotz.push_back(temp);
    temp.clear();
    temp.push_back(sin(ang));
    temp.push_back(cos(ang));
    temp.push_back(0);
    rotz.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(1);
    rotz.push_back(temp);
    temp.clear();

    temp.push_back(cos(ang));
    temp.push_back(-1 * sin(ang));
    temp.push_back(0);
    temp.push_back(0);
    rotxy.push_back(temp);
    temp.clear();
    temp.push_back(sin(ang));
    temp.push_back(cos(ang));
    temp.push_back(0);
    temp.push_back(0);
    rotxy.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(1);
    temp.push_back(0);
    rotxy.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(1);
    rotxy.push_back(temp);
    temp.clear();

    temp.push_back(cos(ang));
    temp.push_back(0);
    temp.push_back(-1 * sin(ang));
    temp.push_back(0);
    rotxz.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(1);
    temp.push_back(0);
    temp.push_back(0);
    rotxz.push_back(temp);
    temp.clear();
    temp.push_back(sin(ang));
    temp.push_back(0);
    temp.push_back(cos(ang));
    temp.push_back(0);
    rotxz.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(1);
    rotxz.push_back(temp);
    temp.clear();

    temp.push_back(1);
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(0);
    rotzz.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(1);
    temp.push_back(0);
    temp.push_back(0);
    rotzz.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(cos(ang));
    temp.push_back(-1 * sin(ang));
    rotzz.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(sin(ang));
    temp.push_back(cos(ang));
    rotzz.push_back(temp);
    temp.clear();
}

double ScaleNum(double n, double minN, double maxN, double min, double max){
    return (((n - minN) / (maxN - minN)) * (max - min)) + min;
}