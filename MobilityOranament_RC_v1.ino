//  프로그램명 : BE10.mode3.1.0
//  4WD 다중제어 BE 1) 버튼제어 2) 라인트레이서 3) 위치센서
//  소프트웨어의 재정비 : 모듈화
//  최종편집 : 2022년 1월 12일
//

#include <SoftwareSerial.h>
SoftwareSerial BTSerial(7, 8); // RX, TX
                               // RX는 HC06의 TX와 연결시킨다.

//------------------------------------------
// 매크로함수
#define LmotorF(speed) {analogWrite(motorL1, speed); analogWrite(motorL2, 0);}
#define LmotorB(speed) {analogWrite(motorL1, 0); analogWrite(motorL2, speed);}
#define RmotorF(speed) {analogWrite(motorR1, 0); analogWrite(motorR2, speed);}
#define RmotorB(speed) {analogWrite(motorR1, speed); analogWrite(motorR2, 0);}
#define RC_Stop {LmotorF(0); RmotorF(0);}
#define RC_Forward {LmotorF(Hspeed); RmotorF(Hspeed);}
#define RC_Backward {LmotorB(Hspeed); RmotorB(Hspeed);}
#define RC_Rtankturn {LmotorF(Tspeed); RmotorB(Tspeed);}
#define RC_Ltankturn {LmotorB(Tspeed); RmotorF(Tspeed);}
#define RC_RFspinturn {LmotorF(Hspeed); RmotorF(Lspeed);}
#define RC_LFspinturn {LmotorF(Lspeed); RmotorF(Hspeed);}
#define RC_RBspinturn {LmotorB(Hspeed); RmotorB(Lspeed);}
#define RC_LBspinturn {LmotorB(Lspeed); RmotorB(Hspeed);}

#define Line_Forward {LmotorF(LFspeed); RmotorF(LFspeed);}
#define Line_Rtankturn {LmotorF(LTspeed); RmotorB(LTspeed);}
#define Line_Ltankturn {LmotorB(LTspeed); RmotorF(LTspeed);}

#define FrontLED_ON {digitalWrite(FrontLED, HIGH);}
#define FrontLED_OFF {digitalWrite(FrontLED, LOW);}

//------------------------------------------
// 하드웨어에 관련된 변수들
// (1) 모터드라이버 298과의 연결
#define motorR1   9     // 오른쪽모터의 1
#define motorR2   10    // 오른쪽모터의 2
#define motorL1   11     // 왼쪽모터의 1
#define motorL2   3    // 왼쪽모터의 2

// (2) 모터속도의 결정
#define Hspeed   255     // 고속
#define Lspeed   90     // 저속
#define Tspeed   150     // 탱크턴에 맞는 속도

#define LFspeed   90     // 라인트레이서의 직진 속도
#define LTspeed   255     // 라인트레이서의 회전 속도

// (3) 전조등
#define FrontLED   12

// (4) 라인트레이서의 정지조건
#define stopcount   15000     // zerocount가 얼마가 되면 멈추나?

//------------------------
// 상태변수들
char ctrl_data;        // 제어데이터
int SensorIR;
int zerocount=0;
int runmode = 1;      // 제어모드 0=버튼, 1=라인, 2=자세
int thres = 15;
int roll, pitch; //roll and pitch sent from Android device
int pos_roll, pos_pitch; //

//=======================================
void setup(){
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(FrontLED, OUTPUT); FrontLED_OFF;
  BTSerial.print("*RR255G0B0*"); BTSerial.print("*YR0G0B0*"); BTSerial.print("*GR0G0B*");
}

//=======================================
void loop(){
  if (BTSerial.available()) {      //데이터가 왔다면
      
    ctrl_data = BTSerial.read();
    
    //디버깅을 위한 코드(시리얼 모니터 출력)
    Serial.write(ctrl_data);
    Serial.write("\n");
  
    fn_setmode();   // 제어모드를 설정한다.
    if(runmode == 0) { fn_run_butt(); }  // RC카를 버튼으로 제어
    if(runmode == 2) { fn_run_pose(); }  // RC카를 자세로 제어
  }
  if(runmode == 1) { fn_run_line(); }
}

//=======================================
void fn_setmode(){
  switch(ctrl_data){
    case 'x': // 버튼으로 제어
      FrontLED_OFF; // 전조등 끄기
      runmode = 0;        // 수동제어 모드
      BTSerial.print("*RR255G0B0*"); BTSerial.print("*YR0G0B0*"); BTSerial.print("*GR0G0B*");
      RC_Stop;
      break;
    case 'X': // 라인트레이서를 동작
      FrontLED_ON; // 전조등 켜기
      runmode = 1;        // 라인트레이서 모드
      BTSerial.print("*RR0G0B0*"); BTSerial.print("*YR255G255B0*"); BTSerial.print("*GR0G0B*");
      break;
    case 'E': // 위치센서로 제어
      FrontLED_ON; // 전조등 켜기
      runmode = 2;        // 위치센서로 제어
      BTSerial.print("*RR0G0B0*"); BTSerial.print("*YR0G0B0*"); BTSerial.print("*GR0G255B*");
      RC_Stop;
      break;
  }
}
void fn_run_butt(){
  switch(ctrl_data){
    case 'S': RC_Stop; break;  // 정지(Stop)
    case 'F': RC_Forward; break;  // 전진(Forward)
    case 'B': RC_Backward; break;  // 후진(Backward)
    case 'R': RC_Rtankturn; break;  // 오른쪽 탱크턴
    case 'L': RC_Ltankturn; break;  // 왼쪽 탱크턴
    case 'I': RC_RFspinturn; break;  // 오른쪽 전진스핀턴
    case 'G': RC_LFspinturn; break;  // 왼쪽 전진스핀턴
    case 'J': RC_RBspinturn; break;  // 오른쪽 후진스핀턴
    case 'H': RC_LBspinturn; break;  // 왼쪽 후진스핀턴
    case 'w': FrontLED_OFF; break;  // 전조등 끄기
    case 'W': FrontLED_ON; break;  // 전조등 켜기
  }
}
void fn_run_line(){
  SensorIR = PINC &=B00000111;
  switch (SensorIR) {
    case 0: RC_Stop; break;
    case 7:
      zerocount +=1;
      if(zerocount > stopcount) {RC_Stop; zerocount -=1;}
      else {Line_Forward;}
      break;
    case 5: Line_Forward; zerocount=0; break;
    case 4:
    case 6: Line_Ltankturn; zerocount=0; break;
    case 1:
    case 3: Line_Rtankturn; zerocount=0; break;
  }
}
void fn_run_pose(){
    if (ctrl_data == 'A') {
      fn_readdata();        // 롤피치정보를 읽고
      fn_run_pose2();}      // RC카를 제어
}
void fn_readdata(){
      roll = BTSerial.parseInt();
      while (ctrl_data != '*') {
        if (BTSerial.available()) {
          ctrl_data = BTSerial.read(); //Get next character from bluetooth
          if (ctrl_data == ',')pitch = BTSerial.parseInt(); }
      }
}
void fn_run_pose2(){
      pos_roll = 0;
      if (roll < -thres) pos_roll = 1;
      if (roll > thres) pos_roll = -1;
      pos_pitch = 0;
      if (pitch < -thres) pos_pitch = 1;
      if (pitch > thres) pos_pitch = -1;
      switch (pos_pitch) {
        case 1:   // 앞줄 3개
          switch (pos_roll) {
            case 1: // G
              RC_LFspinturn; break;
            case 0: // F
              RC_Forward; break;
            case -1: // I
              RC_RFspinturn; break;
          }
          break;
        case 0:   // 중간줄 3개
          switch (pos_roll) {
            case 1: // L
              RC_Ltankturn; break;
            case 0: // S
              RC_Stop; break;
            case -1: // R
              RC_Rtankturn; break;
          }
          break;
        case -1:   // 뒤줄 3개
          switch (pos_roll) {
            case 1: // H
              RC_LBspinturn; break;
            case 0: // B
              RC_Backward; break;
            case -1: // J
              RC_RBspinturn; break;
          }
          break;
      }
}
