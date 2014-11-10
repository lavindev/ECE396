#include "cut.h"

Cut::Cut(){
	present = 0;
	x = y = -1;
	left = right = 0;
	group = NONE;
	group_id = -1;
	conflict = 0;
	wire = 0;
}
Cut::Cut(int x_, int y_){
	present = 1;
	x = x_;
	y = y_;
	left = right = 0;
	group = NONE;
	group_id = -1;
	conflict = 0;
	wire = 0;
}