#pragma once
#include<iostream>
#include"cocos2d.h"
#include"Card.h"
#include "cocos2d\external\win32-specific\icon\include\iconv.h"
USING_NS_CC;
class BaseLayer : public Layer{
public:
	CREATE_FUNC(BaseLayer);
	virtual bool init();
	void setMouse();
	Sprite * mouse;
	int height;
	int width;
	const char* GBKToUTF8(std::string gbkStr, const char* toCode, const char* formCode);
	int select;
};