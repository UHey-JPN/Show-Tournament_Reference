import java.util.Map.Entry;

class LightClientManager{
  private HashMap<String, LightClient> list;
  
  LightClientManager(){
    list = new HashMap<String, LightClient>();
    
  }
                          
  public void add(LightClient a){
    LightClient same = null;
    for(Entry<String, LightClient> entry : list.entrySet()){
      LightClient l = entry.getValue();
      if( a.get_name().equals(l.get_name()) ){
        same = l;
        break;
      }
    }
    
    // 同一名のライトクライアントは削除
    if( same != null ){
      println("same name is deleted");
      list.remove(same);
    }
    
    // 新規データを追加
    list.put(a.get_name(),a);
  }
  
  
  public Set<String> get_keySet(){
    return list.keySet();
  }
  
  public LightClient get(String k){
    return list.get(k);
  }
  
  public int numOfSize(){
    return list.size();
  }
  
}