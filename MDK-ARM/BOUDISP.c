#include "BOUDISP.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#define BG BLACK
#define FG WHITE
#define TG WHITE

void initdisp(void)
	{
ILI9341_Fill_Screen(BLACK);
ILI9341_Set_Rotation(3);
ILI9341_Draw_Text("V TRMS:        V",201,0,FG,1,BG);
ILI9341_Draw_Text("MIN:           V",201,8,FG,1,BG);
ILI9341_Draw_Text("MAX:           V",201,16,FG,1,BG);
ILI9341_Draw_Text("Ef:            V",201,32,FG,1,BG);
ILI9341_Draw_Text("MIN:           V",201,40,FG,1,BG);
ILI9341_Draw_Text("MAX:           V",201,48,FG,1,BG);
ILI9341_Draw_Text("V9:            V",201,64,FG,1,BG);
ILI9341_Draw_Text("MIN:           V",201,72,FG,1,BG);
ILI9341_Draw_Text("MAX:           V",201,80,FG,1,BG);
ILI9341_Draw_Text("THD:           %",201,96,FG,1,BG);
ILI9341_Draw_Text("MIN:           %",201,104,FG,1,BG);
ILI9341_Draw_Text("MAX:           %",201,112,FG,1,BG);
ILI9341_Draw_Text("coupures:      C",201,200,FG,1,BG);
ILI9341_Draw_Text("creux:         C",201,216,FG,1,BG);
ILI9341_Draw_Text("Surtension:    C",201,232,FG,1,BG);
ILI9341_Draw_Text("230V",200,111,FG,1,BG);

ILI9341_Draw_Vertical_Line(200,0,240,FG);
ILI9341_Write_Command(0x33);
ILI9341_Write_Data(0);
ILI9341_Write_Data(10);
ILI9341_Write_Data(0);
ILI9341_Write_Data(190);
ILI9341_Write_Data(0);
ILI9341_Write_Data(120);
ILI9341_Draw_Vertical_Line(1,0,240,BG);
ILI9341_Draw_Horizontal_Line(0,239,200,TG);


}
void updatedisp(uint16_t i){
ILI9341_Write_Command(0x37);
ILI9341_Write_Data(0);
ILI9341_Write_Data(1);
ILI9341_Write_Command(0x2C);
}
