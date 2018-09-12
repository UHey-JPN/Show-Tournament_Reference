#include "robot.hpp"

Robot::Robot(int id_, String name_, String creater_, String grade_, String img_, String desc_) : m_id(id_), name(name_), m_creater(creater_), m_grade(grade_), m_img(img_), m_desc(desc_), texture(L"DB\\img\\" + m_img), img_pos(0, 0), text_pos(0, 0), size(Window::Height() / 3) {}

void Robot::print() {
	cout << m_id << ", " << name.narrow() << ", " << m_creater.narrow() << ", " << m_grade.narrow() << ", " << m_img.narrow() << ", " << m_desc.narrow() << endl;
}

RobotManager::RobotManager(String xml) {
	XMLReader xml_reader(xml);
	XMLElement root = xml_reader.root();

	for (XMLElement robot = root.firstChild(); !robot.isNull(); robot = robot.nextSibling()) {
		int id = FromString<int>(robot.attribute(L"id").value());

		// ノードの順番に依存するコード! 注意!
		String name = robot.firstChild().text();
		String creater = robot.firstChild().nextSibling().text();
		String grade = robot.firstChild().nextSibling().nextSibling().text();
		String img = robot.firstChild().nextSibling().nextSibling().nextSibling().text();
		String desc = robot.firstChild().nextSibling().nextSibling().nextSibling().nextSibling().text();

		m_robots.push_back(new Robot(id, name, creater, grade, img, desc));
	}
}

void Robot::setImgPosTarget(Vec2 target) {
	m_prev_img_pos = img_pos;
	m_next_img_pos = target;
}
void Robot::setTextPosTarget(Vec2 target) {
	m_prev_text_pos = text_pos;
	m_next_text_pos = target;
}
void Robot::setSizeTarget(int target) {
	m_prev_size = size;
	m_next_size = target;
}

Vec2 Robot::getAndSetImgPos(double t) {
	img_pos = EaseOut(m_prev_img_pos, m_next_img_pos, Easing::Quad, t);
	return img_pos;
}
Vec2 Robot::getAndSetTextPos(double t) {
	text_pos = EaseOut(m_prev_text_pos, m_next_text_pos, Easing::Quad, t);
	return text_pos;
}
int Robot::getAndSetSize(double t) {
	size = EaseOut(m_prev_size, m_next_size, Easing::Quad, t);
	return size;
}

int Robot::retId() {
	return m_id;
}

Robot *RobotManager::searchRobotById(int id) {
	for (const auto &r : m_robots) {
		if (r->retId() == id) {
			return r;
		}
	}
}

void RobotManager::print() {
	for (const auto &r : m_robots) {
		r->print();
	}
}

RobotManager::~RobotManager() {
	for (const auto &r : m_robots) {
		delete r;
	}
}