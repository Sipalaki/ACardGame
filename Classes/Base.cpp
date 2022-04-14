#include "Base.h"

bool Base::init(int c) {
	return true;
}

void Base::getdamage(int d) {
	if (this->hujia > 0) {
		hujia -= d;
		if (hujia < 0) {
			nowhealth += hujia;
			hujia = 0;
		}
	}
	else {
		nowhealth -= d;
	}
}

void Base::healthchange(int c) {
	health += c;
	nowhealth += c;
}

void Base::hujiachange(int c) {
	hujia += c;
}

void Base::region(int c) {
	nowhealth += c;
	if (nowhealth > health) {
		nowhealth = health;
	}
}