#include <Siv3D.hpp>
#include "tournament.hpp"
#include "team.hpp"
#include "robot.hpp"
#include "HamFramework.hpp"
#include "GV.h"
#include "sock.hpp"
#include "Encode.hpp"


struct CommonData {
	Font team_desc;
	Font robot_desc;
	Font WinLose_desc;
	TournamentManager *tournament_manager;
	TeamManager *team_manager;
	RobotManager *robot_manager;
	Robot *r_l1;
	Robot *r_l2;
	Robot *r_r1;
	Robot *r_r2;
	Team *t_l;
	Team *t_r;
	VideoPlayer *op;
	VideoPlayer *winner;
	TournamentNode *now_game;
	int now_game_id;
	int twice;
	int robots_hidden_alpha;
};


using MyApp = SceneManager<String, CommonData>;

namespace Scene {
	class Init : public MyApp::Scene {
	public:

		void saveXML(string kind) {
			ifstream ifs(kind + "_.txt");
			string line;
			getline(ifs, line);
			ofstream ofs(kind + ".xml");
			while (getline(ifs, line)) {
				ofs << line << endl;
			}
		}

		void saveTXT(string name, string data) {
			ofstream ofs(name + "_.txt");
			ofs << data;
		}

		void init() override {
			if (winner_effect) {
				m_data->winner = new VideoPlayer(Widen("./resources/winner.wmv"), false);
				if (!m_data->winner->isOpened()) {
					cout << "Winner動画形式エラー" << endl;
				} else {
					cout << "Winner動画OK" << endl;
				}
			}

			if (use_network) {

				TCPClientSync tcp_client1(server_ip_address, server_tcp_data_port);
				string ret1 = tcp_client1.sendAndReceive("tournament,utf-8\r\n");
				cout << utf8_to_multi_winapi(ret1) << endl;

				TCPClientSync tcp_client2(server_ip_address, server_tcp_data_port);
				string ret2 = tcp_client2.sendAndReceive("team,utf-8\r\n");
				cout << utf8_to_multi_winapi(ret2) << endl;

				TCPClientSync tcp_client3(server_ip_address, server_tcp_data_port);
				string ret3 = tcp_client3.sendAndReceive("robot,utf-8\r\n");
				cout << utf8_to_multi_winapi(ret3) << endl;

				saveTXT("tournament", ret1);
				saveTXT("team", ret2);
				saveTXT("robot", ret3);

				/*
				sprintf(cmd1, "echo tournament,utf-8 | nc %s %d >tournament_.txt", server_ip_address.c_str(), GV::ports["database_port"]);
				sprintf(cmd2, "echo team,utf-8 | nc %s %d >team_.txt", server_ip_address.c_str(), GV::ports["database_port"]);
				sprintf(cmd3, "echo robot,utf-8 | nc %s %d >robot_.txt", server_ip_address.c_str(), GV::ports["database_port"]);

				cout << string(cmd1) << endl;

				system(cmd1);
				system(cmd2);
				system(cmd3);
				*/


				if (xml_write_ok) {
					saveXML("tournament");
					saveXML("team");
					saveXML("robot");
				}
			} 


			m_data->tournament_manager = new TournamentManager(L"tournament.xml");
			m_data->team_manager = new TeamManager(L"team.xml", m_data->tournament_manager);
			m_data->robot_manager = new RobotManager(L"robot.xml");
			m_data->now_game_id = 0;
			

			GV::battle_logs.resize(m_data->tournament_manager->GV.game_count);
			for (int i = 0; i < m_data->tournament_manager->GV.game_count; i++) {
				GV::battle_logs[i] = -1;
			}
			cout << team_desc_fontsize << endl;
			cout << robot_desc_fontsize << endl;
			m_data->team_desc = Font(team_desc_fontsize);
			m_data->robot_desc = Font(robot_desc_fontsize);
			m_data->WinLose_desc = Font(winner_desc_fontsize);

		}
		void updateFadeIn(double t) {
			m_data->robot_manager->print();
			changeScene(L"Pause", 0);
		}
	};

	class Pause : public MyApp::Scene {
		void update() override {
			if ((!use_network && Input::KeyEnter.clicked) || (use_network && GV::show_state == "opening")) {
				changeScene(L"Opening", 0);
			}
		}
	};

	class Opening : public MyApp::Scene {
		void init() override {
			m_data->op = new VideoPlayer(Widen(opening_video_path), false);
			if (!m_data->op->isOpened()) {
				cout << "オープニング動画形式エラー" << endl;
				changeScene(L"Main", 100);
			} else {
				cout << "オープニング動画OK" << endl;
			}
			m_data->op->play();
		}
		void update() override {
			if (m_data->op->getPosSec() < m_data->op->getLengthSec() - 0.1) {
				m_data->op->update();
				double w = m_data->op->getSize().x;
				double h = m_data->op->getSize().y;
				double p = Max(w / Window::Width(), h / Window::Height());

				//cout << w << ", " << h << ", " << p << endl;

				m_data->op->getFrameTexture().resize(static_cast<int>(w / p), static_cast<int>(h / p)).drawAt(Window::Center());
			} else {
				changeScene(L"Main", 100);
			}
			if ((!use_network && Input::KeyEnter.clicked) || (use_network && (GV::show_state == "home" || GV::show_state == "preshow" || GV::show_state == "show0" || GV::show_state == "show1" || GV::show_state == "show2"))) {
				changeScene(L"Main", 100);
			}
		}
		void drawFadeIn(double t) const override {
			
		}
		void draw() const override {

		}
	};

	class Main : public MyApp::Scene {
	public:
		void init() override {

		}

		void update() override {

			if (use_network) {
				m_data->now_game_id = -1;
				for (size_t i = 0; i < GV::battle_logs.size(); i++) {
					if (GV::battle_logs[i] != -1) m_data->now_game_id = i;
				}
				m_data->now_game_id = Min(m_data->tournament_manager->GV.game_count - 1, m_data->now_game_id + 1);

				for (size_t i = 0; i < GV::battle_logs.size(); i++) {
					TournamentNode *node = m_data->tournament_manager->getNodeById(i, TournamentNode::Sort::game);
					if (node == nullptr) continue;
					if (GV::battle_logs[i] == 0) node->leftWin();
					else if (GV::battle_logs[i] == 1) node->rightWin();
				}

				if (GV::show_state == "opening") changeScene(L"Opening", 100, true);
				if (GV::show_state == "newgame") changeScene(L"Fight_Init", 0, true);
				return;
			}
			if (Input::KeyEnter.clicked) {
				int this_game_log = m_data->tournament_manager->getNodeById(m_data->now_game_id, TournamentNode::Sort::game)->retBattleLog();
				if (this_game_log == -1) {
					changeScene(L"Fight_Init", 0, true);
				} else if (this_game_log == 0) {
					m_data->tournament_manager->getNodeById(m_data->now_game_id, TournamentNode::Sort::game)->rightWin();
				} else {
					m_data->tournament_manager->getNodeById(m_data->now_game_id, TournamentNode::Sort::game)->leftWin();
				}
			}

			if (Input::KeyLeft.clicked) {
				m_data->now_game_id = Max(0, m_data->now_game_id - 1);
			} else if (Input::KeyRight.clicked) {
				m_data->now_game_id = Min(m_data->tournament_manager->GV.game_count - 1, m_data->now_game_id + 1);
			}
		}
		void drawFadeIn(double t) const override {
			m_data->tournament_manager->draw();
			m_data->team_manager->draw();
			Circle(m_data->tournament_manager->getRoot()->getPosById(m_data->now_game_id), 10).movedBy(m_data->tournament_manager->GV.tournamentPos).draw();
			Window::ClientRect().draw(Color(0, 0, 0, (1 - t) * 200));

		}
		void drawFadeOut(double t) const override {
		}

		void draw() const override {
			m_data->tournament_manager->draw();
			m_data->team_manager->draw();
			Circle(m_data->tournament_manager->getRoot()->getPosById(m_data->now_game_id), 10).movedBy(m_data->tournament_manager->GV.tournamentPos).draw();

		}
	};

	class Fight_Init : public MyApp::Scene {
		void init() override {
			m_data->now_game = m_data->tournament_manager->getNodeById(m_data->now_game_id, TournamentNode::Sort::game);
			m_data->t_l = m_data->team_manager->searchTeamById(m_data->now_game->retpG1()->retTeamId());
			m_data->t_r = m_data->team_manager->searchTeamById(m_data->now_game->retpG2()->retTeamId());
			m_data->r_l1 = m_data->robot_manager->searchRobotById(m_data->t_l->retRobotId1());
			m_data->r_l2 = m_data->robot_manager->searchRobotById(m_data->t_l->retRobotId2());
			m_data->r_r1 = m_data->robot_manager->searchRobotById(m_data->t_r->retRobotId1());
			m_data->r_r2 = m_data->robot_manager->searchRobotById(m_data->t_r->retRobotId2());


			m_data->now_game->print();
			m_data->t_l->print();
			m_data->t_r->print();
			m_data->r_l1->print();
			m_data->r_l2->print();
			m_data->r_r1->print();
			m_data->r_r2->print();


			m_data->t_l->setDescPosTarget(Vec2(-m_data->team_desc(m_data->t_l->name).region().w / 2, 100));
			m_data->r_l1->setImgPosTarget(Vec2(-Window::Height() / 4, Window::Height() / 3));
			m_data->r_l2->setImgPosTarget(Vec2(-Window::Height() / 4, 2 * Window::Height() / 3 + 25));
			m_data->t_r->setDescPosTarget(Vec2(Window::Width() + m_data->team_desc(m_data->t_r->name).region().w / 2, 100));
			m_data->r_r1->setImgPosTarget(Vec2(Window::Width() + Window::Height() / 4, Window::Height() / 3));
			m_data->r_r2->setImgPosTarget(Vec2(Window::Width() + Window::Height() / 4, 2 * Window::Height() / 3 + 25));
			m_data->r_l1->setSizeTarget(Window::Height() / 3);
			m_data->r_l2->setSizeTarget(Window::Height() / 3);
			m_data->r_r1->setSizeTarget(Window::Height() / 3);
			m_data->r_r2->setSizeTarget(Window::Height() / 3);

			m_data->t_l->getAndSetDescPos(1);
			m_data->t_r->getAndSetDescPos(1);
			m_data->r_l1->getAndSetImgPos(1);
			m_data->r_l2->getAndSetImgPos(1);
			m_data->r_r1->getAndSetImgPos(1);
			m_data->r_r2->getAndSetImgPos(1);
			m_data->r_l1->getAndSetSize(1);
			m_data->r_l2->getAndSetSize(1);
			m_data->r_r1->getAndSetSize(1);
			m_data->r_r2->getAndSetSize(1);
			m_data->twice = 0;
			

			m_data->r_l1->setTextPosTarget(Vec2(-m_data->robot_desc(m_data->r_l1->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_l2->setTextPosTarget(Vec2(Window::Width() + m_data->robot_desc(m_data->r_l2->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_l1->getAndSetTextPos(1);
			m_data->r_l2->getAndSetTextPos(1);

			m_data->r_r1->setTextPosTarget(Vec2(-m_data->robot_desc(m_data->r_r1->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_r2->setTextPosTarget(Vec2(Window::Width() + m_data->robot_desc(m_data->r_r2->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_r1->getAndSetTextPos(1);
			m_data->r_r2->getAndSetTextPos(1);
		}
		void update() override {
			changeScene(L"Fight_Main", 500, true);
		}
		void draw() const override {
			m_data->tournament_manager->draw();
			m_data->team_manager->draw();
			Circle(m_data->tournament_manager->getRoot()->getPosById(m_data->now_game_id), 10).movedBy(m_data->tournament_manager->GV.tournamentPos).draw();

		}
	};

	class Fight_Main : public MyApp::Scene {

		void init() override {

			m_data->t_l->setDescPosTarget(Vec2(Window::Width() / 4, 100));
			m_data->r_l1->setImgPosTarget(Vec2(Window::Width() / 4, Window::Height() / 3));
			m_data->r_l2->setImgPosTarget(Vec2(Window::Width() / 4, 2 * Window::Height() / 3 + 25));
			m_data->t_r->setDescPosTarget(Vec2(3 * Window::Width() / 4, 100));
			m_data->r_r1->setImgPosTarget(Vec2(3 * Window::Width() / 4, Window::Height() / 3));
			m_data->r_r2->setImgPosTarget(Vec2(3 * Window::Width() / 4, 2 * Window::Height() / 3 + 25));

			m_data->r_l1->setTextPosTarget(Vec2(-m_data->robot_desc(m_data->r_l1->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_l2->setTextPosTarget(Vec2(Window::Width() + m_data->robot_desc(m_data->r_l2->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_r1->setTextPosTarget(Vec2(-m_data->robot_desc(m_data->r_r1->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_r2->setTextPosTarget(Vec2(Window::Width() + m_data->robot_desc(m_data->r_r2->name).region().w / 2, 5 * Window::Height() / 6));

			m_data->r_l1->setSizeTarget(Window::Height() / 3);
			m_data->r_l2->setSizeTarget(Window::Height() / 3);
			m_data->r_r1->setSizeTarget(Window::Height() / 3);
			m_data->r_r2->setSizeTarget(Window::Height() / 3);
		}
		void update() override {
			if (use_network) {
			
				if (!m_data->twice && GV::show_state == "l_interview") changeScene(L"TeamL", 500, true);
				if (!m_data->twice && GV::show_state == "r_interview") changeScene(L"TeamR", 500, true);
				if (m_data->twice && GV::show_state == "l_interview") {
					for (size_t i = 0; i < GV::battle_logs.size(); i++) {
						TournamentNode *node = m_data->tournament_manager->getNodeById(i, TournamentNode::Sort::game);
						if (node == nullptr) continue;
						if (GV::battle_logs[i] == 0) node->leftWin();
						else if (GV::battle_logs[i] == 1) node->rightWin();
					}
					changeScene(L"TeamL", 500, true);
				}
				if (m_data->twice && GV::show_state == "r_interview") {
					for (size_t i = 0; i < GV::battle_logs.size(); i++) {
						TournamentNode *node = m_data->tournament_manager->getNodeById(i, TournamentNode::Sort::game);
						if (node == nullptr) continue;
						if (GV::battle_logs[i] == 0) node->leftWin();
						else if (GV::battle_logs[i] == 1) node->rightWin();
					}
					changeScene(L"TeamR", 500, true);
				}
				if (GV::show_state == "home" || GV::show_state == "preshow" || GV::show_state == "show0" || GV::show_state == "show1" || GV::show_state == "show2") {
					changeScene(L"Main", 500, true);
				}
				return;
			}

			if (!m_data->twice && Input::KeyEnter.clicked) {
				changeScene(L"TeamL", 500, true);
			}

			if (m_data->twice && Input::KeyLeft.clicked) {
				GV::battle_logs[m_data->now_game_id] = 0;
				m_data->now_game->leftWin();
				changeScene(L"TeamL", 500, true);
			}
			if (m_data->twice && Input::KeyRight.clicked) {
				GV::battle_logs[m_data->now_game_id] = 1;
				m_data->now_game->rightWin();
				changeScene(L"TeamR", 500, true);
			}
			if (!m_data->twice && Input::KeyZ.clicked) {
				changeScene(L"Main", 500, true);
			}
			if (m_data->twice && Input::KeyZ.clicked) {
				m_data->twice = 0;
				changeScene(L"TeamR", 500, true);
			}
		}
		void updateFadeIn(double t) override {
			m_data->t_l->getAndSetDescPos(t);
			m_data->t_r->getAndSetDescPos(t);
			m_data->r_l1->getAndSetImgPos(t);
			m_data->r_l2->getAndSetImgPos(t);
			m_data->r_r1->getAndSetImgPos(t);
			m_data->r_r2->getAndSetImgPos(t);
			m_data->r_l1->getAndSetTextPos(t);
			m_data->r_l2->getAndSetTextPos(t);
			m_data->r_r1->getAndSetTextPos(t);
			m_data->r_r2->getAndSetTextPos(t);
			m_data->r_l1->getAndSetSize(t);
			m_data->r_l2->getAndSetSize(t);
			m_data->r_r1->getAndSetSize(t);
			m_data->r_r2->getAndSetSize(t);
		}
		void drawFadeIn(double t) const override {

			m_data->tournament_manager->draw();
			m_data->team_manager->draw();
			Circle(m_data->tournament_manager->getRoot()->getPosById(m_data->now_game_id), 10).movedBy(m_data->tournament_manager->GV.tournamentPos).draw();
			if (!m_data->twice) {
				Window::ClientRect().draw(Color(0, 0, 0, t * 200));
			} else {
				Window::ClientRect().draw(Color(0, 0, 0, 200));
			}

			const size_t maxChar_l = m_data->team_desc.drawableCharacters(m_data->t_l->name, Window::Width() / 3);
			m_data->team_desc(m_data->t_l->name.substr(0, maxChar_l)).drawCenter(m_data->t_l->desc_pos);
			m_data->r_l1->texture.resize(m_data->r_l1->size, m_data->r_l1->size).drawAt(m_data->r_l1->img_pos);
			m_data->r_l2->texture.resize(m_data->r_l2->size, m_data->r_l2->size).drawAt(m_data->r_l2->img_pos);

			const size_t maxChar_r_l1 = m_data->robot_desc.drawableCharacters(m_data->r_l1->name, Window::Width() / 3);
			const size_t maxChar_r_l2 = m_data->robot_desc.drawableCharacters(m_data->r_l2->name, Window::Width() / 3);
			m_data->robot_desc(m_data->r_l1->name.substr(0, maxChar_r_l1)).drawCenter(m_data->r_l1->text_pos);
			m_data->robot_desc(m_data->r_l2->name.substr(0, maxChar_r_l2)).drawCenter(m_data->r_l2->text_pos);

			const size_t maxChar_r_r1 = m_data->robot_desc.drawableCharacters(m_data->r_r1->name, Window::Width() / 3);
			const size_t maxChar_r_r2 = m_data->robot_desc.drawableCharacters(m_data->r_r2->name, Window::Width() / 3);
			m_data->robot_desc(m_data->r_r1->name.substr(0, maxChar_r_r1)).drawCenter(m_data->r_r1->text_pos);
			m_data->robot_desc(m_data->r_r2->name.substr(0, maxChar_r_r2)).drawCenter(m_data->r_r2->text_pos);

			const size_t maxChar_r = m_data->team_desc.drawableCharacters(m_data->t_r->name, Window::Width() / 3);
			m_data->team_desc(m_data->t_r->name.substr(0, maxChar_r)).drawCenter(m_data->t_r->desc_pos);
			m_data->r_r1->texture.resize(m_data->r_r1->size, m_data->r_r1->size).drawAt(m_data->r_r1->img_pos);
			m_data->r_r2->texture.resize(m_data->r_r2->size, m_data->r_r2->size).drawAt(m_data->r_r2->img_pos);


		}
		void drawFadeOut(double t) const override {
		}
		void draw() const override {
			drawFadeIn(1);
		}
	};

	class TeamL : public MyApp::Scene {

		void init() override {

			m_data->t_l->setDescPosTarget(Vec2(Window::Width() / 2, 100));
			m_data->r_l1->setImgPosTarget(Vec2(Window::Width() / 3 - 25, Window::Height() / 2));
			m_data->r_l2->setImgPosTarget(Vec2(2 * Window::Width() / 3 + 25, Window::Height() / 2));

			m_data->t_r->setDescPosTarget(Vec2(Window::Width() + m_data->team_desc(m_data->t_r->name).region().w / 2, 100));
			m_data->r_r1->setImgPosTarget(Vec2(Window::Width() + Window::Height() / 4, Window::Height() / 3));
			m_data->r_r2->setImgPosTarget(Vec2(Window::Width() + Window::Height() / 4, 2 * Window::Height() / 3 + 25));

			m_data->r_l1->setTextPosTarget(Vec2(Window::Width() / 3 - 25, 5 * Window::Height() / 6));
			m_data->r_l2->setTextPosTarget(Vec2(2 * Window::Width() / 3 + 25, 5 * Window::Height() / 6));
			m_data->r_r1->setTextPosTarget(Vec2(-m_data->robot_desc(m_data->r_r1->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_r2->setTextPosTarget(Vec2(Window::Width() + m_data->robot_desc(m_data->r_r2->name).region().w / 2, 5 * Window::Height() / 6));

			m_data->r_l1->setSizeTarget(Window::Height() / 2);
			m_data->r_l2->setSizeTarget(Window::Height() / 2);
			m_data->r_r1->setSizeTarget(Window::Height() / 2);
			m_data->r_r2->setSizeTarget(Window::Height() / 2);

			if (winner_effect && GV::battle_logs[m_data->now_game_id] == 0) {
				m_data->winner->play();
				m_data->robots_hidden_alpha = 200;
			}
		}
		void update() override {

			if (use_network) {
				if (!m_data->twice && GV::show_state == "game") {
					m_data->twice = 1;
					changeScene(L"Fight_Main", 500, true);
				}

				if (GV::show_state == "r_interview") changeScene(L"TeamR", 500, true);

				if (m_data->twice && GV::show_state == "result") changeScene(L"Fight_Main", 500, true);
				if (m_data->twice && GV::show_state == "vgoal") changeScene(L"Fight_Main", 500, true);
				if (GV::show_state == "home" || GV::show_state == "preshow" || GV::show_state == "show0" || GV::show_state == "show1" || GV::show_state == "show2") {
					changeScene(L"Main", 500, true);
				}
				if (winner_effect) {
					m_data->winner->update();
					if (m_data->robots_hidden_alpha > 0) {
						m_data->robots_hidden_alpha--;
					}
				}
				return;
			}
			if (!m_data->twice && Input::KeyEnter.clicked) {
				changeScene(L"TeamR", 500, true);
			}
			if (!m_data->twice && Input::KeyZ.clicked) {
				changeScene(L"Fight_Main", 500, true);
			}
			if (m_data->twice && Input::KeyEnter.clicked) {
				m_data->now_game_id = Min(m_data->tournament_manager->GV.game_count - 1, m_data->now_game_id + 1);
				changeScene(L"Main", 500, true);
			}
			if (winner_effect) {
				m_data->winner->update();
				if (m_data->robots_hidden_alpha > 0) {
					m_data->robots_hidden_alpha--;
				}
			}

		}
		void updateFadeIn(double t) override {

			m_data->t_l->getAndSetDescPos(t);
			m_data->t_r->getAndSetDescPos(t);
			m_data->r_l1->getAndSetImgPos(t);
			m_data->r_l2->getAndSetImgPos(t);
			m_data->r_r1->getAndSetImgPos(t);
			m_data->r_r2->getAndSetImgPos(t);
			m_data->r_l1->getAndSetTextPos(t);
			m_data->r_l2->getAndSetTextPos(t);
			m_data->r_r1->getAndSetTextPos(t);
			m_data->r_r2->getAndSetTextPos(t);
			m_data->r_l1->getAndSetSize(t);
			m_data->r_l2->getAndSetSize(t);
			m_data->r_r1->getAndSetSize(t);
			m_data->r_r2->getAndSetSize(t);
			if (winner_effect) m_data->winner->update();

		}
		void drawFadeIn(double t) const override {
			m_data->tournament_manager->draw();
			m_data->team_manager->draw();
			Circle(m_data->tournament_manager->getRoot()->getPosById(m_data->now_game_id), 10).movedBy(m_data->tournament_manager->GV.tournamentPos).draw();
			Window::ClientRect().draw(Color(0, 0, 0, 200));
			const size_t maxChar_l = m_data->team_desc.drawableCharacters(m_data->t_l->name, Window::Width() / 3);
			m_data->team_desc(m_data->t_l->name.substr(0, maxChar_l)).drawCenter(m_data->t_l->desc_pos);
			m_data->r_l1->texture.resize(m_data->r_l1->size, m_data->r_l1->size).drawAt(m_data->r_l1->img_pos);
			m_data->r_l2->texture.resize(m_data->r_l2->size, m_data->r_l2->size).drawAt(m_data->r_l2->img_pos);

			const size_t maxChar_r_l1 = m_data->robot_desc.drawableCharacters(m_data->r_l1->name, Window::Width() / 3);
			const size_t maxChar_r_l2 = m_data->robot_desc.drawableCharacters(m_data->r_l2->name, Window::Width() / 3);
			m_data->robot_desc(m_data->r_l1->name.substr(0, maxChar_r_l1)).drawCenter(m_data->r_l1->text_pos);
			m_data->robot_desc(m_data->r_l2->name.substr(0, maxChar_r_l2)).drawCenter(m_data->r_l2->text_pos);

			const size_t maxChar_r_r1 = m_data->robot_desc.drawableCharacters(m_data->r_r1->name, Window::Width() / 3);
			const size_t maxChar_r_r2 = m_data->robot_desc.drawableCharacters(m_data->r_r2->name, Window::Width() / 3);
			m_data->robot_desc(m_data->r_r1->name.substr(0, maxChar_r_r1)).drawCenter(m_data->r_r1->text_pos);
			m_data->robot_desc(m_data->r_r2->name.substr(0, maxChar_r_r2)).drawCenter(m_data->r_r2->text_pos);

			const size_t maxChar_r = m_data->team_desc.drawableCharacters(m_data->t_r->name, Window::Width() / 3);
			m_data->team_desc(m_data->t_r->name.substr(0, maxChar_r)).drawCenter(m_data->t_r->desc_pos);
			m_data->r_r1->texture.resize(m_data->r_r1->size, m_data->r_r1->size).drawAt(m_data->r_r1->img_pos);
			m_data->r_r2->texture.resize(m_data->r_r2->size, m_data->r_r2->size).drawAt(m_data->r_r2->img_pos);
			
			if (GV::battle_logs[m_data->now_game_id] == 0) {
				m_data->WinLose_desc(L"Winner").drawCenter(Window::Width() / 2, Window::Height() - winner_desc_fontsize - 30);
				if (winner_effect) {
					double w = m_data->winner->getSize().x;
					double h = m_data->winner->getSize().y;
					double p = Max(w / Window::Width(), h / Window::Height());
					Rect(Point(0, 0), Window::Width(), Window::Height()).draw(Color(0, 0, 0, m_data->robots_hidden_alpha));
					Graphics2D::SetBlendState(BlendState::Additive);
					if (m_data->winner->isPlaying()) m_data->winner->getFrameTexture().resize(static_cast<int>(w / p), static_cast<int>(h / p)).drawAt(Window::Center());
					Graphics2D::SetBlendState(BlendState::Default);
				}
			}
			
			
		}
		void drawFadeOut(double t) const override {
		}
		void draw() const override {
			drawFadeIn(1);
		}
	};

	class TeamR : public MyApp::Scene {
		void init() override {

			m_data->r_l1->setTextPosTarget(Vec2(-m_data->robot_desc(m_data->r_l1->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_l2->setTextPosTarget(Vec2(Window::Width() + m_data->robot_desc(m_data->r_l2->name).region().w / 2, 5 * Window::Height() / 6));
			m_data->r_r1->setTextPosTarget(Vec2(Window::Width() / 3 - 25, 5 * Window::Height() / 6));
			m_data->r_r2->setTextPosTarget(Vec2(2 * Window::Width() / 3 + 25, 5 * Window::Height() / 6));

			m_data->t_l->setDescPosTarget(Vec2(-m_data->team_desc(m_data->t_l->name).region().w / 2, 100));
			m_data->r_l1->setImgPosTarget(Vec2(-Window::Height() / 4, Window::Height() / 3));
			m_data->r_l2->setImgPosTarget(Vec2(-Window::Height() / 4, 2 * Window::Height() / 3));

			m_data->t_r->setDescPosTarget(Vec2(Window::Width() / 2, 100));
			m_data->r_r1->setImgPosTarget(Vec2(Window::Width() / 3 - 25, Window::Height() / 2));
			m_data->r_r2->setImgPosTarget(Vec2(2 * Window::Width() / 3 + 25, Window::Height() / 2));

			m_data->r_l1->setSizeTarget(Window::Height() / 2);
			m_data->r_l2->setSizeTarget(Window::Height() / 2);
			m_data->r_r1->setSizeTarget(Window::Height() / 2);
			m_data->r_r2->setSizeTarget(Window::Height() / 2);

			if (winner_effect && GV::battle_logs[m_data->now_game_id] == 1) {
				m_data->winner->play();
				m_data->robots_hidden_alpha = 200;
			}
		}
		void update() override {

			if (use_network) {
				if (GV::show_state == "l_interview") changeScene(L"TeamL", 500, true);
				if (!m_data->twice && GV::show_state == "game") {
					m_data->twice = 1;
					changeScene(L"Fight_Main", 500, true);
				}

				if (GV::show_state == "home" || GV::show_state == "preshow" || GV::show_state == "show0" || GV::show_state == "show1" || GV::show_state == "show2") {
					changeScene(L"Main", 500, true);
				}

				if (winner_effect) {
					m_data->winner->update();
					if (m_data->robots_hidden_alpha > 0) {
						m_data->robots_hidden_alpha--;
					}
				}
				return;
			}
			if (!m_data->twice && Input::KeyEnter.clicked) {
				m_data->twice = 1;
				changeScene(L"Fight_Main", 500, true);

			} else if (!m_data->twice && Input::KeyZ.clicked) {
				changeScene(L"TeamL", 500, true);
			} else {

				if (m_data->twice && Input::KeyEnter.clicked) {
					m_data->now_game_id = Min(m_data->tournament_manager->GV.game_count - 1, m_data->now_game_id + 1);
					changeScene(L"Main", 500, true);
				}
			}
			if (winner_effect) {
				m_data->winner->update();
				if (m_data->robots_hidden_alpha > 0) {
					m_data->robots_hidden_alpha--;
				}
			}
			
		}
		void updateFadeIn(double t) override {

			m_data->t_l->getAndSetDescPos(t);
			m_data->t_r->getAndSetDescPos(t);
			m_data->r_l1->getAndSetImgPos(t);
			m_data->r_l2->getAndSetImgPos(t);
			m_data->r_r1->getAndSetImgPos(t);
			m_data->r_r2->getAndSetImgPos(t);
			m_data->r_l1->getAndSetTextPos(t);
			m_data->r_l2->getAndSetTextPos(t);
			m_data->r_r1->getAndSetTextPos(t);
			m_data->r_r2->getAndSetTextPos(t);
			m_data->r_l1->getAndSetSize(t);
			m_data->r_l2->getAndSetSize(t);
			m_data->r_r1->getAndSetSize(t);
			m_data->r_r2->getAndSetSize(t);
			if (winner_effect) m_data->winner->update();

		}
		void drawFadeIn(double t) const override {
			m_data->tournament_manager->draw();
			m_data->team_manager->draw();
			Circle(m_data->tournament_manager->getRoot()->getPosById(m_data->now_game_id), 10).movedBy(m_data->tournament_manager->GV.tournamentPos).draw();
			Window::ClientRect().draw(Color(0, 0, 0, 200));

			const size_t maxChar_l = m_data->team_desc.drawableCharacters(m_data->t_l->name, Window::Width() / 3);
			m_data->team_desc(m_data->t_l->name.substr(0, maxChar_l)).drawCenter(m_data->t_l->desc_pos);
			m_data->r_l1->texture.resize(m_data->r_l1->size, m_data->r_l1->size).drawAt(m_data->r_l1->img_pos);
			m_data->r_l2->texture.resize(m_data->r_l2->size, m_data->r_l2->size).drawAt(m_data->r_l2->img_pos);

			const size_t maxChar_r_l1 = m_data->robot_desc.drawableCharacters(m_data->r_l1->name, Window::Width() / 3);
			const size_t maxChar_r_l2 = m_data->robot_desc.drawableCharacters(m_data->r_l2->name, Window::Width() / 3);
			m_data->robot_desc(m_data->r_l1->name.substr(0, maxChar_r_l1)).drawCenter(m_data->r_l1->text_pos);
			m_data->robot_desc(m_data->r_l2->name.substr(0, maxChar_r_l2)).drawCenter(m_data->r_l2->text_pos);

			const size_t maxChar_r_r1 = m_data->robot_desc.drawableCharacters(m_data->r_r1->name, Window::Width() / 3);
			const size_t maxChar_r_r2 = m_data->robot_desc.drawableCharacters(m_data->r_r2->name, Window::Width() / 3);
			m_data->robot_desc(m_data->r_r1->name.substr(0, maxChar_r_r1)).drawCenter(m_data->r_r1->text_pos);
			m_data->robot_desc(m_data->r_r2->name.substr(0, maxChar_r_r2)).drawCenter(m_data->r_r2->text_pos);

			const size_t maxChar_r = m_data->team_desc.drawableCharacters(m_data->t_r->name, Window::Width() / 3);
			m_data->team_desc(m_data->t_r->name.substr(0, maxChar_r)).drawCenter(m_data->t_r->desc_pos);
			m_data->r_r1->texture.resize(m_data->r_r1->size, m_data->r_r1->size).drawAt(m_data->r_r1->img_pos);
			m_data->r_r2->texture.resize(m_data->r_r2->size, m_data->r_r2->size).drawAt(m_data->r_r2->img_pos);

			if (GV::battle_logs[m_data->now_game_id] == 1) {
				m_data->WinLose_desc(L"Winner").drawCenter(Window::Width() / 2, Window::Height() - winner_desc_fontsize - 30);
				if (winner_effect) {
					double w = m_data->winner->getSize().x;
					double h = m_data->winner->getSize().y;
					double p = Max(w / Window::Width(), h / Window::Height());
					Rect(Point(0, 0), Window::Width(), Window::Height()).draw(Color(0, 0, 0, m_data->robots_hidden_alpha));
					Graphics2D::SetBlendState(BlendState::Additive);
					if (m_data->winner->isPlaying()) m_data->winner->getFrameTexture().resize(static_cast<int>(w / p), static_cast<int>(h / p)).drawAt(Window::Center());
					Graphics2D::SetBlendState(BlendState::Default);
				}
			}
		}
		void drawFadeOut(double t) const override {
		}
		void draw() const override {
			drawFadeIn(1);
		}

	};
	
}

