#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <Siv3D.hpp>
#include "tournament.hpp"

using namespace std;

class Team {
	int m_id;
	int m_robot_id1;
	int m_robot_id2;
	String m_desc;
	TournamentNode *m_pRoot;
	Vec2 m_prev_desc_pos;
	Vec2 m_next_desc_pos;
public:
	Vec2 desc_pos;
	String name;
	Vec2 tournament_pos;
	Team(int id_, String name_, int robot_id1_, int robot_id2_, String desc_, TournamentNode *root_);
	void print();
	int retId();
	int retRobotId1();
	int retRobotId2();

	void setDescPosTarget(Vec2 target);
	Vec2 getAndSetDescPos(double t);
};

class TeamManager {
	const Font m_font_tategaki;
	const int m_font_tategaki_w;
	TournamentManager *m_pTournament_manager;
	vector<Team *> m_teams;
public:
	TeamManager(String xml, TournamentManager *tournament_manager_);

	Team *searchTeamById(int search_id);
	void draw();
	void printData();
	~TeamManager();
};


