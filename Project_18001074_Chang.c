//EDB2063 Microprocessor
//Chang Kwong Ming 18001074 COE
//Mini Game Title (Bounce Away)
//Something like the dino game in chrome 
//objective is to jump over the pillar coming toward and accumulate as much score as possible
//20/8/2020

#include "mbed.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include "C12832.h"

C12832 lcd(SPI_MOSI, SPI_SCK, SPI_MISO, p8, p11);

DigitalIn btn(BUTTON1);                 // input button1
DigitalIn Switch(p25);                  // input switch
AnalogIn speed(p15);                    // input potentiometer
InterruptIn btn2(p30);                  // input button2 as interrupt

PwmOut speaker(p21);                    // output speaker 
DigitalOut led1(LED1);                  // output led1
DigitalOut led2(LED2);                  // output led2
Ticker t1;                              // software interrupt 

void init_Game();                       // initialize of variables
void refresh_screen();                  // refresh the game screen
void countTime();                       // increment of timer
void gameAction();                      // movement of the game
void display_LCD();                     // print game data(score,speed,time) in lcd display
void play_tone(float,float,float,int);  // speaker
void gameOver();                        // to indicate the game end
void pauseScreen();                     // puase game screen
void resetGame();                       // to restart the game   
void gameCore();                        // set conditions of game over and checkpoint


int timer;
int spd;
int x2;
int y; 
int x1; 
int score;
int checkpoint; 
int interval;

void init_Game(){ // initialize of variables
        
        interval = 1;
        timer = 0;
        x2 = 230;
        y = 165;
        x1 = 15;
        score = 0;
        checkpoint = 100;
}

void refresh_screen(){ // refresh the game screen
    
        BSP_LCD_Clear(LCD_COLOR_BLACK);         // clear screen in black
                
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);   // platform of the game (runway)
        BSP_LCD_FillRect(0, 180, 250, 60);
     
        BSP_LCD_SetTextColor(LCD_COLOR_RED);    // Player object
        BSP_LCD_FillCircle(x1,y,15);
        BSP_LCD_SetTextColor(LCD_COLOR_YELLOW); // Obstacle
        BSP_LCD_FillRect(x2,120,10,60);
}

void countTime(){  // increment of timer
    
        timer += interval;
}

void gameAction(){ // movement of the game
    
        if(x2 <= -10){   // the movement of the obstacle 
            x2 = 230;
        }
        else{ 
            x2 -= spd;   // to control the speed of the obstacle
        }
    
        if(btn == 1) {   // Player object jump and led1 light up if button was pushed
            printf("Jump!\n");
            led1 = 1;
            y = 70;
        }
        else{            // if button not push the Player object stay at initial position and led1 wont light up
            led1 = 0;
            y = 165;
        }
}

void display_LCD(){ // print game data(score,speed,timer) in lcd display
    
        lcd.locate(0,0);
        lcd.cls();
        lcd.printf("Score = %d\n",score);
        lcd.printf("Speed = %d\n",spd);
        lcd.printf("Timer: %ds\n", timer);
}

void play_tone(float frequency, float volume, float interval, int rest) { // speaker
    
        speaker.period(1.0 / frequency); // set diff pitch
        speaker = volume;                // set volume
        wait(interval);                  // time pause before the next beep
        speaker = 0.0;                    
        wait(rest);                      // rest time 
}

void gameOver(){ // to indicate the game end
    
        x2 = 25;                                // stop obstacle
        led2 = !led2;                           // blinking of led2
        timer = 0;                              // timer set to zero
        
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
        BSP_LCD_SetFont(&Font20);                                               // set font size
        BSP_LCD_DisplayStringAt(0, 15, (uint8_t *)"GOOD GAME ^^", CENTER_MODE); // print on the touchscreen
        
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Game Over!!!\n");           // print game over on lcd display
        lcd.printf("Score = %d\n",score);       // print the score obtain
        
        //t1.detach ();
        lcd.printf("Timer: %ds\n", timer);
        
        play_tone(300.0, 0.1, 0.5, 0);          // to play the speaker   
        play_tone(300.0, 0.1, 0.5, 0);          
}

void gameCore(){ // set conditions of game over and checkpoint
    
        if((x2 <= 15) && (x2 >= 0) && (btn != 1)){  // to check if player object collide with the obstacle 
                gameOver();                         // if so call gameOver function
        }
        else if(spd > 0){
                score += 5;                         // increment of score if no collision happen  
        }
        
        if(score == checkpoint){                    // condition for speaker to beep every time it reach a checkpoint 
                play_tone(300.0, 0.1, 0.5, 0);      // checkpoint start of with 100 and increase by 100 every loop
                checkpoint += 100;
        }
}

void pauseScreen(){  // puase game screen
    
        BSP_LCD_Clear(LCD_COLOR_WHITE);
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
        BSP_LCD_SetFont(&Font20);                                         // set font size
        BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"PAUSE", CENTER_MODE); // dispaly pause on tochscreen
}

void resetGame(){  // to restart the game   
    
        init_Game(); // initialize of variables
}

int main(){
    
        init_Game();                             // initialize of variables
        t1.attach (callback (&countTime), 1.0f); // to include the timer
    
        while(1){
        
            if(Switch == 1){ // if switch on game run
                interval = 1;                    // set timer interval to 1
                refresh_screen();                // refresh the game screen
                spd = ceil(speed.read()*10);     // attach the speed with potentiometer
                gameAction();                    // movement of the game
                display_LCD();                   // print game data(score,speed,time) in lcd display
                gameCore();                      // set conditions of game over, checkpoint and score increment 
            }
            
            else if(Switch == 0){ // if switch off game pause
                interval = 0;                    // set timer interval to 0
                pauseScreen();                   // puase game screen
            }
            
            btn2.fall(callback(&resetGame));     // to restart the game   
            wait_ms(1);
        }
}
