class OneData{
  public final static int SIZE = 512;
  
  private char[] data = new char[SIZE];
  
  OneData(String line){
    String[] str_data = line.split(",");
    
    for(int i = 0; i < SIZE; i++){
      if( i+1 < str_data.length){
        int d = int(str_data[i+1]);
        if( d < 0 ) d = 0;
        if( d > 255 ) d = 255;
        data[i] = char(d);
      }else{
        data[i] = 0;
      }
    }
  }
  
  char[] get_data(){
    return data;
  }
  
}