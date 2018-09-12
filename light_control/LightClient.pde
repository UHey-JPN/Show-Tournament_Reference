import java.net.InetSocketAddress;

class LightClient{
  private InetSocketAddress addr;
  private String name;
  
  LightClient(String ip, int port, String name){
    this.addr = new InetSocketAddress(ip, port);
    this.name = name;
  }
  
  InetSocketAddress get_address(){
    return addr;
  }

  String get_name(){
    return name;
  }
  
}