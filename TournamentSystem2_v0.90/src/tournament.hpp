#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <Siv3D.hpp>

using namespace std;

struct TournamentGlobalVariable {
	int game_count;
	int team_count;
	int max_depth;
	const double tournamentWidth;
	const double tournamentHeight;
	Vec2 tournamentPos;
	TournamentGlobalVariable() : game_count(0), tournamentWidth(Window::Width() - 100), tournamentHeight(Window::Height() / 2), team_count(0), max_depth(0),
		tournamentPos((Vec2(Window::Width(), Window::Height()) - Vec2(tournamentWidth, tournamentHeight)) / 2 + Vec2(0, -100)) {}
};

class TournamentNode {
	TournamentGlobalVariable *m_pGV;
	TournamentNode *m_pG1;
	TournamentNode *m_pG2;
	TournamentNode *m_pParent;
	vector<Line> m_lines;
	vector<Line> m_determinedLines;
	double m_x, m_y;
	int m_id;
	int m_teamid_count;
	int m_depth;
	int m_battle_log;
public:
	enum Sort {
		game,
		team
	};
	Sort sort;
	TournamentNode(const XMLElement &now, TournamentGlobalVariable *pGV_);
	~TournamentNode();

	void calcPos();
	void calcTournamentLines();
	void getTournamentLines(vector<Line> &ret);
	void getTournamentDeterminedLines(vector<Line> &ret);

	void leftWin();
	void rightWin();

	TournamentNode *retpG1();
	TournamentNode *retpG2();
	Vec2 retPos();
	int retId();
	int retTeamId();
	int retBattleLog();
	Vec2 getPosById(int search_id, Sort search_sort = Sort::game); 
	TournamentNode *getNodeById(int search_id, Sort search_sort = Sort::game);
	int getDepth();
	void print();
	void printDepth();
};

class TournamentManager {
	TournamentNode *m_pRoot;
public:
	TournamentGlobalVariable GV;

	TournamentManager(String xml);
	vector<Line> getAllTournamentLines();
	vector<Line> getAllDeterminedTournamentLines();
	void draw();
	Vec2 getPosById(int search_id, TournamentNode::Sort search_sort = TournamentNode::Sort::game); 
	TournamentNode *TournamentManager::getNodeById(int search_id, TournamentNode::Sort search_sort);
	TournamentNode *getRoot();
	~TournamentManager();
};