import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.border.*;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Color;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.Font;


class SettingLoader implements ActionListener{
  private int num_of_vol = 4;

  private int[] vol = {0,0,0,0};
  private JTextField[] t_vol = {new JTextField(),new JTextField(),new JTextField(),new JTextField()};
  
  private float pos_x, pos_y;
  private float size_x, size_y;

  private String[] vol_name = {"Game", "Normal", "Talking", "Home"};
  
  JFrame f_set = new JFrame("Setting");

    

  
  SettingLoader(float x, float y, float size_x, float size_y){
    this.pos_x = x;
    this.pos_y = y;
    this.size_x = size_x;
    this.size_y = size_y;
    
    File f = new File(dataPath("settings.txt"));
    if ( !f.exists() )err_mes_end("setting file is not exist.");

    String[] lines = null;
    lines = loadStrings("settings.txt");
    
    if(lines == null) err_mes_end("setting file is err.");
    if(lines[0] == null) err_mes_end("setting file is err.");
    
    //for(int i = 0; i < lines.length; i++){
    for(int i = 0; i < num_of_vol; i++){
      String[] str_size = lines[i].split("=");
      if(str_size.length == 2){
        vol[i] = int(str_size[1]);
        println(vol_name[i] + " VOL = " + vol[i]);
      }
    }
      
  }
  
  void draw(){
    int back, character, border;
    if(mouse_over()){
      back = 0;
      border = 120;
      character = 255;
    }else{
      back = 255;
      border = 0;
      character = 0;
    }
    fill(back);
    stroke(border);
    rectMode(CORNER);
    rect(pos_x, pos_y, size_x, size_y);
    fill(character);
    textAlign(CENTER, CENTER);
    textSize(20);
    text("setting", pos_x + size_x*0.5, pos_y + size_y*0.4);
  }
  
  boolean mouse_over(){
    if(pos_x < mouseX && mouseX < pos_x + size_x){
      if(pos_y < mouseY && mouseY < pos_y + size_y){
        return true;
      }
    }
    return false;
  }
  
  void mousePressed(){
    if(mouse_over()){
      setting_window();
    }
  }
  
  int vol_loud(){
    return vol[0];
  }
  
  int vol_quiet(){
    return vol[1];
  }
  
  int vol_silent(){
    return vol[2];
  }
  
  int vol_home(){
    return vol[3];
  }

  void err_mes_end(String mes){
    JFrame f = new JFrame();
    JLabel label = new JLabel( mes );
    JOptionPane.showMessageDialog(f, label);
    System.exit(1);
  }
  
  
  void setting_window(){
    f_set = new JFrame("Setting");
    f_set.setSize(200, 400);
    f_set.setLocationRelativeTo(null);
    
    Font font = new Font("", Font.PLAIN, 20);
    
    JPanel p = new JPanel();
    p.setOpaque(true);
    p.setBackground( Color.WHITE );
    p.setLayout(new GridLayout(num_of_vol+1, 1));
    
    for(int i = 0; i < num_of_vol; i++){
      JPanel summary = new JPanel();
      summary.setOpaque(false);
      summary.setLayout(new GridLayout(1,2));
      
      JLabel name = new JLabel(vol_name[i] + " : ");
      t_vol[i].setText("" + vol[i]);
      name.setFont(font);
      t_vol[i].setFont(font);
      
      name.setOpaque(false);
      t_vol[i].setOpaque(false);

      name.setHorizontalAlignment(JLabel.CENTER);
      t_vol[i].setHorizontalAlignment(JTextField.CENTER);

      summary.add( name );
      summary.add( t_vol[i] );
      p.add(summary);
    }
    
    JPanel empty = new JPanel();
    empty.setBorder(new EmptyBorder(20, 20, 20, 20));
    empty.setLayout(new BorderLayout());
    empty.setOpaque(false);
    
    JButton btn = new JButton("Set");
    btn.addActionListener(this);
    btn.setFont(font);
    empty.add(btn);
    p.add(empty);
    
    f_set.getContentPane().add(p, BorderLayout.CENTER);
    f_set.setVisible(true);
    
  }
  
  void actionPerformed(ActionEvent e){
    for(int i = 0; i < num_of_vol; i++){
      vol[i] = int(t_vol[i].getText());
    }
    String[] lines = new String[4];
    lines[0] = "loud_volume=" + vol[0];
    lines[1] = "quiet_volume=" + vol[1];
    lines[2] = "silent_volume=" + vol[2];
    lines[3] = "home_volume=" + vol[3];
    saveStrings(dataPath("settings.txt"), lines);
    
    print("over write data:");
    for(int i = 0; i < num_of_vol; i++){
      print(vol[i] + ",");
    }
    println();
    
    f_set.setVisible(false);
  }

}