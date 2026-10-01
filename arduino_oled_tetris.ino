#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int pinX = A0; 
const int pinY = A1; 
const int pinSW = 3; 

#define GRID_WIDTH 10
#define GRID_HEIGHT 20
const int blockSize = 3; 
const int offsetX = 49;  
const int offsetY = 3;   

byte grid[GRID_HEIGHT][GRID_WIDTH] = {0};

const uint16_t SHAPES[] PROGMEM = {
  0x0F00, 0x4460, 0x44C0, 0x6600, 0x06C0, 0x0E40, 0x0C60  
};

int currentType = 0;
int currentRotation = 0;
int pieceX = 3;
int pieceY = 0;

unsigned long score = 0;
unsigned long linesCleared = 0;
bool gameOver = false;
unsigned long lastDropTime = 0;
int dropInterval = 700; 

unsigned long lastInputTime = 0;
const int inputDelay = 160; 

bool checkBlockBit(int type, int rotation, int r, int c) {
  int bitIndex = 0;
  if (rotation == 0) bitIndex = r * 4 + c;
  else if (rotation == 1) bitIndex = 12 + r - (c * 4);
  else if (rotation == 2) bitIndex = 15 - (r * 4) - c;
  else if (rotation == 3) bitIndex = 3 - r + (c * 4);
  
  uint16_t shapePattern = pgm_read_word(&(SHAPES[type]));
  return (shapePattern & (1 << (15 - bitIndex))) != 0;
}

bool checkCollision(int tx, int ty, int rot) {
  for (int i = 0; i < 16; i++) {
    int r = i / 4;
    int c = i % 4;
    if (checkBlockBit(currentType, rot, r, c)) {
      int gridX = tx + c;
      int gridY = ty + r;
      if (gridX < 0 || gridX >= GRID_WIDTH || gridY >= GRID_HEIGHT) return true;
      if (gridY >= 0 && grid[gridY][gridX] != 0) return true;
    }
  }
  return false;
}

void createNewPiece() {
  currentType = random(0, 7);
  currentRotation = 0;
  pieceX = 3;
  pieceY = -1; 
  if (checkCollision(pieceX, pieceY + 1, currentRotation)) {
    gameOver = true;
  }
}

void lockPiece() {
  for (int i = 0; i < 16; i++) {
    int r = i / 4;
    int c = i % 4;
    if (checkBlockBit(currentType, currentRotation, r, c)) {
      int gridY = pieceY + r;
      int gridX = pieceX + c;
      if (gridY >= 0 && gridY < GRID_HEIGHT && gridX >= 0 && gridX < GRID_WIDTH) {
        grid[gridY][gridX] = 1;
      }
    }
  }
  
  for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
    bool fullLine = true;
    for (int x = 0; x < GRID_WIDTH; x++) {
      if (grid[y][x] == 0) { fullLine = false; break; }
    }
    if (fullLine) {
      linesCleared++;
      score += 100;
      for (int moveY = y; moveY > 0; moveY--) {
        for (int x = 0; x < GRID_WIDTH; x++) {
          grid[moveY][x] = grid[moveY - 1][x];
        }
      }
      for (int x = 0; x < GRID_WIDTH; x++) grid[0][x] = 0;
      y++; 
    }
  }
  createNewPiece();
}

void handleGameOverState() {
  display.clearDisplay();
  display.setTextSize(2); display.setTextColor(SSD1306_WHITE);
  display.setCursor(12, 10); display.print(F("GAME OVER"));
  display.setTextSize(1);
  display.setCursor(20, 36); display.print(F("SCORE: ")); display.print(score);
  display.setCursor(20, 48); display.print(F("LINES: ")); display.print(linesCleared);
  display.display();
  
  if (digitalRead(pinSW) == LOW) {
    memset(grid, 0, sizeof(grid));
    score = 0; linesCleared = 0; gameOver = false;
    createNewPiece();
    delay(400);
  }
}

void renderGameLayout() {
  display.clearDisplay();
  display.drawRect(offsetX - 2, offsetY - 1, (GRID_WIDTH * blockSize) + 3, (GRID_HEIGHT * blockSize) + 2, SSD1306_WHITE);
  
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      if (grid[y][x] != 0) {
        display.fillRect(offsetX + (x * blockSize), offsetY + (y * blockSize), blockSize - 1, blockSize - 1, SSD1306_WHITE);
      }
    }
  }

  for (int i = 0; i < 16; i++) {
    int r = i / 4;
    int c = i % 4;
    if (checkBlockBit(currentType, currentRotation, r, c)) {
      int displayY = pieceY + r;
      if (displayY >= 0) {
        display.fillRect(offsetX + ((pieceX + c) * blockSize), offsetY + (displayY * blockSize), blockSize - 1, blockSize - 1, SSD1306_WHITE);
      }
    }
  }

  display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 6); display.print(F("TETRIS"));
  display.drawFastHLine(0, 16, offsetX - 6, SSD1306_WHITE);
  
  display.setCursor(2, 22); display.print(F("SCORE:"));
  display.setCursor(2, 32); display.print(score);
  display.setCursor(2, 46); display.print(F("LINES:"));
  display.setCursor(2, 56); display.print(linesCleared);

  display.display();
}

void setup() {
  pinMode(pinSW, INPUT_PULLUP);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { for(;;); }
  randomSeed(analogRead(A2) + analogRead(A3));
  createNewPiece();
}

void loop() {
  unsigned long currentTime = millis();
  
  if (gameOver) {
    handleGameOverState();
    return;
  }

  int rx = analogRead(pinX);
  int ry = analogRead(pinY);
  bool btn = (digitalRead(pinSW) == LOW);

  if (currentTime - lastInputTime > inputDelay) {
    if (rx < 350) { 
      if (!checkCollision(pieceX - 1, pieceY, currentRotation)) { pieceX--; lastInputTime = currentTime; }
    } 
    else if (rx > 670) { 
      if (!checkCollision(pieceX + 1, pieceY, currentRotation)) { pieceX++; lastInputTime = currentTime; }
    }
    if (btn) { 
      int nextRot = (currentRotation + 1) % 4;
      if (!checkCollision(pieceX, pieceY, nextRot)) { currentRotation = nextRot; lastInputTime = currentTime; }
    }
  }

  int dynamicInterval = (ry > 670) ? 60 : dropInterval; 

  if (currentTime - lastDropTime > (unsigned long)dynamicInterval) {
    lastDropTime = currentTime;
    if (!checkCollision(pieceX, pieceY + 1, currentRotation)) {
      pieceY++;
    } else {
      lockPiece();
    }
  }

  renderGameLayout();
}
