//S-JIS���w�肵�Ă��������B

//#define backmode

#define U3IN1 0
#define U3IN2 1
#define U2IN2 2
#define U2IN1 3

//PA���o�ŉ��������΂����������Ƃ悢�ł��傤�B
#define LS_Right 0
#define LS_Center 1
#define LS_Left 2

//PB�ɑ������l�ł��BV�Ƃ��Ă�̂͂��̏��]�B
#ifndef backmode
#define FORWARD 0b0101
#define BACK	0b1010
#define R_TURN  0b0001
#define R_TURNV 0b1001
#define L_TURN  0b0100
#define L_TURNV 0b0110
#define STOP 	0b0000
#define BRAKE	0b1111
#endif

#define PWM_MAX 254
#define ever (;;)

//�錾�W
int SWPwm[4]={0,0,0,0};

void setup();
void TmWait();
void mode_judge();
int PWM_COUNT=PWM_MAX;
int state[3],old_state[3];	//state�̓��C�����/���̎���/���̑��̏��
int pin[3],old_pin[3];		//
int i,j,k;					//counter���񂷗p


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
  


void main(){
	setup();
	for ever {
		PORTA = FORWARD;
		for(i=0;i<200;i++){
			for(i=0;i<200;i++){
			TmWait();
			}
		}
		PORTA = STOP;
		for(i=0;i<200;i++){
			TmWait();
		}
		PORTA = BACK;
		for(i=0;i<200;i++){
			TmWait();
		}
		PORTA = BRAKE;
		for(i=0;i<200;i++){
			TmWait();
		}


	}
}

}

void setup(){
	OPTION=0b00000010;
	/* 0 PBPU �v���A�b�v���g�p����B
	 * 0 INTEDG ������G�b�W�����o�����荞��
	 * 0 TMR0clocksource �����N���b�N
	 * 0 TMR0�̃C���N�������g�^�C�~���O
	 * 0 �Ղ肷���[��[��TMR0�Ɏg���B
	 * 010 0.8192ms 1/8
	   001 0.4096ms 1/16
	 */
	 	
	 INTCON=0b10101000;
	/* 1 GIE
	 * 0 EE  INT EN
	 * 1 TM0 INT EN
	 * 0 INT INT EN
	 * 1 RB  INT EN
	 * 0 TM0 INT FLAG
	 * 0 INT INT FLAG
	 * 0 RB  INT FLAG
	 */
	 	
	 TRISA=0b00010000;
	/* 0 �\��
	 * 0 �\��
	 * 0 �\��
	 * 1 
	 * 0 17 U3 IN1
	 * 0 18 U3 IN2
	 * 0 02 U2 IN2
	 * 0 01 U2 IN1
	 */
	 PORTA=0;
	 
	 TRISB=0b11111111;
	/* 1 13 ���C���Z���T
	 * 1 12 ���C���Z���T
	 * 1 11 ���C���Z���T
	 * 1 
	 * 1 
	 * 1 
	 * 1 
	 * 1 
	 */
	 PORTB=0;
	 
	 
	 //TIMER
	 CLRWDT();
	 TMR0=0;
	 T0IE=0;
	 T0IF=0;
}
	
void TmWait(void){
	while(!T0IF);
	T0IF = 0;
}

void mode_judge(void){
	//escape and reading
	for(i=0;i<3;i++){
		old_pin[i]=pin[i];
		pin[i]=PORTB & (1<<i);
		old_=state[i]=state[i];
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