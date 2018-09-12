import javax.swing.JPanel;
import javax.swing.JLabel;
import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Color;
import javax.swing.border.*;

class SettingManager{
  private String[] lines;
  LightObjectManager lom;

  SettingManager(LightObjectManager lom){
    this.lom = lom;
    lines = loadStrings("data/assign.cfg");
    
    LightObject new_data = null;
    for(int cnt = 0; cnt < lines.length; cnt++){
      String l = lines[cnt];
      
      if( l.startsWith("[") ){
        if( new_data != null ){
          lom.add(new_data);
          println("add : " + new_data.summary());
        }
        
        // 名前の抽出
        new_data = new LightObject( new String( l.getBytes(), 1, l.length()-2) );
        
      }else if( l.startsWith("startAddress") ){
        if(new_data != null)
          new_data.set_start_address( int( l.split("=")[1] ) );
        
      }else if( l.startsWith("size") ){
        if(new_data != null)
          new_data.set_size( int( l.split("=")[1] ) );
      }
    }

    if( new_data != null ){
      lom.add(new_data);
      println("add : " + new_data.summary());
    }

  }
  
  void setting_window(){
    JFrame f = new JFrame("Setting");
    f.setSize(360, 480);
    
    JPanel p = new JPanel();
    p.setOpaque(false);
    p.setLayout(new GridLayout(lom.get_name_list().size(), 1));
    
    for(String k : lom.get_name_list()){
      JPanel summary = new JPanel();
      summary.setOpaque(false);
      summary.setLayout(new GridLayout(1,3));
      
      JLabel name = new JLabel(k);
      JLabel sa = new JLabel("SA:" + lom.get(k).get_start_address() );
      JLabel size = new JLabel("SIZE:" + lom.get(k).get_start_address() );
      
      name.setOpaque(false);
      sa.setOpaque(false);
      size.setOpaque(false);

      name.setHorizontalAlignment(JLabel.CENTER);
      sa.setHorizontalAlignment(JLabel.CENTER);
      size.setHorizontalAlignment(JLabel.CENTER);

      summary.add( name );
      summary.add( sa );
      summary.add( size );
      summary.setBorder(new LineBorder(Color.BLACK, 2));
      p.add(summary);
    }
    
    f.getContentPane().add(p, BorderLayout.CENTER);
    f.setVisible(true);
    
  }

}