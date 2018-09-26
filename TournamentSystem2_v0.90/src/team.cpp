
#include "team.hpp"

Team::Team(int id_, String name_, int robot_id1_, int robot_id2_, String desc_, TournamentNode *root_) : m_id(id_), name(name_), m_robot_id1(robot_id1_), m_robot_id2(robot_id2_), m_desc(desc_), m_pRoot(root_),
tournament_pos(m_pRoot->getPosById(m_id, TournamentNode::Sort::team)), desc_pos(0, 0) {
}

void Team::print() {
	cout << m_id << ", " << name.narrow() << ", " << m_robot_id1 << ", " << m_robot_id2 << ", " << m_desc.narrow() << tournament_pos << endl;
}
int Team::retId() {
	return m_id;
}

int Team::retRobotId1() {
	return m_robot_id1;
}
int Team::retRobotId2() {
	return m_robot_id2;
}

void Team::setDescPosTarget(Vec2 target) {
	m_next_desc_pos = target;
	m_prev_desc_pos = desc_pos;
}

Vec2 Team::getAndSetDescPos(double t) {
	desc_pos = EaseOut(m_prev_desc_pos, m_next_desc_pos, Easing::Quad, t);
	return desc_pos;
}

TeamManager::TeamManager(String xml, TournamentManager *tournament_manager_) : m_font_tategaki(20, L"@"), m_font_tategaki_w(m_font_tategaki(L"あ").region().h), m_pTournament_manager(tournament_manager_) {
	XMLReader xml_reader(xml);
	XMLElement root = xml_reader.root();

	for (XMLElement team = root.firstChild(); !team.isNull(); team = team.nextSibling()) {
		int id = FromString<int>(team.attribute(L"id").value());

		// ノードの順番に依存するコード! 注意!
		String name = team.firstChild().text();
		int robot_id1 = FromString<int>(team.firstChild().nextSibling().attribute(L"id").value());
		int robot_id2 = FromString<int>(team.firstChild().nextSibling().nextSibling().attribute(L"id").value());
		String desc = team.firstChild().nextSibling().nextSibling().nextSibling().text();

		m_teams.push_back(new Team(id, name, robot_id1, robot_id2, desc, m_pTournament_manager->getRoot()));
	}
}

void TeamManager::draw() {
	const size_t nameHeight = static_cast<int>(Window::Height() - m_pTournament_manager->GV.tournamentHeight - m_pTournament_manager->GV.tournamentPos.y - m_font_tategaki(L"あ").region().w);
	for (auto &elem : m_teams) {
		if (elem->tournament_pos == Vec2(0, 0)) continue;
		const Mat3x2 mat = Mat3x2::Rotate(Pi / 2).translate(m_font_tategaki_w / 2, 20).translate(m_pTournament_manager->GV.tournamentPos);
		const Transformer2D transformer(mat.translate(elem->tournament_pos), true);
		const size_t maxChar = m_font_tategaki.drawableCharacters(elem->name, nameHeight);
		m_font_tategaki(elem->name.substr(0, maxChar)).draw(0, 0);
	}
}

Team *TeamManager::searchTeamById(int search_id) {
	for (const auto &t : m_teams) {
		if (t->retId() == search_id) return t;
	}
}

void TeamManager::printData() {
	for (const auto &t : m_teams) {
		t->print();
	}
}

TeamManager::~TeamManager() {
	for (auto &t : m_teams) {
		delete t;
	}
}