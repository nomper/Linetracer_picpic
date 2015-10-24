#include <16f84a.h>
#fuses HS,PUT,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use fast_io(a)
#use fast_io(b)

//PA感覚で何を書けばいいか書くとよいでしょう。
#define LS_Right 0
#define LS_Center 1
#define LS_Left 2

//PBに代入する値です。Vとついてるのはその場回転。
#define nomalmode
#ifdef nomalmode
#define FORWARD 0b0110	
#define BACK	0b1001
#define R_TURN  0b0010
#define R_TURNV 0b1010
#define L_TURN  0b0100
#define L_TURNV 0b0101
#define STOP 	0b0000
#define BRAKE	0b1111
#else
#define FORWARD 0b1001
#define BACK	0b0110
#define R_TURN  0b1000
#define R_TURNV 0b1010
#define L_TURN  0b0001
#define L_TURNV 0b0101
#define STOP 	0b0000
#define BRAKE	0b1111
#endif

#define PWM_MAX 254
#define ever (;;)

//宣言集
void setup();
int pin,old_pin;		//
int i,j,k;					//counterを回す用


void main(){
	setup();
	for ever{
		output_a(STOP);
		old_pin=pin;
		pin=input_b() >> 5;
		switch (pin){
			//
			case 0b010:
			case 0b111:
				output_a(FORWARD);
				break;
			
			case 0b001:
				output_a(R_TURNV);
				break;
				
			case 0b100:
				output_a(L_TURNV);
				break;
				
			case 0b000:
				if(old_pin == 0b010) output_a(FORWARD);
				else if(old_pin == 0b100) output_a(L_TURN);
				else if(old_pin == 0b001) output_a(R_TURN);
				break;
				
			default :
				//未定
		}
		delay_ms(9);
		output_a(STOP);
		delay_ms(1);
}


void setup(){
	 set_tris_A(0b00010000);
	 output_a(0);

	 set_tris_B(0b11111111);
	 output_b(0);
}
