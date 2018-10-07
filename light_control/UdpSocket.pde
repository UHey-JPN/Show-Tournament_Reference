import hypermedia.net.*;
import java.util.concurrent.Executor;
import java.util.Calendar;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
//import java.net.InetAddress;
import java.net.*;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;


public class UdpSocket implements Runnable {
  DatagramSocket soc;
  
  static final String LINE_SEPARATOR_PATTERN =  "\r\n|[\n\r\u2028\u2029\u0085]";
  public static final int PORT_NUM = 58239;
  public static final int BUF_SIZE = 2048;
  
  private volatile boolean port_get = false;
  private int kam_port = -1;
  private String server_ip = "";
  
  private String show_mode = "home";
  private Calendar start_time = Calendar.getInstance();
  private int[] point_list = {0,0};

  UdpSocket(Executor ex){
    try {
      soc = new DatagramSocket(null);
      soc.setReuseAddress(true);
      soc.bind(new InetSocketAddress(PORT_NUM));
      System.out.println("Open UDP Port(" + soc.getLocalPort() + ")");
    } catch (SocketException e) {
      e.printStackTrace();
      JFrame f = new JFrame();
      JLabel label = new JLabel("UDP Port 58239 is already used.");
      JOptionPane.showMessageDialog(f, label);
      System.exit(1);
    }
  
    if(soc.isClosed() == true) {
      JFrame f = new JFrame();
      JLabel label = new JLabel("UDP Port 58239 is already used.");
      JOptionPane.showMessageDialog(f, label);
      exit();
    }
      
    //soc.listen(true);
    ex.execute(this);

  }
  
  public String get_show_mode(){
    return show_mode;
  }

  public long get_now_time(){
    long now_ms = Calendar.getInstance().getTimeInMillis();
    return now_ms - start_time.getTimeInMillis();
  }
  
  public int get_point(int i){
    if( i == 0 | i ==1 ){
      return point_list[i];
    }else{
      return -1;
    }
  }
  
  ///////////////////////////////////////////////////////////////////
  // UDP Socket Process
  ///////////////////////////////////////////////////////////////////
  @Override
  public void run() {
    while(true){
      byte[] buf = new byte[BUF_SIZE];
      DatagramPacket pac = new DatagramPacket(buf, buf.length);
      String str_packet;
      
      //----------------------------------------
      // UDPソケットへのデータ到着待ち
      try {
        soc.receive(pac);
      } catch (IOException e) {
        // エラーのスタックトレースを表示
        e.printStackTrace();
      }
      
      //----------------------------------------
      // 文字列をUTF-8でデコード
      try {
        str_packet = new String(buf, "UTF-8");
      } catch (UnsupportedEncodingException e) {
        e.printStackTrace();
        return;
      }
      
      //----------------------------------------
      // 改行コードで分割
      String[] pac_data = str_packet.split( LINE_SEPARATOR_PATTERN );
      
      //----------------------------------------
      // 受信パケットの分類とそれに伴う処理
      try {
        if(pac_data[0].equals("server")){
          // ------------------------------------------
          // server state packet
          // ------------------------------------------
          for(int i = 1; i < pac_data.length; i++){
            String[] line = pac_data[i].split(",");
            if(line.length == 2){
              if( line[0].equals("kam_port") ){
                kam_port = int(line[1]);
                port_get = true;
              }
            }
          }
          
        }else if(pac_data[0].equals("show")){
          // ------------------------------------------
          // show state packet
          // ------------------------------------------
          // show packet id
          String id = pac_data[1];
          
          // show mode
          show_mode = pac_data[3];
          
          // start time
          String[] str_start_time = pac_data[4].split(",");
          if( str_start_time.length == 7){
            start_time = Calendar.getInstance();
            start_time.set(Calendar.YEAR, int(str_start_time[0]));
            start_time.set(Calendar.MONTH, int(str_start_time[1]));
            start_time.set(Calendar.DATE, int(str_start_time[2]));
            start_time.set(Calendar.HOUR_OF_DAY, int(str_start_time[3]));
            start_time.set(Calendar.MINUTE, int(str_start_time[4]));
            start_time.set(Calendar.SECOND, int(str_start_time[5]));
            start_time.set(Calendar.MILLISECOND, int(str_start_time[6]));
          }
          
          // point list
          String[] str_point = pac_data[5].split(",");
          if( str_point.length == 2 ){
            point_list[0] = int(str_point[0]);
            point_list[1] = int(str_point[1]);
          }
          
          // send keep alive
          //if( port_get == true ){
          //  udp.send(id + ",Score Board", server_ip, kam_port);
          //}
          
        }else{
          // ------------------------------------------
          // unknown packet state packet
          // ------------------------------------------
          
        }
        
        
      } catch(NullPointerException e) {
        System.out.println("Get Packet(unknow packet)\n" + str_packet);
      }
    }
  }
  
  public void close(){
    soc.close();
  }


}
