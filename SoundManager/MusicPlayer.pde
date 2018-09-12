import java.util.Date;
import ddf.minim.*;



class MusicPlayer{
  String[] file_name;
  String folder;
  ArrayList<AudioPlayer> list = new ArrayList<AudioPlayer>();
  
  int now_vol = 0;
  int current_id = 0;
  boolean now_play = false;
  
  MusicPlayer(String folder, Minim minim){
    // list up files
    String path = sketchPath();
    String[] file_names = listFileNames(path + "/" + folder);
    println("list up file in ``" + folder + "''.");
    
    // load
    for(int i = 0; i < file_names.length; i++){
      { // check MP3
        String[] period = file_names[i].split("m");
        if( period.length > 1 ){
          if( period[period.length-1].equals("p3") ){
            print(file_names[i] + ":" + period.length);
            AudioPlayer a = minim.loadFile(folder + "/" + file_names[i]);
            if( a != null ){
              list.add( minim.loadFile(folder + "/" + file_names[i]) );
              println(" -> add");
              continue;
            }
          }
        }
      }
      { // check WAV
        String[] period = file_names[i].split("w");
        if( period.length > 1 ){
          if( period[period.length-1].equals("av") ){
            print(file_names[i] + ":" + period.length);
            print(" -> add");
            AudioPlayer a = minim.loadFile(folder + "/" + file_names[i]);
            if( a == null ){
              err_mes_end( "cannot open audio file.:" + folder + "/" + file_names[i] );
            }else{
              list.add( minim.loadFile(folder + "/" + file_names[i]) );
            }
          }else{
            err_mes_end( "cannot open audio file.:" + folder + "/" + file_names[i] );
          }
        }else{
          err_mes_end( "cannot open audio file.:" + folder + "/" + file_names[i] );
        }
      }
      println();
      
    }
    
    
    this.folder = folder;
  }
  
  public float map_vol(float i_vol){
    i_vol = i_vol / 100.0f;
    i_vol = pow(i_vol, 0.09f) * 100;
    return map(i_vol, 0, 100, -192, 0);
  }
  
  
  void update(int vol, int cap){
    int dif = vol - now_vol;
    if( dif > cap ) dif = cap;
    if( dif < -cap ) dif = -cap;
    now_vol += dif;
    list.get(current_id).setGain( map_vol(now_vol) );
    
    // change music
    if( list.get(current_id).length()*0.8 <= list.get(current_id).position() && !(list.get(current_id).isPlaying()) ){
      next_music();
    }
    
    // stop
    if( now_vol == 0 && list.get(current_id).isPlaying() ){
      println("pause " + current_id + "," + folder);
      list.get(current_id).pause();
      return;
    }
    
    // play
    if( now_vol != 0 && !(list.get(current_id).isPlaying()) ){
      println("play " + current_id + "," + folder);
      list.get(current_id).play();
    }
    
  }
  
  public void next_music(){
    println("next " + current_id + "," + folder);
    list.get(current_id).pause();
    list.get(current_id).play(0);
    list.get(current_id).pause();
    current_id++;
    if( current_id >= list.size() ) {
      current_id = 0;
      println("last file. Return to head file");
    }
  }    
  
  public int get_vol(){
    return now_vol;
  }
  
  public int get_id(){
    return current_id;
  }

  void err_mes_end(String mes){
    JFrame f = new JFrame();
    JLabel label = new JLabel( mes );
    JOptionPane.showMessageDialog(f, label);
    System.exit(1);
  }

  // This function returns all the files in a directory as an array of Strings  
  String[] listFileNames(String dir) {
    File file = new File(dir);
    if (file.isDirectory()) {
      String names[] = file.list();
      return names;
    } else {
      // If it's not a directory
      return null;
    }
  }


}