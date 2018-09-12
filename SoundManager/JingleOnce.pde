class JingleOnce{
  private AudioPlayer ap;
  private boolean flag = false;
  
  JingleOnce(AudioPlayer ap){
    this.ap = ap;
  }
  
  void play(){
    if(flag == false){
      ap.play(0);
      flag = true;
    }
  }
  
  void reset(){
    ap.pause();
    flag = false;
  }
  
}