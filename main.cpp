#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfx.h>
#include <string>
#include <vector>
#include <windows.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <sstream>

#ifndef M_PI
#define M_PI 3.141592653589793238462
#endif // M_PI

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif // M_PI_2

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830961566
#endif // M_PI_4


// EE project :: block game
//by Hirad Davari & Parisa Toumari
/*

const char * font_address_01="c:\\PROGRA~2\\CodeBlocks\\share\\CodeBlocks\\assets\\lazy.ttf";

const char * font_address_02="c:\\PROGRA~2\\CodeBlocks\\share\\CodeBlocks\\assets\\arial.ttf";

const char * font_address_03="c:\\PROGRA~2\\CodeBlocks\\share\\CodeBlocks\\assets\\B_Roya.ttf";

const char * font_address_04="c:\\PROGRA~2\\CodeBlocks\\share\\CodeBlocks\\assets\\SHABNAM.ttf";

const char * font_address_05="c:\\PROGRA~2\\CodeBlocks\\share\\CodeBlocks\\assets\\SHABNAM_BOLD.ttf";

const char * font_address_06="c:\\PROGRA~2\\CodeBlocks\\share\\CodeBlocks\\assets\\SHABNAM_LIGHT.ttf";
*/

using namespace std;

typedef vector<string> stringvec;
SDL_Renderer * m_renderer;
class Texture{
    public:
    SDL_Texture* text;
    int mWidth;
    int mHeight;
    /*Texture(){
        text = SDL_CreateTexture(m_renderer,0,SDL_TEXTUREACCESS_STREAMING,12,12);
    }*/
    ~Texture(){
        SDL_DestroyTexture(text);
    }
};

class imgTexture{
    public:
    SDL_Texture* text;
    int x;
    int y;
    int Width;
    int Height;
    ~imgTexture(){
        SDL_DestroyTexture(text);
    }
};
//RESOUCSES :: fonts
TTF_Font* FONT_arial_60;
TTF_Font* FONT_arial_30;
TTF_Font* FONT_arial_18;
//RESOURCES :: images
imgTexture* img_block_dark_1;
imgTexture* img_block_dark_2;
imgTexture* img_block_dark_3;
imgTexture* img_block_light_1;
imgTexture* img_block_light_2;
imgTexture* img_block_light_3;
imgTexture* img_background;
imgTexture* img_platform;
imgTexture* img_ball;
imgTexture* img_arrow;
imgTexture* img_background_menu1;
imgTexture* img_background_menu2;
imgTexture* img_background_intro;
imgTexture* img_background_valve;
imgTexture* img_background_gameover;

imgTexture* img_cinema;
const int _brickTextures =6;
imgTexture* brickTextures[_brickTextures];


//-----------------------
int stage = 0; //0 == menu , 1 == game , 2 = gameover
int phaze = 0; // 0 == users/start , 1== game/shoot

int pSpeed = 14;

int broken_bricks=0;
int level = 1;
int lives =3;
int score=0;

int animate;
bool pauseP = false;

bool transition = false;
int transition_sequence=0;
bool disp = false;

//functions:
imgTexture* myLoadImage(char* fileName,int width=-1,int height=-1){
    SDL_Texture* tex = IMG_LoadTexture(m_renderer,fileName);
    int w,h;
    SDL_QueryTexture(tex,NULL,NULL,&w,&h);
    imgTexture* temp = new imgTexture;
    temp->x=0;
    temp->y=0;
    temp->text = tex;
    temp->Width = (width == -1 ? w : width);
    temp->Height = (height == -1 ? h : height);
    return temp;
}
void myDrawImage(imgTexture* textu,int x,int y){
    SDL_Rect rect;
    rect.x=x;
    rect.y=y;
    rect.w=textu->Width;
    rect.h=textu->Height;
    SDL_RenderCopy(m_renderer,textu->text,NULL,&rect);
}
void myDrawImageEx(imgTexture* textu,int x,int y,double alpha,SDL_Point center,SDL_RendererFlip flip = SDL_FLIP_NONE){
    SDL_Rect rect;
    rect.x=x;
    rect.y=y;
    rect.w=textu->Width;
    rect.h=textu->Height;
    SDL_RenderCopyEx(m_renderer,textu->text,NULL,&rect,alpha,&center,flip);
}
Texture* CreateTextTexture(SDL_Renderer* renderer, const char* text, TTF_Font* font, int A=255, int R=220,int G=220, int B=220){
    SDL_Color text_color = { R, G, B, A};
    Texture* mytext = new Texture;
    int mWidth = 0;
	int mHeight = 0;
    SDL_Texture* mTexture = NULL;

    SDL_Surface* textSurface = TTF_RenderText_Solid( font,text, text_color );
    mWidth = textSurface->w;
    mHeight = textSurface->h;
    mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
    mytext->text = mTexture;
    mytext->mWidth = mWidth;
    mytext->mHeight = mHeight;
    SDL_FreeSurface( textSurface );
    return mytext;
}
void DrawText(SDL_Renderer* renderer,Texture* text,int x,int y,double angle =0.0,SDL_RendererFlip flip =SDL_FLIP_NONE){
    SDL_Rect renderQuad = { x , y , text->mWidth, text->mHeight };
    SDL_RenderCopyEx( renderer, text->text, NULL, &renderQuad, angle, NULL, flip );
}
bool isValid(char c){
    int a = (int)c;
    return (a>=65 && a<=90) || (a>=97 && a<=122) || (a>=48 && a<=57) || a == 95 || a==45 || a==32 || a==8;
}

void read_directory(const string& name, stringvec& v){
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.push_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
}

double abs(double a){
    return (a<0 ? -a :a);
}
//classes:
class KeyboardEventHandler{
    public:
    int keyState;
    void reset(){
        keyState =0;
    }

    static const int KEY_right = 1;
    static const int KEY_left = 2;
    static const int KEY_up = 4;
    static const int KEY_down = 8;
    static const int KEY_w = 16;
    static const int KEY_a = 32;
    static const int KEY_d = 64;
    static const int KEY_space = 128;
    static const int KEY_enter = 256;
    static const int KEY_esc = 512;
    KeyboardEventHandler(){
        keyState = 0;
    }
    void KeyDownHandler(SDL_Event* e){
        switch(e->key.keysym.sym){
        case SDLK_DOWN:
            keyState |= KEY_down;
        break;
        case SDLK_UP:
            keyState |= KEY_up;
        break;
        case SDLK_RIGHT:
            keyState |= KEY_right;
        break;
        case SDLK_LEFT:
            keyState |= KEY_left;
        break;
        case SDLK_SPACE:
            keyState |= KEY_space;
        break;
        case SDLK_w:
            keyState |= KEY_w;
        break;
        case SDLK_a:
            keyState |= KEY_a;
        break;
        case SDLK_d:
            keyState |= KEY_d;
        break;
        case SDLK_RETURN:
            keyState |= KEY_enter;
        break;
        case SDLK_ESCAPE:
            keyState |= KEY_esc;
        break;
        }
    }
    void KeyUpHandler(SDL_Event* e){
        switch(e->key.keysym.sym){
        case SDLK_DOWN:
            keyState &= ~KEY_down;
        break;
        case SDLK_UP:
            keyState &= ~KEY_up;
        break;
        case SDLK_RIGHT:
            keyState &= ~KEY_right;
        break;
        case SDLK_LEFT:
            keyState &= ~KEY_left;
        break;
        case SDLK_SPACE:
            keyState &= ~KEY_space;
        break;
        case SDLK_w:
            keyState &= ~KEY_w;
        break;
        case SDLK_a:
            keyState &= ~KEY_a;
        break;
        case SDLK_d:
            keyState &= ~KEY_d;
        break;
        case SDLK_RETURN:
            keyState &= ~KEY_enter;
        break;
        case SDLK_ESCAPE:
            keyState &= ~KEY_esc;
        break;
        }
    }
};

class Element{
    public:
    string type;
    string s;
    Texture* text_focus=NULL;
    Texture* text_blur=NULL;
    int x;
    int y;
    int h;
    int w;
    bool focus;
    Element(){focus = false;}
    Element(int XX,int YY){x=XX;y=YY;focus=false;}
    void draw(SDL_Renderer* renderer,bool cursor=false){
        if(type=="textbox"){
            string txt ="";
            roundedRectangleColor(renderer,x,y,x+w,y+h,10,(!focus ? 0x77183318 : 0xF5183318));
            if(s!="") DrawText(renderer,(focus ? text_focus : text_blur),x+12,y+8);
            if(cursor && focus){
                lineRGBA(renderer,x+text_focus->mWidth+14,y+2*h/10,x+text_focus->mWidth+14,y+7*h/10,200,200,200,255);
            }
        }
        else if(type=="button"){
            roundedRectangleColor(renderer,x,y,x+w,y+h,10,(!focus ? 0x77183318 : 0xF5183318));
            if(focus) lineRGBA(renderer,x+10,y+8+text_blur->mHeight,x+14+text_blur->mWidth,y+8+text_blur->mHeight,40 ,60 ,40,255);
            if(s!="") DrawText(renderer,(focus ? text_focus : text_blur),x+12,y+8);
        }
    }
    void assign_texture(SDL_Renderer* r){
        if(text_focus != NULL){
            SDL_DestroyTexture(text_focus->text);
        }
        if(text_blur != NULL){
            SDL_DestroyTexture(text_blur->text);
        }
        text_focus = CreateTextTexture(r,s.c_str(),FONT_arial_30,255,30,60,30);
        text_blur = CreateTextTexture(r,s.c_str(),FONT_arial_30,150,40,40,40);
    }
    void (*action)();
};
class Button : public Element{
    public:
    Button(SDL_Renderer* ren, string S,int X,int Y,int W,int H){
        w=W;
        h=H;
        x=X;
        y=Y;
        s=S;
        assign_texture(ren);
        Element();
        type="button";
    }
};
class TextBox : public Element{
    public:
    bool isIdle;
    TextBox(SDL_Renderer*r,string S, int X, int Y, int W,int H){
        s=S;
        assign_texture(r);
        Element();
        type="textbox";
        x=X;
        y=Y;
        h=H;
        w=W;
    }
};
struct selector{
    int x;
    int y;
    bool draw;
};

typedef struct{
    int x;
    int y;
} Point;

class Line{
    public:
    Point p1;
    Point p2;
    double slope;
    double angle;
    double length;
    Line(int x1,int y1,int x2,int y2){
        p1.x = x1;
        p1.y = y1;
        p2.x = x2;
        p2.y = y2;
        slope = (x1==x2 ? ~0 : (double)(y2-y1)/(x2-x1));
        angle = (x1==x2 ? M_PI_2 : -SDL_atan(slope));
        length = SDL_sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
    }
};

class Brick{
public:
    int prevx;
    int prevy;
    int x;
    int y;
    int v;
    int strength;
    int maxHP;
    Line* lines[6];
    imgTexture* texture;
    int texnum;
    bool visible;
    bool _drop;
    Brick(int X,int Y,int hp,imgTexture* text){
        v=0;
        visible=true;
        _drop=false;
        maxHP = hp;
        x=X;
        y=Y;
        prevx=x;
        prevy=y;
        strength = hp;
        texture = text;
        lines[0] = new Line(x+13,y,x+137,y);
        lines[1] = new Line(x+137,y,x+150,y+25);
        lines[2] = new Line(x+150,y+25,x+137,y+50);
        lines[3] = new Line(x+137,y+50,x+13,y+50);
        lines[4] = new Line(x+13,y+50,x,y+25);
        lines[5] = new Line(x,y+25,x+13,y);
    }
    void calcHitBox(){
        prevx=x;
        prevy=y;
        delete[] lines;
        lines[0] = new Line(x+13,y,x+137,y);
        lines[1] = new Line(x+137,y,x+150,y+25);
        lines[2] = new Line(x+150,y+25,x+137,y+50);
        lines[3] = new Line(x+137,y+50,x+13,y+50);
        lines[4] = new Line(x+13,y+50,x,y+25);
        lines[5] = new Line(x,y+25,x+13,y);
    }
    Brick(){
    }
    void Draw(){
        if(!visible) return;
        myDrawImage(texture,x,y);
        /*
        for(int i=0;i<6;i++){
            thickLineRGBA(m_renderer,lines[i]->p1.x,lines[i]->p1.y,lines[i]->p2.x,lines[i]->p2.y,2,255,0,255,255);
        }
        */
        if(maxHP > 1){
            int alph = 255-255*strength/maxHP;
            boxRGBA(m_renderer,x,y,x+150,y+50,255,255,255,alph);
        }
    }
    void drop(bool start = false){
        if(start) _drop=true;
        if(_drop){
            y+=v;
            v+=3;
        }
    }
    ~Brick(){
        for(int i=0;i<6;i++){
            delete lines[i];
        }
    }
};

class Platform{
public:
    int x;
    int y;
    int v;
    Line* lines[4];
    imgTexture* texture;
    Platform(){
    }
    Platform(int X,int Y,imgTexture* text){
        x=X;
        y=Y;
        v=0;
        texture = text;
        lines[0] = new Line(x+48,y,x+252,y);
        lines[1] = new Line(x+252,y,x+300,y+30);
        lines[3] = new Line(x+300,y+30,x,y+30);
        lines[2] = new Line(x,y+30,x+48,y);
    }
    void calcHitBox(){
        delete[] lines;
        lines[0] = new Line(x+46,y,x+254,y);
        lines[1] = new Line(x+254,y,x+300,y+30);
        lines[3] = new Line(x+300,y+30,x,y+30);
        lines[2] = new Line(x,y+30,x+46,y);
    }
    void Draw(){
        myDrawImage(texture,x,y);
        /*
        for(int i=0;i<4;i++){
            thickLineRGBA(m_renderer,lines[i]->p1.x,lines[i]->p1.y,lines[i]->p2.x,lines[i]->p2.y,2,255,0,255,255);
        }
        */
    }
};

class Ball{
    public:
    int x;
    int y;
    int damage;
    int r;
    double v;
    double vx;
    double vy;
    double theta;
    imgTexture* texture;
    Ball(int X,int Y,int radius, double V,double angle,int Damage,imgTexture* tex){
        x=X;
        y=Y;
        damage = Damage;
        v=V;
        r=radius;
        theta = angle;
        vx = v*SDL_cos(angle);
        vy = v*SDL_sin(angle);
        texture = tex;
    }
    void setV(double V){
        v=V;
        vx = v*SDL_cos(theta);
        vy = v*SDL_sin(theta);
    }
    void setTheta(double Theta){
        theta = Theta;
        vx = v*SDL_cos(theta);
        vy = v*SDL_sin(theta);
    }
    void check(){
        if(vx <0 && vy >=0 && theta <0){
            theta +=M_PI;
            //cout<<"correct 1"<<endl;
        }else if(vx>0 && vy<0 && theta>0){
            theta-=M_PI;
            //cout<<"correct 2"<<endl;
        }else if(vx<0 && vy<0 && theta>0){
            theta-=M_PI;
            //cout<<"correct 3"<<endl;
        }
    }
    void setVx(double Vx){
        vx = Vx;
        v = SDL_sqrt( vx*vx + vy*vy );
        theta = SDL_atan( vy/vx );
        check();
    }
    void setVy(double Vy){
        vy = Vy;
        v = SDL_sqrt( vx*vx + vy*vy );
        theta = SDL_atan( vy/vx );
        check();
    }
    void Draw(){
        myDrawImage(texture,x-r,y-r);
    }
    void Update();
};
//==================================================================================================================ELEMENTS
    vector<Element*> elementsS0P0;
    vector<Element*> elementsS0P1;
    vector<Element*> elementsS1p;
    vector<Element*> elementsS2;
    vector<string> load_dirs;
    vector<Brick*> bricks;
    string saveFileName;
    string dirName;

    TextBox* tb1_s0p0;
    Button* b1_s0p0;

    Button** dirList_s0p1;
    int _dirList_s0p1;
    Button* bNew_s0p1;
    Button* bLoad_s0p1;
    Button* bBack_s0p1;
    Button* bClear_s0p1;
    selector selector1;

    Button* bBack_s1;
    Button* bCont_s1;

    Platform* platform1;
    Ball* ball;

    Brick* flasher;

    Button* bRet_s2;

//====================================================================================================================/ELEMENTS

bool xinA(double x,double A1,double A2){
    if(A1 > A2){
        double temp = A1;
        A1=A2;
        A2 = temp;
    }
    return ( (x>=A1) && (x<=A2) );
}
void generateBricks(){
    int n=0;
    int A[7];
    while(n<2){
        n=0;
        for(int i=0;i<7;i++){
            A[i] = rand()%2;
            n+=A[i];
        }
    }
    /*
    for(int i=0;i<bricks.size();i++){
        bricks[i]->y +=20;
        bricks[i]->calcHitBox();
    }
    */
    for(int i=0;(i<7);i++){
        if(A[i] == 0 ) continue;
        int texnum = rand()%_brickTextures;
        Brick* temp = new Brick(20+i*170,-50,rand()%(level>3 ? level-2 : level)+1,brickTextures[texnum]);
        temp->texnum = texnum;
        bricks.push_back(temp);
    }
}
void saveGame(){
    string path = "players/"+dirName+"/"+saveFileName;
    ofstream fout(path.c_str());
    fout<<"ok"<<endl<<level<<endl<<score<<endl<<lives<<endl;
    fout<<bricks.size()<<endl;
    for(int i=0;i<bricks.size();i++){
        fout<<bricks[i]->x<<" "<<bricks[i]->y<<" "<<bricks[i]->maxHP<<" "<<bricks[i]->strength<<" "<<bricks[i]->texnum<<endl;
    }
}

void  Ball::Update(){
    int newx = x+vx;
    int newy = y-vy;
    if(abs(vx) <1){
        if(vx>0) setVx(1);
        else setVx(-1);
    }
    if(newx > 1200-r && newy < r){
        setVy(-vy);
        setVx(-vx);
        x = 1200-r-1;
        y = r+1;
        return;
    }
    if(newx < r && newy < r){
        setVy(-vy);
        setVx(-vx);
        x = r+1;
        y = r+1;
        return;
    }
    if(newx > 1200-r || newx < r){
        setVx(-vx);
        if(abs(vy)<1){
            setVy(rand()%3+1);
        }
        return;
    }
    if(newy < r){
        setVy(-vy);
        return;
    }
    if( newy > 800){ // lose a life
        lives--;
        x = platform1->x+150;
        y = platform1->y-r;
        v=11.0;
        setTheta( ((rand()%100 + 40)*M_PI)/180 );
        phaze = 0;
        if(lives == 0){
            transition = true;
            transition_sequence=1;
            return;
        }
        //cout<<lives<<endl;
        saveGame();
        return;
    }
    double m,mp,h,hp;
    m = (double)(newy-y)/(newx-x);
    h = y - m*x;
    for(int i=0;i<bricks.size();i++){
        for(int j=0;j<6;j++){
            if( xinA(x,bricks[i]->x+13,bricks[i]->x+137) && xinA(y,bricks[i]->y,bricks[i]->y+50) ){   // BUG
                if(vy < 0){
                    y = bricks[i]->y-r-1;
                    setVy(-vy);
                    //cout<<"fault down"<<endl;
                    // =============================== collision
                    int stre = bricks[i]->strength;
                    if(stre <= damage){
                        Brick* temp = bricks[i];
                        bricks.erase(bricks.begin()+i);
                        delete temp;
                        damage-=stre;
                        broken_bricks++;
                    }else{
                        bricks[i]->strength -= damage;
                        damage -= stre;
                    }
                    if(damage <= 0){
                        score+=level*broken_bricks;
                        broken_bricks=0;
                        level++;
                        x = platform1->x+150;
                        y = platform1->y-r;
                        setTheta( ((rand()%100 + 40)*M_PI)/180 );
                        damage = level;
                        animate=250;//animate the blocks
                        generateBricks();
                        phaze=0;
                    }
                    // ============================== /collision
                    return;
                }
                y = bricks[i]->y+51+r;
                setVy(-vy);
                //cout<<"fault up"<<endl;
                // =============================== collision
                    int stre = bricks[i]->strength;
                    if(stre <= damage){
                        Brick* temp = bricks[i];
                        bricks.erase(bricks.begin()+i);
                        delete temp;
                        damage-=stre;
                        broken_bricks++;
                    }else{
                        bricks[i]->strength -= damage;
                        damage -= stre;
                    }
                    if(damage <= 0){
                        score+=level*broken_bricks;
                        broken_bricks=0;
                        level++;
                        x = platform1->x+150;
                        y = platform1->y-r;
                        setTheta( ((rand()%100 + 40)*M_PI)/180 );
                        damage = level;
                        animate=250;//animate the blocks
                        generateBricks();
                        saveGame();
                        phaze=0;
                    }
                    // ============================== /collision
                return;
            }
            mp = (double)(bricks[i]->lines[j]->p2.y - bricks[i]->lines[j]->p1.y)/(bricks[i]->lines[j]->p2.x - bricks[i]->lines[j]->p1.x);
            hp = bricks[i]->lines[j]->p1.y - mp*bricks[i]->lines[j]->p1.x;
            double xans = -(h-hp)/(m-mp);
            if( xinA(xans,x,newx) && xinA(xans,bricks[i]->lines[j]->p1.x,bricks[i]->lines[j]->p2.x) ){
                double alpha = bricks[i]->lines[j]->angle;
                setTheta( 2*alpha - theta );
                x+=vx;
                y-=vy;
                // =============================== collision
                    int stre = bricks[i]->strength;
                    if(stre <= damage){
                        Brick* temp = bricks[i];
                        bricks.erase(bricks.begin()+i);
                        delete temp;
                        damage-=stre;
                        broken_bricks++;
                    }else{
                        bricks[i]->strength -= damage;
                        damage -= stre;
                    }
                    if(damage <= 0){
                        score+=level*broken_bricks;
                        broken_bricks=0;
                        level++;
                        x = platform1->x+150;
                        y = platform1->y-r;
                        setTheta( ((rand()%100 + 40)*M_PI)/180 );
                        damage = level;
                        animate=250;//animate the blocks
                        generateBricks();
                        saveGame();
                        phaze=0;
                    }
                    // ============================== /collision
                return;
            }
        }
    }
    for(int i=0;i<3;i++){
        mp = (double)(platform1->lines[i]->p1.y - platform1->lines[i]->p2.y)/(platform1->lines[i]->p1.x - platform1->lines[i]->p2.x);
        hp = platform1->lines[i]->p1.y - mp*platform1->lines[i]->p1.x;
        double xans = -(h-hp)/(m-mp);
        if( xinA(xans,x,newx) && xinA(xans,platform1->lines[i]->p1.x ,platform1->lines[i]->p2.x ) ){
            double alpha = platform1->lines[i]->angle;
            setTheta( 2*alpha - theta );
            //if(i!=0) {setVx(vx+(platform1->v/3));}
            x+=vx;
            y-=vy;
            return;
        }
    }
    x=newx;
    y=newy;
}



void b1_s0p0_action(){
    if(tb1_s0p0->s.size() <3) return;
    ifstream fin("players/players.save");
    string temp;
    if(!fin.good()){
        fin.close();
        system("mkdir players");
        ofstream fout("players/players.save");
        fout<<tb1_s0p0->s<<endl;
        fout.close();
        temp = "cd players && mkdir \""+tb1_s0p0->s+"\"";
        system(temp.c_str());
        dirName = tb1_s0p0->s;
        phaze =1;
        return;
    }
    bool flag = false;
    while(getline(fin,temp)){
        if(temp == tb1_s0p0->s)
            {flag=true; break;}
    }
    if(!flag){
        ofstream fout("players/players.save",ios::app);
        fout<<tb1_s0p0->s<<endl;
        fout.close();
        temp = "cd players && mkdir \""+tb1_s0p0->s+"\"";
        system(temp.c_str());
        dirName = tb1_s0p0->s;
    }
    load_dirs.clear();
    delete[] dirList_s0p1;
    _dirList_s0p1 =0;
    dirList_s0p1 = new Button*[5];
    read_directory("players/"+tb1_s0p0->s,load_dirs);
    //cout<<load_dirs.size();
    for(int i=0;i<load_dirs.size();i++){
        string r=load_dirs[i];
        if(r.size()>4){
            if(r.substr(r.size()-4,4) == "save"){
                Button* temp1 = new Button(m_renderer,r,200,200+_dirList_s0p1*80,200,60);
                dirList_s0p1[_dirList_s0p1++] = temp1;
                //temp1->isIdle = true;
                //elementsS0P1.push_back(temp1);
            }
        }
    }
    dirName = tb1_s0p0->s;
    fin.close();
    phaze =1;
}
void bNew_s0p1_action(){

    int n = load_dirs.size();
    if(n-2>4){
        return;
    }
    if(saveFileName != ""){
        saveFileName ="";
        selector1.draw = false;
    }
    ostringstream ss;
    string path = "players/"+dirName+"/";
    ss<<path<<"game"<<n-1<<".save";
    ofstream fout(ss.str().c_str());
    fout<<"empty";
    fout.close();
    load_dirs.clear();
    delete[] dirList_s0p1;
    _dirList_s0p1 =0;
    dirList_s0p1 = new Button*[5];
    read_directory(path,load_dirs);
    for(int i=0;i<load_dirs.size();i++){
        string r=load_dirs[i];
        if(r.size()>4){
            if(r.substr(r.size()-4,4) == "save"){
                Button* temp1 = new Button(m_renderer,r,200,200+_dirList_s0p1*80,200,60);
                dirList_s0p1[_dirList_s0p1++] = temp1;
                //temp1->isIdle = true;
                //elementsS0P1.push_back(temp1);
            }
        }
    }
}

void bLoad_s0p1_action(){
    if(saveFileName == ""){
        return;
    }
    string path = "players/"+dirName+"/"+saveFileName;
    ifstream fin(path.c_str(),ios::in);
    string line;
    fin>>line;
    if(line=="empty"){
        ofstream fout(path.c_str());
        level=1;
        score=0;
        lives=3;
        ball->damage=1;
        fout<<"ok"<<endl<<level<<endl<<score<<endl<<lives<<endl;
        int n=0;
        int A[7];
        while(n<4){
            n=0;
            for(int i=0;i<7;i++){
                A[i] = rand()%2;
                n+=A[i];
            }
        }
        fout<<n<<endl;
        for(int i=0;(i<7);i++){
                if(A[i] == 0 ) continue;
            int texnum = rand()%_brickTextures;
            Brick* temp = new Brick(20+i*170,20,1,brickTextures[texnum]);
            temp->texnum = texnum;
            bricks.push_back(temp);
            fout<<temp->x<<" "<<temp->y<<" "<<temp->maxHP<<" "<<temp->strength<<" "<<texnum<<endl;
        }
    }else{
        int n;
        fin>>level>>score>>lives>>n;
        ball->damage = level;
        Brick*  temp;
        int f;
        for(int i=0;i<n;i++){
            temp=new Brick;
            fin>>temp->x>>temp->y>>temp->maxHP>>temp->strength>>temp->texnum;
            temp->texture = brickTextures[temp->texnum];
            temp->visible=true;
            temp->calcHitBox();
            bricks.push_back(temp);
        }
    }
    stage=1;
    phaze=0;
}
void bBack_s0p1_action(){
    saveFileName = "";
    dirName = "";
    selector1.draw=false;
    phaze =0;
}
void bBack_s1_action(){
    pauseP=false;
    while(bricks.size()>0){
        Brick* temp = bricks[0];
        bricks.erase(bricks.begin());
        delete temp;
    }
    stage =0;
    phaze=1;
}
void bCont_s1_action(){
    pauseP=false;
}

void bClear_s0p1_action(){
    if(saveFileName == ""){
        return;
    }
    string path = "players/"+dirName+"/"+saveFileName;
    ofstream fout(path.c_str());
    fout<<"empty";
}

void bRet_s2_action(){
    disp=false;
    animate =0;
    saveFileName="";
    selector1.draw = false;
    bricks.clear();
    lives=3;
    score=0;
    level=1;
    stage = 0;
    phaze=1;
}


void TERMINATOR(){
    for(int i=0;i<bricks.size();i++){
        if(bricks[i]->y > 700){
            flasher = bricks[i];
            transition = true;
            return;
        }
    }
}

int main( int argc, char * argv[] )
{
    //Initialization of SDL windows
    srand(time(NULL));
    Uint32 SDL_flags = SDL_INIT_VIDEO | SDL_INIT_TIMER;
    Uint32 WND_flags = SDL_WINDOW_SHOWN;// | SDL_WINDOW_FULLSCREEN_DESKTOP;//SDL_WINDOW_BORDERLESS ;
    SDL_Window * m_window;
    //Texture for loading image
    SDL_Init( SDL_flags );
    SDL_CreateWindowAndRenderer( 1200, 800, WND_flags, &m_window, &m_renderer );
    //Pass the focus to the drawing window
    SDL_RaiseWindow(m_window);
    //Get screen resolution
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int W = DM.w;
    int H = DM.h;
    // Clear the window with a black background
    SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, 255 );
    SDL_RenderClear( m_renderer );
    // Show the window
    SDL_RenderPresent( m_renderer );
    SDL_Event* e = new SDL_Event();
    TTF_Init();
    selector1.draw=false;
    saveFileName ="";
    dirName="";
    animate=0;

    //load resources
    FONT_arial_60 = TTF_OpenFont("c:\\PROGRA~2\\CodeBlocks\\share\\CodeBlocks\\assets\\arial.ttf", 60 );
    FONT_arial_30 = TTF_OpenFont("c:\\PROGRA~2\\CodeBlocks\\share\\CodeBlocks\\assets\\arial.ttf", 30 );
    FONT_arial_18 = TTF_OpenFont("c:\\PROGRA~2\\CodeBlocks\\share\\CodeBlocks\\assets\\arial.ttf", 18 );
    //initialize Elements
    //s0p0
    tb1_s0p0 = new TextBox(m_renderer,"player name",388,350,400,60);
    tb1_s0p0->focus=true;
    elementsS0P0.push_back(tb1_s0p0);

    b1_s0p0 = new Button(m_renderer,"Play!",538,460,100,50);
    b1_s0p0->action = &b1_s0p0_action;
    elementsS0P0.push_back(b1_s0p0);

    //s0p1
    dirList_s0p1 = new Button*[5];

    bNew_s0p1 = new Button(m_renderer,"New",755,200,180,80);
    bNew_s0p1->focus=true;
    bNew_s0p1->action = &bNew_s0p1_action;
    elementsS0P1.push_back(bNew_s0p1);

    bClear_s0p1 = new Button(m_renderer,"Clear",755,365,180,80);
    bClear_s0p1->action = &bClear_s0p1_action;
    elementsS0P1.push_back(bClear_s0p1);

    bLoad_s0p1 = new Button(m_renderer,"Load",755,530,180,80);
    bLoad_s0p1->action = &bLoad_s0p1_action;
    elementsS0P1.push_back(bLoad_s0p1);

    bBack_s0p1 = new Button(m_renderer,"Back",50,45,150,60);
    bBack_s0p1->action = &bBack_s0p1_action;
    elementsS0P1.push_back(bBack_s0p1);


    bBack_s1 = new Button(m_renderer,"Return to menu",610,420,230,60);
    bBack_s1->action = &bBack_s1_action;
    elementsS1p.push_back(bBack_s1);

    bCont_s1 = new Button(m_renderer,"Back to game",360,420,230,60);
    bCont_s1->focus=true;
    bCont_s1->action = &bCont_s1_action;
    elementsS1p.push_back(bCont_s1);

    bRet_s2 = new Button(m_renderer,"Return to menu",450,550,300,60);
    bRet_s2->focus = true;
    bRet_s2->action = &bRet_s2_action;


    //s1
    img_block_dark_1 = myLoadImage("brick_dark1.png",150,50);
    img_block_light_1 = myLoadImage("brick_light1.png",150,50);
    img_block_dark_2 = myLoadImage("brick_dark2.png",150,50);
    img_block_light_2 = myLoadImage("brick_light2.png",150,50);
    img_block_dark_3 = myLoadImage("brick_dark3.png",150,50);
    img_block_light_3 = myLoadImage("brick_light3.png",150,50);
    brickTextures[0] = img_block_dark_1;
    brickTextures[1] = img_block_light_1;
    brickTextures[2] = img_block_dark_2;
    brickTextures[3] = img_block_light_2;
    brickTextures[4] = img_block_dark_3;
    brickTextures[5] = img_block_light_3;

    img_background = myLoadImage("bg.jpg",1200,800);
    img_platform = myLoadImage("platform.png",300,30);
    img_ball = myLoadImage("ball.png",24,24);
    img_arrow = myLoadImage("arrow.png",18,100);
    img_background_intro = myLoadImage("bgintro.jpeg",1200,800);
    img_background_menu1 = myLoadImage("bgmenu.png",1200,800);
    img_background_menu2 = myLoadImage("bgmenu2.jpg",1200,800);
    img_background_valve = myLoadImage("valve.jpg",1200,800);
    img_background_gameover = myLoadImage("gobg.jpg",1200,800);
    img_cinema = myLoadImage("cinema.png",1200,800);

    platform1 = new Platform(500,740,img_platform);
    ball = new Ball(650,725,12,11.0, ((rand()%100 + 40)*M_PI)/180  ,1,img_ball);

    //prog-----------------------------------------start

    int delay = 10;
    bool curr=false;
    bool internal = false;
    KeyboardEventHandler KK;
    int frameN =0;
    int localDelay =0;
    char currentKey = '\0';
    Texture* tempText = NULL;
    ostringstream ss;
    //intro
    int NN=0;
    double auxy = -800;
    double auxv=1;
    //goto A;
    while(NN<800){
        SDL_PollEvent(e);
        SDL_SetRenderDrawColor(m_renderer,0,0,0,255);
        SDL_RenderClear(m_renderer);

        myDrawImage(img_background_valve,0,0);
        myDrawImage(img_cinema,0,-200+5*(NN%80) );

        if(NN<255){
            boxRGBA(m_renderer,0,0,1200,800,0,0,0,255-NN);
        }
        if(NN>545){
            boxRGBA(m_renderer,0,0,1200,800,0,0,0,NN-545);
        }
        SDL_RenderPresent(m_renderer);
        SDL_Delay(10);
        NN++;
    }
    delete img_background_valve;
    delete img_cinema;
    NN=0;
    while(NN<600){
        SDL_PollEvent(e);
        SDL_SetRenderDrawColor(m_renderer,0,0,0,255);
        SDL_RenderClear(m_renderer);

        myDrawImage(img_background_intro,0,0);

        if(NN<128){
            boxRGBA(m_renderer,0,0,1200,800,0,0,0,255-2*NN);
        }
        if(NN>472){
            boxRGBA(m_renderer,0,0,1200,800,0,0,0,2*(NN-472));
        }

        SDL_RenderPresent(m_renderer);
        SDL_Delay(5);
        NN++;
    }
    A:
    while(true){ // main cycle
        //events
        SDL_PollEvent(e);
        if(e->type == SDL_KEYDOWN){
            KK.KeyDownHandler(e);
            currentKey = (char)e->key.keysym.sym;
            //cout<<(int)currentKey<<endl;
        }else if(e->type==SDL_KEYUP){
            KK.KeyUpHandler(e);
            currentKey = '\0';
        }else if(e->type==SDL_WINDOWEVENT){
            if(e->window.event == SDL_WINDOWEVENT_CLOSE) break;
        }

        //update
        int index;
        if(stage==0){
            if(phaze == 0){
                for(index=0;index<elementsS0P0.size();index++){
                    if(elementsS0P0[index]->focus){
                        break;
                    }
                }
                if(frameN%50==0){
                    curr = !curr;
                }
                if( (KK.keyState& KeyboardEventHandler::KEY_down) !=0 && localDelay ==0){
                    elementsS0P0[index]->focus = false;
                    elementsS0P0[(index+1)%elementsS0P0.size()]->focus = true;
                    localDelay =13;
                }else if( (KK.keyState& KeyboardEventHandler::KEY_up) !=0 && localDelay ==0){
                    elementsS0P0[index]->focus = false;
                    elementsS0P0[(index-1 <0 ? elementsS0P0.size()-1 : index-1)]->focus = true;
                    localDelay =13;
                }else if( (KK.keyState& KeyboardEventHandler::KEY_enter) !=0 && localDelay ==0 && elementsS0P0[index]->type=="button"){
                    (*elementsS0P0[index]->action)();
                    localDelay=13;
                }else{
                    if(elementsS0P0[index]->type=="textbox" && currentKey != '\0' && localDelay == 0){
                        if((int)currentKey != 8){
                            if(isValid(currentKey) && elementsS0P0[index]->text_focus->mWidth < elementsS0P0[index]->w-50){
                                elementsS0P0[index]->s+=currentKey;
                                elementsS0P0[index]->assign_texture(m_renderer);
                            }
                        }else{
                            string rr = elementsS0P0[index]->s;
                            if(rr.size()>1){
                                elementsS0P0[index]->s = rr.substr(0,rr.size()-1);
                                elementsS0P0[index]->assign_texture(m_renderer);
                            }
                            if(rr.size()==1){
                                elementsS0P0[index]->s = "";
                                //elementsS0P0[index].assign_texture(m_renderer);
                            }
                        }
                        localDelay=10;
                    }
                }
            }
            else if(phaze == 1){
                if(!internal){
                    for(index=0;index<elementsS0P1.size();index++){
                        if(elementsS0P1[index]->focus){
                            break;
                        }
                    }
                }else{
                    for(index=0;index<_dirList_s0p1;index++){
                        if(dirList_s0p1[index]->focus){
                            break;
                        }
                    }
                }
                if(frameN%50==0){
                    curr = !curr;
                }
                if( (KK.keyState& KeyboardEventHandler::KEY_down) !=0 && localDelay ==0){
                    if(internal){
                        dirList_s0p1[index]->focus = false;
                        dirList_s0p1[(index+1)%_dirList_s0p1]->focus = true;
                        localDelay =13;
                    }else{
                        elementsS0P1[index]->focus = false;
                        elementsS0P1[(index+1)%elementsS0P1.size()]->focus = true;
                        localDelay =13;
                    }
                }else if( (KK.keyState& KeyboardEventHandler::KEY_up) !=0 && localDelay ==0){
                    if(internal){
                        dirList_s0p1[index]->focus = false;
                        dirList_s0p1[(index-1 <0 ? _dirList_s0p1-1 : index-1)]->focus = true;
                        localDelay =13;
                    }else{
                        elementsS0P1[index]->focus = false;
                        elementsS0P1[(index-1 <0 ? elementsS0P1.size()-1 : index-1)]->focus = true;
                        localDelay =13;
                    }
                }else if( (KK.keyState& KeyboardEventHandler::KEY_right) !=0 && localDelay ==0 && internal && _dirList_s0p1 !=0){
                    dirList_s0p1[index]->focus=false;
                    elementsS0P1[0]->focus=true;
                    index=0;
                    internal=false;
                }else if( (KK.keyState& KeyboardEventHandler::KEY_left) !=0 && localDelay ==0 && !internal && _dirList_s0p1 !=0){
                    elementsS0P1[index]->focus=false;
                    dirList_s0p1[0]->focus=true;
                    index=0;
                    internal = true;
                }else if( (KK.keyState& KeyboardEventHandler::KEY_enter) !=0 && localDelay ==0 && ((!internal && elementsS0P1[index]->type=="button") || internal) ){
                    if(!internal){
                        //cout<<saveFileName;
                        (*elementsS0P1[index]->action)();
                        //delay = 100;
                    }else{
                        saveFileName = dirList_s0p1[index]->s;
                        selector1.x= dirList_s0p1[index]->x+dirList_s0p1[index]->w+30;
                        selector1.y = dirList_s0p1[index]->y+12;
                        selector1.draw = true;
                    }
                    localDelay=13;
                }
            }
        }
        else if(stage==1){
            for(index=0;index<elementsS1p.size() && pauseP;index++){
                if(elementsS1p[index]->focus){
                    break;
                }
            }
            if(phaze == 0){
                if((KK.keyState & KeyboardEventHandler::KEY_left) !=0 && !pauseP && !transition){
                    if(platform1->x > -50){
                        platform1->x-=pSpeed;
                        ball->x-=pSpeed;
                        platform1->calcHitBox();
                    }
                }else if((KK.keyState & KeyboardEventHandler::KEY_right) !=0 && !pauseP && !transition){
                    if(platform1->x < 1000){
                        platform1->x+=pSpeed;
                        ball->x+=pSpeed;
                        platform1->calcHitBox();
                    }
                }
                if((KK.keyState & KeyboardEventHandler::KEY_up) !=0 && !pauseP && !transition){
                    if(ball->theta*180/M_PI > 40){
                        ball->setTheta(ball->theta-0.05);
                    }
                }else if((KK.keyState & KeyboardEventHandler::KEY_down) !=0 && !pauseP && !transition){
                    if(ball->theta*180/M_PI < 140){
                        ball->setTheta(ball->theta+0.05);
                    }
                }
                if((KK.keyState & KeyboardEventHandler::KEY_space) !=0 && !pauseP && !transition){
                    phaze = 1;
                }
            }
            else if(phaze == 1){
                if((KK.keyState & KeyboardEventHandler::KEY_left) !=0 && !pauseP && !transition){
                    if(platform1->x > -50){
                        platform1->x-=pSpeed;
                        platform1->v=-pSpeed;
                        platform1->calcHitBox();
                    }
                }else{
                    platform1->v = 0;
                }
                if((KK.keyState & KeyboardEventHandler::KEY_right) !=0 && !pauseP && !transition){
                    if(platform1->x < 1000){
                        platform1->x+=pSpeed;
                        platform1->v=pSpeed;
                        platform1->calcHitBox();
                    }
                }else{
                    platform1->v = 0;
                }
                if(!pauseP && !transition) ball->Update();
            }

            if((KK.keyState & KeyboardEventHandler::KEY_esc) !=0 && localDelay ==0  && !transition){
                    pauseP = !pauseP;
                    KK.reset();
                    localDelay =15;
            }
            if(pauseP && !transition){
                if( (KK.keyState& KeyboardEventHandler::KEY_left) !=0 && localDelay ==0  && !transition){
                    elementsS1p[index]->focus = false;
                    elementsS1p[(index+1)%elementsS1p.size()]->focus = true;
                    localDelay =13;
                }else if( (KK.keyState& KeyboardEventHandler::KEY_right) !=0 && localDelay ==0 && !transition){
                    elementsS1p[index]->focus = false;
                    elementsS1p[(index-1 <0 ? elementsS1p.size()-1 : index-1)]->focus = true;
                    localDelay =13;
                }
                if( (KK.keyState& KeyboardEventHandler::KEY_enter) !=0 && localDelay ==0 && !transition){
                    (*elementsS1p[index]->action)();
                    localDelay=15;
                }
            }
            if(animate ==0 && !transition) {TERMINATOR(); NN=0;}
            if(transition){
                if(transition_sequence ==0){
                    if(NN<240){
                        flasher->visible = (NN%60>30 ? true : false);
                        NN++;
                    }else{
                        flasher->visible = true;
                        transition_sequence=1;
                        NN=0;
                    }
                }else if(transition_sequence == 1){
                    if(frameN%8 == 0 &&(NN < bricks.size())) bricks[NN++]->drop(true);
                    if( NN >= bricks.size()-3){
                        //cout<<bricks.size();
                        transition_sequence = 2;
                    }
                }else if(transition_sequence == 2){
                    auxy += auxv;
                    auxv+=0.7;
                    if(auxy>0){
                        auxv = (-1)*auxv*7/10;
                        auxy=0;
                    }
                    if((auxv>0 && abs(auxv)<0.4) && auxy > -10){
                        auxy =0;
                        auxv=0;
                        transition=false;
                        transition_sequence =0;
                        stage =2;
                        string path = "players/"+dirName+"/"+saveFileName;
                        ofstream fout(path.c_str());
                        fout<<"empty";
                        while(bricks.size()>0){
                            Brick* temp = bricks[0];
                            bricks.erase(bricks.begin());
                            delete temp;
                        }
                        NN=0;
                    }
                }
            }
        }
        else if(stage == 2){
            if(NN<150){
                NN++;
            }else{
                disp = true;
            }
            if( (KK.keyState& KeyboardEventHandler::KEY_enter) !=0 && disp){
                auxy=-800;
                (*bRet_s2->action)();
            }
        }

        //draw
        SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, 255 );
        SDL_RenderClear( m_renderer );
        if(stage==0){
            if(phaze == 0){
                myDrawImage(img_background_menu1,0,0);
                for(int i=0;i<elementsS0P0.size();i++){
                    elementsS0P0[i]->draw(m_renderer,curr);
                }
            }
            else if(phaze == 1){
                myDrawImage(img_background_menu2,0,0);
                roundedRectangleRGBA(m_renderer,150,150,500,650,12,40,40,40,255);
                for(int i=0;i<_dirList_s0p1;i++){
                    dirList_s0p1[i]->draw(m_renderer);
                }
                for(int i=0;i<elementsS0P1.size();i++){
                    elementsS0P1[i]->draw(m_renderer,curr);
                }
                if(selector1.draw){
                    boxRGBA(m_renderer,selector1.x,selector1.y,selector1.x+30,selector1.y+30,20,20,20,200);
                }
            }
        }
        else if(stage==1){
            do{
                    int r = animate;
            if(animate>0){
                    SDL_PollEvent(e);
                    SDL_SetRenderDrawColor(m_renderer,0,0,0,255);
                    SDL_RenderClear(m_renderer);
            }
            myDrawImage(img_background,0,0);
            if(phaze == 0){
                myDrawImageEx(img_arrow,ball->x-9,ball->y-120,-ball->theta*180/M_PI + 90,{9,120});
            }
            for(int i=0;i<bricks.size();i++){
                if(animate>0) bricks[i]->y = bricks[i]->prevy + (250-animate)*70/250;
                if(animate == 1)bricks[i]->calcHitBox();
                if(transition) bricks[i]->drop();
                bricks[i]->Draw();
            }
            ball->Draw();
            platform1->Draw();
            boxRGBA(m_renderer,0,770,1200,800,100,193,223,220);
            ss<<"LEVEL:  "<<level<<"  |  SCORE:  "<<score<<"  |  LIVES:  "<<lives<<"  |  DAMAGE:"<<ball->damage<<"  |  BROKEN BRICKS:"<<broken_bricks;
            string ttt = ss.str();
            if(tempText != NULL) SDL_DestroyTexture(tempText->text);
            tempText = CreateTextTexture(m_renderer,ttt.c_str(),FONT_arial_18,255,255,0,0);
            DrawText(m_renderer,tempText,300,780);
            delete tempText;
            if(animate >0){
                tempText = CreateTextTexture(m_renderer,"LEVEL UP!",FONT_arial_60,255,255,0,0);
                DrawText(m_renderer,tempText,600 - tempText->mWidth/2 ,400,(75*SDL_sin(((double)(250-animate)/250)*4*M_PI))/10);
                delete tempText;
            }
            ss.str("");
            if(animate==1){
                saveGame();
                KK.reset();
            }
            if(animate>0){
                animate--;
                SDL_RenderPresent(m_renderer);
                SDL_Delay(3);
            }
            }while(animate>0);
            if(pauseP && !transition){
                roundedBoxColor(m_renderer,350,250,850,550,20,0xFFEEEEDD);
                roundedRectangleColor(m_renderer,350,250,850,550,20,0xFF0000FF);
                tempText = CreateTextTexture(m_renderer,"Paused!",FONT_arial_60,255,210,150,100);
                DrawText(m_renderer,tempText,600 - tempText->mWidth/2 ,260,(75*SDL_sin(((double)(frameN*delay)/1000)*2*M_PI))/10);
                delete tempText;
                for(int i=0;i<elementsS1p.size();i++){
                    elementsS1p[i]->draw(m_renderer);
                    if(elementsS1p[0]->focus){
                        tempText = CreateTextTexture(m_renderer,"game is only saved at the begining of each level!",FONT_arial_18,255,255,40,40);
                        DrawText(m_renderer,tempText,600 - tempText->mWidth/2 ,340);
                        delete tempText;

                        tempText = CreateTextTexture(m_renderer,"unsaved progress will be lost!",FONT_arial_18,255,255,40,40);
                        DrawText(m_renderer,tempText,600 - tempText->mWidth/2 ,365);
                        delete tempText;
                    }
                }
                delete tempText;
            }
            if(transition_sequence == 2){
                myDrawImage(img_background_gameover,0,auxy);
            }
        }
        else if(stage == 2){
            myDrawImage(img_background_gameover,0,0);
            if(disp){
                bRet_s2->draw(m_renderer);
            }
        }
        SDL_RenderPresent( m_renderer );
        SDL_Delay(delay);
        frameN = (frameN+1)%(1000/delay);
        if(localDelay >0) localDelay--;
    }

    //prog-------------------------------------------end
    //Wait for a key to be pressed
    //while(e->type != SDL_KEYDOWN)
    //SDL_PollEvent(e);

    //Finalize and free resources
    //SDL_DestroyTexture(m_img);
    SDL_DestroyWindow( m_window );
    SDL_DestroyRenderer( m_renderer );
	IMG_Quit();
	SDL_Quit();
    return 0;
}
