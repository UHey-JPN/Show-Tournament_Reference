#pragma once
#include <vector>
#include <string>
#include <map>

// setting

int console_visible;
int full_screen;
int window_width;
int window_height;
int use_network;
string server_ip_address;
int server_tcp_data_port;

int team_desc_fontsize;
int robot_desc_fontsize;
string opening_video_path;
int xml_write_ok;

// Ç±Ç±Ç©ÇÁâ∫ÇÕïÅí ÇÃGV
namespace GV {
	map<string, int> ports;
	int show_packet_id;
	int now_game;



	string show_state;
	vector<int> battle_logs;
	int udp_ok;
};