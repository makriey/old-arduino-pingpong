// Ping Pong для Arduino Nano
// библиотеки: Adafruit SSD1306 и Adafruit GFX

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define POT_LEFT  A6   // левый потенциометр
#define POT_RIGHT A3   // правый потенциометр
#define BUTTON    9
#define BUZZER    7

float leftY = 25, rightY = 25;       // высота ракеток
float ballX, ballY, ballVX, ballVY;  // мяч
int leftScore = 0, rightScore = 0;
int state = 0;                       // 0 = меню, 1 = игра, 2 = конец
unsigned long lastFrame = 0;

// кнопка, начало
bool buttonClicked() {
  if (digitalRead(BUTTON) == HIGH) {   // если кнопка не работает,надо LOW
    while (digitalRead(BUTTON) == HIGH) { }
    delay(50);
    return true;
  }
  return false;
}

// мяч в центр, dir: 1 = вправо, -1 = влево
void newBall(int dir) {
  ballX = 64;
  ballY = 32;
  ballVX = 1.6 * dir;
  ballVY = random(6, 11) / 10.0;
  if (random(2) == 1) ballVY = -ballVY;
}

void setup() {
  pinMode(BUTTON, INPUT);
  pinMode(BUZZER, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);
  randomSeed(analogRead(A7));
}

void loop() {
  if (millis() - lastFrame < 24) return; 
  lastFrame = millis();
  display.clearDisplay();

  if (state == 0) {
    // меню
    display.setTextSize(2);
    display.setCursor(11, 14);
    display.print("PING PONG");
    display.setTextSize(1);
    display.setCursor(28, 44);
    display.print("PRESS BUTTON");

    if (buttonClicked()) {
      leftScore = 0;
      rightScore = 0;
      newBall(1);
      state = 1;
    }
  }
  else if (state == 1) {
    // ракетки (14 высота ракетки, 64 - 14 = 50 пикселей хода)
    leftY = analogRead(POT_LEFT) / 1023.0 * 50;
    rightY = analogRead(POT_RIGHT) / 1023.0 * 50;

    // движение мяча
    ballX = ballX + ballVX;
    ballY = ballY + ballVY;

    // стенки сверху и снизу
    if (ballY < 0) {
      ballY = 0;
      ballVY = -ballVY;
      tone(BUZZER, 600, 20);
    }
    if (ballY > 61) {
      ballY = 61;
      ballVY = -ballVY;
      tone(BUZZER, 600, 20);
    }

    // левая ракетка
    if (ballVX < 0 && ballX <= 6 && ballX >= 0 && ballY + 3 >= leftY && ballY <= leftY + 14) {
      ballVX = -ballVX * 1.05;
      ballVY = (ballY + 1.5 - (leftY + 7)) / 7 * 1.5; // угол от места удара
      tone(BUZZER, 900, 20);
    }

    // правая ракетка
    if (ballVX > 0 && ballX + 3 >= 122 && ballX <= 125 && ballY + 3 >= rightY && ballY <= rightY + 14) {
      ballVX = -ballVX * 1.05;
      ballVY = (ballY + 1.5 - (rightY + 7)) / 7 * 1.5;
      tone(BUZZER, 700, 20);
    }

    ballVX = constrain(ballVX, -3.5, 3.5);

    // мячик вылетел за экран
    if (ballX < -3 || ballX > 128) {
      int dir;
      if (ballX < -3) {
        rightScore++;
        dir = 1;
      } else {
        leftScore++;
        dir = -1;
      }
      tone(BUZZER, 300, 200);

      if (leftScore == 5 || rightScore == 5) {
        state = 2;
      } else {
        newBall(dir);
        delay(500);
      }
    }

    //счёт, ракетки и мяч
    display.setTextSize(2);
    display.setCursor(44, 0);
    display.print(leftScore);
    display.setCursor(72, 0);
    display.print(rightScore);
    display.fillRect(3, (int)leftY, 3, 14, SSD1306_WHITE);
    display.fillRect(122, (int)rightY, 3, 14, SSD1306_WHITE);
    display.fillRect((int)ballX, (int)ballY, 3, 3, SSD1306_WHITE);
  }
  else {
    // победв
    display.setTextSize(2);
    display.setCursor(5, 8);
    if (leftScore > rightScore) {
      display.print("LEFT WINS");
    } else {
      display.print("RIGHT WINS");
    }
    display.setCursor(47, 30);
    display.print(leftScore);
    display.print(":");
    display.print(rightScore);
    display.setTextSize(1);
    display.setCursor(28, 54);
    display.print("PRESS BUTTON");

    if (buttonClicked()) {
      leftScore = 0;
      rightScore = 0;
      newBall(1);
      state = 1;
    }
  }

  display.display();
}
