#ifndef CUT_H
#define CUT_H

class Cut {
public:
	int present;
	int left, right;
	int group;
	int conflict;
	int x,y;
	int wire;

	Cut();
	Cut(int x_, int y_);

};

#endif