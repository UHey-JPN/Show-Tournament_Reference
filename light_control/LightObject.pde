class LightObject{
  private String name;
  private int start_address;
  private int size;
  
  LightObject(String name, int start_address, int size){
    this.name = name;
    this.start_address = start_address;
    this.size = size;
  }
  
  LightObject(String name){
    this.name = name;
    this.start_address = 0;
    this.size = 1;
  }
  
  void set_start_address(int sa){
    this.start_address = sa;
  }
  
  void set_size(int size){
    this.size = size;
  }
  
  int get_start_address(){
    return start_address;
  }
  
  int[] get_data(int[] data){
    int[] ret = new int[size];
    for(int i = 0; i < size; i++){
      ret[i] = int(data[start_address+i]);
    }
    return ret;
  }
  
  int size(){
    return size;
  }

  String get_name(){
    return name;
  }
  
  String summary(){
    return "" + name + "," + start_address + "," + size;
  }
  
}