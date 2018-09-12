import java.util.TreeMap;
import java.util.Map;

class LightData{
  Map<Integer, OneData> data = new TreeMap<Integer, OneData>();
  
  LightData(String file){
    String[] lines = loadStrings(file);
    
    // 1行目がtime = 0でない
    if(int( lines[0].split(",")[0] ) != 0 ){
      data.put( 0, new OneData( lines[0] ) );
    }
    
    for(int i = 0; i < lines.length; i++){
      int time = int( lines[i].split(",")[0] );
      data.put( time, new OneData( lines[i] ) );
    }
  }
  
  int[] get_data(int time){
    int[] ret = new int[OneData.SIZE];
    
    int pre_time = 0;
    int next_time = 0;
    for(int k : data.keySet()){
      next_time = k;
      
      if( k > time ) break;
      
      pre_time = k;
    }
    
    char[] pre_data = data.get(pre_time).get_data();
    char[] next_data = data.get(next_time).get_data();
    
    for(int i = 0; i < OneData.SIZE; i++){
      int p = pre_data[i];
      int n = next_data[i];
      
      int t_width = next_time - pre_time;
      if( t_width == 0 ){
        ret[i] = n;
      }else{
        ret[i] = ( n * (time - pre_time) + p * (t_width - time + pre_time) ) / t_width;
      }
    }
    
    return ret;
  }
  
  
}