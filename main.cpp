#include <iostream>
#include <ctime>
#include <string>
#include <cmath>
#include <random>
#include "png.h"
#include "cut.h"
#include "rgbapixel.h"


#define WIDTH 1000
#define HEIGHT 1000
#define PROB 0.15f
#define ACTIVE 1
#define DUMMY 0
#define CELLSIZE 20

enum direction_t {
	TOPLEFT,
	TOPRIGHT,
	BOTTOMLEFT,
	BOTTOMRIGHT
};


Cut board[HEIGHT][WIDTH];
static int group_identifier = 1;


int generateRandomBoard(float prob);
void removeOddCuts();
void assignWires();
void writeToFile(const char * fname);
void drawCut(PNG * file, int y, int x);
void drawWire(PNG * file, int y, int x);
void optimizeBoard();
direction_t shouldMove(int x, int y);
int costOfMove(int x, int y, direction_t dir);
int determineTemplates();
bool checkTemplate(int i, int j, template_t type);
int identifyConflicts();

using namespace std;

int main(){
	srand(time(NULL));
	generateRandomBoard(PROB);
	removeOddCuts();
	assignWires();
	writeToFile("u.png");

	optimizeBoard();
	determineTemplates();
	identifyConflicts();
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
	printf("Board Generated\n");
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
	printf("%s Written\n", fname);
}

void drawCut(PNG * file, int y, int x){

	int r=0,g=0,b=0;

	int img_x = x * CELLSIZE;
	int img_y = y * CELLSIZE;

	int center_x = img_x +10;
	int center_y = img_y +10;
	double radius = 8.0f;

	if (board[y][x].conflict)
		r=255;

	/* draw a circle of radius 5 in the center */
	for (int i=img_x; i<img_x+CELLSIZE; i++){
		for (int j=img_y; j<img_y+CELLSIZE; j++){

			if (pow(center_x - i,2) + pow(center_y - j, 2) <= pow(radius,2)){
				(*file)(i,j)->red = r;
				(*file)(i,j)->blue = g;
				(*file)(i,j)->green = b;
			}

		}
	}

	/* template, if any */
	if (board[y][x].group != NONE){
		switch (board[y][x].group){
			case FOUR:
				for (int i=img_x; i<img_x+CELLSIZE; i++){
					(*file)(i,img_y+10)->red = 0;
					(*file)(i,img_y+10)->blue = 255;
					(*file)(i,img_y+10)->green = 0;
				}
			/* /four */
				/* no break, draw vertical line as well */
			case THREE_VERT:
				for (int i=img_y; i<img_y+CELLSIZE; i++){
					(*file)(img_x+10,i)->red = 0;
					(*file)(img_x+10,i)->blue = 255;
					(*file)(img_x+10,i)->green = 0;
				}
			/* /vert */
			break;
			case TWO_VERT:
				for (int i=img_y; i<img_y+CELLSIZE; i++){
					(*file)(img_x+10,i)->red = 0;
					(*file)(img_x+10,i)->blue = 255;
					(*file)(img_x+10,i)->green = 0;
				}
			/* /vert */
			break;
			default: break;

		}
	}

	/* wire that pokes out */
	if (board[y][x].right)
		for (int i=15; i<20; i++){
			(*file)(img_x+i, img_y+10)->red = 0;
			(*file)(img_x+i, img_y+10)->blue = 0;
			(*file)(img_x+i, img_y+10)->green = 0;
		}
	if (board[y][x].left)
		for (int i=0; i<5; i++){
			(*file)(img_x+i, img_y+10)->red = 0;
			(*file)(img_x+i, img_y+10)->blue = 0;
			(*file)(img_x+i, img_y+10)->green = 0;
		}
}

void drawWire(PNG * file, int y, int x){
	int img_x = x * CELLSIZE;
	int img_y = y * CELLSIZE;

	for (int i=0; i<20; i++){
		(*file)(img_x+i, img_y+10)->red = 0;
		(*file)(img_x+i, img_y+10)->blue = 0;
		(*file)(img_x+i, img_y+10)->green = 0;
	}
}

void optimizeBoard(){
	printf("Board Optimized\n");
}

direction_t shouldMove(int x, int y){
	return TOPLEFT;
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

int determineTemplates(){

	int template_count = 0;

	for (int i=0; i<HEIGHT; i++){
		for (int j=0; j<WIDTH; j++){
			if (checkTemplate(i,j,FOUR)){
				board[i][j].group = FOUR;
				board[i+1][j].group = FOUR;
				board[i][j+1].group = FOUR;
				board[i+1][j+1].group = FOUR;
				board[i][j].group_id = group_identifier;
				board[i+1][j].group_id = group_identifier;
				board[i][j+1].group_id = group_identifier;
				board[i+1][j+1].group_id = group_identifier;
				template_count++;
				group_identifier++;
				continue;
			}
			if (checkTemplate(i,j,THREE_VERT)){
				board[i][j].group = THREE_VERT;
				board[i+1][j].group = THREE_VERT;
				board[i+2][j].group = THREE_VERT;
				board[i][j].group_id = group_identifier;
				board[i+1][j].group_id = group_identifier;
				board[i+2][j].group_id = group_identifier;
				template_count++;
				group_identifier++;
				continue;
			}
			if (checkTemplate(i,j,TWO_VERT)){
				board[i][j].group = TWO_VERT;
				board[i+1][j].group = TWO_VERT;
				board[i][j].group_id = group_identifier;
				board[i+1][j].group_id = group_identifier;
				template_count++;
				group_identifier++;
				continue;
			}
			if (checkTemplate(i,j,TWO_DIAG)){
				board[i][j].group = TWO_DIAG;
				board[i][j].group_id = group_identifier;
				template_count++;
				group_identifier++;
				continue;
			}
			if (checkTemplate(i,j,ONE)){
				board[i][j].group = ONE;
				board[i][j].group_id = group_identifier;
				template_count++;
				group_identifier++;
				continue;
			}
		}
	}
	printf("%d templates assigned.\n", template_count);
	return template_count;
}

bool checkTemplate(int i, int j, template_t type){
	switch (type){
		
		case ONE:
			if (board[i][j].present && board[i][j].group==NONE)
				return true;
		break;
		case TWO_DIAG:
			return false;
		break;
		case TWO_VERT:
			if (i < HEIGHT-1){
				if (board[i][j].present &&
				    board[i+1][j].present &&
				    board[i][j].group == NONE &&
				    board[i+1][j].group == NONE)
					return true;
			}
		break;
		case THREE_VERT:
			if (i < HEIGHT-2){
				if (board[i][j].present && 
					board[i+1][j].present && 
					board[i+2][j].present &&
					board[i][j].group == NONE && 
					board[i+1][j].group == NONE && 
					board[i+2][j].group == NONE )
					return true;
			}
		break;
		case FOUR:
			if (i<HEIGHT-1 && j<WIDTH-1){
				if (board[i][j].present && 
					board[i+1][j].present && 
					board[i][j+1].present && 
					board[i+1][j+1].present &&
					board[i][j].group == NONE && 
					board[i+1][j].group == NONE && 
					board[i][j+1].group == NONE && 
					board[i+1][j+1].group == NONE)
					return true;
			}

		break;
		default: break;
	}

	return false;
}

int identifyConflicts(){
	int conflict_count = 0;

	for (int i=0; i<HEIGHT; i++){
		for (int j=0; j<WIDTH; j++){
			if (board[i][j].present){
				switch(board[i][j].group){
					case NONE: break;
					case ONE: break;
					case FOUR: /* same */
					case THREE_VERT: /* same as TWO VERT */
					case TWO_VERT:
						/* check left, top left, bottom left */
						if (j>0){
							if (board[i][j-1].present && 
								board[i][j].group_id != board[i][j-1].group_id){
									board[i][j].conflict = 1;
									board[i][j-1].conflict = 1;
							}
							if (i>0){
								if (board[i-1][j-1].present && 
									board[i][j].group_id != board[i-1][j-1].group_id){
									board[i][j].conflict = 1;
									board[i-1][j-1].conflict = 1;
								}
							}
							if (i<HEIGHT-1){
								if (board[i+1][j-1].present && 
									board[i][j].group_id != board[i+1][j-1].group_id){
										board[i][j].conflict = 1;
										board[i+1][j-1].conflict = 1;
								}
							}
						}
						/* check right, top right, bottom right */
						if (j<WIDTH-1){
							if (board[i][j+1].present && 
								board[i][j].group_id != board[i][j+1].group_id){
									board[i][j].conflict = 1;
									board[i][j+1].conflict = 1;
							}
							if (i>0){
								if (board[i-1][j+1].present && 
									board[i][j].group_id != board[i-1][j+1].group_id){
										board[i][j].conflict = 1;
										board[i-1][j+1].conflict = 1;
								}
							}
							if (i<HEIGHT-1){
								if (board[i+1][j+1].present && 
									board[i][j].group_id != board[i+1][j+1].group_id){
										board[i][j].conflict = 1;
										board[i+1][j+1].conflict = 1;
								}
							}
						}

						/* check up and down */
						if (i>0){
							if (board[i-1][j].present &&
								board[i][j].group_id != board[i-1][j].group_id){
									board[i][j].conflict = 1;
									board[i-1][j].conflict = 1;
							}
						}
						if (i<HEIGHT-1){
							if (board[i+1][j].present &&
								board[i][j].group_id != board[i+1][j].group_id){
									board[i][j].conflict = 1;
									board[i+1][j].conflict = 1;
							}
						}
					break;
					default: break;

				}
			}
		}
	}
	return conflict_count;
}
