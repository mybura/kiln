#ifndef Style_H
#define Style_H

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

#define BUTTON_EDGE_RADIUS 15

#define COLOR565(r,g,b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

#define COLOR_BACKGROUND_DARK COLOR565(5, 5, 5)
#define COLOR_BACKGROUND_LIGHT COLOR565(200, 200, 200)
#define COLOR_IN_PROGRESS COLOR565(230, 138, 0)
#define COLOR_LABEL_ON_DARK COLOR_BACKGROUND_LIGHT
#define COLOR_LABEL_ON_LIGHT COLOR565(100, 100, 100)
#define COLOR_NORMAL_ON_DARK COLOR_BACKGROUND_LIGHT
#define COLOR_NORMAL_ON_LIGHT COLOR_BACKGROUND_DARK

#define COLOR_BUTTON_TEXT COLOR565(100, 100, 100)
#define COLOR_BUTTON_FACE_ACTION COLOR565(150, 150, 255)
#define COLOR_BUTTON_FACE_DEFAULT COLOR565(190, 190, 190)
#define COLOR_BUTTON_FACE_NEGATIVE COLOR565(255, 90, 90)

#define TEXT_XLARGE 4
#define TEXT_LARGE 3
#define TEXT_MEDIUM 2
#define TEXT_SMALL 1

#define GRID_MARGIN 5

#define GRID_BIG(col,row) CONST_RECT(GRID_MARGIN + (DISPLAY_WIDTH / 2) * col,                                          \
                                     GRID_MARGIN + (DISPLAY_HEIGHT / 3) * row,                                         \
                                     GRID_MARGIN + (DISPLAY_WIDTH / 2) * col + (DISPLAY_WIDTH / 2) - GRID_MARGIN * 3,  \
                                     GRID_MARGIN + (DISPLAY_HEIGHT / 3) * row + (DISPLAY_HEIGHT / 3) - GRID_MARGIN * 3)

#endif


