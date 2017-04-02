#include <Time.h>
#include <Wire.h>
#define SIZE 20
#define SIMONGAME 4
#define REACTIONGAME 5
#define COUNTDOWN 6
#define COUNTDOWN2 9
#define COUNTDOWN3 10
#define BUFFALO 7
#define ALKOMETER 8
int currentDir;
int activeUser = 1;
int switchState;
int switchState2;
int switchState3;
int switchState4;
int p1status = 1;
int p2status = 1;
int p3status = 1;
int left = 3;
int simonArr[SIZE+1];
int redLED = 11;
int greenLED = 10;
int sound = 12;
int totalGames = 4;
int games[5]={4,5,7,8}; //TOTALGAMES+1
int p1Green = 2;
int p1Red = 6;
int p2Green = 3;
int p2Red = 7;
int p3Green = 4;
int p3Red = 8;
int res = 0;




void setup() {
  Wire.begin();                // join i2c bus as master
  Serial.begin(9600);
  pinMode(sound, OUTPUT); //Højtaler
  
  pinMode(greenLED, OUTPUT); //GRØN
  pinMode(redLED, OUTPUT); //RØD
  pinMode(p1Green, INPUT); //P1 GRØN
  pinMode(p2Green, INPUT); //P2 GRØN
  pinMode(p3Green, INPUT); //P3 GRØN //TILFØJ
  pinMode(p1Red, INPUT); //P1 RØD
  pinMode(p2Red, INPUT); //P2 RØD
  pinMode(p3Red, INPUT); //P3 RØD //TILFØJ
  pinMode(A0, INPUT);
  
  
  //TILFØJ EKSTRA OUTPUTS

  
}

void loop() {
  selectGame();
  /*startGame();
  //res = playSimon();
  res = playReaction();
  screenWrite(res);
  delay(5000);*/
}

void setActiveUser(int in){
  if(in == 0) activeUser = 1;
  if(in == 1) activeUser = 2;
  if(in == 2) activeUser = 3;  
}

void selectGame(){
  int i = 0;
  screenWrite(games[i]);
  while(digitalRead(p1Green) == LOW){
    if (digitalRead(p1Red)==HIGH) {
      Serial.println(i);
      if (i+1 < totalGames){
        i++;
        screenWrite(games[i]);
        delay(350);
      } else {
        i = 0;
        screenWrite(games[i]);
        delay(350);
      }
    }
  } 
  Serial.println(games[i]);
  playGame(games[i]);
}



void playGame(int gameID){
  gameSetup();
  countDown();
  int res;
  if (gameID == SIMONGAME) res = playSimon();
  if (gameID == REACTIONGAME) res = playReaction();
  if (gameID == BUFFALO) res = playBuffalo();
  if (gameID == ALKOMETER) res = alcoChallenge(); //TODO: LAV ALKOCHALLENGE
  screenWrite(res);
  delay(5000);
}

int alcoChallenge(){
  int p1res = 0;
  int p2res = 0;

  while(true){
    int blow = 0;
    switchState = digitalRead(p1Green);
    digitalWrite(redLED, HIGH);
    while(switchState == LOW){
      switchState = digitalRead(p1Green);
    }
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    while(switchState == HIGH) {
      switchState = digitalRead(p1Green);
      blow = analogRead(A0);
       if ( blow > p1res){
        p1res = blow;
       }
    }
    digitalWrite(greenLED, LOW);
    delay(500);
    playNote();
    switchState = digitalRead(p1Green);
    digitalWrite(redLED, HIGH);
    while(switchState == LOW){
      switchState = digitalRead(p1Green);
    }
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    while(switchState == HIGH) {
      switchState = digitalRead(p1Green);
      blow = analogRead(A0);
       if ( blow > p2res){
        p1res = blow;
       }
    }
    digitalWrite(greenLED, LOW);
    delay(500);
    playNote();
    if (p1res >= p2res) return 0;
    return 1;
  }
};

void countDown(){
  screenWrite(COUNTDOWN);
  playNote();
  delay(1000);
  screenWrite(COUNTDOWN2);
  playNote();
  delay(1000);
  screenWrite(COUNTDOWN3);
  playNote();
  delay(1000);
  tone(sound, 800);
  delay(200);
  noTone(sound);
};

int playBuffalo(){
  screenWrite(BUFFALO);
  int counter = 0;
  int bLeft;
  int bRight;
  int killed;
  while(left > 1){
    killed = 0;
    counter++;
    Serial.println(activeUser);
    if(activeUser == 1){
      bLeft = p1Green;
      bRight = p1Red;
    } else if(activeUser == 2){
      bLeft = p2Green;
      bRight = p2Red;
    } else if(activeUser == 3){
      bLeft = p3Green;
      bRight = p3Red;
    }
    switchState = LOW;
    switchState2 = LOW;
    while (switchState == LOW && switchState2 == LOW){
        switchState = digitalRead(bLeft);
        switchState2 = digitalRead(bRight);
    };
    delay(100);
    switchState = digitalRead(bLeft);
    switchState2 = digitalRead(bRight);
    if((counter % 7 != 0) && containsSeven(String(counter)) == -1){
      if (switchState == HIGH && switchState2 == HIGH) {
        killPlayer();
        nextUser(currentDir);
        killed = 1;
        Serial.println("USER DID DIE! - Due to doubleclicking!"); 
      }
      if (killed == 0 && switchState == HIGH){
        nextUser(0);  
      } else if (killed == 0 && switchState2 == HIGH){
        nextUser(1);
      }
    } else {

      if(killed == 0 && switchState == HIGH && switchState2 == HIGH){
        nextUser(currentDir);  
      } else {
        killPlayer();
        nextUser(currentDir);
        Serial.println("USER DID DIE! - Due to not doubleclicking!");
      }
    }
  }
  if(left <= 1){
    if(p1status == 1) return 0;
    if(p2status == 1) return 1;
    if(p3status == 1) return 2;  
  } 
  
}

int containsSeven(String str){
  return str.indexOf('7');
}

void killPlayer(){
  if (activeUser == 1) {
    p1status = 0;
    youLost(1);
    left--;
    return;
  }
  if (activeUser == 2) {
    p2status = 0;
    youLost(2);
    left--;  
    return;
  }
  if (activeUser == 3) {
    p3status = 0;
    youLost(3);
    left--;
    return;
  }
}

void nextUser(int dir){ //0 er venstre, 1 er højre

  currentDir = dir;
  if (dir == 0){
    tone(sound, 400);
    delay(100);
    noTone(sound);
    digitalWrite(greenLED, HIGH);
    delay(500);
    digitalWrite(greenLED, LOW);
    if (activeUser == 1){
      activeUser = 2+p3status;
    }
    
    else if (activeUser == 3) {
      activeUser = 1+p2status;
    }
    else {
      if(p1status == 1){
        activeUser = 1;  
      }  else {
        activeUser = 3;
        }
    }
    
  } else if (dir == 1){
      tone(sound, 500);
      delay(100);
      noTone(sound);
      digitalWrite(redLED, HIGH);
      delay(500);
      digitalWrite(redLED, LOW);
    if (activeUser == 1){
      activeUser = 3-p2status;
    }
    
    else if (activeUser == 3) {
      activeUser = 2-p1status;;
    }
    else {
      if(p3status == 1){
        activeUser = 3;  
      }  else {
        activeUser = 1;
        }
    }
  }
}
int playSimon(){
  screenWrite(SIMONGAME);
  for (int i = 0; i < SIZE; i++){
      for (int j = 0; j <= i; j++){
        if (simonArr[j] == 0){
          digitalWrite(greenLED, HIGH);
          delay(200);
          digitalWrite(greenLED, LOW);
          delay(200);
        } else if (simonArr[j] == 1) {
          digitalWrite(redLED, HIGH);
          delay(200);
          digitalWrite(redLED, LOW);
          delay(200);
        }
      }
  fillPressedArrays(i+1);
    if(left <= 1){
      if(p1status == 1) return 0;
      if(p2status == 1) return 1;
      if(p3status == 1) return 2;
      return 11; //TIE
    }
  }
}

void bothflash(){
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, HIGH);
  delay(200);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  delay(600);
}

void playNote(){
  tone(sound, 440);
  delay(100);
  tone(sound, 440);
  delay(100);
  noTone(sound);
}

void youLost(int times){
  for (int i = 0; i < times; i++){
    tone(sound, 200);
    delay(200);
    noTone(sound);
    delay(200);
  }
}

void startGame(){
    tone(sound, 262);
    delay(200);
    tone(sound, 294);
    delay(200);
    tone(sound, 330);
    delay(200);
    tone(sound, 350);
    delay(200);
    tone(sound, 392);
    delay(200);
    noTone(sound);
}





void fillPressedArrays(int newSIZE){
    int p1[newSIZE+1];
    int p1rnd = 0;
    int p2[newSIZE+1];
    int p2rnd = 0;
    int p3[newSIZE+1];
    int p3rnd = 0;
    int b1;
    int b2;
    int b3; 
    int b4;
    int b5;
    int b6;
    int b1prev = LOW;
    int b2prev = LOW;
    int b3prev = LOW;
    int b4prev = LOW;
    int b5prev = LOW;
    int b6prev = LOW;


    while(true){
      if(p1status == 0 && p2status == 0 && p3status == 0) break;
    if (p1status ==1){
      b1 = digitalRead(p1Red);
      if(b1 == HIGH && b1prev == LOW && p1rnd < newSIZE){
        p1[p1rnd] = 1;
        Serial.println("1 - Added 1");
        p1rnd++;
        b1prev = HIGH;
      } else if (b1 == LOW && b1prev == HIGH){
        b1prev = LOW;
      }
      b2 = digitalRead(p1Green);
      if(b2 == HIGH && b2prev == LOW && p1rnd < newSIZE){
        p1[p1rnd] = 0;
        p1rnd++;
        Serial.println("1 - Added 0");
        b2prev = HIGH;
      } else if (b2 == LOW && b2prev == HIGH){
        b2prev = LOW;
      }
   }
   if(p2status == 1){
      b3 = digitalRead(p2Red);
      if(b3 == HIGH && b3prev == LOW && p2rnd < newSIZE){
        p2[p2rnd] = 1;
        Serial.println("2 - Added 1");
        p2rnd++;
        b3prev = HIGH;
      } else if (b3 == LOW && b3prev == HIGH){
        b3prev = LOW;
      }
      b4 = digitalRead(p2Green);
      if(b4 == HIGH && b4prev == LOW && p2rnd < newSIZE){
        p2[p2rnd] = 0;
        p2rnd++;
        Serial.println("2 - Added 0");
        b4prev = HIGH;
      } else if (b4 == LOW && b4prev == HIGH){
        b4prev = LOW;
      }
    }
    if(p3status == 1){
        b5 = digitalRead(p3Red);
        if(b5 == HIGH && b5prev == LOW && p3rnd < newSIZE){
          p3[p3rnd] = 1;
          Serial.println("3 - Added 1");
          p3rnd++;
          b5prev = HIGH;
        } else if (b5 == LOW && b5prev == HIGH){
          b5prev = LOW;
        }
        b6 = digitalRead(p3Green);
        if(b6 == HIGH && b6prev == LOW && p3rnd < newSIZE){
          p3[p3rnd] = 0;
          Serial.println("3 - Added 0");
          p3rnd++;
          b6prev = HIGH;
        } else if (b6 == LOW && b6prev == HIGH){
          b6prev = LOW;
        }
    } 

      if (((p1rnd == newSIZE || p1status == 0) && (p2rnd == newSIZE || p2status == 0) && (p3rnd == newSIZE || p3status == 0))){
        break;  
      }

    } //End of While-true
    for (int i = 0; i < newSIZE; i++){
      if (p1[i] != simonArr[i] && p1status == 1){
        Serial.println("PLAYER 1 ELIMINATED");
        youLost(1);  
        p1status = 0;
        left--;
      }
      if (p2[i] != simonArr[i] && p2status == 1){
        Serial.println("PLAYER 2 ELIMINATED");
        youLost(2); 
        p2status = 0;
        left--;
      }

      if (p3[i] != simonArr[i] && p3status == 1){
        Serial.println("PLAYER 3 ELIMINATED");
        youLost(3); 
        p3status = 0;
        left--;
      }

    } 
  delay(200);
  playNote();
  delay(1500);
}

int playReaction(){
  screenWrite(REACTIONGAME);
  while(left > 1){
    digitalWrite(greenLED,LOW);
    digitalWrite(redLED, HIGH);
    int rnd = random(5000, 20000);
    int p1pressed = 0;
    int p2pressed = 0;
    int p3pressed = 0;
    if (p1status != 0) p1status = 2;
    if (p2status != 0) p2status = 2;
    if (p3status != 0) p3status = 2;
    
    for (int i = 0; i < rnd; i++){
      
      switchState = digitalRead(p1Green);
      switchState2 = digitalRead(p2Green);
      switchState3 = digitalRead(p3Green);
      
      if (switchState == HIGH){
        Serial.println("P1 false start");
        p1status = 0;
        left--;
        youLost(1);
        delay(1500); 
      }
      if (switchState2 == HIGH){
        Serial.println("P2 false start");
        p2status = 0;
        left--;
        youLost(2);  
        delay(1500);
      }
      if (switchState3 == HIGH){
        Serial.println("P3 false start");
        p3status = 0;
        left--; 
        youLost(3);
        delay(1500);   
      }
      delay(1);
    };
    
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    int winner = left;
    
    while(winner > 1) {
      if(p1status == 2) {
        if(digitalRead(p1Green) == HIGH){
          p1status = 1; //Videre til næste runde
          Serial.println("P1 pressed the button!");
          winner--;
        }
      }
      if(p2status == 2) {
        if(digitalRead(p2Green) == HIGH){
          p2status = 1;
          Serial.println("P2 pressed the button!");
          winner--;
        }
      }
      if(p3status == 2) {
        if(digitalRead(p3Green) == HIGH){
          p3status == 1;
          Serial.println("P3 pressed the button!");
          winner--;
        }
      }
    }
    
        if(p1status == 2 && left > 1){ 
          p1status = 0;
          Serial.println("P1 lost");
          youLost(1);
          left--;
          delay(2000);
        }
        if(p2status == 2 && left > 1){
          p2status = 0;
          Serial.println("P2 lost");
          youLost(2);
          left--;
          delay(2000);
        }
        if(p3status == 2 && left > 1){
          p3status = 0;
          Serial.println("P3 lost");
          youLost(3);
          left--;
          delay(2000);
        }  
    if(left <= 1){
      if(p1status != 0) return 0;
      if(p2status != 0) return 1;
      if(p3status != 0) return 2;  
    } 
  }
  
}

void gameSetup(){
  randomSeed(millis());
  for(int i = 0; i < SIZE; i++){
    int rnd = random(2);
    simonArr[i] = rnd;
  }
  left = 3;
  p1status = 1;
  p2status = 1;
  p3status = 1;  
}

void screenWrite(int out){
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(out);              // sends out 
  Wire.endTransmission();
}