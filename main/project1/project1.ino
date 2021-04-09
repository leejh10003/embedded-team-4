void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

int from;
int destination;
int stack[100][2];//[i][0] -> 사람이 누른 층, [i][1] -> 누른 층에서 가고 싶은 곳
int transport[100]; //-1 = 끝, 0 = 탑승 대기, 1 = 탑승 중
int stack_count=0;//총 사람 수
int remain_people=0;//대기 사람 수
int goal_state=0;//지금 목표로 정해둔 사람.
int current_state=1;//현재 층
void loop() {
  // put your main code here, to run repeatedly:
 if(Serial.available()){
    from = Serial.parseInt(); //임시로 층 수 입력
    destination = Serial.parseInt();
    Serial.read();
    Serial.read();
    stack_input(from, destination);
    stack_count++;
    remain_people++;
    
    //delay(1000);
 }
 else{ //층 수 입력이 받아지지 않았을 때
    if(remain_people == 0){
      Serial.print("cureent floor is: ");
      Serial.println(current_state);
      delay(1000);
    }
    else{
      if(goal_state - current_state > 0){
        current_state++;
      }
      else if(goal_state - current_state < 0){
        current_state--;
      }
      is_people();
      goal_people();//도착 처리, goal_state갱신도 여기서 하므로 else를 안넣어도 된다 위에.
    }
    Serial.print("cureent floor is: ");
    Serial.println(current_state);
    delay(1000);
 }
}

void stack_input(int from, int destination){
      stack[stack_count][0] = from;
      stack[stack_count][1] = destination;
      if(remain_people == 0){
        goal_state = from;
      }
      /*
      for(int i=0; i<stack_count+1; i++){
        Serial.println(stack[i][0]);
        Serial.println(stack[i][1]);
      }
      */
      return;
}

void is_people(){//탈 사람 탐색
  for(int i=0; i<stack_count; i++){
    if(stack[i][0] == current_state && transport[i] == 0){
      Serial.print(i);
      Serial.print("번째 people enter at ");
      Serial.println(current_state);
      transport[i] = 1;//탑승 시키고
      if(goal_state == stack[i][0]){
        goal_state = stack[i][1];
      }
    }
  }
}

void goal_people(){//내릴 사람 탐색
  for(int i=0; i<stack_count; i++){
    if(transport[i] == 1 && current_state == stack[i][1]){
      remain_people--;
      transport[i] = -1;
      Serial.print("goal floor is: ");
      Serial.println(current_state);
      Serial.print(i);
      Serial.println("번째 people exit");
      delay(1000);

      if(stack[i][1] == goal_state){//도착 했을 때 갱신
        for(int j=0; j<stack_count; j++){//다음 우선순위 사람의 목적지로
          if(transport[j] == 0){
            goal_state = stack[j][0];
            return;
          }
          else if(transport[j] == 1){
            goal_state = stack[j][1];
            return;
          }
        }
      }
    }
  }
  return;
}
