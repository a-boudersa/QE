#include "BOUDISP.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "arm_math.h"                   // ARM::CMSIS:DSP
#include <stdio.h>

#define BG BLACK
#define FG WHITE
#define TG RED

char buffert[10];
		float32_t Vmax=0,Vmin=480,Dfmax=0,Dfmin=480, Afmax=0,Afmin=480,THDmax=0,THDmin=800;
		uint16_t Creux=0,Surt=0,Inter=0;
void initdisp(void)
	{
ILI9341_Fill_Screen(BLACK);
ILI9341_Set_Rotation(1);
ILI9341_Draw_Text("V TRMS:        V",201,0,FG,1,BG);
ILI9341_Draw_Text("MIN:           V",201,8,FG,1,BG);
ILI9341_Draw_Text("MAX:           V",201,16,FG,1,BG);
ILI9341_Draw_Text("Ef:            V",201,32,FG,1,BG);
ILI9341_Draw_Text("MIN:           V",201,40,FG,1,BG);
ILI9341_Draw_Text("MAX:           V",201,48,FG,1,BG);
ILI9341_Draw_Text("V9:            V",201,64,FG,1,BG);
ILI9341_Draw_Text("MIN:           V",201,72,FG,1,BG);
ILI9341_Draw_Text("MAX:           V",201,80,FG,1,BG);
ILI9341_Draw_Text("THD:           %",201,127,FG,1,BG);
ILI9341_Draw_Text("MIN:           %",201,136,FG,1,BG);
ILI9341_Draw_Text("MAX:           %",201,145,FG,1,BG);
ILI9341_Draw_Text("coupures:         C",201,200,FG,1,BG);
ILI9341_Draw_Text("creux:            C",201,216,FG,1,BG);
ILI9341_Draw_Text("Surtension:       C",201,232,FG,1,BG);
ILI9341_Draw_Text("230V",200,111,FG,1,BG);

ILI9341_Draw_Vertical_Line(200,0,240,FG);
ILI9341_Write_Command(0x33);
ILI9341_Write_Data(0);
ILI9341_Write_Data(0);
ILI9341_Write_Data(0);
ILI9341_Write_Data(200);
ILI9341_Write_Data(0);
ILI9341_Write_Data(120);
ILI9341_Draw_Vertical_Line(1,0,240,BG);
ILI9341_Draw_Horizontal_Line(200,120,20,TG);
}

void updategraph(q15_t Vtrms,q15_t THDu,q15_t A9,q15_t THDd,uint16_t i)
	{ 		float32_t V,TH,Dfo,Dfi,Af,Vn;
				uint16_t x;
		arm_q15_to_float(&Vtrms,&V,1);arm_q15_to_float(&THDu,&TH,1);arm_q15_to_float(&THDd,&Dfo,1);arm_q15_to_float(&A9,&Af,1);
// 260Vrms max---+370V==+0.6---617==+1
 Vn=V*6170*3.904; //Af=Af*617*3.0904; 
		Dfi=Dfo*617*3.904;
		if(Vn>Vmax)
			Vmax=Vn;
		if(Vn<Vmin)
			Vmin=Vn;
		if(Vn<23)
			Inter+=10;
				else if(Vn<207)
						Creux+=10;
						else if(Vn>253)
							Surt+=10;
						TH=(TH/Dfo)*100;
						if(Vn==0)
							TH=0;
						if(TH>THDmax)
						THDmax=TH;
						if(TH<THDmin)
						THDmin=TH;
								if(Af>Afmax)
								Afmax=Af;
								if(Af<Afmin)
								Afmin=Af;
									if(Dfi>Dfmax)
									Dfmax=Dfi;
									if(Dfi<Dfmin)
									Dfmin=Dfi;
snprintf(buffert,7,"%5.0f",Vn);
ILI9341_Draw_Text(buffert,257,0,FG,1,BG);
snprintf(buffert,7,"%5.0f",Vmin);
ILI9341_Draw_Text(buffert,257,8,FG,1,BG);						
snprintf(buffert,7,"%5.0f",Vmax);
ILI9341_Draw_Text(buffert,257,16,FG,1,BG);
snprintf(buffert,7,"%05.1f",TH);
ILI9341_Draw_Text(buffert,257,127,FG,1,BG);
snprintf(buffert,7,"%05.1f",THDmin);
ILI9341_Draw_Text(buffert,257,136,FG,1,BG);						
snprintf(buffert,7,"%05.1f",THDmax);
ILI9341_Draw_Text(buffert,257,145,FG,1,BG);
snprintf(buffert,7,"%06.1f",Af);
ILI9341_Draw_Text(buffert,248,64,FG,1,BG);
snprintf(buffert,7,"%06.1f",Afmin);
ILI9341_Draw_Text(buffert,248,72,FG,1,BG);
snprintf(buffert,7,"%06.1f",Afmax);
ILI9341_Draw_Text(buffert,248,80,FG,1,BG);	
snprintf(buffert,7,"%05.1f",Dfi);
ILI9341_Draw_Text(buffert,257,32,FG,1,BG);
snprintf(buffert,7,"%05.1f",Dfmin);
ILI9341_Draw_Text(buffert,257,40,FG,1,BG);
snprintf(buffert,7,"%05.1f",Dfmax);
ILI9341_Draw_Text(buffert,257,48,FG,1,BG);	
									
snprintf(buffert,9,"%06d",Inter);
ILI9341_Draw_Text(buffert,272,200,FG,1,BG);
snprintf(buffert,9,"%06d",Creux);
ILI9341_Draw_Text(buffert,272,216,FG,1,BG);
snprintf(buffert,9,"%06d",Surt);
ILI9341_Draw_Text(buffert,272,232,FG,1,BG);
x=240-(uint16_t) (Vn*0.5217);
ILI9341_Draw_Vertical_Line(0+i%200,0,240,BLACK);
ILI9341_Draw_Pixel(0+i%200,x,TG);


ILI9341_Write_Command(0x37);
ILI9341_Write_Data(0);
ILI9341_Write_Data(i%200);
//ILI9341_Write_Command(0x2C);
}
	// update graph
