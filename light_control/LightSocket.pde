import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.InetSocketAddress;
import java.io.IOException;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.InputStreamReader;


class LightSocket implements Runnable{
  private LightClientManager lcm;
  private LightObjectManager lom;
  private DatagramSocket socket;
  private int port;
  private boolean flg_open = false;
  
  LightSocket(int port, LightObjectManager lom){
    this.lcm = new LightClientManager();
    this.lom = lom;
    this.port = port;

    Executors.newSingleThreadExecutor().execute(this);
    while( !flg_open );
    println("light socket opened");
  }
  
  public void send(InetSocketAddress addr, byte[] mes){
    try{
      DatagramPacket packet = new DatagramPacket(mes, mes.length, addr);
      socket.send(packet);
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
  public void send(InetSocketAddress addr, String mes){
    send(addr, mes.getBytes());
    
  }
  public void send(InetSocketAddress addr, int[] data){
    byte[] d = new byte[data.length];

    for(int i = 0; i < data.length; i++){
      d[i] = (byte)data[i];
    }
    
    send(addr, d);
  }
  
  // すべてのホストにデータを送信
  public void send_all(int data[]){
    for(String k : lcm.get_keySet()){
      LightObject obj = lom.serach(new String(k));
      if( obj == null ){
        println("cannot find obj( name = " + k + " )");
        continue;
      }
      int[] data_set = new int[obj.size()];
      
      for(int i = 0; i < obj.size(); i++){
        int ch = obj.get_start_address() + i;
        if(ch < data.length){
          data_set[i] = data[ch];
        }else{
          data_set[i] = 0;
        }
      }
      
      send(lcm.get(k).get_address(), data_set);
    }
    
  }
    
  
  private void receive(String data, String ip, int port){
    String[] pac_data;
    {
      BufferedReader buf_reader = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(data.getBytes())));
      ArrayList<String> buf_array = new ArrayList<String>();
      String line;
      try {
        while( (line = buf_reader.readLine()) != null){
          buf_array.add(line);
        }
      } catch (IOException e) {
        e.printStackTrace();
      }
      pac_data = buf_array.toArray(new String[buf_array.size()]);
    }
    
    String name = pac_data[0].split("=")[1];
    lcm.add(new LightClient(ip, port, name));
  }
  
  public void run(){
    try {
      socket = new DatagramSocket(port);
      flg_open = true;
      while(true){
        byte[] buf = new byte[1024];
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        socket.receive(packet);
        this.receive(new String(buf, "UTF-8"), packet.getAddress().getHostAddress(), packet.getPort() );
      }
    } catch (IOException e) {
      e.printStackTrace();
    } finally {
      if (socket != null) {
        socket.close();
      }
    }

  }
}