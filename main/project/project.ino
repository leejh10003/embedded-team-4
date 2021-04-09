void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

int from;
int destination;
int stack[10][2];//[i][0] -> 누른 층, [i][1]->누른 층에서 가고 싶은 곳.
int stack_count=0;//입력된 값 수
int current_state=1;//현재 층
void loop() {
  // put your main code here, to run repeatedly:
 if(Serial.available()){
    from = Serial.parseInt(); //임시로 층 수 입력
    destination = Serial.parseInt();
    Serial.read();
    Serial.read();//공백 처리
    stack_input(from, destination);
    stack_count++;

    //delay(1000);
 }
 else{ //층 수 입력이 받아지지 않았을 때
    if(stack[0][0] != 0){
      if(stack[0][0] > current_state)
        current_state++;
      else if(stack[0][0] < current_state)
        current_state--;
      else{ // stack[0][0] == current_state
        if(stack[0][1] != 0){ //5층 사람이 20층을 가고싶으면 타서 20층을 가면 끝이니까 (5,20) -> (20,0)으로 바꾸고, 20층 도착하면 호출은 사라지니까 없애자.
          stack[0][0] = stack[0][1];
          stack[0][1] = 0;
        }
        else{
          stack[0][0] = 0;
          stack[0][1] = 0;
          first_stack_remove();
          stack_count--;
        }
        Serial.println("goal");
        delay(3000); //그 층에 멈춤
      }
    }
    Serial.print("cureent floor is: ");
    Serial.println(current_state);
    delay(1000);
 }
}

void first_stack_remove(){
  for(int i=0; i<stack_count; i++){
    stack[i][0] = stack[i+1][0];
    stack[i][1] = stack[i+1][1];
  }
}





void stack_input(int from, int destination){
  for(int i=0; i<10; i++){
    if(stack[i][0] == 0){
      stack[i][0] = from;
      stack[i][1] = destination;
      //stack_sort();
      return;
    }
  }
  return;
}


void stack_sort(){ // 엘리베이터 우선순위 알고리즘
  int temp_count = stack_count;
  int compare = current_state - stack[0][0]; //현재 층 - 현재 목적지(양수면 하강, 음수면 승강)
 
  if(compare<0){//승강중 일 때,
    for(int i=1; i<stack_count; i++){
      if((stack[i][0] < stack[i][1])){
        
      }
    }
    for(int i=temp_count-1; i>0; i--){
     for(int j=0; j<i; j++){
        
      }      
     } 
   }
  else if(compare>0){
      
   }
  
  return;
}
