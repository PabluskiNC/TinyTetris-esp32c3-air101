#include <Arduino.h>

//#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
//#include <User_Setup.h>
//#include <SPI.h>

#define offsetx   0   //pixel offset X
#define offsety  16   //pixel offset Y
#define blockSize 8   //block size
#define fieldx   10   //how many blocks wide (Standard is 10)
#define fieldy   18  //how many blocks tall (Standard is 20)




int piece[7][4][2][4]={{{{0,1,0,1},{0,0,1,1}},{{0,1,0,1},{0,0,1,1}},{{0,1,0,1},{0,0,1,1}},{{0,1,0,1},{0,0,1,1}}},
                        {{{0,-1,0,1},{0,0,1,1}},{{0,1,1,0},{0,0,-1,1}},{{0,-1,0,1},{0,0,1,1}},{{0,1,1,0},{0,0,-1,1}}},
                        {{{0,-1,0,1},{0,0,-1,-1}},{{0,0,1,1},{0,-1,0,1}},{{0,-1,0,1},{0,0,-1,-1}},{{0,0,1,1},{0,-1,0,1}}},
                        {{{1,0,-1,1},{0,0,0,-1}},{{0,-1,0,0},{0,0,1,2}},{{0,1,2,0},{0,0,0,1}},{{1,0,0,0},{1,1,0,-1}}},
                        {{{1,0,0,0},{-1,-1,0,1}},{{1,1,0,-1},{1,0,0,0}},{{-1,0,0,0},{1,1,0,-1}},{{0,0,1,2},{-1,0,0,0}}},
                        {{{1,0,-1,0},{0,0,0,1}},{{0,0,0,-1},{-1,0,1,0}},{{1,0,-1,0},{0,0,0,-1}},{{0,0,0,1},{-1,0,1,0}}},
                        {{{0,0,0,0},{-1,0,1,2}},{{-1,0,1,2},{0,0,0,0}},{{0,0,0,0},{-1,0,1,2}},{{-1,0,1,2},{0,0,0,0}}}};
// piece[block][rot][y/x][4 spots]
int score;
byte multiplier;
byte digit[10][7];
#define dLength 4
const int cLength = dLength*7;
int pc;
int rot;
int block;
int color;
int level;
byte music;
long unsigned speed;
long unsigned timez;
long unsigned downHold;
boolean collide;
boolean game;
boolean release;
int posX;
int posY;
int a, b, c, f, g, h,i,j,k;
int linePos[4];
int linez;
int blockposY;
int blockposX;
int lineCount;
  
byte field[fieldy][fieldx];

  

//Libs
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

//Comunication pins for SPI
#define TFT_SCLK 2   //Pin_SCK
#define TFT_MOSI 3   //Pin_SDA
#define TFT_RST 10   //Pin_Res
#define TFT_DC   6   //Pin_DC
#define TFT_CS   7   //Pin_CS

//Define screen size
#define SCREEN_WIDTH   80
#define SCREEN_HEIGHT 160

//Dfone the Joystick pins
#define BUTTON_LEFT   8   //Left
#define BUTTON_UP     9   //Up
#define BUTTON_CENTER 4   //Center
#define BUTTON_DOWN   5   //Down
#define BUTTON_RIGHT 13   //Right

//TFT screen object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

int pins[5]={BUTTON_RIGHT, BUTTON_LEFT, BUTTON_CENTER, BUTTON_DOWN, BUTTON_UP};
boolean buttons[5];
boolean hold[5];

int stage;

void setup() {
  //delay(5000);
  posX=4;
  posY=4;
  Serial.begin(115200);
  Serial.println("Starting");
  tft.initR(INITR_MINI160x80_PLUGIN);
  tft.setRotation(2); // 180 degrees

  for (int a = 0; a < 5; a++) {
    pinMode(pins[a], INPUT_PULLUP);
  }

  Serial.print("screen width: ");
  Serial.print(tft.width());
  Serial.println("");
  Serial.print("screen height: ");
  Serial.print(tft.height());
  Serial.println("");

  tft.fillScreen(ST77XX_BLACK);
}

void readInputs() {
  for (int d=0; d < 5; d++) {
    buttons[d]=0;
  }
  for (int d=0; d < 5; d++) {
    if (!digitalRead(pins[d])){
      buttons[d] = 1;
      break;
    }
  }
}




//////////////


void bloff(int y,int x, int brightz){
  tft.fillRect((x*blockSize)+offsetx,(y*blockSize)+offsety,blockSize-1,blockSize-1,2113*brightz);
}
void blockDraw(int y, int x, int colorz){
  colorz%=96;
  int red=0;
  int gre=0;
  int blu=0;
  if(colorz<32){
    red=31-colorz;
    gre=colorz;
  }
  if(colorz>=32&&colorz<64){
    colorz-=32;
    gre=31-colorz;
    blu=colorz*2;
  }
  if(colorz>=64){
    colorz-=64;
    blu=(31-colorz)*2;
    red=colorz;
  }
  int returnz=(red*2048)+(gre*64)+blu;
  int posx=(x*blockSize)+offsetx;
  int posy=(y*blockSize)+offsety;
  tft.drawRect(posx,posy,blockSize-1,blockSize-1,returnz);
  red/=2;
  gre/=2;
  blu/=2;
  returnz=(red*2048)+(gre*64)+blu;
  tft.fillRect(posx+1,posy+1,blockSize-3,blockSize-3,returnz);
  tft.drawPixel(posx,posy,ST7735_WHITE);
  tft.drawPixel(posx+1,posy+1,48599);
  tft.drawPixel(posx+2,posy+1,48599);
  tft.drawPixel(posx+1,posy+2,48599);
}

void showField(int startz, int finish,boolean showpiece){
  startz=max(0,startz);
  finish=min(fieldy,finish);
  for(f=startz;f<finish;f++){
    for(g=0;g<fieldx;g++){
      int colorz=field[f][g];
      if(colorz){blockDraw(f,g,colorz);}
      if(!colorz){bloff(f,g,0);}
    }
  }
  if(showpiece){
    for(f=0;f<4;f++){ // grid max
      blockposY=posY+piece[block][rot][0][f];
      blockposX=posX+piece[block][rot][1][f];

      blockDraw(blockposY,blockposX,color);
    }
  }
}

void clearLines(){
  linez=0;
  for(a=0;a<fieldy;a++){
    byte counter=0;
    for(b=0;b<fieldx;b++){
      counter+=field[a][b]>0;
    }
    if(counter==fieldx){
      linePos[linez]=a;
      linez++;
    }
  }
  if(linez==1){
    level++;
    score+=multiplier;
    for(a=0;a<fieldx;a++){
      for(b=31;b>=0;b-=1){
        bloff(linePos[0],a,b);
        delay(1);
      }
    }
    delay(200);
  }
  
  if(linez==2){
    level+=2;
    score+=3*multiplier;
    for(a=0;a<fieldx;a++){
      bloff(linePos[0],a,31);
      bloff(linePos[1],a,31);
      delay(50);
    }
    for(a=0;a<50;a++){
      bloff(linePos[random(2)],random(fieldx),0);
      delay(15);
    }
  }
  
  if(linez==3){
    level+=3;
    score+=8*multiplier;
    for(a=0;a<fieldx;a++){
      for(b=0;b<3;b++){
        blockDraw(linePos[b],a,(10*b));
      }
      delay(50);
    }
    for(a=0;a<fieldx;a++){
      for(b=0;b<3;b++){
        bloff(linePos[b],a,0);
      }
      delay(50);
    }
  }
  
  if(linez==4){
    level+=4;
    score+=20*multiplier;
    for(a=0;a<192;a+=1){
      for(b=linePos[0];b<linePos[3]+1;b++){
        for(c=0;c<fieldx;c++){
          blockDraw(b,c,(3*c)+(b*8)+a);
        }
      }
      delay(10);
    }
  }
  for(a=0;a<linez;a++){  //clear
    for(c=0;c<fieldx;c++){
      field[linePos[a]][c]=0;
    }
    for(b=linePos[a];b>0;b--){
      for(c=0;c<fieldx;c++){
        field[b][c]=field[b-1][c];
      }
    }
  }
  lineCount+=linez;
  if(lineCount>9){
    lineCount-=10;
    (speed*4)/5;
  }
  showField(0,fieldy,0);
  if(level>9){
    stage++;
    level-=10;
    speed=((speed*4)/5);
  }
  for(int a=0;a<linez;a++){
    for(int b=0;b<fieldx;b++){
      field[a][b]=0;
    }
  }
  timez=millis();
}

boolean test(int y, int x){
  boolean hitz=0;
  int blockposY[4];
  int blockposX[4];
  
  for(a=0;a<4;a++){ // grid max
    blockposY[a]=posY+y+piece[block][rot][0][a];
    if(blockposY[a]<0){hitz=1;}
    if(blockposY[a]>(fieldy-1)){hitz=1;}
    blockposX[a]=posX+x+piece[block][rot][1][a];
    if(blockposX[a]<0){hitz=1;}
    if(blockposX[a]>(fieldx-1)){hitz=1;}
  }
  
  for(a=0;a<4;a++){ // collision
    int collide=field[blockposY[a]][blockposX[a]];
    if(collide>0){hitz=1;}
  }
  return(!hitz);
}

void newPiece(boolean setPiece){
  if(setPiece){
    for(a=0;a<4;a++){
      field[posY+piece[block][rot][0][a]][posX+piece[block][rot][1][a]]=color;
    }
    clearLines();
  }
  posY=1;
  posX=4;
  rot=0;
  block=random(7);
  color=random(96)+1;
  showField(0,20,1);
  game=test(0,0);
}

void moveLeft(){
  posX-=test(0,-1);
  showField(posY-2,posY+3,1);
}

void moveDown(){
  if(test(1,0)){
    posY++;
    showField(posY-3,posY+3,1);
  } else{
    collide=0;
  }
  timez=millis();
}

void moveRight(){
  posX+=(test(0,1));
  showField(posY-2,posY+3,1);
}

void plummet(){
  multiplier=2;posY;
  while(collide){
    moveDown();
    delay(12);
  }
  timez-=speed;
}

void rotateLeft(){
  rot+=4;
  rot--;
  rot%=4;
  if(!test(0,0)){
    rot++;
    rot%=4;
  }
  showField(posY-2,posY+3,1);
}

void rotateRight(){
  rot++;
  rot%=4;
  if(!test(0,0)){
    rot+=4;
    rot--;
    rot%=4;
  }
  showField(posY-2,posY+3,1);
}

void wait(){
  timez+=speed;
  while(millis()<(timez+speed)){
    if(millis()>(downHold+150)){hold[1]=0;}
    for(int d=0;d<5;d++){
      buttons[d]=0;
      if(!digitalRead(pins[d])&&!hold[d]){
        buttons[d]=1;
        hold[d]=1;
        delay(25);
      }
      if(digitalRead(pins[d])&&hold[d]){
        hold[d]=0;
        delay(25);
      }
    }
    if(buttons[0]){moveLeft();}
    if(buttons[4]){;moveDown();downHold=millis();}
    if(buttons[1]){moveRight();}
    if(buttons[2]){plummet();}
    if(buttons[3]){rotateLeft();}
    //if(buttons[4]){rotateRight();}
 //   if(buttons[6]){pause();}
 //   if(buttons[7]){reset();}
  }
}

void scoreBoardWrite(int scorez){
  tft.setCursor(5,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  //tft.drawString("Score: :"+String(scorez),5,231);
  tft.print("Score: :"+String(scorez));
}

void loop(){
  speed=250;
  timez=millis();
  score=0;
  stage=0;
  newPiece(0);
  game=1;
  showField(0,fieldy,1);
  while(game){
    collide=1;
    multiplier=1;
    scoreBoardWrite(score);
    while(collide){
      wait();
      if(collide){moveDown();}
    }
    newPiece(1);
  }
  for(a=0;a<fieldy;a++){
    for(b=0;b<fieldx;b++){
      field[a][b]=0;
    }
  }
  delay(5000);
}
