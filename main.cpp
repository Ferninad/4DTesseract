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
vector<vector<double>> SubMatrixs(vector<vector<double>> mat1, vector<vector<double>> mat2);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int screenWidth = 500;
int screenHeight = 500;
int cornerSize = 3;
int lineSize = 1;
double ang = 0;
double dist = 2;
double w;
double xang = .47;
double yang = .9;
double zang = 0;
double wang = 0;
double per = 1;
double cx = 0;
double cy = 0;
double cz = 3;
double ax = 0;
double ay = 0;
double az = 0;

vector<vector<double>> points;
vector<vector<double>> cameraPosition;
vector<vector<double>> cameraOrientation;
vector<vector<double>> display;
vector<vector<double>> rotx;
vector<vector<double>> roty;
vector<vector<double>> rotz;
vector<vector<double>> rotxy;
vector<vector<double>> rotzw;
vector<vector<double>> rotxz;
vector<vector<double>> rotxw;
vector<vector<double>> camrotx;
vector<vector<double>> camroty;
vector<vector<double>> camrotz;
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
    display.push_back({0});
    display.push_back({0});
    display.push_back({1.5});

    while (gameLoop)
    {   
        wang += .015;
        if(wang > 2*M_PI)
            wang -= 2 * M_PI;
        Draw();
        SDL_RenderPresent(renderer);
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
                    case SDLK_w:
                        wang += .01;
                        if(wang > 2 * M_PI)
                            wang -= 2 * M_PI;
                        break;
                    case SDLK_s:
                        wang -= .01;
                        if(wang < 0)
                            wang += 2 * M_PI;
                        break;
                    case SDLK_e:
                        xang += .01;
                        if(xang > 2 * M_PI)
                            xang -= 2 * M_PI;
                        break;
                    case SDLK_d:
                        xang -= .01;
                        if(xang < 0)
                            xang += 2 * M_PI;
                        break;
                    case SDLK_r:
                        yang += .01;
                        if(yang > 2 * M_PI)
                            yang -= 2 * M_PI;
                        break;
                    case SDLK_f:
                        yang -= .01;
                        if(yang < 0)
                            yang += 2 * M_PI;
                        break;
                    case SDLK_UP:
                        per += .1;
                        break;
                    case SDLK_DOWN:
                        per -= .1;
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
    vector<vector<double>> xyzw;
    vector<double> temp;
    Rots();
    for(int i = 0; i < points.size(); i++){
        xyzw.clear();
        temp.clear();
        for(int j = 0; j < points[i].size(); j++){
            xyzw.push_back({points[i][j]});
        }
        vector<vector<double>> rotated = MultMatrixs(rotzw, xyzw);
        //rotated = MultMatrixs(rotxz, rotated);
        //vector<vector<double>> rotated = MultMatrixs(rotxw, xyzw);
        w = 1.0 / (dist - rotated[3][0]);
        ProjDs();
        vector<vector<double>> xyzs = MultMatrixs(projection3D, rotated);

        rotated = MultMatrixs(roty, xyzs);
        rotated = MultMatrixs(rotx, rotated);
        rotated = MultMatrixs(rotz, rotated);
        vector<vector<double>> diffpc = SubMatrixs(rotated, cameraPosition);
        vector<vector<double>> transform = MultMatrixs(diffpc, camrotx);
        transform = MultMatrixs(camroty, transform);
        transform = MultMatrixs(camrotz, transform);
        pps.push_back({(display[2][0] / transform[2][0]) * transform[0][0] + display[0][0], (display[2][0] / transform[2][0]) * transform[1][0] + display[1][0]});

        pos.x = pps[i][0] * 100 + screenWidth / 2;
        pos.y = pps[i][1] * 100 + screenHeight / 2;
        pos.w = cornerSize;
        pos.h = cornerSize;
        SDL_RenderFillRect(renderer, &pos);
    }
    
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
    for(int i = 0; i < 8; i++){
        Connect(0, i, i + 8, pps);
    }
}

void Connect(int offset, int i, int j, vector<vector<double>> pps){
    int ix = pps[i + offset][0] * 200 + screenWidth / 2;
    int iy = pps[i + offset][1] * 200 + screenHeight / 2;
    int jx = pps[j + offset][0] * 200 + screenWidth / 2;
    int jy = pps[j + offset][1] * 200 + screenHeight / 2;
    double ang = atan2((jy - iy), (jx - ix));
    double cx = cos(ang);
    double cy = sin(ang);
    double x = static_cast<double>(ix);
    double y = static_cast<double>(iy);
    while(static_cast<int>(x) != jx || static_cast<int>(y) != jy){
        if(static_cast<int>(x) != jx)
            x += cx;
        if(static_cast<int>(y) != jy)
            y += cy;
        pos.x = static_cast<int>(x);
        pos.y = static_cast<int>(y);
        pos.w = lineSize;
        pos.h = lineSize;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
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

vector<vector<double>> SubMatrixs(vector<vector<double>> mat1, vector<vector<double>> mat2){
    vector<vector<double>> result;
    vector<double> temp;
    double a = 0;
    for(int j = 0; j < mat1.size(); j++){
        for(int k = 0; k < mat1[j].size(); k++){
            a = mat1[j][k] - mat2[j][k];
            temp.push_back(a);
            a = 0;
        }
        result.push_back(temp);
        temp.clear();
    }
    return result;
}

void Setup(){
    points.push_back({-1, -1, -1, 1});
    points.push_back({1, -1, -1, 1});
    points.push_back({1, 1, -1, 1});
    points.push_back({-1, 1, -1, 1});
    points.push_back({-1, -1, 1, 1});
    points.push_back({1, -1, 1, 1});
    points.push_back({1, 1, 1, 1});
    points.push_back({-1, 1, 1, 1});
    
    points.push_back({-1, -1, -1, -1});
    points.push_back({1, -1, -1, -1});
    points.push_back({1, 1, -1, -1});
    points.push_back({-1, 1, -1, -1});
    points.push_back({-1, -1, 1, -1});
    points.push_back({1, -1, 1, -1});
    points.push_back({1, 1, 1, -1});
    points.push_back({-1, 1, 1, -1});
}

void ProjDs(){
    projection2D.clear();
    projection2D.push_back({per, 0, 0});
    projection2D.push_back({0, per, 0});

    projection3D.clear();
    projection3D.push_back({w, 0, 0, 0});
    projection3D.push_back({0, w, 0, 0});
    projection3D.push_back({0, 0, w, 0});
}

void Rots(){
    vector<double> temp;
    rotx.clear();
    roty.clear();
    rotz.clear();
    rotxy.clear();
    rotzw.clear();
    rotxz.clear();
    rotxw.clear();
    camrotx.clear();
    camroty.clear();
    camrotz.clear();
    cameraPosition.clear();
    cameraOrientation.clear();

    cameraPosition.push_back({cx});
    cameraPosition.push_back({cy});
    cameraPosition.push_back({cz});
    cameraOrientation.push_back({ax, ay, az});

    camrotx.push_back({1, 0, 0});
    camrotx.push_back({0, cos(cameraOrientation[0][0]), sin(cameraOrientation[0][0])});
    camrotx.push_back({0, -1*sin(cameraOrientation[0][0]), cos(cameraOrientation[0][0])});

    camroty.push_back({cos(cameraOrientation[0][1]), 0, -1*sin(cameraOrientation[0][1])});
    camroty.push_back({0, 1, 0});
    camroty.push_back({sin(cameraOrientation[0][1]), 0, cos(cameraOrientation[0][1])});

    camrotz.push_back({cos(cameraOrientation[0][2]), sin(cameraOrientation[0][2]), 0});
    camrotz.push_back({-1*sin(cameraOrientation[0][2]), cos(cameraOrientation[0][2]), 0});
    camrotz.push_back({0, 0, 1});

    rotx.push_back({1, 0, 0});
    rotx.push_back({0, cos(xang), -1*sin(xang)});
    rotx.push_back({0, sin(xang), cos(xang)});

    roty.push_back({cos(yang), 0, sin(yang)});
    roty.push_back({0, 1, 0});
    roty.push_back({-1*sin(yang), 0, cos(yang)});

    rotz.push_back({cos(zang), -1*sin(zang), 0});
    rotz.push_back({sin(zang), cos(zang), 0});
    rotz.push_back({0, 0, 1});

    rotxy.push_back({cos(wang), -1*sin(wang), 0, 0});
    rotxy.push_back({sin(wang), cos(wang), 0, 0});
    rotxy.push_back({0, 0, 1, 0});
    rotxy.push_back({0, 0, 0, 1});

    rotxz.push_back({cos(wang), 0, -1*sin(wang), 0});
    rotxz.push_back({0, 1, 0, 0});
    rotxz.push_back({sin(wang), 0, cos(wang), 0});
    rotxz.push_back({0, 0, 0, 1});

    rotxw.push_back({cos(wang), 0, 0, -1*sin(wang)});
    rotxw.push_back({0, 1, 0, 0});
    rotxw.push_back({0, 0, 1, 0});
    rotxw.push_back({sin(wang), 0, 0, cos(wang)});

    rotzw.push_back({1, 0, 0, 0});
    rotzw.push_back({0, 1, 0, 0});
    rotzw.push_back({0, 0, cos(wang), -1*sin(wang)});
    rotzw.push_back({0, 0, sin(wang), cos(wang)});
}