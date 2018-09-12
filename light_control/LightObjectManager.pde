import java.util.Set;

class LightObjectManager{
  private HashMap<String, LightObject> list;
  
  LightObjectManager(){
    list = new HashMap<String, LightObject>();
    
  }

  public void add(LightObject a){
    LightObject same = null;
    for(Entry<String, LightObject> entry : list.entrySet()){
      LightObject l = entry.getValue();
      if( a.get_name().equals(l.get_name()) ){
        same = l;
        break;
      }
    }
    
    // 同一名のライトオブジェクトは削除
    if( same != null ){
      list.remove(same);
    }
    
    // 新規データを追加
    list.put(a.get_name(),a);
    println("name ; '" + a.get_name() + "'");
  }
  
  int numOfSize(){
    return list.size();
  }
  
  Set<String> get_name_list(){
    return list.keySet();
  }
  
  LightObject get(String k){
    return list.get(k);
  }
  
  LightObject serach(String k){
    for(Entry<String, LightObject> entry : list.entrySet()){
      LightObject l = entry.getValue();
      if( k.equals(l.get_name()) ){
        return l;
      }
    }
    return null;
  }
  
}