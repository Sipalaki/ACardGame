#pragma once
#include<iostream>
#include"cocos2d.h"
#include"BaseLayer.h"
#include"Card.h"
#include"SceneManager.h"
#include"enemy.h"
#include"item.h"

USING_NS_CC;

class battleLayer :public BaseLayer{
public:
	virtual bool init();
	CREATE_FUNC(battleLayer);
	SceneManager *tsm;
	Point m;
	int height;
	int width;
	Vector<Card*> Deck;
	Vector<Card*> e_cardList;//敌人手牌
	Vector<Card*> s_cardList;//选中卡牌
	Vector<Card*> m_cardList;
	Card* m_pSelect;
	Vec2 pSelect;
	Vec2 m_Select;
	int cardid[2];
	int handcards = 5;
	bool ismove = false;
	int level = 1;
	int nowcard = 0;
	bool draw = false;
	bool discard = false;
	void makeResult(Ref* pSender);
	MenuItemImage* yesbutton;
	MenuItemImage* skipbutton;
	Card* m_pPlay;//牌的指针，记录前一张翻的牌
	bool m_matching;//匹配两张牌ing
	int m_point;
	Card* p_card;
	Card* n_card;
	int num1 = -1;
	int num2 = -1;
	void set_card();
	void set_ui();
	void setMouseListener();
	void update(float t);
	void enemyturn();
	void drawcard(int d, Vector<Card*>* list,bool m);
	void init_deck();
	void onAfterCreateAction(Node * pSender);
	bool isenemyturn = false;
	bool pick = true;
	int id;
	Label* hj;
	Label* ehj;
	Label* hp;
	int extradraw;
	Label* ehp;
	void enemydiscard();
	Player* pPlayer;
	Item* item_;
	enemy* pEnemy;
	ValueVector envir;
	Sprite* hujia;
	Sprite* ehujia;
	Sprite* bedamaged;
	Sprite* hujiaadd;
	Sprite* regrass;
	Sprite* attackup;
	Sprite* defenceup;
	Sprite* poison;
	Sprite* tubiao;

	bool isDone = true;
	void battlemanager(int cardid,bool four,bool isdiscard,Base* m,Base* e);
	void show(int i);
	void disappear();
	void init_effect();
	void init_pic();
	void updateCustom(float dt);
	void showlogo(std::string path);
	std::vector<int> sT;
	void itemManager(int i);
	int itemdamage = 0;
};