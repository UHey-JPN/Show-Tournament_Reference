#include "tournament.hpp"

TournamentNode::TournamentNode(const XMLElement &now, TournamentGlobalVariable *pGV_) : m_pG1(nullptr), m_pG2(nullptr), m_pParent(nullptr), m_id(-1), m_pGV(pGV_), m_battle_log(-1){
	m_id = FromString<int>(now.attribute(L"id").value());
	if (now.name() == L"game") {
		m_pG1 = new TournamentNode(now.firstChild(), m_pGV);
		m_pG2 = new TournamentNode(now.firstChild().nextSibling(), m_pGV);
		m_pG1->m_pParent = this;
		m_pG2->m_pParent = this;
		m_depth = Max(m_pG1->m_depth, m_pG2->m_depth) + 1;
		m_pGV->max_depth = Max(m_depth, m_pGV->max_depth);
		m_pGV->game_count++;
		sort = Sort::game;
	} else {
		m_depth = 0;
		sort = Sort::team;
		m_pGV->max_depth = Max(m_depth, m_pGV->max_depth);
		m_teamid_count = m_pGV->team_count++;
	}
}

TournamentNode::~TournamentNode() {
	if (m_pG1) delete m_pG1;
	if (m_pG2) delete m_pG2;
	m_pG1 = nullptr;
	m_pG2 = nullptr;
}

void TournamentNode::calcPos() {
	if (sort == Sort::team) {
		m_x = m_teamid_count * (m_pGV->tournamentWidth / (m_pGV->team_count - 1));
		m_y = m_pGV->tournamentHeight; 
	} else {
		if (m_pParent) {
			m_depth = m_pParent->m_depth - 1;
		}
		m_pG1->calcPos();
		m_pG2->calcPos();
		m_x = (m_pG1->m_x + m_pG2->m_x) / 2;
		m_y = m_pGV->tournamentHeight - m_depth * (m_pGV->tournamentHeight / (m_pGV->max_depth + 1));
	}
}
void TournamentNode::calcTournamentLines() {
	if (sort == Sort::game) {
		m_pG1->calcTournamentLines();
		m_pG2->calcTournamentLines();
		m_lines.push_back(Line(m_pG1->m_x, m_y, m_x, m_y));
		m_lines.push_back(Line(m_pG2->m_x, m_y, m_x, m_y));
	}
	if (m_pParent) {
		if (sort == Sort::team) {
			m_determinedLines.push_back(Line(m_x, m_y, m_x, m_pParent->m_y));
		}
		m_lines.push_back(Line(m_x, m_y, m_x, m_pParent->m_y));
	} else {
		m_lines.push_back(Line(m_x, m_y, m_x, 0));
	}
}
void TournamentNode::getTournamentLines(vector<Line> &ret) {
	if (sort == Sort::game) {
		m_pG1->getTournamentLines(ret);
		m_pG2->getTournamentLines(ret);
	}
	for (auto &&l : m_lines) {
		ret.push_back(l);
	}
}

void TournamentNode::getTournamentDeterminedLines(vector<Line> &ret) {
	if (sort == Sort::game) {
		m_pG1->getTournamentDeterminedLines(ret);
		m_pG2->getTournamentDeterminedLines(ret);
	}
	for (auto &&l : m_determinedLines) {
		ret.push_back(l);
	}
}

void TournamentNode::leftWin() {
	m_battle_log = 0;
	m_determinedLines.clear();
	m_determinedLines.push_back(Line(m_pG1->m_x, m_y, m_x, m_y));
	if (m_pParent) {
		m_determinedLines.push_back(Line(m_x, m_y, m_x, m_pParent->m_y));
	} else {
		m_determinedLines.push_back(Line(m_x, m_y, m_x, 0));
	}
}
void TournamentNode::rightWin() {
	m_battle_log = 1;
	m_determinedLines.clear();
	m_determinedLines.push_back(Line(m_pG2->m_x, m_y, m_x, m_y));
	if (m_pParent) {
		m_determinedLines.push_back(Line(m_x, m_y, m_x, m_pParent->m_y));
	} else {
		m_determinedLines.push_back(Line(m_x, m_y, m_x, 0));
	}
}

TournamentNode *TournamentNode::retpG1() {
	return m_pG1;
}

TournamentNode *TournamentNode::retpG2() {
	return m_pG2;
}

int TournamentNode::retId() {
	return m_id;
}

int TournamentNode::retTeamId() {
	if (sort == Sort::game) {
		if (m_battle_log == 0) {
			return m_pG1->retTeamId();
		} else {
			return m_pG2->retTeamId();
		}
	}
	return m_id;
}

Vec2 TournamentNode::retPos() {
	return Vec2(m_x, m_y);
}

int TournamentNode::retBattleLog() {
	return m_battle_log;
}

Vec2 TournamentNode::getPosById(int search_id, Sort search_sort) {
	TournamentNode *searched = getNodeById(search_id, search_sort);
	if (!searched) return Vec2(0, 0);
	return searched->retPos();
	/*
	Vec2 ret(0, 0);
	if (m_id == search_id && sort == search_sort) {
		ret = Vec2(m_x, m_y);
	} else {
		if (sort == Sort::team) return ret;
		ret = m_pG1->getPosById(search_id, search_sort);
		if (ret == Vec2(0, 0)) {
			ret = m_pG2->getPosById(search_id, search_sort);
		}
	}
	return ret;
	*/
}

TournamentNode *TournamentNode::getNodeById(int search_id, Sort search_sort) {
	TournamentNode *ret = nullptr;
	if (m_id == search_id && sort == search_sort) {
		ret = this;
	} else {
		if (sort == Sort::team) return ret;
		ret = m_pG1->getNodeById(search_id, search_sort);
		if (ret == nullptr) {
			ret = m_pG2->getNodeById(search_id, search_sort);
		}
	}
	return ret;
}

int TournamentNode::getDepth() {
	return m_depth;
}

void TournamentNode::print() {
	if (sort == Sort::team) {
		cout << m_id;
	} else {
		cout << "[<" << m_id << "> ";
		m_pG1->print();
		cout << ", ";
		m_pG2->print();
		cout << "]";
	}
}
void TournamentNode::printDepth() {
	if (sort == Sort::team) {
		cout << m_depth;
	} else {
		cout << "[<" << m_depth << "> ";
		m_pG1->printDepth();
		cout << ", ";
		m_pG2->printDepth();
		cout << "]";
	}
}

TournamentManager::TournamentManager(String xml) : GV() {
	XMLReader xml_reader(xml);
	m_pRoot = new TournamentNode(xml_reader.root().firstChild(), &GV);
	GV.max_depth = m_pRoot->getDepth();
	m_pRoot->calcPos();
	m_pRoot->calcTournamentLines();
}
vector<Line> TournamentManager::getAllTournamentLines() {
	vector<Line> ret;
	m_pRoot->getTournamentLines(ret);
	return ret;
}
vector<Line> TournamentManager::getAllDeterminedTournamentLines() {
	vector<Line> ret;
	m_pRoot->getTournamentDeterminedLines(ret);
	return ret;
}

void TournamentManager::draw() {
	vector<Line> lines = getAllTournamentLines();
	for (auto &&l : lines) {
		l.movedBy(GV.tournamentPos).draw();
	}
	vector<Line> dlines = getAllDeterminedTournamentLines();
	for (auto &&l : dlines) {
		l.movedBy(GV.tournamentPos).draw(5, Palette::Lime);
	}
}
Vec2 TournamentManager::getPosById(int search_id, TournamentNode::Sort search_sort) {
	return m_pRoot->getPosById(search_id, search_sort);
}
TournamentNode *TournamentManager::getNodeById(int search_id, TournamentNode::Sort search_sort) {
	return m_pRoot->getNodeById(search_id, search_sort);
}
TournamentNode *TournamentManager::getRoot() {
	return m_pRoot;
}
TournamentManager::~TournamentManager() {
	delete m_pRoot;
}