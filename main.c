#include "lcd/lcd.h"

void Inp_init(void)
{
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

void Adc_init(void) 
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1);
    RCU_CFG0|=(0b10<<14)|(1<<28);
    rcu_periph_clock_enable(RCU_ADC0);
    ADC_CTL1(ADC0)|=ADC_CTL1_ADCON;
}

void IO_init(void)
{
	Inp_init();	// inport init
	Adc_init();	// A/D init
	Lcd_Init();	// LCD init
}

FlagStatus Get_BOOT0SW(void)
{
	return(gpio_input_bit_get(GPIOA, GPIO_PIN_8));
}

uint16_t Get_adc(int ch)
{
    ADC_RSQ2(ADC0)=0;
    ADC_RSQ2(ADC0)=ch;
    ADC_CTL1(ADC0)|=ADC_CTL1_ADCON;
    while(!(ADC_STAT(ADC0)&ADC_STAT_EOC));
    uint16_t ret=ADC_RDATA(ADC0)&0xFFFF;
    ADC_STAT(ADC0)&=~ADC_STAT_EOC;
    return(ret);
}

/*************************************************************************************
 0123456789012345678
0
1
2
3
4
**************************************************************************************/
int dir;
int men;
int man_x, man_y;
int box_e;
#define dir_max 4
#define men_x_max 19
#define men_y_max  5
#define men_max 7
const u8 sokomen[men_max][men_y_max][men_x_max]={{
{"#        #    No.0 "},
{"# +      # SokoMan "},
{"#        #         "},
{"#        # egarake "},
{"#        #         "}},{
/*0123456789012345678*/
{"# +    + #    No.1 "},
{"# # ## # # SokoMan "},
{"#   ##   #         "},
{"#        # egarake "},
{"###    ###         "}},{
/*0123456789012345678*/
{"#++ ######    No.2 "},
{"#    ##### SokoMan "},
{"##    ####         "},
{"###    ### egarake "},
{"####    ##         "}},{
/*0123456789012345678*/
{"##    ###     No.3 "},
{"##    ###  SokoMan "},
{"##    ###          "},
{"##     ##  egarake "},
{"##+++++##          "}},{
/*0123456789012345678*/
{"#   +   ##    No.4 "},
{"# #+ +# ## SokoMan "},
{"#   #   ##         "},
{"##    #### egarake "},
{"##########         "}},{
/*0123456789012345678*/
{"#  +  + ##    No.5 "},
{"#+  +   ## SokoMan "},
{"##     ###         "},
{"##    +### egarake "},
{"###   ####         "}},{
/*0123456789012345678*/
{"#    ### + #  No.6 "},
{"# # ###   +# Soko  "},
{"#         +#   Man "},
{"#######   +# egara "},
{"#######  + #    ke "}}};
const u8 sokomov[men_max][men_y_max][men_x_max]={{
{"                   "},
{"                   "},
{"                   "},
{"       @           "},
{"        V          "}},{
/*0123456789012345678*/
{"                   "},
{"                   "},
{"                   "},
{"   @  @            "},
{"     V             "}},{
/*0123456789012345678*/
{"                   "},
{"                   "},
{"                   "},
{"    @@             "},
{"     V             "}},{
/*0123456789012345678*/
{"                   "},
{"    @              "},
{"   @V@             "},
{"    @@             "},
{"                   "}},{
/*0123456789012345678*/
{"                   "},
{"    @              "},
{"  @ V @            "},
{"                   "},
{"                   "}},{
/*0123456789012345678*/
{"                   "},
{"  V@ @             "},
{"  @                "},
{"   @@              "},
{"                   "}},{
/*0123456789012345678*/
{"    V              "},
{"         @         "},
{"  @   @  @         "},
{"         @         "},
{"                   "}}};

u8 sokoply[men_y_max][men_x_max]={
{"                   "},
{"                   "},
{"                   "},
{"                   "},
{"                   "}};
/*0123456789012345678*/


void Draw_feild(int y, int x, u8 c, int wm){
	switch(c){
		case ' ' : LCD_ShowChar(x*8, y*16, ' ', wm, WHITE ); break;
		case '|' : LCD_ShowChar(x*8, y*16, '|', wm, WHITE ); break;
		case '<' : LCD_ShowChar(x*8, y*16, '<', wm, YELLOW); break;
		case 'A' : LCD_ShowChar(x*8, y*16, 'A', wm, YELLOW); break;
		case '>' : LCD_ShowChar(x*8, y*16, '>', wm, YELLOW); break;
		case 'V' : LCD_ShowChar(x*8, y*16, 'V', wm, YELLOW); break;
		case '#' : LCD_ShowChar(x*8, y*16, '#', wm, RED   ); break;
		case '@' : LCD_ShowChar(x*8, y*16, '@', wm, GREEN ); break;
		case '+' : LCD_ShowChar(x*8, y*16, '+', wm, BLUE  ); break;
		default  : LCD_ShowChar(x*8, y*16,  c , wm, BLUE  ); break;
	}
}

void Move_man(void){
	switch(dir){
		case  0 : // A
			if(man_y<men_y_max-1){
				switch(sokomen[men][man_y+1][man_x]){
					case '+' :
					case ' ' :
						if(sokoply[man_y+1][man_x]=='@'){
							if(man_y<men_y_max-2){switch(sokomen[men][man_y+2][man_x]){	case '+' :	case ' ' :	if(sokoply[man_y+2][man_x]==' '){sokoply[  man_y][  man_x]=' ';	sokoply[++man_y][  man_x]='A';	sokoply[1+man_y][  man_x]='@';}	break;}}
						}
						else{
							sokoply[  man_y][  man_x]=' ';	sokoply[++man_y][  man_x]='A';
						}
						break;
				}
			}
			break;
		case  1 : // <
			if(man_x<men_x_max-1){
				switch(sokomen[men][man_y][man_x+1]){
					case '+' :
					case ' ' :
						if(sokoply[man_y][man_x+1]=='@'){
							if(man_x<men_x_max-2){switch(sokomen[men][man_y][man_x+2]){	case '+' :	case ' ' :	if(sokoply[man_y][man_x+2]==' '){sokoply[  man_y][  man_x]=' ';	sokoply[  man_y][++man_x]='<';	sokoply[  man_y][1+man_x]='@';}	break;}}
						}
						else{
							sokoply[  man_y][  man_x]=' ';	sokoply[  man_y][++man_x]='<';
						}
						break;
				}
			}
			break;
		case  2 : // V 
			if(man_y>0){
				switch(sokomen[men][man_y-1][man_x]){
					case '+' :
					case ' ' :
						if(sokoply[man_y-1][man_x]=='@'){
							if(man_y>1){switch(sokomen[men][man_y-2][man_x]){	case '+' :	case ' ' :	if(sokoply[man_y-2][man_x]==' '){sokoply[  man_y][  man_x]=' ';	sokoply[--man_y][  man_x]='V';	sokoply[man_y-1][  man_x]='@';}break;}}
						}
						else{
							sokoply[  man_y][  man_x]=' ';	sokoply[--man_y][  man_x]='V';
						}
						break;
				}
			}
			break;
		case  3 : // > 
			if(man_x>0){
				switch(sokomen[men][man_y][man_x-1]){
					case '+' :
					case ' ' :
						if(sokoply[man_y][man_x-1]=='@'){
							if(man_x>1){switch(sokomen[men][man_y][man_x-2]){	case '+' :	case ' ' :	if(sokoply[man_y][man_x-2]==' '){	sokoply[  man_y][  man_x]=' ';	sokoply[  man_y][--man_x]='>';	sokoply[  man_y][man_x-1]='@';}break;}
							}
						}
						else{
							sokoply[  man_y][  man_x]=' ';	sokoply[  man_y][--man_x]='>';
						}
						break;
				}
			}
			break;
		default : 
			break;
	}
}

int Check_end(void){
	int x, y, e=0;
	for(y=0; y<men_y_max; y++){for(x=0; x<men_x_max; x++){if(sokomen[men][y][x]=='+' && sokoply[y][x]=='@'){++e;}}}
	if(box_e==e){
		men=(men+1)%men_max;
		LCD_ShowString( 8*10,16*2, (u8*)"* Clear *", WHITE);
		return 0;
	}
	return 1;
}

void Sele_dir(void){
	dir=((4096/16)-Get_adc(0)/(4096/16))%dir_max;
	switch(dir){
		case  0 : sokoply[man_y][man_x]='A'; break;
		case  1 : sokoply[man_y][man_x]='<'; break;
		case  2 : sokoply[man_y][man_x]='V'; break;
		case  3 : sokoply[man_y][man_x]='>'; break;
		default : sokoply[man_y][man_x]='X'; break;
	}
}

void Disp_soko(void){
	int x, y;
	for(y=0; y<men_y_max; y++){for(x=0; x<men_x_max; x++){
		Draw_feild(y, x, sokomen[men][y][x],0);
		Draw_feild(y, x, sokoply[y][x],1);
	}}
}

void Sett_soko(void){
	int x,y;
	box_e=0;
	for(y=0; y<men_y_max; y++){for(x=0; x<men_x_max; x++){
		sokoply[y][x]=sokomov[men][y][x];
		if(sokomov[men][y][x]=='@'){++box_e;}
		if(sokomov[men][y][x]=='V'){man_x=x; man_y=y;}
	}}
}

void Sele_men(void){
	men=(Get_adc(0)/(4096/men_max))%men_max;
}

/* Longan Nano SokoBan */
int main( void ) 
{
	IO_init();	// IO init

	men=0;
	while(1){
		BACK_COLOR=BLACK;
		LCD_Clear(BLACK);
		do{
			Sele_men();
			Sett_soko();
			Disp_soko();
			delay_1ms(100);
		}while(!Get_BOOT0SW());
		while(Get_BOOT0SW());
		do{
			Sele_dir();
			Disp_soko();
			if(Get_BOOT0SW()){
				Move_man();
				Disp_soko();
				while(Get_BOOT0SW());
			}
		}while(Check_end());
		while(!Get_BOOT0SW());
		while(Get_BOOT0SW());
	}
}
