UdpSocket udp;

LightData d_home;
LightData d_game;
LightData d_result;
LightData d_interview;
LightData d_l_interview;
LightData d_r_interview;
LightData d_newgame;
LightData d_opening;

int[] now_data = new int[OneData.SIZE];

final int NUMBER_LINE = 32;

SettingButton btn_set;

LightSocket light_soc;
SettingManager sm;
LightObjectManager lom;

void setup(){
  size(1024,700);
  
  lom = new LightObjectManager();
  sm = new SettingManager(lom);

    Executor ex = Executors.newCachedThreadPool();
  udp = new UdpSocket(ex);
  
  light_soc = new LightSocket(58240, lom);

  d_home = new LightData("DB/home.csv");
  d_game = new LightData("DB/game.csv");
  d_result = new LightData("DB/result.csv");
  d_interview = new LightData("DB/interview.csv");
  d_l_interview = new LightData("DB/l_interview.csv");
  d_r_interview = new LightData("DB/r_interview.csv");
  d_newgame = new LightData("DB/newgame.csv");
  d_opening = new LightData("DB/opening.csv");

  btn_set = new SettingButton(sm, width*0.8, 10, width*0.1, 30);
  
  for(int i = 0; i < OneData.SIZE; i++){
    now_data[i] = 0;
  }
  
  frameRate(25);
  
  smooth();
}

void draw(){
  boolean any_mode = false;
  
  long now_ms = udp.get_now_time();
  int now_frame = int(now_ms/40);
  String now_mode = udp.get_show_mode();
  
  
  /////////////////////////////////////////////////////////////
  //                                                         //
  //                 mode data processing                    //
  //                                                         //
  /////////////////////////////////////////////////////////////
  if( now_ms >= 0 ){
    //--------------------------------------------------
    // show mode = home
    //--------------------------------------------------
    if( now_mode.equals("home") ){
      any_mode = true;
      now_data = d_home.get_data(now_frame);
    }
    
    //--------------------------------------------------
    // show mode = game
    //--------------------------------------------------
    if(now_mode.equals("game")){
      any_mode = true;
      now_data = d_game.get_data(now_frame);
      
    }
    
    //--------------------------------------------------
    // show mode = result
    //--------------------------------------------------
    if(now_mode.equals("result")){
      any_mode = true;
      now_data = d_result.get_data(now_frame);
    }
    
    //--------------------------------------------------
    // show mode = interview
    //--------------------------------------------------
    if(now_mode.equals("interview")){
      any_mode = true;
      now_data = d_interview.get_data(now_frame);
    }
    
    //--------------------------------------------------
    // show mode = r_interview
    //--------------------------------------------------
    if(now_mode.equals("r_interview")){
      any_mode = true;
      now_data = d_r_interview.get_data(now_frame);
    }
    
    //--------------------------------------------------
    // show mode = l_interview
    //--------------------------------------------------
    if(now_mode.equals("l_interview")){
      any_mode = true;
      now_data = d_l_interview.get_data(now_frame);
    }
    
    //--------------------------------------------------
    // show mode = newgame
    //--------------------------------------------------
    if(now_mode.equals("newgame")){
      any_mode = true;
      now_data = d_newgame.get_data(now_frame);
    }
    
    //--------------------------------------------------
    // show mode = opening
    //--------------------------------------------------
    if(now_mode.equals("opening")){
      any_mode = true;
      now_data = d_opening.get_data(now_frame);
    }
    
    
    //--------------------------------------------------
    // show mode = unknown
    //--------------------------------------------------
    if(any_mode == false){
      now_data = d_home.get_data(now_frame);
    }
    
  }else{
    // if now_ms is lower than zero
    int[] next_data;
    if( now_mode.equals("home") ){
      next_data = d_home.get_data(0);
    }else if(now_mode.equals("game")){
      next_data = d_game.get_data(0);
    }else if(now_mode.equals("result")){
      next_data = d_result.get_data(0);
    }else if(now_mode.equals("interview")){
      next_data = d_interview.get_data(0);
    }else if(now_mode.equals("r_interview")){
      next_data = d_r_interview.get_data(0);
    }else if(now_mode.equals("l_interview")){
      next_data = d_l_interview.get_data(0);
    }else if(now_mode.equals("newgame")){
      next_data = d_newgame.get_data(0);
    }else if(now_mode.equals("opening")){
      next_data = d_opening.get_data(0);
    }else{
      next_data = d_home.get_data(0);
    }
    
    for(int i = 0; i < OneData.SIZE; i++){
      int d = next_data[i] - now_data[i];
      if( now_frame == 0 ){
        now_data[i] = next_data[i];
      }else{
        d = d / (-now_frame);
        now_data[i] += d;
      }
    }
    
      
  }
  
  //--------------------------------------------------
  // send data
  //--------------------------------------------------
  light_soc.send_all(now_data);
  
  
  
  
  
  
  
  /////////////////////////////////////////////////////////////
  //                                                         //
  //                 drawing information                     //
  //                                                         //
  /////////////////////////////////////////////////////////////
  background(255);
  
  // --------------------------------------
  // drawing system status
  // --------------------------------------
  fill(0);
  rect(0, 0, width, 50);
  textAlign(CENTER, CENTER);
  textSize(45);
  fill(255);
  text("Light System", width/2, 17);

  fill(0);
  line(0, 100, width, 100);
  line(width*0.333, 50, width*0.333, 100);
  line(width*0.666, 50, width*0.666, 100);
  
  textSize(15);
  textAlign(LEFT, TOP);
  text("MODE", 0+4, 50+2);
  text("TIME", width*0.333+4, 50+2);
  text("FRAME", width*0.666+4, 50+2);
  
  textSize(30);
  textAlign(CENTER, CENTER);
  text(now_mode, width*0.167, 75);
  text(int(now_ms), width*0.5, 75);
  text(int(now_frame), width*0.833, 75);
  
  btn_set.draw();

  // --------------------------------------
  // drawing data to send to light client
  // --------------------------------------
  float cell_width  = (width-0.0) / NUMBER_LINE;
  float cell_height = (height-100.0) / (OneData.SIZE/NUMBER_LINE);
  stroke(200);
  for(float i = 100 + cell_height*0.5; i < height+1; i += cell_height) 
    line(0, i, width, i);
  stroke(0);
  for(float i = 0; i < width+1; i += cell_width)
    line(i, 100, i, height);
  for(float i = 100; i < height+1; i += cell_height)
    line(0, i, width, i);
  
  fill(0);
  textSize(10);
  textAlign(CENTER, CENTER);
  for(int i = 0; i < OneData.SIZE; i++){
    float x = (i % NUMBER_LINE + 0.5) * cell_width;
    float ch_y = (i / NUMBER_LINE + 0.25) * cell_height+100;
    float val_y = (i / NUMBER_LINE + 0.75) * cell_height+100;
    text(i+1, x, ch_y);
    text(now_data[i], x, val_y);
  }
  
  fill(0,0,0,100);
  textSize(80);
  textAlign(CENTER, CENTER);
  text(""+frameRate, width/2, height/2);

}

void mousePressed(){
  btn_set.mousePressed();
}

void dispose() {
  udp.close();
  println("exit.");
  exit();
}
