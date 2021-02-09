#ifndef Buttons_H
#define Buttons_H

#include "Common.h"

class BaseForm;

class Button
{  
    public:
        ValueLogger<bool> Visible;
        const char *Title;
        Rect Bounds;
        uint16_t FaceColor;
        uint16_t TitleColor;
        byte TitleSize;
        std::function<void(void)> Action;
};
 
#endif
