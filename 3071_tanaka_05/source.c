#include <16f84a.h>
#fuses HS,PUT,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use fast_io(a)
#use fast_io(b)

//#define backmode

#define U3IN1 0
#define U3IN2 1
#define U2IN2 2
#define U2IN1 3

//PA感覚で何を書けばいいか書くとよいでしょう。
#define LS_Right 0
#define LS_Center 1
#define LS_Left 2

//PBに代入する値です。Vとついてるのはその場回転。
#ifndef backmode
#define FORWARD 0b0110	
#define BACK	0b1001
#define R_TURN  0b0010
#define R_TURNV 0b1010
#define L_TURN  0b0100
#define L_TURNV 0b0101
#define STOP 	0b0000
#define BRAKE	0b1111
#endif

#define PWM_MAX 254
#define ever (;;)

//宣言集
int SWPwm[4]={0,0,0,0};		//unused

void setup();
void TmWait();				//カウンタがフルになるまで待つ
void mode_judge();			//割り込みモード判定
void teiki();
unsigned int PWM_COUNT=PWM_MAX;
unsigned int state[3],old_state[3];	//stateはメイン状態/その時間/その他の状態
int pin[3],old_pin[3];		//
int i,j,k;					//counterを回す用

/*
interrupt warikomi(){
	if(T0IF){
		PWM_COUNT--;
	}
	if(RBIF){
		mode_judge();
		RBIF=0;
	}
	//if while mode judging ,T0 if full.
	T0IF = 0;
}
  */


void main(){
	setup();
	while(1){
		for(i=0;i<3;i++){
			old_pin[i]=pin[i];
			pin[i]=input_b() & (1<<(7-i));
			}

		if(pin[1]==0){output_a(FORWARD);delay_ms(4);j=0;}
		else if((pin[0]==1)&&(pin[2]==0)){output_a(R_TURNV);delay_ms(4);j=1;}
		else if((pin[0]==0)&&(pin[2]==1)){output_a(L_TURNV);delay_ms(4);j=1;}
		else if((pin[0]==0)&&(pin[2]==0)){output_a(BACK);delay_ms(4);}
		else {
				if(j==0) {output_a(FORWARD);}
				else if(j==1){
				output_a(BACK);
				delay_ms(6);
				output_a(L_TURNV);
				delay_ms(5);
				if((old_pin[0]||old_pin[2])){
					//delay_ms(150);
				}
				output_a(BRAKE);
				delay_us(100);
				}
		}
		//if(j==20){output_a(BACK);delay_ms(50);j=1;}
		delay_ms(4);
		output_a(BRAKE);
		delay_us(100);
	}
	}



void setup(){
	//OPTION=0b00000010;
	/* 0 PBPU プルアップを使用する。
	 * 0 INTEDG 立下りエッジを検出し割り込む
	 * 0 TMR0clocksource 内部クロック
	 * 0 TMR0のインクリメントタイミング
	 * 0 ぷりすけーらーをTMR0に使う。
	 * 010 0.8192ms 1/8
	   001 0.4096ms 1/16
	 */
	 	
	 //INTCON=0b10101000;
	/* 1 GIE
	 * 0 EE  INT EN
	 * 1 TM0 INT EN
	 * 0 INT INT EN
	 * 1 RB  INT EN
	 * 0 TM0 INT FLAG
	 * 0 INT INT FLAG
	 * 0 RB  INT FLAG
	 */
	 	
	 set_tris_A(0b00010000);
	 //TRISA=0b00010000;
	/* 0 予約
	 * 0 予約
	 * 0 予約
	 * 1 
	 * 0 17 U3 IN1
	 * 0 18 U3 IN2
	 * 0 02 U2 IN2
	 * 0 01 U2 IN1
	 */
	 //PORTA=0;
	 output_a(0);

	 set_tris_B(0b11111111);
	 //TRISB=0b11111111;
	/* 1 13 ラインセンサ
	 * 1 12 ラインセンサ
	 * 1 11 ラインセンサ
	 * 1 
	 * 1 
	 * 1 
	 * 1 
	 * 1 
	 */
	 //PORTB=0;
	 output_b(0);
	j=2;
}
/*	
void TmWait(void){
	while(!T0IF);
	T0IF = 0;
}
*/

void mode_judge(void){
	//escape and reading
	for(i=0;i<3;i++){
		old_pin[i]=pin[i];
		pin[i]=input_b() & (1<<i);
		//pin[i]=PORTB & (1<<i);
		old_state[i]=state[i];
	}
	
	//mode judge
	if(!pin[LS_Center]){
		state[0] = FORWARD;
		state[2] = STOP;
		if(pin[LS_Left]==pin[LS_Right]){
			// b b b or w b w
			state[1]=PWM_MAX+1;
		}
		else{
			if(!pin[LS_Left]){
				//b b w
				state[0]=L_TURN;
			}
			else{
				//w b b
				state[0]=R_TURN;
			}
			state[1]=PWM_MAX/2;
			state[2]=FORWARD;
		}
	}
	else if(!pin[LS_Left]){
		if(old_state[0]==L_TURN){
			state[0]=L_TURNV;
		}
		else{
			state[0]=L_TURN;
		}
		state[1]=PWM_MAX/2;
		state[2]=STOP;
	}
	else if(!pin[LS_Right]){
		if(old_state[0]==R_TURN){
			state[0]=R_TURNV;
		}
		else{
			state[0]=R_TURN;
		}
		state[1]=PWM_MAX/2;
		state[2]=STOP;
	}
	else{
		state[0]=FORWARD;
		state[1]=PWM_MAX-1;
		state[2]=STOP;
	}
}

void teiki(void){
	//escape and reading
	for(i=0;i<3;i++){
		old_pin[i]=pin[i];
		pin[i]=input_b() & (1<<i);
		//pin[i]=PORTB() & (1<<i);
		old_state[i]=state[i];
	}
	
	//mode judge
	if(!pin[LS_Center]){
		state[0] = FORWARD;
		state[2] = STOP;
		if(pin[LS_Left]==pin[LS_Right]){
			// b b b or w b w
			state[1]=PWM_MAX+1;
		}
		else{
			if(!pin[LS_Left]){
				//b b w
				state[0]=L_TURN;
			}
			else{
				//w b b
				state[0]=R_TURN;
			}
			state[1]=PWM_MAX/2;
			state[2]=FORWARD;
		}
	}
	else if(!pin[LS_Left]){
		if(old_state[0]==L_TURN){
			state[0]=L_TURNV;
		}
		else{
			state[0]=L_TURN;
		}
		state[1]=PWM_MAX/2;
		state[2]=STOP;
	}
	else if(!pin[LS_Right]){
		if(old_state[0]==R_TURN){
			state[0]=R_TURNV;
		}
		else{
			state[0]=R_TURN;
		}
		state[1]=PWM_MAX/2;
		state[2]=STOP;
	}
	else{
		state[0]=FORWARD;
		state[1]=PWM_MAX-1;
		state[2]=STOP;
	}
}
