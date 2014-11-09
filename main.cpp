#include <iostream>
#include <ctime>
#include <string>
#include <cmath>
#include <random>
#include "png.h"
#include "cut.h"
#include "rgbapixel.h"


#define WIDTH 100
#define HEIGHT 100
#define PROB 0.15f
#define ACTIVE 1
#define DUMMY 0
#define CELLSIZE 20

enum direction_t {
	TOPLEFT,
	TOPRIGHT,
	BOTTOMLEFT,
	BOTTOMRIGHT
}

Cut board[HEIGHT][WIDTH];

int generateRandomBoard(float prob);
void removeOddCuts();
void assignWires();
void writeToFile(const char * fname);
void drawCut(PNG * file, int y, int x);
void drawWire(PNG * file, int y, int x);
void optimizeBoard();
direction_t shouldMove(int x, int y);
int costOfMove(int x, int y, direction_t dir);

using namespace std;

int main(){
	srand(time(NULL));
	generateRandomBoard(PROB);
	removeOddCuts();
	assignWires();
	writeToFile("u.png");

	optimizeBoard();
	writeToFile("o.png");
	return 0;
}

int generateRandomBoard(float prob){
	int c;
	bool assigned;
	int num_cuts;

	std::random_device rd;

	for (int i=0; i<HEIGHT; i++){
		for (int j=0; j<WIDTH; j++){
			c = rd()%(10000);
			assigned = c > (prob*10000);
			board[i][j] = (assigned)? Cut() : Cut(i,j);
			if (assigned) num_cuts++;
		}
	}
	return num_cuts;
}

void removeOddCuts(){
	int count=0;
	int last_node=-1;
	for (int i=0; i<HEIGHT; i++){
		count = 0;
		for (int j=0; j<WIDTH; j++){
			if (board[i][j].present){
				count++;
				last_node = j;
			}
		}
		if (count%2) /* is odd */
			board[i][last_node] = Cut();
	}
}

void assignWires(){
	int current = DUMMY;

	for (int i=0; i<HEIGHT; i++){
		current = DUMMY; /* reset for each line */
		for (int j=0; j<WIDTH; j++){
			if (board[i][j].present && current == DUMMY){
				/* left cut */
				board[i][j].wire = ACTIVE;
				board[i][j].left = DUMMY;
				board[i][j].right = ACTIVE;
				current = ACTIVE;
			} else if (board[i][j].present && current == ACTIVE){
				/* right cut */
				board[i][j].wire = ACTIVE;
				board[i][j].left = ACTIVE;
				board[i][j].right = DUMMY;
				current = DUMMY;
			} else {
				/* no cut */
				board[i][j].wire = current;
			}
		}
	}


}

void writeToFile(const char * fname){
	int imgw = WIDTH * CELLSIZE;
	int imgh = HEIGHT * CELLSIZE;
	PNG * file = new PNG(imgw, imgh);

	/* white background */
	for (int i=0; i<imgh; i++){
		for (int j=0; j<imgw; j++){
			(*file)(j,i)->red = (*file)(j,i)->green = (*file)(j,i)->blue = 255;
		}
	}

	for (int i=0; i<HEIGHT; i++){
		for (int j=0; j<WIDTH; j++){
			if (board[i][j].present)
				drawCut(file, i, j);
			else if (board[i][j].wire == ACTIVE)
				drawWire(file, i, j);
		}
	}

	file->writeToFile(fname);
	delete file;
}

void drawCut(PNG * file, int y, int x){
	int img_x = x * CELLSIZE;
	int img_y = y * CELLSIZE;

	int center_x = img_x +10;
	int center_y = img_y +10;
	double radius = 5.0f;
	/* draw a circle of radius 5 in the center */
	for (int i=img_x; i<img_x+CELLSIZE; i++){
		for (int j=img_y; j<img_y+CELLSIZE; j++){

			if (pow(center_x - i,2) + pow(center_y - j, 2) <= pow(radius,2)){
				(*file)(i,j)->red = 255;
				(*file)(i,j)->blue = 0;
				(*file)(i,j)->green = 0;
			}

		}
	}

	/* wire that pokes out */
	if (board[y][x].right)
		for (int i=15; i<20; i++){
			(*file)(img_x+i, img_y+10)->red = 255;
			(*file)(img_x+i, img_y+10)->blue = 0;
			(*file)(img_x+i, img_y+10)->green = 0;
		}
	if (board[y][x].left)
		for (int i=0; i<5; i++){
			(*file)(img_x+i, img_y+10)->red = 255;
			(*file)(img_x+i, img_y+10)->blue = 0;
			(*file)(img_x+i, img_y+10)->green = 0;
		}
}

void drawWire(PNG * file, int y, int x){
	int img_x = x * CELLSIZE;
	int img_y = y * CELLSIZE;

	for (int i=0; i<20; i++){
		(*file)(img_x+i, img_y+10)->red = 255;
		(*file)(img_x+i, img_y+10)->blue = 0;
		(*file)(img_x+i, img_y+10)->green = 0;
	}
}

void optimizeBoard(){

}

direction_t shouldMove(int x, int y){

}


int costOfMove(int x, int y, direction_t dir){
	int cost=0;
	switch (dir){
		case TOPLEFT:
			
		break;
		case TOPRIGHT:
			
		break;
		case BOTTOMLEFT:
			
		break;
		case BOTTOMRIGHT:
			
		break;
		default: break;
	}
	return cost;
}
