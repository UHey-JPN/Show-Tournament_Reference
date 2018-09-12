#pragma once
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <Siv3D.hpp>

#include "team.hpp"

using namespace std;

class Robot {
	int m_id;
	String m_creater;
	String m_grade;
	String m_img;
	String m_desc;
	Vec2 m_prev_img_pos;
	Vec2 m_prev_text_pos;
	int m_prev_size;
	Vec2 m_next_img_pos;
	Vec2 m_next_text_pos;
	int m_next_size;

public:
	String name; 
	Texture texture;
	Vec2 img_pos;
	Vec2 text_pos;
	int size;
	Robot(int id_, String name_, String creater_, String grade_, String img_, String desc_);
	int retId();
	void print();

	void setImgPosTarget(Vec2 target);
	void setTextPosTarget(Vec2 target);
	void setSizeTarget(int target);

	Vec2 getAndSetImgPos(double t);
	Vec2 getAndSetTextPos(double t);
	int getAndSetSize(double t);

};

class RobotManager {
	vector<Robot *> m_robots;
public:
	RobotManager(String xml);
	Robot *searchRobotById(int id);
	void print();
	~RobotManager();
};


