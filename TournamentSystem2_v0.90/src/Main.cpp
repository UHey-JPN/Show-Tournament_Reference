
#define _CRT_SECURE_NO_WARNINGS 1

#include <Siv3D.hpp>
#include <iostream>
#include <string>
#include <fstream>


#include "scene.hpp"

#include "GV.h"
#include "sock.hpp"

Udp_server *udp_server;
UDPSocket *udp_sock;

void readSetting() {
	ifstream ifs("setting.txt");
	ifs >> console_visible >> full_screen;
	ifs >> window_width >> window_height;
	ifs >> use_network >> server_ip_address >> server_tcp_data_port;
	ifs >> team_desc_fontsize >> robot_desc_fontsize;
	ifs >> opening_video_path;
	ifs >> xml_write_ok >> winner_effect >> winner_desc_fontsize;
}

void parseUDPData(string data) {
	stringstream ss(data);
	string line;
	if (std::getline(ss, line)) {
		if (line == "server\r") {
			std::getline(ss, line);
			line[line.length() - 1] = '\0';
			server_ip_address = line;
			for (size_t i = 0; i < 3; i++) {
				std::getline(ss, line);
				replace(line.begin(), line.end(), ',', ' ');
				stringstream ss2(line);
				string kind;
				int port;
				ss2 >> kind >> port;
				GV::ports[kind] = port;
				
			}
			GV::udp_ok = 1;
		} else if (line == "show\r") {
			ss >> GV::show_packet_id >> GV::now_game >> GV::show_state;
		} else if (line == "tournament\r") {
			std::getline(ss, line);
			replace(line.begin(), line.end(), ',', ' ');
			stringstream ss2(line);
			int battle_log;
			GV::battle_logs.clear();
			while (ss2 >> battle_log) {
				GV::battle_logs.push_back(battle_log);
			}
		}
	}
}


void Main() {

	readSetting();
	udp_sock = new UDPSocket(58239);
	if (console_visible) Console::Open();
	//use_network = 0;

	// フルスクリーンモード
	const Size targetSize(window_width, window_height);
	if (full_screen) {
		if (!Window::SetFullscreen(true, targetSize)) {
			System::Exit();
		}
	} else {
		Window::Resize(window_width, window_height);
	}

	MyApp manager;
	manager.add<Scene::Init>(L"Init");
	manager.add<Scene::Pause>(L"Pause");
	manager.add<Scene::Opening>(L"Opening");
	manager.add<Scene::Main>(L"Main");
	manager.add<Scene::Fight_Init>(L"Fight_Init");
	manager.add<Scene::Fight_Main>(L"Fight_Main");
	manager.add<Scene::TeamL>(L"TeamL");
	manager.add<Scene::TeamR>(L"TeamR");

	while (System::Update()) {
		if (!use_network || GV::udp_ok) manager.updateAndDraw(); // 初期化が終わってから
		
		//udp_server->readsome(&err);
		if (use_network) {
			string r = udp_sock->recv();
			if (r != "") {
				cout << utf8_to_multi_winapi(r) << endl << "----" << endl;
				parseUDPData(r);
			}
		}
	}
}
/*
void Main() {
	// ムービー (AVI, WMV)
	VideoPlayer video(Dialog::GetOpenVideo().value_or(L""));

	if (!video.isOpened()) {
		MessageBox::Show(L"サポートしていない形式です。");
		return;
	}

	Window::Resize(video.getSize());

	video.play();

	while (System::Update()) {
		video.update();

		video.getFrameTexture().draw();

		Window::SetTitle(L"{:.1f} / {:.1f}"_fmt, video.getPosSec(), video.getLengthSec());
	}
}
*/
/*
# include <Siv3D.hpp>

void Main() {
	VideoWriter writer(L"test.avi", 640, 480, VideoFormat::AVI_Uncompressed, 30);

	Image image(640, 480, Palette::White);
	
	for (int frame = 0; frame < 240; ++frame) {
		image.fill(Palette::White);

		for (int i = 0; i < 36; ++i) {
			const double radian = Radians(i * 10 + frame);

			const Vec2 pos = Circular(200, radian) + image.size / 2;

			RectF(25).setCenter(pos).rotated(radian).write(image, HSV(i * 10));
		}

		writer.writeFrame(image);
	}

	writer.close();


	Image image(640, 480, Palette::White);
	VideoReader reader(L"test.avi");

	if (!reader) {
		return;
	}

	int frame = 0;

	DynamicTexture texture;

	while (System::Update()) {
		if (System::FrameCount()) {
			reader.readFrame(frame, image);

			texture.fill(image);

			++frame %= reader.num_frames;
		}

		texture.draw();
	}
}
*/

/*
void Main() {

	const Texture texture(L"Example/Windmill.png");
	Font font(30);
	double r = 100.0;
	double p = 0.5;
	while (System::Update()) {
		double t = 2 * System::FrameCount() * Pi / 180;
		Quad(Mouse::Pos() + Vec2(r * cos(t), r * sin(t) * p - r),
			Mouse::Pos() + Vec2(r * cos(t + Pi), r * sin(t + Pi) * p - r),
			Mouse::Pos() + Vec2(r * cos(t + Pi), r * sin(t + Pi) * p + r),
			Mouse::Pos() + Vec2(r * cos(t), r * sin(t) * p + r))(texture).draw();
		if (Input::KeyUp.pressed) {
			p = Min(1.0, p + 0.01);
		} else if (Input::KeyDown.pressed) {
			p = Max(0.0, p - 0.01);
		}
		font(L"見下ろし度合い: ", p).draw();
	}
}
*/