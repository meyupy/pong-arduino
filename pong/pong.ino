const byte PINS_ROW[8] = {A0, 12, A2, 13, 5, A3, 7, 2};
const byte PINS_COL[8] = {9, 8, 4, A1, 3, 10, 11, 6};
const byte PIN_BUTTON_1 = A4, PIN_BUTTON_2 = A5;

byte state[8][8] = {};

class Game{

  private:
    
    class Button {

      private:

        bool pressed = false;

      public:

        bool isClicked(int isPushedButton) {
          if(!this->pressed && isPushedButton) {
            this->pressed = true;
            return true;
          }
          if(this->pressed && !isPushedButton) this->pressed = false;
          return false;
        }

    };

    class Paddle{

      public:

        Paddle(byte x){
          this->x = x;
        }

        byte x;
        bool headingRight;

        void move(int isClickedButton){
          if(isClickedButton) this->headingRight = !this->headingRight;
          if(this->headingRight){
            if(this->x < 6){
              this->x++;
            } else{
              this->headingRight = false;
              this->x--;
            }
          }
          else{
            if(this->x > 0){
              this->x--;
            } else{
              this->headingRight = true;
              this->x++;
            }
          }
        }

    };

    class Ball{

      private:

        byte direction;  // 1: 45, 2: 135, 3: 225, 4: 315 degrees

      public:

        Ball(){this->resetValues();}

        byte x, y;

        void resetValues(){
          this->direction = rand()%4+1;
          this->x = rand()%2+3; this->y = rand()%2+3;
        }

        void move(byte p1X, byte p2X){
          switch(this->direction){
            case 1:
              if(this->x<7&&this->y==1 && this->x>=p1X-1&&this->x<=p1X+1) this->direction = this->x==p1X-1 ? 3 : 4;  // 1.3 : 1.2
              else if(this->x==7) this->direction = this->y>1 ? 2 : (p1X==5||p1X==6 ? 3 : 2);  // 2.1 : (2.3 : 2.2)
            break;
            case 2:
              if(this->x>0&&this->y==1 && this->x>=p1X&&this->x<=p1X+2) this->direction = this->x==p1X+2 ? 4 : 3;
              else if(this->x==0) this->direction = this->y>1 ? 1 : (p1X==0||p1X==1 ? 4 : 1);
            break;
            case 3:
              if(this->x>0&&this->y==6 && this->x>=p1X&&this->x<=p1X+2) this->direction = this->x==p1X+2 ? 1 : 2;
              else if(this->x==0) this->direction = this->y<6 ? 4 : (p1X==0||p1X==1 ? 1 : 4);
            break;
            case 4:
              if(this->x<7&&this->y==6 && this->x>=p1X-1&&this->x<=p1X+1) this->direction = this->x==p1X-1 ? 2 : 1;
              else if(this->x==7) this->direction = this->y<6 ? 3 : (p1X==5||p1X==6 ? 2 : 3);
            break;
          }
          switch(this->direction){
            case 1: this->x++; this->y--; break;
            case 2: this->x--; this->y--; break;
            case 3: this->x--; this->y++; break;
            case 4: this->x++; this->y++; break;
          }
        }

    };

    Button button1, button2;
    Paddle paddle1, paddle2;
    Ball ball;

    void updateState(byte state[8][8], byte p1X, byte p2X, byte ballX, byte ballY){
      for(byte i=0; i<8; i++){
        for(byte j=0; j<8; j++){
          state[i][j] = 0;
        }
      }
      for(byte i=p1X; i<p1X+2; i++){
        state[i][0] = 1;
      }
      for(byte i=p2X; i<p2X+2; i++){
        state[i][7] = 1;
      }
      state[ballX][ballY] = 1;
    }

    const byte FTW = 8;  // frames to wait
    const byte FTW_BALL = 6;
    long int totalFrames = 0;
    bool running = false;

  public:

    Game() : paddle1(6), paddle2(0) {}

    void iterate(byte state[8][8], int isPushedButton1, int isPushedButton2){
      if(this->totalFrames % this->FTW == 0){
        if(this->running){
          this->paddle1.move(this->button1.isClicked(isPushedButton1));
          this->paddle2.move(this->button2.isClicked(isPushedButton2));
          if(this->totalFrames % FTW_BALL == 0) this->ball.move(this->paddle1.x, this->paddle2.x);
          if(this->ball.y==0 || this->ball.y==7){
            this->ball.resetValues();
            this->running = false;
          }
        }
        else if(!this->running && isPushedButton1 && isPushedButton2){
          this->paddle1.x = 6; this->paddle1.headingRight = false;
          this->paddle2.x = 0; this->paddle2.headingRight = true;
          this->running = true;
        }
        this->updateState(state, this->paddle1.x, this->paddle2.x, this->ball.x, this->ball.y);
      }
      this->totalFrames++;
    }

};

Game game;

void setup() {

  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);

  for(byte i=0; i<8; i++){
    pinMode(PINS_ROW[i], OUTPUT);
    pinMode(PINS_COL[i], OUTPUT);
    digitalWrite(PINS_COL[i], HIGH);
  }

}

void loop() {

  game.iterate(state, !digitalRead(PIN_BUTTON_1), !digitalRead(PIN_BUTTON_2));
  displayMatrix(state);

}

void displayMatrix(byte state[8][8]) {

  for(byte r=0; r<8; r++){
    digitalWrite(PINS_ROW[r], HIGH);
    for(byte c=0; c<8; c++){
      const byte pixelState = state[c][r];  // horizontal
      digitalWrite(PINS_COL[c], !pixelState);
      delayMicroseconds(250);
      if(pixelState) digitalWrite(PINS_COL[c], HIGH);
    }
    digitalWrite(PINS_ROW[r], LOW);
  }

}