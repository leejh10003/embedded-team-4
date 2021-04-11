#include <Arduino.h>
#include "ArduinoSTL.h"
using namespace std;
typedef enum Direction{
  Stop,
  Downward,
  Upward
} Direction;
typedef struct MoveRequest{
  int floor;
  Direction direction;
} MoveRequest;
typedef struct WaitInfo{
  int time;
  int floor;
} WaitInfo;
const int betweenFloorTime = 5;
const int openDoorFloorTime = 10;
/*
 * This class use ArduinoSTL, install it regarding https://www.arduino.cc/reference/en/libraries/arduinostl/
 * Because of Arduino provided code's issue, have to use Arduino AVR Boards package to version 1.8.2
 * Go to Tools -> Board -> Boards Manager... And downgrade Arduino AVR Boards package to version 1.8.2
*/
class RequestResolver {
  private:
  static vector<MoveRequest> requests;
  static bool check_redundant(MoveRequest new_input){
    for (MoveRequest element: RequestResolver::requests){
      if (new_input.floor == element.floor && new_input.direction == element.direction){
        return true;
      }
    }
    return false;
  }
  static vector<MoveRequest> realign_request(int current_floor, Direction current_direction, MoveRequest new_request){
    vector<MoveRequest> temp = RequestResolver::requests;
    temp.push_back(new_request);
    vector<MoveRequest> only_floor_inputs = {};
    vector<MoveRequest> upward_inputs = {};
    vector<MoveRequest> downward_inputs = {};
    for(MoveRequest element: temp){
      if (element.direction == Stop){
        only_floor_inputs.push_back(element);
      } else if (element.direction == Upward){
        upward_inputs.push_back(element);
      } else if (element.direction == Downward){
        downward_inputs.push_back(element);
      }
    }
    if (current_direction == Downward){
      vector<MoveRequest> temp = {};
      vector<MoveRequest> aligned = {};
      vector<MoveRequest> result = {};
      temp.insert(temp.end(), only_floor_inputs.begin(), only_floor_inputs.end());
      temp.insert(temp.end(), downward_inputs.begin(), downward_inputs.begin());
      for(MoveRequest element: temp){
        if (element.floor <= current_floor){
          aligned.push_back(element);
        }
      }
      std::sort(aligned.begin(), aligned.end(), [](MoveRequest a, MoveRequest b) {
        return a.floor > b.floor;
      });
      result.insert(result.end(), aligned.begin(), aligned.end());
      temp = {};
      aligned = {};
      temp.insert(temp.end(), only_floor_inputs.begin(), only_floor_inputs.end());
      for(MoveRequest element: temp){
        if (element.floor > current_floor){
          aligned.push_back(element);
        }
      }
      aligned.insert(aligned.end(), upward_inputs.begin(), upward_inputs.end());
      std::sort(aligned.begin(), aligned.end(), [](MoveRequest a, MoveRequest b) {
        return a.floor < b.floor;
      });
      result.insert(result.end(), aligned.begin(), aligned.end());
      aligned = {};
      for(MoveRequest element: downward_inputs){
        if (element.floor > current_floor){
          aligned.push_back(element);
        }
      }
      std::sort(aligned.begin(), aligned.end(), [](MoveRequest a, MoveRequest b) {
        return a.floor > b.floor;
      });
      result.insert(result.end(), aligned.begin(), aligned.end());
      return result;
    } else if (current_direction == Upward){
      vector<MoveRequest> temp = {};
      vector<MoveRequest> aligned = {};
      vector<MoveRequest> result = {};
      temp.insert(temp.end(), only_floor_inputs.begin(), only_floor_inputs.end());
      temp.insert(temp.end(), upward_inputs.begin(), upward_inputs.begin());
      for(MoveRequest element: temp){
        if (element.floor > current_floor){
          aligned.push_back(element);
        }
      }
      std::sort(aligned.begin(), aligned.end(), [](MoveRequest a, MoveRequest b) {
        return a.floor < b.floor;
      });
      result.insert(result.end(), aligned.begin(), aligned.end());
      temp = {};
      aligned = {};
      temp.insert(temp.end(), only_floor_inputs.begin(), only_floor_inputs.end());
      for(MoveRequest element: temp){
        if (element.floor <= current_floor){
          aligned.push_back(element);
        }
      }
      aligned.insert(aligned.end(), upward_inputs.begin(), upward_inputs.end());
      std::sort(aligned.begin(), aligned.end(), [](MoveRequest a, MoveRequest b) {
        return a.floor > b.floor;
      });
      result.insert(result.end(), aligned.begin(), aligned.end());
      aligned = {};
      for(MoveRequest element: upward_inputs){
        if (element.floor <= current_floor){
          aligned.push_back(element);
        }
      }
      std::sort(aligned.begin(), aligned.end(), [](MoveRequest a, MoveRequest b) {
        return a.floor < b.floor;
      });
      result.insert(result.end(), aligned.begin(), aligned.end());
      return result;
    }
  }
  static vector<WaitInfo> generate_wait_info(int current_floor){
    int waitTimeTillNow = 0;
    vector<WaitInfo> generated = {};
    for(int i = 0; i < RequestResolver::requests.size(); i++){
      waitTimeTillNow += abs(i == 0 ? current_floor - RequestResolver::requests[i].floor : RequestResolver::requests[i].floor - requests[i - 1].floor) * betweenFloorTime + openDoorFloorTime;
      generated.push_back({
        .time = waitTimeTillNow,
        .floor = requests[i].floor
      });
    }
  }
  public:
  static void new_request(MoveRequest new_input){
    int current_floor = 0;//TODO: get_current_floor();
    Direction current_direction = Downward;//TODO: get_current_direction();
    bool is_redundant = RequestResolver::check_redundant(new_input);
    if (is_redundant){
      return;
    }
    else {
      vector<MoveRequest> new_requests = RequestResolver::realign_request(current_floor, current_direction, new_input);
      if (new_requests[0].floor != requests[0].floor || new_requests[0].direction != requests[0].direction){
        //TODO: new_move(new_requests[0]);
      }
      RequestResolver::requests = new_requests;
      vector<WaitInfo> generated_wait_info = RequestResolver::generate_wait_info(current_floor);
      //TODO: display_new_info(generated_wait_info);
    }
  }
};
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
