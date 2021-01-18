 /*FDS MINI PROJECT for SEMESTER 3
    2048 Game using Linked List and Open Graphics Library
Submitted by:
    1) Navneet Das (3231 SE Comp-A)
    2) Madhukar    (3224 SE Comp-A)
*/

//Add the <windows.h> header file if you are operating on Windows

#include <GL/glut.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include <unistd.h>//for windows you can remove this header file.


#define GAME_W0N 1
#define GAME_LOST 2
#define GAME_NOT_OVER 0
#define MOVE_IS_NOT_VALID false
#define MOVE_IS_VALID true

using namespace std;

struct Node {
  int size;
  long long curr_moves;
  long long curr_score;
  int winning_score;
  bool options[4];
  int *board;
  Node *previs;
};

Node *currentNode = NULL;
int size = -1;
int opt_flag = 0;
char moveChar = '\0';
int gameEnd = GAME_NOT_OVER;
bool wasMoveMade = false;
bool undoCheck = false;

int getSize() {

  return 4;
}
void getUserInput(Node *currentNode, char &moveChar) {

}

void setBoard(Node *&node) {
  node->board = new int[node->size * node->size];
  if (node->board == NULL) {
    setBoard(node);
  }
  for (int index = 0; index < node->size * node->size; index++) {
    node->board[index] = 0;
  }
}
void copyNodeBoard(Node *&originalNode, Node *&nodeCopy) {
  for (int index = 0; index < originalNode->size * originalNode->size;
       index++) {
    nodeCopy->board[index] = originalNode->board[index];
  }
}
void copyNode(Node *originalNode, Node *&nodeCopy) {
  nodeCopy->size = originalNode->size;
  setBoard(nodeCopy);
  copyNodeBoard(originalNode, nodeCopy);
  nodeCopy->curr_moves = originalNode->curr_moves;
  nodeCopy->curr_score = originalNode->curr_score;
  nodeCopy->winning_score = originalNode->winning_score;
}
void determineMaxTileValue(Node *&node) {
  node->winning_score = 2048; // Reduce the score in order to test the game(e.g. 512, 1024)
  for (int i = 4; i < node->size; i++) {
    node->winning_score += node->winning_score;
  }
}
void randNumOnBoard(Node *node) {
  int randomTile = rand() % (node->size * node->size);
  int randNum = -1;

  int x = rand() % 100;
  if (x >= 0 && x < 75) {
    randNum = 2;
  } else if (x >= 75 && x < 100) {
    randNum = 4;
  }

  if (node->board[randomTile] != 0) {
    randNumOnBoard(node);
  }

  else {
    node->board[randomTile] = randNum;
  }
}

int mergeUp(Node *&node, int col) {
  int greatestTileChange = -1;
  int row = 0;
  int sum = 0;
  while (row < node->size) {
    if (node->board[node->size * row + col] ==
        node->board[node->size * (row + 1) + col]) {
      sum = node->board[node->size * row + col] * 2;
      node->board[node->size * row + col] = sum;
      node->board[node->size * (row + 1) + col] = 0;
      row += 2;
      node->curr_score = node->curr_score + sum;
      if (sum > greatestTileChange) {
        greatestTileChange = sum;
      }
    } else
      row++;
  }
  return greatestTileChange;
}
int findNextTileUp(Node *&node, int row, int col) {

  int index = -1;
  for (; row < node->size; row++) {
    if (node->board[node->size * row + col] != 0) {
      index = row;
      break;
    }
  }
  return index;
}
void slideUp(Node *&node, int col) {
  int nextRowIndex = -1;
  for (int row = 0; row < node->size; row++) {
    if (node->board[node->size * row + col] == 0) {

      nextRowIndex = findNextTileUp(node, row + 1, col);
      if (nextRowIndex == -1) {
        break;
      }
      node->board[node->size * row + col] =
          node->board[node->size * nextRowIndex + col];
      node->board[node->size * nextRowIndex + col] = 0;
    }
  }
}
int moveUp(Node *node) {
  int greatestTileChange = -1;
  for (int col = 0; col < node->size; col++) {
    slideUp(node, col);
    greatestTileChange = mergeUp(node, col);
    slideUp(node, col);
  }
  return greatestTileChange;
}

int mergeDown(Node *&node, int col) {
  int greatestTileChange = -1;
  int row = node->size - 1;
  long sum = 0;
  while (row >= 0) {
    if (node->board[node->size * row + col] ==
        node->board[node->size * (row - 1) + col]) {
      sum = node->board[node->size * row + col] * 2;
      node->board[node->size * row + col] = sum;
      node->board[node->size * (row - 1) + col] = 0;
      row -= 2;
      node->curr_score = node->curr_score + sum;
      if (sum > greatestTileChange) {
        greatestTileChange = sum;
      }
    } else
      row--;
  }
  return greatestTileChange;
}
int findNextTileDown(Node *&node, int row, int col) {
  int index = -1;
  for (; row >= 0; row--) {
    if (node->board[node->size * row + col] != 0) {
      index = row;
      break;
    }
  }
  return index;
}
void slideDown(Node *&node, int col) {
  for (int row = node->size - 1; row >= 0; row--) {
    if (node->board[node->size * row + col] == 0) {
      int nextRowIndex = findNextTileDown(node, row - 1, col);
      if (nextRowIndex == -1)
        break;
      node->board[node->size * row + col] =
          node->board[node->size * nextRowIndex + col];
      node->board[node->size * nextRowIndex + col] = 0;
    }
  }
}
int moveDown(Node *&node) {
  int greatestTileChange = -1;
  for (int col = 0; col < node->size; col++) {
    slideDown(node, col);
    greatestTileChange = mergeDown(node, col);
    slideDown(node, col);
  }
  return greatestTileChange;
}

int mergeLeft(Node *&currentNode, int row) {
  int greatestTileChange = -1;
  int col = 0;
  int sum = 0;
  while (col < currentNode->size) {
    if (currentNode->board[currentNode->size * row + col] ==
        currentNode->board[currentNode->size * row + col + 1]) {
      sum = currentNode->board[currentNode->size * row + col] * 2;
      currentNode->board[currentNode->size * row + col] = sum;
      currentNode->board[currentNode->size * row + col + 1] = 0;
      col += 2;
      currentNode->curr_score = currentNode->curr_score + sum;
      if (sum > greatestTileChange) {
        greatestTileChange = sum;
      }
    } else
      col++;
  }
  return greatestTileChange;
}
int findNextTileLeft(Node *&node, int row, int col) {
  int index = -1;
  for (; col < node->size; col++)
    if (node->board[node->size * row + col] != 0) {
      index = col;
      break;
    }
  return index;
}
void slideLeft(Node *&node, int row) {
  for (int col = 0; col <= (node->size - 1); col++) {
    if (node->board[node->size * row + col] == 0) {
      int nextColIndex = findNextTileLeft(node, row, col + 1);
      if (nextColIndex == -1) {
        break;
      } else {
        node->board[node->size * row + col] =
            node->board[node->size * row + nextColIndex];
        node->board[node->size * row + nextColIndex] = 0;
      }
    }
  }
}
int moveLeft(Node *&node) {
  int greatestTileChange = -1;
  for (int row = 0; row < node->size; row++) {
    slideLeft(node, row);
    greatestTileChange = mergeLeft(node, row);
    slideLeft(node, row);
  }
  return greatestTileChange;
}

int mergeRight(Node *&node, int row) {
  int greatestTileChange = -1;
  int col = node->size;
  int sum = 0;
  while (col > 0) {
    if (node->board[node->size * row + col] ==
        node->board[node->size * row + col - 1]) {
      sum = node->board[node->size * row + col] * 2;
      node->board[node->size * row + col] = sum;
      node->board[node->size * row + col - 1] = 0;
      node->curr_score = node->curr_score + sum;
      col -= 2;
      if (sum > greatestTileChange) {
        greatestTileChange = sum;
      }
    } else
      col--;
  }
  return greatestTileChange;
}
int findNextTileRight(Node *&node, int row, int col) {
  int index = -1;
  for (; col >= 0; col--) {
    if (node->board[node->size * row + col] != 0) {
      index = col;
      break;
    }
  }
  return index;
}
void slideRight(Node *&node, int row) {
  for (int col = node->size - 1; col > 0; col--) {
    if (node->board[node->size * row + col] == 0) {
      int nextColIndex = findNextTileRight(node, row, col - 1);
      if (nextColIndex == -1) {
        break;
      } else {
        node->board[node->size * row + col] =
            node->board[node->size * row + nextColIndex];
        node->board[node->size * row + nextColIndex] = 0;
      }
    }
  }
}
int moveRight(Node *&node) {
  int greatestTileChange = -1;
  for (int row = 0; row < node->size; row++) {
    slideRight(node, row);
    greatestTileChange = mergeRight(node, row);
    slideRight(node, row);
  }
  return greatestTileChange;
}

bool areNodesTheSame(Node *currentNode, Node *nodeCopy) {
  int bSize = currentNode->size;
  for (int i = 0; i < bSize * bSize; i++) {
    if (currentNode->board[i] != nodeCopy->board[i]) {
      return false;
    }
  }
  return true;
}
bool checkIfGameIsWon(Node *currentNode) {
  for (int i = 0; i < currentNode->size * currentNode->size; i++) {
    if (currentNode->board[i] == currentNode->winning_score) {
      return true;
    }
  }
  return false;
}
bool checkIfGameIsLost(Node *currentNode) {
  if (currentNode->options[0] == MOVE_IS_NOT_VALID &&
      currentNode->options[1] == MOVE_IS_NOT_VALID &&
      currentNode->options[2] == MOVE_IS_NOT_VALID &&
      currentNode->options[3] == MOVE_IS_NOT_VALID) {
    return true;
  }
  return false;
}
int checkGameEnd(Node *currentNode) {
  if (checkIfGameIsWon(currentNode)) {
    return GAME_W0N;
  } else if (checkIfGameIsLost(currentNode)) {
    return GAME_LOST;
  } else {
    return GAME_NOT_OVER;
  }
}
void possibleMoves(Node *&currentNode) {
  for (int i = 0; i < 4; i++) {
    Node *tempNode = new Node;
    copyNode(currentNode, tempNode);
    switch (i) {
    case 0:
      moveLeft(tempNode);
      break;
    case 1:
      moveUp(tempNode);
      break;
    case 2:
      moveDown(tempNode);
      break;
    case 3:
      moveRight(tempNode);
      break;
    }
    if (areNodesTheSame(currentNode, tempNode)) {
      currentNode->options[i] = MOVE_IS_NOT_VALID;
    } else {
      currentNode->options[i] = MOVE_IS_VALID;
    }
    delete[] tempNode->board;
    delete tempNode;
  }
}

void deleteNode(Node *&currentNode) {
  Node *pTemp = currentNode->previs;
  delete[] currentNode->board;
  delete currentNode;
  currentNode = pTemp;
}
void deleteLinkedList(Node *&currentNode) {
  while (currentNode != NULL) {
    deleteNode(currentNode);
  }
}
void addNewNode(Node *&currentNode) {
  Node *newNode = new Node;
  if (newNode == NULL) {
    cout << "\nError: Failed to allocate new node.\nExiting...\n";
    exit(EXIT_FAILURE);
  }

  if (currentNode != NULL) {
    copyNode(currentNode, newNode);
    newNode->curr_moves = currentNode->curr_moves + 1;
  }

  newNode->previs = currentNode;
  currentNode = newNode;
}
void undoLastMove(Node *&currentNode) { deleteNode(currentNode); }

void optimal_move(Node *&currentNode, bool wasMoveMade) {
  possibleMoves(currentNode);
  int x = -1;
  int pos = -1;
  int greatestTileChange[4] = {-1, -1, -1, -1};
  for (int i = 0; i < 4; i++) {
    if (currentNode->options[i] == 1) {
      Node *optimal_node = new Node;
      copyNode(currentNode, optimal_node);
      switch (i) {
      case 0:
        greatestTileChange[i] = moveLeft(optimal_node);
        if (optimal_node->curr_score > x) {
          x = optimal_node->curr_score;
          pos = i;
        }
        break;
      case 1:
        greatestTileChange[i] = moveUp(optimal_node);
        if (optimal_node->curr_score > x) {
          x = optimal_node->curr_score;
          pos = i;
        }
        break;
      case 2:
        greatestTileChange[i] = moveDown(optimal_node);
        if (optimal_node->curr_score > x) {
          x = optimal_node->curr_score;
          pos = i;
        }
        break;
      case 3:
        greatestTileChange[i] = moveRight(optimal_node);
        if (optimal_node->curr_score > x) {
          x = optimal_node->curr_score;
          pos = i;
        }
        break;
      }

      delete[] optimal_node->board;
      delete optimal_node;
    } else {
      greatestTileChange[i] = 0;
    }
  }

  int indexOfMax = pos;

  switch (indexOfMax) {
  case 0:
    addNewNode(currentNode);
    wasMoveMade = true;
    moveLeft(currentNode);
    break;
  case 1:
    addNewNode(currentNode);
    wasMoveMade = true;
    moveUp(currentNode);
    break;
  case 2:
    addNewNode(currentNode);
    wasMoveMade = true;
    moveDown(currentNode);
    break;
  case 3:
    addNewNode(currentNode);
    wasMoveMade = true;
    moveRight(currentNode);
    break;
  }
}


 void init(){
	glMatrixMode(GL_PROJECTION);
 	glLoadIdentity();
	gluOrtho2D(0,500,0,620);
	glMatrixMode(GL_MODELVIEW);
}

static char tx[100],text[100];

void drawtext(float x,float y,void *font,char *string){
	char *c;
	glRasterPos2f(x,y);
	for(c=string;*c!='\0';c++){
		glutBitmapCharacter(font,*c);
 	}
}

void displayAndInitializeGameData(Node *&currentNode, bool &moveMade,
                                  char moveChar) {


  if (currentNode == NULL) {
    int boardSize = getSize();
    addNewNode(currentNode);
    currentNode->size = boardSize;
    currentNode->curr_moves = 1;
    currentNode->curr_score = 0;
    setBoard(currentNode);

    determineMaxTileValue(currentNode);

    randNumOnBoard(currentNode);
    randNumOnBoard(currentNode);
  }

  if (moveMade == true && moveChar != 'P') {
    randNumOnBoard(currentNode);
  }

  moveMade = false;
}

float colormat[10][3]={{0.8392, 0.6313, 0.6313},
                        {0.7960,0.9411,0.3960},
                        {0.7960,0.6784,1},
                        {1,0.6901,0.1098},
                        {0.4509,1,0.9450},
                        {1,0.8117,0.8588},
                        {0.04705,1,0.6039},
                        {0.8392, 0.6313, 0.6313},
                        {0.7960,0.9411,0.3960},
                        {0.7960,0.6784,1}
                     };
void display(){
    glClearColor(0.2431 ,0.4 , 0.5490,0);
     if(gameEnd==GAME_W0N){
        glClearColor(1.0,0.5,0.0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0,0.5,0.0,1);
        glColor3f(1,1,1);
        drawtext(135,550,GLUT_BITMAP_TIMES_ROMAN_24,"FDS MINI PROJECT");
        drawtext(40,140,GLUT_BITMAP_TIMES_ROMAN_24,"2048 Game Using Linked List and OpenGL");
        drawtext(55,80,GLUT_BITMAP_TIMES_ROMAN_24,"Created by Navneet Das and Madhukar");
        drawtext(55,480,GLUT_BITMAP_TIMES_ROMAN_24,"*********************************");
        drawtext(110,400,GLUT_BITMAP_TIMES_ROMAN_24,"YOU WON THE GAME!");
        drawtext(123,340,GLUT_BITMAP_TIMES_ROMAN_24,"Thank You For Playing!");
        drawtext(55,260,GLUT_BITMAP_TIMES_ROMAN_24,"*********************************");
        glFlush();
        glutSwapBuffers();
        sleep(5000);
        exit(0);
      }

      if(gameEnd==GAME_LOST && currentNode->curr_moves>2 && opt_flag==0){
        glClearColor(0.73,0.16,0.96,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.73,0.16,0.96,1);
        glColor3f(1,1,1);
        drawtext(135,550,GLUT_BITMAP_TIMES_ROMAN_24,"FDS MINI PROJECT");
        drawtext(40,140,GLUT_BITMAP_TIMES_ROMAN_24,"2048 Game Using Linked List and OpenGL");
        drawtext(55,80,GLUT_BITMAP_TIMES_ROMAN_24,"Created by Navneet Das and Madhukar");
        drawtext(55,480,GLUT_BITMAP_TIMES_ROMAN_24,"*********************************");
        drawtext(110,400,GLUT_BITMAP_TIMES_ROMAN_24,"YOU LOST THE GAME!");
        drawtext(123,340,GLUT_BITMAP_TIMES_ROMAN_24,"Thank You For Playing!");
        drawtext(55,260,GLUT_BITMAP_TIMES_ROMAN_24,"*********************************");
        glFlush();
        glutSwapBuffers();
        sleep(5000);
        exit(0);
      }

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.2431 ,0.4 , 0.5490,0);


    displayAndInitializeGameData(currentNode,wasMoveMade,moveChar);

    glColor3f(1 ,1 , 1);
     int y=0;
    for(int i=0;i<4;i++){
            int x=0;
        for(int j=0;j<4;j++){
                if(currentNode->board[i*4+j]==2){
                    glColor3f(colormat[0][0],colormat[0][1] ,colormat[0][2]);
                }else if(currentNode->board[i*4+j]==4){
                    glColor3f(colormat[1][0],colormat[1][1] ,colormat[1][2]);
                }else if(currentNode->board[i*4+j]==8){
                    glColor3f(colormat[2][0],colormat[2][1] ,colormat[2][2]);
                }else if(currentNode->board[i*4+j]==16){
                    glColor3f(colormat[3][0],colormat[3][1] ,colormat[3][2]);
                }else if(currentNode->board[i*4+j]==32){
                    glColor3f(colormat[4][0],colormat[4][1] ,colormat[4][2]);
                }else if(currentNode->board[i*4+j]==64){
                    glColor3f(colormat[5][0],colormat[5][1] ,colormat[5][2]);
                }else if(currentNode->board[i*4+j]==128){
                    glColor3f(colormat[6][0],colormat[6][1] ,colormat[6][2]);
                }else if(currentNode->board[i*4+j]==256){
                    glColor3f(colormat[7][0],colormat[7][1] ,colormat[7][2]);
                }
                else if(currentNode->board[i*4+j]==512){
                    glColor3f(colormat[7][0],colormat[7][1] ,colormat[7][2]);
                }
                else if(currentNode->board[i*4+j]==1024){
                    glColor3f(colormat[7][0],colormat[7][1] ,colormat[7][2]);
                }
                else{
                    glColor3f(0.5803,0.7960 , 1);
                }

                glBegin(GL_POLYGON);
                glVertex2f(20+x,380+y);
                glVertex2f(120+x,380+y);
                glVertex2f(120+x,480+y);
                glVertex2f(20+x,480+y);
                glEnd();
                glColor3f(0,0,0);
                sprintf(tx," %d",currentNode->board[i*4+j]);
                drawtext(60+x,420+y,GLUT_BITMAP_TIMES_ROMAN_24,tx);
                x+=120;
        }
        y-=120;
    }
    glColor3f(1,1,1);
    drawtext(55,596,GLUT_BITMAP_TIMES_ROMAN_24,"Created by Navneet Das and Madhukar");
    drawtext(180,560,GLUT_BITMAP_TIMES_ROMAN_24,"2048 GAME");
    drawtext(20,500,GLUT_BITMAP_HELVETICA_18,"Total Moves:- ");
    sprintf(tx," %d",currentNode->curr_moves);
    drawtext(160,500,GLUT_BITMAP_HELVETICA_18,tx);
    drawtext(240,500,GLUT_BITMAP_HELVETICA_18,"Current Score:- ");
    sprintf(tx," %d",currentNode->curr_score);
    drawtext(380,500,GLUT_BITMAP_HELVETICA_18,tx);



    if (moveChar != '\0') {
      possibleMoves(currentNode);
    }

    if (currentNode == NULL) {
      gameEnd = GAME_NOT_OVER;
    } else {
      gameEnd = checkGameEnd(currentNode);
    }
	glFlush();
}


void keyboard(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W')
	{
         moveChar='W';
        if (currentNode->options[1]) {
        addNewNode(currentNode);
        moveUp(currentNode);
        wasMoveMade = true;
      }
	}
	else if (key == 'S' || key == 's')
	{
         moveChar='S';
        if (currentNode->options[2]) {
        addNewNode(currentNode);
        moveDown(currentNode);
        wasMoveMade = true;
      }
	}

	else if(key == 'D' || key == 'd')
    {
        moveChar='D';
        if (currentNode->options[3]) {
        addNewNode(currentNode);
        moveRight(currentNode);
        wasMoveMade = true;
      }
    }
    else if(key == 'A' || key == 'a')
    {
        moveChar='A';
        if (currentNode->options[0]) {
        addNewNode(currentNode);
        moveLeft(currentNode);
        wasMoveMade = true;
      }
    }
    else if(key == 'R' || key == 'r')
    {
            moveChar='R';
            deleteLinkedList(currentNode);
      moveChar = '\0';
    }
    else if(key == 'X' || key == 'x')
    {
            moveChar='X';
              deleteLinkedList(currentNode);
      cout << "Thank you for playing. Exiting...\n";
      exit(0);
    }
    else if(key == 'U' || key == 'u')
    {
            moveChar='U';
            if (currentNode->previs == NULL) {
        undoCheck = true;
      } else {
        undoLastMove(currentNode);
      }
    }
    else if(key == 'I' || key == 'i')
    {
            moveChar='I';
            opt_flag=1;
            displayAndInitializeGameData(currentNode, wasMoveMade, moveChar);
            possibleMoves(currentNode);
            optimal_move(currentNode, wasMoveMade);
            gameEnd = checkGameEnd(currentNode);
    }

    glutPostRedisplay();
}

void Select(int ch){

	switch(ch){
    case 1:
        {
            deleteLinkedList(currentNode);
            cout << "Thank you for playing. Exiting...\n";
            exit(0);
        }
            break;
    case 2:
        {
            deleteLinkedList(currentNode);
            moveChar = '\0';
        }
            break;
    case 3:
          {
                if (currentNode->previs == NULL) {
                    undoCheck = true;
                }
                else {
                    undoLastMove(currentNode);
                }
          }
            break;
    case 4:
         {
             displayAndInitializeGameData(currentNode, wasMoveMade, moveChar);
             possibleMoves(currentNode);
             optimal_move(currentNode, wasMoveMade);
             gameEnd = checkGameEnd(currentNode);
         }
             break;

	}
	glutPostOverlayRedisplay();
}

void createmenu(){
    glutCreateMenu(Select);
        glutAddMenuEntry("QUIT",1);
        glutAddMenuEntry("RESTART",2);
        glutAddMenuEntry("UNDO",3);
        glutAddMenuEntry("NEXT MOVE",4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(500,600);
	cout<<"\t\t\tFDS MINI PROJECT\n"
        <<"\t\t2048 Game Using Linked List and OpenGL\n"
        <<"\t\tCreated by Navneet Das and Madhukar"
        <<"\n\n\t\tINSTRUCTIONS FOR THE GAME\n\n"
        <<"1. USE THE W,A,S,D keys to shift the tiles in up, left, down, right direction respectively.\n"
        <<"2. USE THE U key to Undo the Move You Made.\n"
        <<"3. USE THE R key to restart the game.\n"
        <<"4. USE THE I key to access the Optimal Algorithm for attaining a maximum score(even win too).\n"
        <<"5. USE THE X key to exit the game.\n"
        <<"6. USE THE Right Mouse Button to access the menu and perform the above functions just using mouse.\n";
	glutCreateWindow("2048 by Navneet and Madhukar");
	createmenu();
    init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
 	glutMainLoop();
	return 0;
}
