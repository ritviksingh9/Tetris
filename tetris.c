//NOTE: Do NOT use the "run" button up top, just type ./tetris
//Arrow keys to move pieces, z to rotate

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <curses.h>
#define BOARDX 12
#define BOARDY 21

void printBoard(int** board, int x, int y, int pieceArray[4][4], WINDOW* win, int score, int shadowY) {
  refresh();
  wrefresh(win);
  for(int i = 0; i < BOARDY; i++) {
    for(int j = 0; j < BOARDX; j++) {
      if(board[i][j] == 2)
        mvwprintw(win, i, j, "#");
      else if(!board[i][j]) {
        if(i-y >= 0 && i-y < 4 && j-x >= 0 && j-x < 4 && pieceArray[i-y][j-x] == 1) 
          mvwprintw(win, i, j, "X");
        else if(i-shadowY >= 0 && i-shadowY < 4 && j-x >= 0 && j-x < 4 && pieceArray[i-shadowY][j-x] == 1) 
          mvwprintw(win, i, j, ".");
        else
          mvwprintw(win, i, j, " ");
      }
      else if(board[i][j] == 1)
        mvwprintw(win, i, j, "X");
    }
    mvwprintw(win, i, BOARDX, "\n");
  }
  mvwprintw(win, BOARDY/2, BOARDX+1, "SCORE: %d", score);
}

int** initBoard() {
  int** board = calloc(BOARDY, sizeof(int*));

  for(int i = 0; i < BOARDY; i++)
    board[i] = calloc(BOARDX, sizeof(int));

  for(int i = 0; i < BOARDY; i++) {
    board[i][0] = 2;
    board[i][BOARDX-1] = 2;
  }
  for(int i = 0; i < BOARDX; i++)
    board[BOARDY-1][i] = 2;
  
  return board;
}

void deleteBoard(int** board) {
  for(int i = 0; i < BOARDX; i++)
    free(board[i]);
  free(board);
}

void rotatePiece(int pieceArray[4][4]) {
  //rotating 90 degrees by calculating transpose and then fliping horizontal
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < i; j++) {
      int place = pieceArray[i][j];
      pieceArray[i][j] = pieceArray[j][i];
      pieceArray[j][i] = place;
    }
  }
  
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 2; j++) {
      int place = pieceArray[i][j];
      pieceArray[i][j] = pieceArray[i][3-j];
      pieceArray[i][3-j] = place;
    }
  }
}

void resetPieceArray(int pieceArray[4][4]) {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++)
      pieceArray[i][j] = 0;
  }
}

int checkPiece(int piece, int pieceTracker[4]) {
  for(int i = 0; i < 4; i++) {
    if(piece == pieceTracker[i])
      return 0;
  }
  return 1;
}

void genPiece(int pieceArray[4][4], int pieceTracker[4]) {
  int piece = rand()%7;

  while(!checkPiece(piece, pieceTracker))
    piece = rand()%7;
  
  for(int i = 1; i < 4; i++) 
    pieceTracker[i-1] = pieceTracker[i];
  pieceTracker[3] = piece;

  resetPieceArray(pieceArray);
  if(piece == 0) {pieceArray[0][1] = 1; pieceArray[1][1] = 1; pieceArray[2][1] = 1; pieceArray[3][1] = 1;}
  if(piece == 1) {pieceArray[0][0] = 1; pieceArray[1][0] = 1; pieceArray[1][1] = 1; pieceArray[1][2] = 1;};
  if(piece == 2) {pieceArray[0][2] = 1; pieceArray[1][0] = 1; pieceArray[1][1] = 1; pieceArray[1][2] = 1;};
  if(piece == 3) {pieceArray[0][2] = 1; pieceArray[0][3] = 1; pieceArray[1][2] = 1; pieceArray[1][3] = 1;};
  if(piece == 4) {pieceArray[0][2] = 1; pieceArray[0][3] = 1; pieceArray[1][1] = 1; pieceArray[1][2] = 1;};
  if(piece == 5) {pieceArray[1][1] = 1; pieceArray[1][2] = 1; pieceArray[1][3] = 1; pieceArray[2][2] = 1;};
  if(piece == 6) {pieceArray[0][1] = 1; pieceArray[0][2] = 1; pieceArray[1][2] = 1; pieceArray[1][3] = 1;};
}

//debugging purposes
void printPiece(int pieceArray[4][4]) {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) 
      printf("%d ", pieceArray[i][j]);
    printf("\n");
  }
}

int checkCollision(int pieceArray[4][4], int x, int y, int** board) {
  int check = 0;
  int squares[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
  for(int i = 0, k = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      if(pieceArray[i][j] == 1) {
        squares[k][1] = i;
        squares[k][0] = j;
        k++;
      }
    }
  }

  for(int i = 0; i < 4 && !check; i++) {
    if(squares[i][0]+x > 0 && squares[i][0]+x < BOARDX && squares[i][1]+y < BOARDY) {
      if(board[squares[i][1]+y][squares[i][0]+x]) 
       check = 1;
    }
    else {check = 1;}
  }
  return check;
}

void addToBoard(int** board, int x, int y, int pieceArray[4][4]) {
  for(int i = y; i < y+4; i++) {
    for(int j = x; j < x+4; j++)
      if(pieceArray[i-y][j-x] == 1)
        board[i][j] = 1;
  }
}

void copyPieceArray(int a[4][4], int b[4][4]) {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++)
      a[i][j] = b[i][j];
  }
}

int checkLines(int** board, int x, int y, WINDOW* win) {
  static int level = 0;
  static int linesPerLevel = 0;
  int lines = 0;
  int startClear = -1;

  for(int i = y; i < y+4 && i < BOARDY-1; i++) {
    int check = 1;
    for(int j = 1; j < BOARDX-1 && check; j++) {
      if(board[i][j] == 0) 
        check = 0;
    }
    if(check) {
      if(lines == 0) 
        startClear = i-1;
      linesPerLevel++;
      lines++;
      if(linesPerLevel == 10) {
        linesPerLevel = 0;
        level++;
      }
      for(int j = 1; j < BOARDX-1; j++)
        board[i][j] = 0;
    }
  }

  if(lines) {
    //implementation of naive gravity
    for(int i = startClear; i >= 0; i--) {
      for(int j = 0; j < BOARDX; j++) 
        board[i+lines][j] = board[i][j];
    }

    //animation for clearing lines
    refresh();
    wrefresh(win);
    
    for(int i = startClear+1; i <= startClear+lines; i++) {
      for(int j = 1; j < BOARDX-1; j++) 
        mvwprintw(win, i, j, ".");
    }
    wrefresh(win);
    usleep(400000);
  }

  if(lines == 1)
    return (level+1)*40;
  else if(lines == 2)
    return (level+1)*100;
  else if(lines == 3)
    return (level+1)*300;
  else if(lines == 4)
    return (level+1)*1200;
  else
    return 0;
}

void play() {
  int** board = initBoard();
  WINDOW* win;
  initscr();
  refresh();
  win = newwin(BOARDY+2, BOARDX*3, 0, 0);
  int playOn = 1;
  int pieceGenerated = 0;
  int piece = 0;
  int pieceArray[4][4];
  //reduce number of repeated instances of pieces
  int pieceTracker[4] = {0, 0, 0, 0};
  //top left corner of tetromino array
  int x = 0; 
  int y = 0;
  int shadowY = 0;
  int frame = 0;
  int frameCycle = 50;
  int score = 0;
  srand(time(0));

  refresh();

  while(playOn) {
    int ch;
    if(!pieceGenerated) {
      genPiece(pieceArray, pieceTracker);
      pieceGenerated = 1;
      x = 3;
      y = 0;
      if(checkCollision(pieceArray, x, y, board))
        playOn = 0;
      else {
        for(shadowY = 0; !checkCollision(pieceArray, x, shadowY, board); shadowY++);
      }
    }
    else {
      noecho();
      cbreak();
      nodelay(stdscr, TRUE);
      keypad(stdscr, TRUE);
      ch = getch();
      if(ch == 'z' || ch == 'Z') {
        int tempPieceArray[4][4];
        copyPieceArray(tempPieceArray, pieceArray);
        rotatePiece(tempPieceArray);
        if(!checkCollision(tempPieceArray, x, y, board))
          rotatePiece(pieceArray);
        else if(!checkCollision(tempPieceArray, x-1, y, board)) {
          rotatePiece(pieceArray);
          x--;
        }
        else if(!checkCollision(tempPieceArray, x+1, y, board)) {
          rotatePiece(pieceArray);
          x++;
        }
      }
      else if(ch == KEY_LEFT && !checkCollision(pieceArray, x-1, y, board))
        x--;
      else if(ch == KEY_RIGHT && !checkCollision(pieceArray, x+1, y, board))
        x++;
      else if(ch == KEY_DOWN)
        frame = frameCycle-1; //speeding up the cycle so it goes down faster
        
      if(!checkCollision(pieceArray, x, y+1, board)) {
        frame = (frame + 1) % frameCycle; //slowing down the animation while not slowing down the refresh rate
        if(frame == 0)
          y++;
      }
      else {
        addToBoard(board, x, y, pieceArray);
        score += checkLines(board, x, y, win);
        pieceGenerated = 0;
        frame = 0;
      }


      for(shadowY = y; !checkCollision(pieceArray, x, shadowY, board); shadowY++);
    }
    printBoard(board, x, y, pieceArray, win, score, --shadowY);
    usleep(20000);
    wrefresh(win);
  }
  mvwprintw(win, BOARDY/2-1, BOARDX+1, "#################");
  mvwprintw(win, BOARDY/2, BOARDX+1, "#   GAME OVER   #");
  mvwprintw(win, BOARDY/2+1, BOARDX+1, "#################");
  mvwprintw(win, BOARDY/2+2, BOARDX+1, "FINAL SCORE: %d", score);
  wrefresh(win);
  deleteBoard(board);
  delwin(win);
}

int main(void) {
  play();
}