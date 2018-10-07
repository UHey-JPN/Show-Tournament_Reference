import java.io.FileNotFoundException;
import java.util.concurrent.Executors;


UdpSocket udp;

SettingLoader s;

MusicPlayer normal;
MusicPlayer battle;
Minim minim;


JingleOnce jingle_preshow;
JingleOnce jingle_opening;
JingleOnce[] jingle_game = new JingleOnce[10];
JingleOnce jingle_result;
JingleOnce jingle_vgoal;

AudioPlayer ap_preshow;
AudioPlayer ap_opening;
AudioPlayer ap_start_count_down;
AudioPlayer ap_start;
AudioPlayer ap_end_count_down;
AudioPlayer ap_stop;
AudioPlayer ap_result;
AudioPlayer ap_vgoal;

boolean game_request2home = false;

boolean game_reset = false;

void setup(){
  size(640, 350);
  
  background(255);
  
  minim = new Minim(this);
  
  // settings
  s = new SettingLoader(500, 280, 100, 50);

  Executor ex = Executors.newCachedThreadPool();
  udp = new UdpSocket(ex);
  
  // normal list
  normal = new MusicPlayer("sound_list/normal_music", minim);
  battle = new MusicPlayer("sound_list/battle_music", minim);

  // Audio Player
  ap_preshow = minim.loadFile("sound_list/preshow.mp3");
  if( ap_preshow == null ){
    ap_preshow = minim.loadFile("sound_list/preshow.wav");
    if( ap_preshow == null ){
      err_mes_end("preshow" + ".wav can't find.");
    }
  }
    
  ap_opening = minim.loadFile("sound_list/opening.mp3");
  if( ap_opening == null ){
    ap_opening = minim.loadFile("sound_list/opening.wav");
    if( ap_opening == null ){
      err_mes_end("opening" + ".wav can't find.");
    }
  }
    
  ap_start_count_down = minim.loadFile("sound_list/start_count_down.mp3");
  if( ap_start_count_down == null ){
    ap_start_count_down = minim.loadFile("sound_list/start_count_down.wav");
    if( ap_start_count_down == null ){
      err_mes_end("start_count_down" + ".wav can't find.");
    }
  }
    
  ap_start = minim.loadFile("sound_list/start.mp3");
  if( ap_start == null ){
    ap_start = minim.loadFile("sound_list/start.wav");
    if( ap_start == null ){
      err_mes_end("start" + ".wav can't find.");
    }
  }
    
  ap_end_count_down = minim.loadFile("sound_list/end_count_down.mp3");
  if( ap_end_count_down == null ){
    ap_end_count_down = minim.loadFile("sound_list/end_count_down.wav");
    if( ap_end_count_down == null ){
      err_mes_end("end_count_down" + ".wav can't find.");
    }
  }
  
  ap_stop = minim.loadFile("sound_list/stop.mp3");
  if( ap_stop == null ){
    ap_stop = minim.loadFile("sound_list/stop.wav");
    if( ap_stop == null ){
      err_mes_end("stop" + ".wav can't find.");
    }
  }
    
  ap_result = minim.loadFile("sound_list/result.mp3");
  if( ap_result == null ){
    ap_result = minim.loadFile("sound_list/result.wav");
    if( ap_result == null ){
      err_mes_end("result" + ".wav can't find.");
    }
  }
    
  ap_vgoal = minim.loadFile("sound_list/vgoal.mp3");
  if( ap_vgoal == null ){
    ap_vgoal = minim.loadFile("sound_list/vgoal.wav");
    if( ap_vgoal == null ){
      err_mes_end("vgoal" + ".wav can't find.");
    }
  }
    


  // Jingle setup
  jingle_preshow = new JingleOnce(ap_preshow);
  jingle_opening = new JingleOnce(ap_opening);
  jingle_game[0] = new JingleOnce(ap_start_count_down);
  jingle_game[1] = new JingleOnce(ap_start_count_down);
  jingle_game[2] = new JingleOnce(ap_start_count_down);
  jingle_game[3] = new JingleOnce(ap_start);
  jingle_game[4] = new JingleOnce(ap_end_count_down);
  jingle_game[5] = new JingleOnce(ap_end_count_down);
  jingle_game[6] = new JingleOnce(ap_end_count_down);
  jingle_game[7] = new JingleOnce(ap_end_count_down);
  jingle_game[8] = new JingleOnce(ap_end_count_down);
  jingle_game[9] = new JingleOnce(ap_stop);
  jingle_result = new JingleOnce(ap_result);
  jingle_vgoal = new JingleOnce(ap_vgoal);
  
  
  
  frameRate(15);
  
  smooth();
}

void draw(){
  boolean any_mode = false;
  
  long now_ms = udp.get_now_time();
  String now_mode = udp.get_show_mode();
  
  if( now_ms < 0 ){
    return;
  }

  
  
  //--------------------------------------------------
  // show mode = game
  //--------------------------------------------------
  if(now_mode.equals("game")){
    if( game_reset == false){
      battle.next_music();
      game_reset = true;
    }
    any_mode = true;
    
    game_request2home = false;

    long sec = now_ms / 1000;
    int game_sec = (int)( (180+5) - sec );
    if( game_sec == 183 ) jingle_game[0].play();
    else if( game_sec == 182 ) jingle_game[1].play();
    else if( game_sec == 181 ) jingle_game[2].play();
    else if( game_sec == 180 ) jingle_game[3].play();
    else if( game_sec == 5 ) jingle_game[4].play();
    else if( game_sec == 4 ) jingle_game[5].play();
    else if( game_sec == 3 ) jingle_game[6].play();
    else if( game_sec == 2 ) jingle_game[7].play();
    else if( game_sec == 1 ) jingle_game[8].play();
    else if( game_sec == 0 ) jingle_game[9].play();
    else if( 0 < game_sec && game_sec < 180 ) battle.update( s.vol_loud(), 10 );
    else if( game_sec <= 0 ) { battle.update( 0, 5 ); game_request2home = true; }

  }else{
    game_reset = false;
    battle.update( 0, 5 );
    for(int i = 0; i < jingle_game.length; i++){
      jingle_game[i].reset();
    }
  }
  

  
  //--------------------------------------------------
  // show mode = opening
  //--------------------------------------------------
  if(now_mode.equals("opening")){
    any_mode = true;
    jingle_opening.play();
  }else{
    jingle_opening.reset();
  }
  
  
  //--------------------------------------------------
  // show mode = preshow
  //--------------------------------------------------
  if(now_mode.equals("preshow")){
    any_mode = true;
    jingle_preshow.play();
  }else{
    jingle_preshow.reset();
  }
  
  
  //--------------------------------------------------
  // show mode = vgoal
  //--------------------------------------------------
  if(now_mode.equals("vgoal")){
    any_mode = true;
    jingle_vgoal.play();
  }else{
    jingle_vgoal.reset();
  }
  
  
  if( now_mode.equals("home") ){
    //--------------------------------------------------
    // show mode = home
    //--------------------------------------------------
    any_mode = true;
    normal.update(s.vol_home(), 3);
    jingle_result.reset();
    
  }else if(now_mode.equals("interview")){
    //--------------------------------------------------
    // show mode = interview
    //--------------------------------------------------
    any_mode = true;
    normal.update(s.vol_silent(), 3);
    jingle_result.reset();

  }else if(now_mode.equals("r_interview")){
    //--------------------------------------------------
    // show mode = r_interview
    //--------------------------------------------------
    any_mode = true;
    normal.update(s.vol_silent(), 3);
    jingle_result.reset();

  }else if(now_mode.equals("l_interview")){
    //--------------------------------------------------
    // show mode = l_interview
    //--------------------------------------------------
    any_mode = true;
    normal.update(s.vol_silent(), 3);
    jingle_result.reset();

  }else if(now_mode.equals("result")){
    //--------------------------------------------------
    // show mode = result
    //--------------------------------------------------
    any_mode = true;
    if( now_ms > 6000){
      normal.update(s.vol_quiet(), 1);
    }else{
      normal.update(s.vol_silent(), 3);
    }
    jingle_result.play();

  }else if(now_mode.equals("newgame")){
    //--------------------------------------------------
    // show mode = newgame
    //--------------------------------------------------
    any_mode = true;
    normal.update(s.vol_quiet(), 3);
    jingle_result.reset();

  }else if(any_mode == false){
    normal.update(s.vol_home(), 3);
    jingle_result.reset();
    
  }else{
    if( game_request2home == true ){
      normal.update(s.vol_home(), 3);
    }else{
      normal.update(0, 10);
      jingle_result.reset();
    }
  }
  
  background(255);
  
  fill(0);
  
  textAlign(CENTER, CENTER);
  textSize(50);
  text("Sound System", width/2, 50);

  textSize(30);
  text("mode = " + now_mode, width/2, 300);
  

  textAlign(LEFT, TOP);
  textSize(20);

  text("NORMAL", 80, 150);
  text("VOL : " + normal.get_vol() , 110, 175);
  text("I D : " + normal.get_id() , 110, 200);
  text("BATTLE", width/2 + 70, 150);
  text("VOL : " + battle.get_vol() , width/2 + 100, 175);
  text("I D : " + battle.get_id() , width/2 + 100, 200);
  
  
  s.draw();
  
  
}

void err_mes_end(String mes){
  JFrame f = new JFrame();
  JLabel label = new JLabel( mes );
  JOptionPane.showMessageDialog(f, label);
  exit();
}

void mousePressed(){
  s.mousePressed();
}


void dispose() {
  udp.close();
  println("exit.");
  System.exit(1);
}
