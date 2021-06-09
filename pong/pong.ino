#include <Arduino.h>
//#include <Tone432.h>
//#include <Tone434.h>
//#include <Tone436.h>
//#include <Tone438.h>
#include <Tone440.h>
//#include <Tone442.h>
//#include <Tone444.h>
//#include <Tone446.h>
//#include <ToneDefault.h>
//#include <ToneUndef.h>
//#include <ToneVerify.h>
#define TONE_USE_INT
#define TONE_PITCH 440

#include <Pitch.h>

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#include <TouchScreen.h>
#include <BetterJoystick.h>


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MINPRESSURE 200
#define MAXPRESSURE 1000

int x = 0;
int y = 0;
int pressed;
int scale = 8;

int screenWidth = 320;
int screenHeight = 480;

int playerWidth = 100; 
int playerHeight = 25; 

class Player
{
// private variables.
public:
    int x, y, score;
 

    // parametrized constructor
    Player(int cx, int cy, int cscore)
    {
        x = cx;
        y = cy;
        score = cscore;
    }
    
    void display() 
    { 
      checkOnScreen();
      
      tft.fillRect(x + playerWidth, y, 10, 25, BLACK);
      tft.fillRect(x - 10, y, 10, 25, BLACK);
      
      tft.fillRect(x, y, playerWidth, playerHeight, WHITE);
    }

    void checkOnScreen()
    {
      int maxX = screenWidth - playerWidth; 
       
      if (x < 0) { x = 0; }
      if (x > maxX) { x = maxX; }
    }
};

Player player1 = Player(0,0,0);
Player player2 = Player(10,screenHeight - playerHeight,0);


void resetBalls();

int ballSize = 10;
int numberOfBalls = 1; 

class Ball
{
public:
    int x, y, xVel, yVel;
 

    // parametrized constructor
    Ball(int cx, int cy, int cxVel, int cyVel)
    {
        x = cx;
        y = cy;
        xVel = cxVel;
        yVel = cyVel;
    }
    
    void display() 
    { 
      tft.fillRect(x + ballSize, y, 5, 5 + ballSize, BLACK);
      tft.fillRect(x - 5, y, 5, 5 + ballSize, BLACK);
      
      tft.fillRect(x - 5, y - 5, 10 + ballSize, 5, BLACK);
      tft.fillRect(x, y + ballSize, 5 + ballSize, 5, BLACK);
    
      tft.fillRect(x, y, ballSize, ballSize, WHITE);

      move();
      checkCollisions();
    }

    void move()
    {
      x += xVel;
      y += yVel;
    }

    void checkCollisions()
    {
      if (x <= 0 || x > screenWidth - ballSize)
      {
        xVel *= -1;
      }
    
      if (y <= player1.y + playerHeight && x >= player1.x && x <= player1.x + playerWidth)
      {
        yVel *= -1.1;
        if (numberOfBalls < 10)
        {
          numberOfBalls++;
        }
        
      }
    
      if (y >= player2.y - ballSize && x >= player2.x && x <= player2.x + playerWidth)
      {
        yVel *= -1.1;
         if (numberOfBalls < 10)
        {
          numberOfBalls++;
        }
      }
    
      if(y < 0)
      {
        tft.fillRect(x, y, ballSize, ballSize, BLACK);
        y = screenHeight/2;
        xVel = rand() % 3 + 1;

        resetBalls();
      }
    
      if(y > screenHeight)
      {
        tft.fillRect(x, y, ballSize, ballSize, BLACK);
        y = screenHeight/2;
        xVel = rand() % 3 + 1;

        resetBalls();
      }
    }
};

// Ball ball = Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2);
// Ball ball2 = Ball(screenWidth/2 + 30, screenHeight/2, rand() % 3 + 1, -2);
// Ball ball3 = Ball(screenWidth/2 - 30, screenHeight/2, rand() % 3 + 1, -2);



Ball allBalls[10] = {
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2), 
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2), 
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2), 
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2), 
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2), 
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2), 
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2), 
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2), 
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2), 
  Ball(screenWidth/2, screenHeight/2, rand() % 3 + 1, 2)}; 
 
void resetBalls()
{
  numberOfBalls = 1; 

  for(int i = 0; i < 9; i++) 
  {
    // tft.fillRect(0, 0, screenWidth, screenHeight, BLACK);

    allBalls[i].x = screenWidth/2;
    allBalls[i].y = screenHeight/2;
    allBalls[i].xVel = rand() % 3 + 1;
  }

}

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 6, XM = A2, YP = A1, YM = 7; //ID=0x9341
const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button on_btn, off_btn;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}


bool autopilot = false; 
int autopilotTracker = 0;
Joystick joystick(A9, A10, A8);

const int aButton = A14;     
const int bButton = A15;     

int aButtonState = 0;
int bButtonState = 0;
int joystickButtonState = 0;
int previousAButtonState = LOW;
int previousBButtonState = LOW;
int previousJoystickButtonState = LOW;

const int speakerPin = A13;

unsigned long lastPeriodStart;
const float onDuration = 1000;
float periodDuration = 1000 / 2;
int currentNoteIndex = 0;

int melody[] = {
NOTE_D4, 
NOTE_D4, 
NOTE_D4,

NOTE_G4, 
NOTE_D5,

NOTE_C5, 
NOTE_B4, 
NOTE_A4,

NOTE_G5, 
NOTE_D5,

NOTE_C5, 
NOTE_B4, 
NOTE_A4,

NOTE_G5, 
NOTE_D5,

NOTE_C5, 
NOTE_B4,
NOTE_C5,

NOTE_A4

};



float noteDurations[] = {
  12,12,12, 
  2,2,
  12,12,12, 
  2,4, 
  12,12,12, 
  2,4,
  12,12,12, 
  0.5
};

int noteArraySize = sizeof(melody) / sizeof(melody[0]);

void setup(void)
{
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486; // write-only shield
  tft.begin(ID);
  tft.setRotation(0);            //PORTRAIT
  tft.fillScreen(BLACK);

  pinMode(aButton, INPUT);
  pinMode(bButton, INPUT);
  pinMode(speakerPin, OUTPUT); 
}

void loop(void)
{
  aButtonState = digitalRead(aButton);
  bButtonState = digitalRead(bButton);

  if (aButtonState == HIGH && previousAButtonState == LOW) { previousAButtonState = HIGH; }
  else { previousAButtonState = aButtonState; }
  
  if (bButtonState == HIGH && previousBButtonState == LOW) { previousBButtonState = HIGH; }
  else { previousBButtonState = bButtonState; }

  if (bButtonState == HIGH) { player2.x += scale; }
  if (aButtonState == HIGH) { player2.x -= scale; }

//  if (aButtonState == HIGH)
//  {
//    tone(speakerPin, NOTE_C4, 100);
//  }

  music();

  // ball.display();
  // ball2.display();
  // ball3.display();

  // for(int i = 0; i < numberOfBalls; i++) 
  // {
  //   allBalls[i].display();
  // }

  player1.display();
  player2.display();

  retrievePositions();

  

  // if(autopilot)
  // {
  //   if(ball.y < ball2.y)
  //   {
  //     if (autopilotTracker != 1)
  //     {
  //       autopilotTracker = 1;
  //       tft.fillRect(player1.x, player1.y, playerWidth, playerHeight, BLACK);
  //     }
  //     player1.x = ball.x - (playerWidth / 2);
  //   }
  //   else
  //   {
  //     if (autopilotTracker != 2)
  //     {
  //       autopilotTracker = 2;
  //       tft.fillRect(player1.x, player1.y, playerWidth, playerHeight, BLACK);
  //     }
  //     player1.x = ball2.x - (playerWidth / 2);
  //   }
    
  // }
}


void music()
{
  if (millis()-lastPeriodStart >= periodDuration)
  {
    noTone(speakerPin);
    float playNoteFor = 1000 / noteDurations[currentNoteIndex];
    
    periodDuration = (1000 / 4) ;
    lastPeriodStart += periodDuration;
 
    tone(speakerPin, melody[currentNoteIndex], 1800 / noteDurations[currentNoteIndex]);   

    if (currentNoteIndex <= noteArraySize - 2)
    {
      currentNoteIndex++; 
    }
    else
    {
      currentNoteIndex = 0;
    }
  }
}


void directions() 
{
  // retrieving the direction the joystick is facing
  DIRECTION direction = joystick.facingDirection();

  // checking against directions
  if (direction == DIRECTION::UP) {
    y-= scale;
  } 
  else if (direction == DIRECTION::DOWN) 
  {
    y+= scale;
  } 
  else if (direction == DIRECTION::LEFT) 
  {
    x-= scale;
  } 
  else if (direction == DIRECTION::RIGHT) 
  {
    x+= scale;
  }
  else 
  {
    // DIRECTION::CENTER
    // the joystick is not being touched
    // note: if any of the above is being printed when the joystick is centered, change the
    // CENTER_DETECT_RADIUS macro to a larger value in the header file
    // Serial.println("Joystick is in its resting position.");
  }

  pressed = joystick.isPressed();
}

void retrievePositions() 
{
  int xPos = joystick.x();
  int yPos = joystick.y();

  int mappedX = (joystick.x(-7, 8) * -1); // maps from the default 0, 1023 to 0, 255
  int mappedY = (joystick.y(-7, 8));

  player1.x += mappedX;
  
  joystickButtonState = joystick.isPressed(); // 0: not pressed, 1: pressed

  if (joystickButtonState == HIGH && previousJoystickButtonState == LOW) 
  { 
    previousJoystickButtonState = HIGH; 
    autopilot = !autopilot;
    Serial.print("autopilot: "); 
    Serial.println(autopilot); 
  }
  else { previousJoystickButtonState = joystickButtonState; }

}

void magnitudes() 
{
  double magnitude = joystick.magnitude();
}













