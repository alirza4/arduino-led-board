#include <MD_MAX72xx.h>

#define BUF_SIZE 15
#define MAX_DEVICES 6
#define CHAR_SIZE 6
#define MAXCHARS_ONSCREEN 6
#define CLK_PIN   13 
#define DATA_PIN  11  
#define CS_PIN    10  

uint8_t alph[37][5] = 
{  {0,0,0,0,0}              // ETC
  ,{126, 9, 9, 9, 126}      // A
  ,{127, 73, 73, 73, 54}    // B
  ,{62, 65, 65, 65, 65}     // C
  ,{127, 65, 65, 65, 62}    // D
  ,{ 127, 73, 73, 73, 65}   // E
  ,{127, 9, 9, 9, 1}        // F
  ,{62, 65, 65, 73, 121}    // G
  ,{127, 8, 8, 8, 127}      // H
  ,{0, 65, 127, 65, 0}      // I 
  ,{48, 65, 65, 65, 63}     // J
  ,{127, 8, 20, 34, 65}     // K
  ,{127, 64, 64, 64, 64}    // L
  ,{127, 2, 12, 2, 127}     // M
  ,{127, 4, 8, 16, 127}     // N
  ,{62, 65, 65, 65, 62}     // O
  ,{127, 9, 9, 9, 6}        // P
  ,{62, 65, 65, 97, 126}    // Q
  ,{127, 9, 25, 41, 70}     // R
  ,{70, 73, 73, 73, 49}     // S
  ,{1, 1, 127, 1, 1}        // T
  ,{63, 64, 64, 64, 63}     // U
  ,{31, 32, 64, 32, 31}     // V
  ,{63, 64, 56, 64, 63}     // W
  ,{99, 20, 8, 20, 99}      // X
  ,{3, 4, 120, 4, 3}        // Y
  ,{97, 81, 73, 69, 67}     // Z
  ,{62, 81, 73, 69, 62,}    // 0
  ,{0, 4, 2, 127, 0}        // 1
  ,{113, 73, 73, 73, 70}    // 2
  ,{65, 73, 73, 73, 54}     // 3
  ,{15, 8, 8, 8, 127}       // 4
  ,{79, 73, 73, 73, 49}     // 5
  ,{62, 73, 73, 73, 48}     // 6
  ,{3, 1, 1, 1, 127}        // 7
  ,{54, 73, 73, 73, 54}     // 8
  ,{6, 73, 73, 73, 62}      // 9
};

class Show {
public:
  Show(MD_MAX72XX *mx): md(mx) {}
//animation when showing strings start
  void start(){
    md->setRow(0, 0xFF);
    md->setRow(1, 0xFF);
    md->setRow(2, 0xFF);
    md->setRow(3, 0xFF);
    md->setRow(4, 0xFF);
    md->setRow(5, 0xFF);
    md->setRow(6, 0xFF);
    md->setRow(7, 0xFF);
  
    for (int i = 3; i >= 0; i--){
      delay(200);
      md->setRow(3 - i, 0);
      md->setRow(4 + i, 0);
    }
    for (int i = 0; i < 4; i++){
      delay(200);
      md->setRow(1 + i, 0);
      md->setRow(6 - i, 0);
    }
    delay(10);
}

//animation when showing strings End
  void end() {
  int delayTime = 200;
  int startCol = 31;
  int endCol = 0;

  for (int i = 0; i <= 15; i++) {
    delay(delayTime);
    md->setColumn(startCol, 0xff);
    md->setColumn(endCol, 0xff);
    startCol--;
    endCol++;
  }

  delay(delayTime);
}

private:
  MD_MAX72XX *md = 0 ;
};

MD_MAX72XX mx = MD_MAX72XX(MD_MAX72XX::PAROLA_HW, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
Show effect = Show(&mx);
//Print char
void displayCharacter(uint8_t col, uint8_t inchar)
{
  uint8_t charc;
  
  if(inchar >= 'A' && inchar <= 'Z')
    charc = inchar - 64;
  else if(inchar >= 'a' && inchar <= 'z')
    charc = inchar - 96;
  else if(inchar >= '0' && inchar <= '9' )
    charc = inchar - 21;
  else
    charc = 0;

  mx.setBuffer(col,5,alph[charc]);
}

//first way to show
void showMovingText1(const char *str, uint8_t len) {
  int i, c;
  int startCol = 0, currentCol = 0;
  static char currentMessage[BUF_SIZE];
  uint32_t animationLength;

  if (len <= 0)
    return;

  strcpy(currentMessage, str);

  animationLength = (len + MAXCHARS_ONSCREEN) * CHAR_SIZE;

  for (i = 0; i < animationLength; i++) {
    int numChars = (int)(i / CHAR_SIZE) + 1;
    numChars = (numChars > len) ? len : numChars;
    startCol = currentCol;
    int minChar = ((numChars < MAXCHARS_ONSCREEN) ? 0 : numChars - MAXCHARS_ONSCREEN);
    mx.clear();
    for (c = numChars - 1; c >= minChar; c--) {
      displayCharacter(startCol, (uint8_t)currentMessage[c]);
      startCol += 6;
    }
    currentCol += 1;
    if (currentCol >= CHAR_SIZE && numChars != len)
      currentCol = 0;
    delay(10);
  }
}

//second way to show
void showMovingText2(const char *str, uint8_t len) {
  static char curr_msg[BUF_SIZE];
  static bool charState[MAXCHARS_ONSCREEN] = {0};  
  int mid_col = (len % 2) ? 18 : 15;
  int mid_char_indx = len / 2;
  int i, right_char_col, left_char_col;

  if (len <= 0)
    return;

  strcpy(curr_msg, str);

  for (i = 0; i < (len / 2) + 1; i++) {
    right_char_col = mid_col - i * CHAR_SIZE;
    left_char_col = mid_col + i * CHAR_SIZE;

    if (mid_char_indx - i >= 0 && left_char_col >= 0) {
      displayCharacter(left_char_col, curr_msg[mid_char_indx - i]);
      charState[mid_char_indx - i] = true;
    } else {
      charState[mid_char_indx - i] = false;
    }

    if (mid_char_indx + i < len && i != 0 && right_char_col <= 32 - CHAR_SIZE) {
      displayCharacter(right_char_col, curr_msg[mid_char_indx + i]);
      charState[mid_char_indx + i] = true;
    } else {
      charState[mid_char_indx + i] = false;
    }
  }

  for (i = 0; i < len; i++) {
    if (!charState[i])
      displayCharacter(mid_col + (i - mid_char_indx) * CHAR_SIZE, curr_msg[i]);
  }
}

void animation() {
  effect.start();
  showMovingText1("spring02",8);
  showMovingText2("END",3);
  delay(500);
  effect.end();
}

void setup() {
  mx.begin();
}

void loop() {
  animation();
}