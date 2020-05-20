//
// Created by jamie on 20/05/2020.
//

#ifndef MAIN_C_FPODCLASSIC_H
#define MAIN_C_FPODCLASSIC_H

#include "os.h"
#include "audio.h"
#include "bmp.h"
#include "bmp_lcd.h"

void mainGUI();
void musicGUI();
void picturesGUI();
void optionsGUI();
int openSongs();
int openPictures();
int playSong(int);
void startSong();
void stopSong();
int moveSelector(int);
int makeSelection(int);
int updateMusicDisplay(int);
void banner(void);
void loadSong(char[]);
void brightnessUp(void);
void brightnessDown(void);
void showPicture(void);
void read_image_bytes(bmp_data_request*);

//File management
FIL File;                   /* FAT File */
FATFS FatFs;
FIL Audio;
FIL Pic;
DIR dir;
FILINFO fno;

//Used to control menus
uint16_t curSelectorPos;
int position;
int max;
int selEnabled;
int numOfSongs;
int numOfPictures;
int playSong1 = 0;
int imageMode = 0;

//Used for loading songs and images
char songName[10];
char picName[10];
FRESULT res;
bmp_image_loader_state imageState;
void (*choice[21])();
char musicFiles[21][7] = {};
char imageFiles[21][7] = {};

//Used for drawing of menus
rectangle rect = {0, LCDHEIGHT, 20, 23};
rectangle rect2 = {0, LCDHEIGHT, 197, 200};
rectangle rect3 = {0, LCDHEIGHT, 201, LCDWIDTH};
rectangle rect4 = {0, LCDHEIGHT, 24, 196};

uint8_t brightness = 70;

#endif //MAIN_C_FPODCLASSIC_H
