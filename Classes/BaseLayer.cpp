#include"BaseLayer.h"

bool BaseLayer::init() {
	setMouse();
	auto visiblesize = Director::getInstance()->getVisibleSize();
	height = visiblesize.height;
	width = visiblesize.width;
	return true;
}

void BaseLayer::setMouse() {
	ShowCursor(false);
	auto visibleSize = Director::getInstance()->getVisibleSize();

	this->mouse = Sprite::create("pic/mouse.png");
	this->mouse->setAnchorPoint(Point(0,this->mouse->getScaleY()));
	this->addChild(mouse,999);
	auto myMouseListener = EventListenerMouse::create();

	myMouseListener->onMouseMove = [=](Event *event)
	{
		EventMouse* e = (EventMouse*)event;
		this->mouse->setPosition(e->getCursorX(), e->getCursorY());
	};
	myMouseListener->onMouseDown = [=](Event *event)
	{
		this->mouse->setScale(0.8f);
	};
	myMouseListener->onMouseUp = [=](Event *event)
	{
		this->mouse->setScale(1.0f);
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(myMouseListener, this);
}


const char* BaseLayer::GBKToUTF8(std::string gbkStr, const char* toCode, const char* formCode)
{
	iconv_t iconvH;
	iconvH = iconv_open(formCode, toCode);
	if (iconvH == 0)
	{
		return 0;
	}
	const char* strChar = gbkStr.c_str();
	const char** pin = &strChar;
	size_t strLength = gbkStr.length();
	char* outbuf = (char*)malloc(strLength * 4);
	char* pBuff = outbuf;
	memset(outbuf, 0, strLength * 4);
	size_t outLength = strLength * 4;
	if (-1 == iconv(iconvH, pin, &strLength, &outbuf, &outLength))
	{
		iconv_close(iconvH);
		return 0;
	}
	gbkStr = pBuff;
	iconv_close(iconvH);
	return gbkStr.c_str();
}
