#include"battleLayer.h"
#include"ui/UILayout.h"
#include<algorithm>
#include"card.h"
#include"battleManager.h"
#include "Player.h"
#include "Base.h"
#include <algorithm>
#include <ui/CocosGUI.h>
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

bool battleLayer::init() {
	int ids = 1;
	init_effect();
	init_pic();
	pEnemy = new enemy();
	pEnemy->init(ids);
	pEnemy->initcard();
	pPlayer = new Player();
	pPlayer->init(false);
	pEnemy->cardList = &e_cardList;
	pPlayer->cardList = &m_cardList;

	item_ = new Item();
	item_->getdata();


	for (int i = 0; i < 4; i++)
	{
		envir.push_back((Value)"1");
		envir.push_back((Value)"2");
		envir.push_back((Value)"3");
	}

	DataCenter newdata = DataCenter();
	pPlayer->cardlist = newdata.getValueVectorByKey("cardlist");
	pEnemy->ecardlist = newdata.getValueVectorByKey("ecardlist");
	setMouseListener();
	setMouse();
	set_ui();
	init_deck();
	set_card();
	showlogo("pic/yourturn.png");
	auto audioengine = SimpleAudioEngine::getInstance();
	audioengine->playBackgroundMusic("sound/battle_theme.mp3", true);
	pPlayer->extradamage += pPlayer->extradamage_e;
	if (pPlayer->extradamage > 0) {
		show(4);
	}
	this->scheduleUpdate();
	pPlayer->savedata();
	return true;	
}

void battleLayer::makeResult(Ref* pSender) {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	switch (((MenuItemImage*)pSender)->getTag())
	{
	case 1:
		m_pSelect->setPosition(m_Select);
		m_pSelect->scaleCard();
		m_pSelect->m_isSelect = false;
		this->m_cardList.pushBack(m_pSelect);
		battlemanager(m_pSelect->cardid, false,false, pPlayer, pEnemy);
		ehp->setString(StringUtils::format("%d", pEnemy->nowhealth));
		hp->setString(StringUtils::format("%d", pPlayer->nowhealth));
		e_cardList.erase(num1);
		ismove = true;
		pick = false;
		draw = true;
		break;
	case 11:
		discard = false;
		ismove = false;
		isenemyturn = true;
		if(tubiao)
			tubiao->setVisible(false);
		skipbutton->setEnabled(false);
		if (itemdamage > 0) {
			pEnemy->getdamage(itemdamage);
			ehp->setString(StringUtils::format("%d", pEnemy->nowhealth));
		}
		showlogo("pic/enemyturn.png");
		s_cardList.clear();
		m_pSelect = nullptr;
		break;

	case 2:
		battlemanager(s_cardList.at(0)->cardid,s_cardList.size()>2,true,pPlayer,pEnemy);
		ehp->setString(StringUtils::format("%d", pEnemy->nowhealth));
		hp->setString(StringUtils::format("%d", pPlayer->nowhealth));
		for (int i = s_cardList.size()-1; i >= 0; i--) {
			auto p = s_cardList.at(i);
			sT.push_back(p->id);
			//auto createAction = MoveTo::create(0.2, Vec2(width / 2, height / 2));
			//Sequence* sequence = Sequence::create(createAction, CallFunc::create([p, this]() {p->removeFromParent(); }) , NULL);
			//p->runAction(sequence);
			p->removeFromParentAndCleanup(true);
			s_cardList.erase(i);
		}
		std::sort(sT.begin(), sT.end());
		for (int i = sT.size() - 1; i >= 0; i--) {
			m_cardList.erase(sT.at(i));
		}
		sT.clear();
		if (m_cardList.size() == 0) {

			drawcard(3, &m_cardList, true);
			
		}
		
		for (int i = 0; i < m_cardList.size(); i++) {
			Vec2 position = Vec2(width * 9 / 12 / m_cardList.size() * i + width / 15, height / 5);
			m_cardList.at(i)->setPosition(position);
		}
		
		break;
	default:
		addChild(item_->showitem(StringUtils::format("%d", ((MenuItemImage*)pSender)->getTag()-100).c_str()),10);
		break;
	}
	
}



void battleLayer::set_card() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	m_pPlay = nullptr;
	int baseY = visibleSize.height / 5;
	int baseX = visibleSize.width * 9 / 12 /5;
	for (int i = 0; i < handcards; i++)
	{
		m_cardList.pushBack(Deck.at(nowcard++));

		this->addChild(m_cardList.at(i));
		this->m_cardList.at(i)->setPosition(Point(visibleSize.width * 9 / 10, visibleSize.height / 2));
		MoveTo* move = MoveTo::create(0.5, Vec2(baseX + i * visibleSize.width / 11, baseY));
		this->m_cardList.at(i)->runAction(move);
	}
	for (int i = 0; i < handcards; i++)
	{
		e_cardList.pushBack(Deck.at(nowcard++));
		this->addChild(e_cardList.at(i));
		e_cardList.at(i)->scaleCard();
		e_cardList.at(i)->setPosition(Point(visibleSize.width * 9 / 10, visibleSize.height / 2));
		MoveTo* move = MoveTo::create(0.5, Vec2(baseX + i * visibleSize.width / 11, baseY * 4));
		e_cardList.at(i)->runAction(move);
	}
	m_Select = Vec2(baseX + handcards * visibleSize.width / 11, baseY);
}

void battleLayer::set_ui() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	height = visibleSize.height;
	width = visibleSize.width;


	Sprite*cards = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/cardlist.png"));
	cards->setPosition(Point(visibleSize.width * 9 / 10, visibleSize.height / 2));
	this->addChild(cards);

	Sprite* player = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/knight.png"));
	player->setPosition(Point(visibleSize.width * 11 / 12, visibleSize.height / 5));
	this->addChild(player);

	Sprite* health = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/health.png"));
	health->setPosition(Point(visibleSize.width * 11 / 12, visibleSize.height / 20));
	health->setScale(1.2);
	this->addChild(health);

	hp = Label::createWithTTF(StringUtils::format("%d",pPlayer->nowhealth), "fonts/BT.ttf", 40);
	hp->setPosition(health->getPosition());
	this->addChild(hp);

	hujia = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/hujia.png"));
	hujia->setPosition(Point(visibleSize.width * 10 / 12, visibleSize.height / 20));
	hujia->setScale(1.2);
	this->addChild(hujia);
	hujia->setVisible(false);

	hj = Label::createWithTTF(StringUtils::format("%d", pPlayer->hujia), "fonts/BT.ttf", 40);
	hj->setPosition(hujia->getPosition());
	this->addChild(hj);
	hj->setVisible(false);

	Sprite* ehealth = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/health.png"));
	ehealth->setPosition(Point(visibleSize.width * 11 / 12, visibleSize.height *19/ 20));
	ehealth->setScale(1.2);
	this->addChild(ehealth);

	ehp = Label::createWithTTF(StringUtils::format("%d", pEnemy->nowhealth), "fonts/BT.ttf", 40);
	ehp->setPosition(ehealth->getPosition());
	this->addChild(ehp);
	
	ehujia = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/hujia.png"));
	ehujia->setPosition(Point(visibleSize.width * 10 / 12, visibleSize.height * 19 / 20));
	ehujia->setScale(1.2);
	this->addChild(ehujia);
	ehujia->setVisible(false);

	ehj = Label::createWithTTF(StringUtils::format("%d", pEnemy->hujia), "fonts/BT.ttf", 40);
	ehj->setPosition(ehujia->getPosition());
	this->addChild(ehj);
	ehj->setVisible(false);

	Sprite* enemy = Sprite::create("pic/eyesmonster.png");
	enemy->setPosition(Point(visibleSize.width * 11 / 12, visibleSize.height * 4 / 5));
	this->addChild(enemy);

	yesbutton = MenuItemImage::create("pic/yes.png", "pic/yes_default.png", "pic/yes_default.png", CC_CALLBACK_1(battleLayer::makeResult, this));
	yesbutton->setPosition(Point(visibleSize.width * 8 / 10, visibleSize.height * 7 / 20));
	yesbutton->setEnabled(false);
	yesbutton->setTag(1);
	skipbutton = MenuItemImage::create("pic/skip.png", "pic/skip_default.png", "pic/skip_default.png", CC_CALLBACK_1(battleLayer::makeResult, this));
	skipbutton->setPosition(Point(visibleSize.width * 9 / 10, visibleSize.height * 7 / 20));
	skipbutton->setEnabled(false);
	skipbutton->setTag(11);


	Menu* menu = Menu::create(yesbutton, skipbutton, NULL);
	menu->setPosition(Point::ZERO);
	for (int i = 0; i < item_->itemlist.size(); i++) {
		MenuItemImage* m_item = MenuItemImage::create(StringUtils::format("pic/item%d_S.png", item_->itemlist.at(i).asInt()).c_str(), StringUtils::format("pic/item%d.png", item_->itemlist.at(i).asInt()).c_str(), StringUtils::format("pic/item%d.png", item_->itemlist.at(i).asInt()).c_str(), CC_CALLBACK_1(battleLayer::makeResult, this));
		itemManager(item_->itemlist.at(i).asInt());
		m_item->setScale(0.5);
		m_item->setAnchorPoint(Vec2(0.5, 0.5));
		m_item->setPosition(Point(visibleSize.width * (i + 1) / 20, visibleSize.height * 7 / 20));
		m_item->setTag(100 + item_->itemlist.at(i).asInt());
		menu->addChild(m_item);
	}
	addChild(menu);



	bedamaged = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/bedamaged.png"));
	bedamaged->setAnchorPoint(Vec2(0.5, 0.5));
	bedamaged->setPosition(Vec2(width / 2, height / 2));
	this->addChild(bedamaged);
	bedamaged->setVisible(false);

	poison = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/poison.png"));
	poison->setAnchorPoint(Vec2(0.5, 0.5));
	poison->setPosition(Vec2(width / 2, height / 2));
	this->addChild(poison);
	poison->setVisible(false);

	hujiaadd = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/hujiazengjia.png"));
	hujiaadd->setAnchorPoint(Vec2(0.5, 0.5));
	hujiaadd->setPosition(Vec2(width / 2, height / 2));
	this->addChild(hujiaadd);
	hujiaadd->setVisible(false);

	regrass = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/regrass.png"));
	regrass->setAnchorPoint(Vec2(0.5, 0.5));
	regrass->setPosition(Vec2(width / 2, height / 2));
	this->addChild(regrass);
	regrass->setVisible(false);

	attackup = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/attackup.png"));
	attackup->setAnchorPoint(Vec2(0.5, 0.5));
	attackup->setPosition(Vec2(width / 2, height / 2));
	this->addChild(attackup);
	attackup->setVisible(false);

	defenceup = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/defup.png"));
	defenceup->setAnchorPoint(Vec2(0.5, 0.5));
	defenceup->setPosition(Vec2(width / 2, height / 2));
	this->addChild(defenceup);
	defenceup->setVisible(false);


}

void battleLayer::setMouseListener() {
	auto myMouseListener = EventListenerMouse::create();
	myMouseListener->onMouseMove = [=](Event *event)
	{
		EventMouse* e = (EventMouse*)event;
		m = Point(e->getCursorX(), e->getCursorY());
	};
	myMouseListener->onMouseDown = [=](Event *event)
	{
		if (ismove) {
			int i = -1;

			for (i = m_cardList.size() - 1; i >= 0;i--) {
				if (m_cardList.at(i)->getBoundingBox().containsPoint(m)) {
					m_cardList.at(i)->moveCard(50);
					if ((m_cardList.at(i)->m_isSelect)) {
						s_cardList.pushBack(m_cardList.at(i));
						m_cardList.at(i)->id = i;
						break;
					}
					else {
						for (int j = s_cardList.size() - 1; j >= 0;j--) {
							if (s_cardList.at(j)->getBoundingBox().containsPoint(m)) {
								s_cardList.erase(j);
								break;
							}
						}
					}
					break;
				}

			}
			
		}
		if (pick) {
			if (!ismove) {
				int i = -1;
				for (auto p : e_cardList) {
					i += 1;
					if (p->getBoundingBox().containsPoint(m)) {
						discard = false;
						yesbutton->setEnabled(true);
						if (m_pSelect == nullptr) {
							ismove = true;
							pSelect = p->getPosition();
							m_pSelect = p;
							p->moveCard(-50);
							ismove = false;
							num1 = i;
							break;
						}
						else if (m_pSelect != p) {
							ismove = true;
							m_pSelect->moveCard(-50);
							pSelect = p->getPosition();
							m_pSelect = p;
							p->moveCard(-50);
							ismove = false;
							num1 = i;
							break;
						}
					}
				}
			}
		}
	};
	myMouseListener->onMouseUp = [=](Event *event)
	{

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(myMouseListener, this);
}

void battleLayer::drawcard(int d, Vector<Card*>* list,bool m) {
	draw = false;
	if (nowcard < Deck.size()&&list->size()<=9) {

		auto visibleSize = Director::getInstance()->getVisibleSize();
		m_pPlay = nullptr;
		int baseX = visibleSize.width / 15;
		int baseY = visibleSize.height / 5;
		for (int i = 0; (i < d)&&(nowcard < Deck.size()); i++) {
			list->pushBack(Deck.at(nowcard++));
			addChild(list->at(list->size() - 1));
			list->at(list->size() - 1)->setPosition(Vec2(baseX + (list->size() - 1) * visibleSize.width / 11, baseY));
		}
		if (nowcard >= Deck.size()) {
			nowcard += 1;
			
			m_pPlay = Card::create();
			auto eFront = Sprite::create(StringUtils::format("pic/cd000.png"));
			eFront->setPosition(Point(0, 0));
			auto eBack = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/cardback.png"));
			eBack->setPosition(Point(0, 0));
			m_pPlay->bindBackSprite(eBack);
			m_pPlay->bindFrontSprite(eFront);
			m_pPlay->setName(StringUtils::format("cd000.png"));
			m_pPlay->cardid = 0;
			m_pPlay->setPosition(Point(0, 0));
			m_pPlay->setDt(0.1f);
			list->pushBack(m_pPlay);
			this->addChild(m_pPlay);
			Deck.clear();
			nowcard = 0;
			init_deck();
		}
	}
	if (m_cardList.size() > 0) {
		int X = width * 9 / 12 / m_cardList.size();
		for (int i = 0; i < m_cardList.size(); i++) {
			m_cardList.at(i)->setPosition(Vec2(width / 15 + i * X, height / 5));
		}
		if (m) {
			discard = true;
			skipbutton->setEnabled(true);
		}
	}
	if (e_cardList.size() > 0) {
		int X = width * 9 / 12 / e_cardList.size();
		for (int i = 0; i < e_cardList.size(); i++) {
			e_cardList.at(i)->setPosition(Vec2(width / 15 + i * X, height *4/ 5));
		}
		if (m) {
			discard = true;
			skipbutton->setEnabled(true);
		}
	}
}

void battleLayer::enemyturn() {
	srand(timeGetTime());
	int randnum = rand() % m_cardList.size();
	e_cardList.pushBack(m_cardList.at(randnum));
	battlemanager(m_cardList.at(randnum)->cardid, false,false, pEnemy, pPlayer);
	ehp->setString(StringUtils::format("%d", pEnemy->nowhealth));
	hp->setString(StringUtils::format("%d", pPlayer->nowhealth));
	m_cardList.erase(randnum);
	drawcard(3, &e_cardList,false);
	if (e_cardList.size() > 0) {
		for (int i = 0; i < e_cardList.size(); i++)
		{
			int X = width * 9 / 12 / e_cardList.size();
			e_cardList.at(i)->setPosition(Vec2(width / 15 + i * X, height * 4 / 5));
		}
	}

	yesbutton->setTag(1);
	for (auto p : m_cardList) {
		p->m_isSelect = false;
	}
	for (auto p : e_cardList) {
		p->m_isSelect = false;
	}
	enemydiscard();
	for (int i = 0; i < e_cardList.size(); i++) {
		if (e_cardList.at(i)->m_isFront) {
			e_cardList.at(i)->scaleCard();
		}
	}
}

void battleLayer::init_deck() {
	for (int i = 0; i < pPlayer->cardlist.size(); i++) {
		m_pPlay = Card::create();
		auto pFront = Sprite::create(StringUtils::format("pic/kn%d.png", pPlayer->cardlist[i].asInt()));
		pFront->setPosition(Point(0, 0));
		auto pBack = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/cardback.png"));
		pBack->setPosition(Point(0, 0));
		m_pPlay->bindBackSprite(pBack);
		m_pPlay->bindFrontSprite(pFront);
		m_pPlay->setName(StringUtils::format("kn%d", pPlayer->cardlist[i].asInt()));
		m_pPlay->cardid = pPlayer->cardlist[i].asInt();
		m_pPlay->setPosition(Point(0, 0));
		m_pPlay->setDt(0.1f);
		Deck.pushBack(m_pPlay);
	}

	for (int i = 0; i < pEnemy->ecardlist.size(); i++) {
		m_pPlay = Card::create();
		auto pFront = Sprite::create(StringUtils::format("pic/em%d.png", pEnemy->ecardlist[i].asInt()));
		pFront->setPosition(Point(0, 0));
		auto pBack = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/cardback.png"));
		pBack->setPosition(Point(0, 0));
		m_pPlay->bindBackSprite(pBack);
		m_pPlay->bindFrontSprite(pFront);
		m_pPlay->setName(StringUtils::format("em%d", pEnemy->ecardlist[i].asInt()));
		m_pPlay->cardid = pEnemy->ecardlist[i].asInt()+100;
		m_pPlay->setPosition(Point(0, 0));
		m_pPlay->setDt(0.1f);
		Deck.pushBack(m_pPlay);
	}

	for (int i = 0; i < envir.size(); i++) {
		m_pPlay = Card::create();
		auto pFront = Sprite::create(StringUtils::format("pic/en%d.png", envir[i].asInt()));
		pFront->setPosition(Point(0, 0));
		auto pBack = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/cardback.png"));
		pBack->setPosition(Point(0, 0));
		m_pPlay->bindBackSprite(pBack);
		m_pPlay->bindFrontSprite(pFront);
		m_pPlay->setName(StringUtils::format("en%d", envir[i].asInt()));
		m_pPlay->cardid = envir[i].asInt()+500;
		m_pPlay->setPosition(Point(0, 0));
		m_pPlay->setDt(0.1f);
		Deck.pushBack(m_pPlay);
	}
	srand(timeGetTime());
	std::random_shuffle(Deck.begin(), Deck.end());
}

void battleLayer::update(float t) {
	if (draw) {
		drawcard(3+extradraw, &m_cardList, true);
		for (auto p : m_cardList) {
			p->m_isSelect = false;
			if (!p->m_isFront)
				p->scaleCard();
		}
	}
	if (discard) {
		bool flag = true;
		yesbutton->setTag(2);
		yesbutton->setEnabled(false);
		if (s_cardList.size() % 2 == 0&&s_cardList.size()>0) {
			for (int i = 0; i < s_cardList.size()-1;i++) {
				if (s_cardList.at(i)->getName().compare(s_cardList.at(i + 1)->getName())) {
					flag = false;
				}
			}
			if (flag) {
				yesbutton->setEnabled(true);
			}
		}
	}
	if (isenemyturn) {
		enemyturn();
		isenemyturn = false;
	}
}

void battleLayer::onAfterCreateAction(Node * pSender) {
	Card* pthis = (Card*)pSender;
	pthis->removeFromParentAndCleanup(true);
}

void battleLayer::enemydiscard() {
	if (e_cardList.size() == 0) {
		drawcard(3, &e_cardList, false);
		if (e_cardList.size() > 0) {
			for (int i = 0; i < e_cardList.size(); i++)
			{
				int X = width * 9 / 12 / e_cardList.size();
				e_cardList.at(i)->setPosition(Vec2(width / 15 + i * X, height * 4 / 5));
			}
		}
		for (int i = 0; i < e_cardList.size(); i++) {
			if (e_cardList.at(i)->m_isFront) {
				e_cardList.at(i)->scaleCard();
			}
		}
	}
	if (e_cardList.size() > 0) {
		for (int i = 0; i < e_cardList.size(); i++)
		{
			int X = width * 9 / 12 / e_cardList.size();
			e_cardList.at(i)->setPosition(Vec2(width / 15 + i * X, height * 4 / 5));
		}
	}
	schedule(schedule_selector(battleLayer::updateCustom), 1.2f, kRepeatForever, 0);

}

void battleLayer::battlemanager(int cardid, bool four, bool isdiscard, Base* m, Base* e) {
	int a;
	if(!four){
	if (isdiscard) {
		switch (cardid)
		{
		case 3:
			m->hujiachange(4);
			show(2);
			SimpleAudioEngine::getInstance()->playEffect("sound/hujia.wav");

			break;
		case 4:
			m->extradamage += 1;
			show(4);
			SimpleAudioEngine::getInstance()->playEffect("sound/sword.wav");

			break;
		case 5:
			a = 0;
			for (auto p : *(m->cardList)) {
				if (p->getName().compare("kn1.png") == 0||p->getName().compare("kn5.png") == 0)
					a += 1;
			}
			show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			e->getdamage(a * 2 + m->extradamage*a - e->defence);
			break;
		case 6:
			m->defence += 1;
			show(5);
			SimpleAudioEngine::getInstance()->playEffect("sound/sword.wav");
			break;
		case 101:
			e->getdamage(4 + m->extradamage - e->defence);
			show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}

			break;
		case 102:
			m->hujiachange(3);
			show(2);
			SimpleAudioEngine::getInstance()->playEffect("sound/hujia.wav");

			break;
		case 501:
			break;
		case 1:
			e->getdamage(5 + m->extradamage - e->defence); show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			break;
		case 2:
			m->region(3);
			show(3);
			break;
		case 502:
			e->getdamage(2 + m->extradamage - e->defence); show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			break;
		case 503:
			m->region(2);
			show(3);
			break;
		default:
			break;
		}
	}
	else {
		switch (cardid)
		{
		case 0:
			m->healthchange(-10);
			break;
		case 3:
			e->hujiachange(1);
			show(2);
			SimpleAudioEngine::getInstance()->playEffect("sound/hujia.wav");

			break;
		case 4:
			break;
		case 5:
			a = 0;
			for (auto p : *(m->cardList)) {
				if (p->getName().compare("kn1.png") == 0 || p->getName().compare("kn5.png") == 0)
					a += 1;
			}
			m->getdamage(a * 1 + e->extradamage - m->defence);
			show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			break;
		case 101:
			m->getdamage(2 + e->extradamage - m->defence);
			show(1);
			if (m->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}

			break;
		case 102:
			break;
		case 501:
			break;
		case 1:
			m->getdamage(2 + e->extradamage - m->defence); show(1);
			if (m->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			break;
		case 2:
			m->region(1);
			show(3);
			break;
		case 502:
			m->hujiachange(1);
			show(2);
			SimpleAudioEngine::getInstance()->playEffect("sound/hujia.wav");

			break;
		case 503:
			e->poison += 1;
			show(6);
			break;
		default:

			break;
		}
	}
}
else {
	if (isdiscard) {
		switch (cardid)
		{
		case 3:
			m->hujiachange(12);
			show(2);
			SimpleAudioEngine::getInstance()->playEffect("sound/hujia.wav");

			break;
		case 4:
			m->extradamage += 3;
			show(4);
			SimpleAudioEngine::getInstance()->playEffect("sound/sword.wav");

			break;
		case 5:
			a = 0;
			for (auto p : *(m->cardList)) {
				if (p->getName().compare("kn1.png") == 0)
					a += 1;
			}
			show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			e->getdamage(a * 5 + m->extradamage*a - e->defence);
			break;
		case 6:
			m->defence += 3;
			show(5);
			SimpleAudioEngine::getInstance()->playEffect("sound/sword.wav");
			break;
		case 101:
			e->getdamage(12 + m->extradamage - e->defence);
			show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}

			break;
		case 102:
			m->hujiachange(12);
			show(2);
			SimpleAudioEngine::getInstance()->playEffect("sound/hujia.wav");

			break;
		case 501:
			break;
		case 1:
			e->getdamage(15 + m->extradamage - e->defence); show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			break;
		case 2:
			m->region(9);
			show(3);
			break;
		case 502:
			e->getdamage(6 + m->extradamage - e->defence); show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			break;
		case 503:
			e->poison += 4;
			break;
		default:
			break;
		}
	}
	else {
		switch (cardid)
		{
		case 0:
			m->healthchange(-10);
			break;
		case 3:
			e->hujiachange(1);
			show(2);
			SimpleAudioEngine::getInstance()->playEffect("sound/hujia.wav");

			break;
		case 4:
			break;
		case 5:
			a = 0;
			for (auto p : *(m->cardList)) {
				if (p->getName().compare("kn1.png") == 0)
					a += 1;
			}
			m->getdamage(a * 1 + e->extradamage - m->defence);
			show(1);
			if (e->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			break;
		case 101:
			m->getdamage(2 + e->extradamage - m->defence);
			show(1);
			if (m->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}

			break;
		case 102:
			break;
		case 501:
			break;
		case 1:
			m->getdamage(2 + e->extradamage - m->defence); show(1);
			if (m->tag == 0) {
				SimpleAudioEngine::getInstance()->playEffect("sound/Player_heated.wav");
			}
			else {
				SimpleAudioEngine::getInstance()->playEffect("sound/monster_heated.wav");
			}
			break;
		case 2:
			m->region(1);
			show(3);
			break;
		case 502:
			m->hujiachange(1);
			show(2);
			SimpleAudioEngine::getInstance()->playEffect("sound/hujia.wav");

			break;
		default:

			break;
		}
	}
}
	if (pPlayer->hujia > 0) {
		hujia->setVisible(true);
		hj->setVisible(true);
		hj->setString(StringUtils::format("%d", pPlayer->hujia));
	}
	else {
		hujia->setVisible(false);
		hj->setVisible(false);
	}

	if (pEnemy->hujia > 0) {
		ehujia->setVisible(true);
		ehj->setVisible(true);
		ehj->setString(StringUtils::format("%d", pEnemy->hujia));
	}
	else {
		ehujia->setVisible(false);
		ehj->setVisible(false);
	}
	if (pEnemy->nowhealth <= 0) {
		tsm->goChooseScene();
		pPlayer->money += pEnemy->money;
		pPlayer->extradamage_e = 0;
		pPlayer->savedata();
	}
	if (pPlayer->nowhealth <= 0) {
		tsm->goOpenScene();
	}
	
}



void battleLayer::show(int i) {
	switch (i)
	{
	case 1:
		tubiao = bedamaged;
		break;
	case 2:
		tubiao = hujiaadd;
		break;
	case 3:
		tubiao = regrass;
		break;
	case 4:
		tubiao = attackup;
		if (pPlayer->extradamage > 0) {
			auto atk = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/attackup_small.png"));
			atk->setPosition(Vec2(width * 9 / 12, height / 20));
			atk->setName("atkup");
			addChild(atk);
		}
		else {
			if (Node*atkup = getChildByName("atkup")) {
				atkup->removeFromParentAndCleanup(true);
			}
		}
		if (pEnemy->extradamage > 0) {
			auto atk = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/attackup_small.png"));
			atk->setPosition(Vec2(width * 9 / 12, height * 19 / 20));
			atk->setName("eatkup");
			addChild(atk);
		}
		else {
			if (Node*atkup = getChildByName("eatkup")) {
				atkup->removeFromParentAndCleanup(true);
			}
		}
		break;
	case 5:
		tubiao = defenceup;
		if (pPlayer->defence > 0) {
			auto def = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/defup_small.png"));
			def->setPosition(Vec2(width * 8 / 12, height / 20));
			def->setName("defup");
			addChild(def);
		}
		else {
			if (Node*defup = getChildByName("defup")) {
				defup->removeFromParentAndCleanup(true);
			}
		}
		if (pEnemy->defence > 0) {
			auto def = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/defup_small.png"));
			def->setPosition(Vec2(width * 8 / 12, height * 19 / 20));
			def->setName("edefup");
			addChild(def);
		}
		else {
			if (Node*defup = getChildByName("edefup")) {
				defup->removeFromParentAndCleanup(true);
			}
		}
		break;
	case 6:
		tubiao = poison;
		if (pPlayer->poison > 0) {
			auto lpoi = Label::createWithSystemFont(StringUtils::format("%d", pPlayer->poison), "Verdana", 30);
			auto poi = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/poison_small.png"));
			poi->setPosition(Vec2(width * 7 / 12, height / 20));
			lpoi->setPosition(poi->getPosition());
			poi->setName("poi");
			lpoi->setName("lpoi");
			addChild(lpoi,2);
			addChild(poi);
		}
		else {
			if (Node*poi = getChildByName("poi")) {
				poi->removeFromParentAndCleanup(true);
			}
			if (Node*lpoi = getChildByName("lpoi")) {
				lpoi->removeFromParentAndCleanup(true);
			}
		}
		if (pEnemy->poison > 0) {
			auto lpoi = Label::createWithSystemFont(StringUtils::format("%d", pEnemy->poison), "Verdana", 30);
			auto poi = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("pic/poison_small.png"));
			poi->setPosition(Vec2(width * 7 / 12, height * 19 / 20));
			poi->setName("epoi");
			lpoi->setName("elpoi");
			lpoi->setPosition(poi->getPosition());
			addChild(poi);
			addChild(lpoi,2);
		}
		else {
			if (Node*poi = getChildByName("epoi")) {
				poi->removeFromParentAndCleanup(true);
			}
			if (Node*lpoi = getChildByName("elpoi")) {
				lpoi->removeFromParentAndCleanup(true);
			}
		}
		break;
	default:
		break;
	}

	tubiao->setVisible(true);
	FadeIn* fadIn = FadeIn::create(0.5);
	Sequence* flash = Sequence::create(fadIn, CCCallFunc::create(this, callfunc_selector(battleLayer::disappear)), NULL);
	tubiao->runAction(flash);
}

void battleLayer::disappear() {
	tubiao->setVisible(false);
}

void battleLayer::init_effect() {
	SimpleAudioEngine::getInstance()->preloadEffect("sound/hujia.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/monster_heated.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/Player_heated.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/sword.wav");
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("sound/battle_theme.mp3");
}

void battleLayer::updateCustom(float dt) {
	for (int i = 0; i < e_cardList.size() - 1; i++) {
		for (int j = i + 1; j < e_cardList.size(); j++) {
			if (e_cardList.at(i)->getName().compare(e_cardList.at(j)->getName()) == 0) {
				battlemanager(e_cardList.at(i)->cardid, false,true, pEnemy, pPlayer);
				ehp->setString(StringUtils::format("%d", pEnemy->nowhealth));
				hp->setString(StringUtils::format("%d", pPlayer->nowhealth));
				e_cardList.at(i)->removeFromParent();
				e_cardList.at(j)->removeFromParent();
				e_cardList.erase(i);
				e_cardList.erase(j - 1);
				return;
			}
		}
	}
	unschedule(schedule_selector(battleLayer::updateCustom));
	pick = true;
	if (e_cardList.size() == 0) {
		drawcard(3, &e_cardList, false);
	}
	if (pEnemy->poison > 0) {
		pEnemy->healthchange(-pEnemy->poison);
		pEnemy->poison -= 1;
		show(6);
	}
	if (pPlayer->poison > 0) {
		pPlayer->healthchange(-pPlayer->poison);
		pPlayer->poison -= 1;
		show(6);
	}
	
	showlogo("pic/yourturn.png");
}

void battleLayer::init_pic() {
	TextureCache::getInstance()->addImage("pic/cardback.png");
	TextureCache::getInstance()->addImage("pic/yourturn.png");
	TextureCache::getInstance()->addImage("pic/enemyturn.png");
	TextureCache::getInstance()->addImage("pic/bedamaged.png");
	TextureCache::getInstance()->addImage("pic/regrass.png");
	TextureCache::getInstance()->addImage("pic/hujiazengjia.png");
	TextureCache::getInstance()->addImage("pic/knight.png");
	TextureCache::getInstance()->addImage("pic/health.png");
	TextureCache::getInstance()->addImage("pic/hujia.png");
	TextureCache::getInstance()->addImage("pic/attackup.png");
	TextureCache::getInstance()->addImage("pic/attackup_small.png");
	TextureCache::getInstance()->addImage("pic/defup.png");
	TextureCache::getInstance()->addImage("pic/defup_small.png");
	TextureCache::getInstance()->addImage("pic/cardlist.png");
	TextureCache::getInstance()->addImage("pic/poison.png");
	TextureCache::getInstance()->addImage("pic/poison_small.png");
}

void battleLayer::showlogo(std::string path) {
	auto logo = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(path));
	logo->setAnchorPoint(Vec2(0.5, 0.5));
	logo->setPosition(Vec2(-0.5*width, height / 2));
	auto move = MoveBy::create(0.3f, Vec2(width, 0));
	auto Fadin = FadeIn::create(0.5f);
	auto seq = Sequence::create(move, Fadin, CallFunc::create([logo, this]() {logo->removeFromParentAndCleanup(true); }), NULL);
	logo->runAction(seq);
	addChild(logo);
}

void battleLayer::itemManager(int i) {
	switch (i)
	{
	case 1:
		srand(timeGetTime());
		if(rand()%2>0.5)
			extradraw += 1;
		break;
	case 2:
		itemdamage += 1;
		break;
	default:
		break;
	}
}