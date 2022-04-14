#pragma once
#include"cocos2d.h"
#include "Card.h"
USING_NS_CC;
class Base {
public:
	int health;
	int nowhealth;
	int hujia;
	int money;
	int tag;
	int extradamage = 0;
	int defence = 0;
	bool alive = true;
	void getdamage(int d);
	void healthchange(int c);
	void hujiachange(int c);
	bool init(int id);
	void region(int c);
	int poison = 0;
	int extradamage_e = 0;
	Vector<Card*>* cardList;
};

