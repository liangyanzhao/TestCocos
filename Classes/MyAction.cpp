#include "MyAction.h"
#include "RandomNum.h"

#define random_num RandomNum::getInstance()
#define director Director::getInstance()
#define PI 3.14159265 // Բ����
#define MAX_TOUCH_TIME 2.0f // �������ʱ�䣬��������ٶ�
#define MAX_UINT 0xFFFFFFFF

/*
��������  �����������鲻�á�����
*/
void MyAction::changeScene(cocos2d::Scene * scene)
{
	//auto reScene = CCTransitionProgressInOut::create(1.0f, scene);
	director->replaceScene(scene);
}

/*
����һ��Node
*/
cocos2d::Node * MyAction::createNode(int tag, cocos2d::PhysicsBody * physics_body, bool dynamic)
{
	auto node = Node::create();
	node->setTag(tag);
	if (physics_body != NULL) {
		node->setPhysicsBody(physics_body);
		node->getPhysicsBody()->setContactTestBitmask(MAX_UINT);
		node->getPhysicsBody()->setDynamic(dynamic);
	}
	return node;
}

/*
����һ��Sprite(��������AnchorPoint)
*/
cocos2d::Sprite * MyAction::createSprite(std::string file_name, int tag, Vec2 position, cocos2d::PhysicsBody * physics_body, bool dynamic)
{
	auto sprite = Sprite::create(file_name);
	sprite->setTag(tag);
	sprite->setPosition(position);
	if (physics_body != NULL) {
		sprite->setPhysicsBody(physics_body);
		sprite->getPhysicsBody()->setContactTestBitmask(MAX_UINT);
		sprite->getPhysicsBody()->setDynamic(dynamic);
	}
	return sprite;
}

/*
����һ��Sprite(������AnchorPoint)
*/
cocos2d::Sprite * MyAction::createSprite(std::string file_name, int tag, Vec2 position, Vec2 anchorPoint, cocos2d::PhysicsBody * physics_body, bool dynamic)
{
	auto sprite = Sprite::create(file_name);
	sprite->setTag(tag);
	sprite->setAnchorPoint(anchorPoint);
	sprite->setPosition(position);
	if (physics_body != NULL) {
		sprite->setPhysicsBody(physics_body);
		sprite->getPhysicsBody()->setContactTestBitmask(MAX_UINT);
		sprite->getPhysicsBody()->setDynamic(dynamic);
	}
	return sprite;
}

/*
����һ��Node(Sprite, Label, Menu, MenuItemLabel...)��Layer��
*/
void MyAction::addNode(cocos2d::Layer * layer, cocos2d::Node * node, int localZOrder)
{
	layer->addChild(node, localZOrder);
}

/*
player������shootVelocity���ٶȷ���bullet
*/
void MyAction::shootAction(cocos2d::Layer * layer, Vec2 shootVelocity, cocos2d::Sprite * bullet, int localZOrder)
{
	bullet->getPhysicsBody()->setVelocity(shootVelocity);
	layer->addChild(bullet, localZOrder);
}

/*
����AI����Velocity
random_rangeȡֵ��Χ��[0, 1)
eg��random_range=0.1����ʵ���ٶ�=׼ȷ�ٶ�*[0.9, 1.1]
*/
cocos2d::Vec2 MyAction::calAIShootVelocity(cocos2d::Vec2 shootPos, cocos2d::Vec2 targetPos, float g, float random_range)
{
	float t = sqrt(2 * (shootPos.x + shootPos.y - targetPos.x - targetPos.y) / g);
	float vx = (shootPos.x - targetPos.x) / t;
	float vy = vx;
	if (random_range > 0 && random_range < 1) {
		int t = (int)(random_range * 1000);
		int r1 = random_num->getRandomNum(-t, t);
		int r2 = random_num->getRandomNum(-t, t);
		float k1 = 1 + (r1 / 1000.0f), k2 = 1 + (r2 / 1000.0f);
		return Vec2(-vx * k1, vy * k2);
	}
	return Vec2(-vx, vy);
}

/*
����player����Velocity
*/
cocos2d::Vec2 MyAction::calPlayerShootVelocity(cocos2d::Vec2 shootPos, cocos2d::Vec2 targetPos, int init_speed, float touch_time)
{
	if (touch_time > MAX_TOUCH_TIME) touch_time = MAX_TOUCH_TIME;
	float a = targetPos.x - shootPos.x;
	float b = targetPos.y - shootPos.y;
	float c = sqrt(a * a + b * b);
	return Vec2(init_speed * touch_time / c * a, init_speed * touch_time / c * b);
}

/*
�Ƴ�һ��Sprite(����0.1s����Ч��)
*/
void MyAction::spriteFadeOut(cocos2d::Sprite * sprite)
{
	// ������ײ�����  �����ڵ���ʱ����Ҳ�ܴ�����ײ�¼�
	sprite->getPhysicsBody()->setContactTestBitmask(0x00000000);
	// ���õ���ʱ��
	auto fadeOut = FadeOut::create(0.1f);
	// �þ���ִ��һ���¼����������Ƴ��� ���Ƴ�ֻ�Ǳ�͸������
	sprite->runAction(Sequence::create(fadeOut,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sprite)), NULL));
}

/*
���ӷ���
score += add_score
*/
void MyAction::addScore(int & score, const int add_score)
{
	score += add_score;
}

/*
���·������
*/
void MyAction::updateLabelScore(cocos2d::Label * score_label, int player_score, int AI_score, char * label_format)
{
	char t[30];
	sprintf(t, label_format, player_score, AI_score);
	score_label->setString(t);
}

/*
����Ŀ�����ʾ�÷֣�1s����ʧ
*/
void MyAction::showPerScore(cocos2d::Vec2 pos, int score, cocos2d::Layer* layer)
{
	char t[30];
	sprintf(t, "%d", score);
	auto label = Label::createWithTTF(t, "fonts/Marker Felt.ttf", 48);
	label->setColor(Color3B::RED);
	label->setPosition(pos);
	layer->addChild(label, 2);
	auto fadeIn = FadeIn::create(0.2f);
	auto fadeOut = FadeOut::create(0.8f);
	label->runAction(Sequence::create(fadeIn, fadeOut,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, label)), NULL));
}

/*
��ͷ��ת(��ʼ����Ϊ��ͷ����)
*/
void MyAction::arrowRotation(cocos2d::Sprite * arrow, Vec2 shootPos, Vec2 mousePos)
{
	float a = mousePos.x - shootPos.x;
	float b = mousePos.y - shootPos.y;
	float rotation;
	if (a < 0) rotation = -180 / PI * atan(b / a) + 180;
	else rotation = -180 / PI * atan(b / a);
	arrow->setRotation(rotation);
}

/*
�ı��ͷ��ɫ
*/
void MyAction::arrowColor(cocos2d::Sprite * arrow, float touchTime)
{
	if (touchTime > 2) touchTime = 2;
	int temp = (int)(touchTime * 100);
	arrow->setColor(Color3B(255 - temp, 255 - temp, 255 - temp));
}

/*
ѣ��Ч��
*/
void MyAction::showDizzyPic(cocos2d::Sprite * sprite, float dizzy_time)
{
	auto rotateBy = RotateBy::create(dizzy_time, 1440);
	sprite->runAction(Sequence::create(rotateBy,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sprite)), NULL));
}