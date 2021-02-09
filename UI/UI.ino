#include "Common.h" 
#include "Display.h" 
#include "Input.h" 
#include "I2CInterface.h" 

volatile Display Display;
volatile Input Input;
volatile I2CInterface I2CInterface(&Display, &Input);

void setup(void);
void loop(void);

void AddTestInstructions()
{
    GPUInstruction newInstruction;
    newInstruction.Operation = OP_FILLSCREEN;
    newInstruction.Parameter[0] = COLOR_BACKGROUND_DARK;    
    Display.AddInstruction(newInstruction);

    newInstruction.Operation = OP_FILLRECT;
    newInstruction.Parameter[0] = 0;
    newInstruction.Parameter[1] = 0;
    newInstruction.Parameter[2] = DISPLAY_WIDTH;
    newInstruction.Parameter[3] = DISPLAY_HEIGHT / 2;
    newInstruction.Parameter[4] = COLOR_BACKGROUND_DARK;
    Display.AddInstruction(newInstruction);

    newInstruction.Operation = OP_FILLRECT;
    newInstruction.Parameter[0] = 0;
    newInstruction.Parameter[1] = DISPLAY_HEIGHT / 3;
    newInstruction.Parameter[2] = DISPLAY_WIDTH;
    newInstruction.Parameter[3] = 2 * DISPLAY_HEIGHT / 3;
    newInstruction.Parameter[4] = COLOR_BACKGROUND_LIGHT;
    Display.AddInstruction(newInstruction);
    
    newInstruction.Operation = OP_FILLROUNDRECT;
    newInstruction.Parameter[0] = 5;
    newInstruction.Parameter[1] = 5;
    newInstruction.Parameter[2] = 50;
    newInstruction.Parameter[3] = 20;
    newInstruction.Parameter[4] = COLOR_BACKGROUND_LIGHT;
    Display.AddInstruction(newInstruction);

    newInstruction.Operation = OP_PRINTTEXT;
    newInstruction.Parameter[0] = 5;
    newInstruction.Parameter[1] = 50;
    newInstruction.Parameter[2] = 0;
    Display.AddInstruction(newInstruction);    

    newInstruction.Operation = OP_SETTEXTSTYLE;
    newInstruction.Parameter[0] = COLOR_LABEL_ON_DARK;
    newInstruction.Parameter[1] = COLOR_LABEL_ON_DARK;
    newInstruction.Parameter[2] = TEXT_LARGE;
    Display.AddInstruction(newInstruction);    

    newInstruction.Operation = OP_PRINTTEXT;
    newInstruction.Parameter[0] = 50;
    newInstruction.Parameter[1] = 5;
    newInstruction.Parameter[2] = 128;
    Display.AddInstruction(newInstruction);    
    
    newInstruction.Operation = OP_PRINTFLOAT;
    newInstruction.Parameter[0] = 50;
    newInstruction.Parameter[1] = 50;
    newInstruction.ParameterF[1] = freeMemory();
    Display.AddInstruction(newInstruction);    
}

void ClearScreen()
{
    GPUInstruction newInstruction;
    newInstruction.Operation = OP_FILLSCREEN;
    newInstruction.Parameter[0] = COLOR_BACKGROUND_DARK;    
    Display.AddInstruction(newInstruction);
}

void setup(void) {
    Serial.begin(9600);
    Serial.println("LCD/Touch Screen Controller V1 .... Operational!");
    
    Display.Setup();
    Input.Setup();
    I2CInterface.Setup();

    ClearScreen();
}

 void ProcessInput()
 {
    Input.ProcessInput();
 }

void UpdateDisplay()
{
    Display.Draw();
}

void loop(void) 
{ 
    //if (!Display.InstructionsOutstanding()) AddTestInstructions();
    ProcessInput();
    UpdateDisplay();    
}

