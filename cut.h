#ifndef CUT_H
#define CUT_H


enum template_t {
	NONE,
	ONE,
	TWO_DIAG,
	TWO_VERT,
	THREE_VERT,
	FOUR
};

class Cut {
public:
	int present;
	int left, right;
	template_t group;
	int group_id;
	int conflict;
	int x,y;
	int wire;

	Cut();
	Cut(int x_, int y_);

};

#endif