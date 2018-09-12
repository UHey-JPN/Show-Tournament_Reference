class SettingButton{
  private SettingManager sm;
  private float sizex;
  private float sizey;
  private float posx;
  private float posy;
  
  SettingButton(SettingManager sm, float posx, float posy, float sizex, float sizey){
    this.sm = sm;
    this.posx = posx;
    this.posy = posy;
    this.sizex = sizex;
    this.sizey = sizey;
  }
  
  void draw(){
    char c_back;
    char c_str;
    if( mouse_over() ){
      c_back = 204;
      c_str = 0;
    }else{
      c_back = 51;
      c_str = 255;
    }
    fill(c_back);
    stroke(255);
    rect(posx, posy, sizex, sizey);
    fill(c_str);
    textAlign(CENTER, CENTER);
    textSize(width*0.02);
    text("setting", posx + sizex*0.5, posy + sizey*0.4);
  }
  
  boolean mouse_over(){
    if( posx < mouseX && mouseX <= posx+sizex ){
      if( posy < mouseY && mouseY <= posy+sizey ){
        return true;
      }
    }
    return false;
  }
  
  void mousePressed(){
    if( mouse_over() ){
      sm.setting_window();
    }
  }
}