/*-------------------------INCLUDES--------------------------------------*/
#include <stdio.h>
#include <SDL.h>
#include <math.h>
/*-------------------------DEFINES---------------------------------------*/
# define REZ_X 800
# define REZ_Y 800
# define safedo(f,x, m) f; ERRORCHECK(x,m,window_ptr, renderer_ptr)
# define PARTICLE_COUNT 240000
#define SCREEN_FPS 600
#define SCREEN_TICKS_PER_FRAME (1000 / SCREEN_FPS)
/*---------------------------STRUCTS-----------------------------------*/
typedef enum colours{
    RED,
    BLUE,
    YELLOW,
    GREEN
} colours;
typedef struct vector{
    float x;
    float y;
} vector;
typedef struct particle{
    vector position;
    vector acceleration;
    colours colour;
    colours adjacentcolours[3];
    float strength;
} particle;
typedef struct cell{
    int OCCUPIED;
    particle* occupant;
} cell;
/*---------------------------PUBLIC VARIABLES--------------------------*/
SDL_Window* window_ptr = NULL;
SDL_Renderer* renderer_ptr = NULL;
int E;
int QUIT_FLAG = 0;
particle Particles[PARTICLE_COUNT];
cell currentGrid[REZ_X][REZ_Y];

//Test particle 
particle p0;


/*----------------------FUNCTION DECLARATIONS-------------------------*/
//Engine stuff
void ERRORCHECK(void* e, char* msg, SDL_Window* w, SDL_Renderer* r);
void INPUTPHASE();
void PROCESSPHASE();
void DISPLAYPHASE();
//Vector stuff
vector position_transform(vector p);
float scalarProduct(vector a, vector b);
vector crossProduct(vector a, vector b);
vector vecAdd(vector a, vector b);
vector vecScalarMult(vector a, float c);
float findDisplacement(vector a, vector b);
int MyRenderLine(vector p, vector d, float s);
float length(vector p1, vector p2);
float findX(float m, float c, float y);
float findY(float m, float x, float c);
float findC(float m, float x, float y);

int main(int argc, char** argv)
{   
    srand(0);
    SDL_version nb;
    SDL_VERSION(&nb);
    int start_tick, diff_tick;
    float fps = 60.0f;
    char title[16];
    printf("Bienvenue sur la SDL %d.%d.%d! \n", nb.major, nb.minor, nb.patch);

    safedo(E = SDL_Init(SDL_INIT_VIDEO),&E,"Error Initialising Video");

    safedo(window_ptr = SDL_CreateWindow("pak pak pak",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,REZ_X,REZ_Y, 0),window_ptr, "Error Creating Window");

    safedo(renderer_ptr = SDL_CreateRenderer(window_ptr,-1,SDL_RENDERER_SOFTWARE),renderer_ptr,"Error creating Renderer 0");

    SDL_RenderPresent(renderer_ptr);
    p0.position = (vector){400,400};
    p0.strength = 100.0f;
     //program execution

    while( QUIT_FLAG == 0 ){
        start_tick = SDL_GetTicks();
         if(SCREEN_TICKS_PER_FRAME > diff_tick)
        {
            SDL_Delay((int)(SCREEN_TICKS_PER_FRAME-diff_tick));
        }

        INPUTPHASE();
        /*
        for (size_t i = 0; i < REZ_Y; i+=1)
        {   
            //sprintf(title, "%f%%", (100.0*i/REZ_Y));
            //SDL_SetWindowTitle(window_ptr,title);
            for (size_t j = 0; j < REZ_X; j+=1)
            {
                safedo(E= SDL_SetRenderDrawColor(renderer_ptr,rand()%255,rand()%255,rand()%255, SDL_ALPHA_OPAQUE),&E,"Error setting RenderDrawColour");
                safedo(E = SDL_RenderDrawPoint(renderer_ptr,rand()%(REZ_X-j), rand()%(REZ_Y-i)),&E,"Error Drawing Point");
            }   
        }
        */
        PROCESSPHASE();
        DISPLAYPHASE();
        //safedo(E =SDL_SetRenderDrawColor(renderer_ptr,0,0,0, SDL_ALPHA_OPAQUE),&E, "Error Setting RenderDrawColour");

        SDL_RenderPresent(renderer_ptr);

        safedo(E = SDL_RenderClear(renderer_ptr),&E,"Error clearing renderer");
       
       
        diff_tick = SDL_GetTicks() - start_tick;
        fps = 1000.0f/diff_tick;
        //printf("%f --- %d\n",fps,diff_tick);
        sprintf(title,"%f",fps);
        SDL_SetWindowTitle(window_ptr,title);

    }
    //COMPLETED!
    SDL_DestroyRenderer(renderer_ptr);
    SDL_DestroyWindow(window_ptr);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

/* BUILD COMMANDS
commande pour build
gcc src/main.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2
*/


/*-----------------------FUNCTION DEFINITIONS-----------------------------------------*/

void ERRORCHECK(void* e, char* msg, SDL_Window* w, SDL_Renderer* r)
{   
    //SDL_Log("%p",e);
    if (e == NULL || (sizeof(int) == sizeof(*e) && *(int*)e != 0) )
    {   
        SDL_Log(msg);
        SDL_Log("Error >> %s \n", SDL_GetError());
        if(w != NULL)
        {
            SDL_DestroyWindow(w);
        }
        if(r != NULL)
        {
            SDL_DestroyRenderer(r);
        }
        //printf("kaka in fanner bro \n");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}


vector position_transform(vector p){
    if(p.x > REZ_X){
            p.x = 0 + ((int)p.x % REZ_X);
        }
    else if (p.x < 0){
            p.x = REZ_X - ((int)p.x*-1 % REZ_X );
        }
    if(p.y > REZ_Y){
            p.y = 0 + ((int)p.y % REZ_Y);
        }
    else if (p.y < 0){
            p.y = REZ_Y - ((int)p.y*-1 % REZ_Y );
        }
    return p;
}

void INPUTPHASE()
{   
    SDL_Event e;
     while( SDL_PollEvent( &e )==1 ){
            if( e.type == SDL_QUIT ) QUIT_FLAG = 1;
            else{
                //WHEN A KEY IS PRESSED DOWN
                if(e.type == SDL_KEYDOWN){
                    SDL_Keycode k = e.key.keysym.sym;
                    if(k == SDLK_w){
                        printf("W");
                        p0.position.y -= 2;
                    }
                    if(k == SDLK_s){
                        p0.position.y += 2;
                        printf("S");
                    }
                    if(k == SDLK_a){
                        printf("A");
                        p0.position.x -= 2;
                    }
                    if(k == SDLK_d){
                        printf("D");
                        p0.position.x += 2;
                    }
            
                }
            }
            
        } 

}

void PROCESSPHASE(){
    p0.position = position_transform(p0.position);
    float dTheta = M_PI/10;
    safedo(E = SDL_SetRenderDrawColor(renderer_ptr,255,0,0,255),&E,"Error Setting drawcolour");
    for(float theta = 0; theta < (2*M_PI); theta += dTheta )
    {   
        //MyRenderLine(p0.position,(vector){cos(theta),sin(theta)} ,p0.strength);
        safedo(E = SDL_RenderDrawLine(renderer_ptr,p0.position.x, p0.position.y,p0.position.x + cos(theta)*p0.strength,p0.position.y + sin(theta)*p0.strength),&E, "Error drawing line of particle P0");
    }
    safedo(E = SDL_SetRenderDrawColor(renderer_ptr,0,0,0,255),&E,"Error Setting drawcolour");
}

void DISPLAYPHASE(){} 

float length(vector p1, vector p2){
    return(sqrt(pow(p1.x - p2.x, 2) + pow(p1.y-p2.y,2)));
}

float findY(float m, float x, float c){
    return ((m*x)+c);
}
float findX(float m, float c, float y){
    return((y-c)/m);
}
float findC(float m, float x, float y){
    return (y - (m * x));
}

int MyRenderLine(vector p, vector d, float s)
{   
    vector pf;
    vector pcheck;
    //If Line is Vertical
    while (s > 0.0001){
        pf.x = p.x + s * d.x; pf.y = p.x + s * d.y; //get what pf would be in an infinite plane
        float s1, s2;
        s1 = (REZ_X - p.x)/d.x;
        s2 = (REZ_Y - p.y)/d.y;
        

    }
}