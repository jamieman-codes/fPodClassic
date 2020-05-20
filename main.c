/* COMP2215 fPod Classic WAV and BMP player
 * Future Plans :
 *  - Add support for long file names
 *  - Add control page
 *  - Mini game
 * */

#include "fPodClassic.h"

int main(void) {
    os_init();
    f_mount(&FatFs, "", 0);
    lcd_brightness(brightness); //set brightness to system default

    //Open files
    numOfSongs = openSongs();
    numOfPictures = openPictures();

    mainGUI();
    os_add_task( moveSelector,   100, 1);
    os_add_task( makeSelection, 100 ,1);
    os_add_task(playSong, 1000, 1);
    os_add_task(updateMusicDisplay, 100, 1);


    sei();
    for(;;);
}



void mainGUI(){
        banner();
        display_string_xy(">" , 1, curSelectorPos);
        display_string_xy("Music", 10, 25);
        display_string_xy("Pictures", 10, 35);
        display_string_xy("Options", 10, 45);
        max = 3;
        choice[0] = musicGUI;
        choice[1] = picturesGUI;
        choice[2] = optionsGUI;
}

void musicGUI(){
    banner();
    int idx = 0;

    display_string_xy(">" , 1, curSelectorPos);
    display_string_xy("Back", 10, 25);

    //Display all song names
    int displayY = 35;
    for(idx = 0 ; idx <= numOfSongs; idx++){
        display_string_xy(musicFiles[idx], 10, displayY);
        displayY += 10;
        choice[idx + 1] = startSong;
    }
    max = numOfSongs + 1;
    choice[0] = mainGUI;
}

int openSongs(){
    int numOfFiles = 0;
    char *token;
    FRESULT res;
    //load all file names into array
    if(( f_opendir(&dir, "/songs")) == FR_OK){
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break; /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {}
            else{
                token = strtok(fno.fname, ".wav");
                strcpy(musicFiles[numOfFiles], token);
                numOfFiles++;
            }
        }
    }
    f_closedir(&dir);
    return numOfFiles;
}

int openPictures(){
    int numOfFiles = 0;
    char *token;
    FRESULT res;

    //load all picture names into an array
    if(( f_opendir(&dir, "/pics")) == FR_OK){
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break; /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {}
            else{
                token = strtok(fno.fname, ".bmp");
                strcpy(imageFiles[numOfFiles], token);
                numOfFiles++;
            }
        }
    }
    f_closedir(&dir);
    return numOfFiles;
}

int updateMusicDisplay(int state){
    //Change filename being shown
    if(audio_isplaying() || playSong1 == 1){
        display_string_xy(songName, 130, 205);
    }
    else{
        fill_rectangle(rect3, BLACK);
    }
    return state;
}

void startSong(){
    //Load song name
    playSong1 = 1;
    strcpy(songName, musicFiles[position - 1]);
    strcat(songName, ".WAV");
}

void stopSong(){
    //Stop playback
    playSong1 = 0;
    audio_close();
}

int playSong(int state){
    if(!audio_isplaying() && (playSong1 == 1)){
            if ((f_chdir("/songs")) == FR_OK) {
                res = f_open(&Audio, songName, FA_READ);
                if (res == FR_OK) {
                    audio_load(&Audio);
                }
            }
    }
    return state;
}

void picturesGUI(){
    banner();
    int idx;
    imageMode = 0;
    selEnabled = 1;

    display_string_xy(">" , 1, curSelectorPos);
    display_string_xy("Back", 10, 25);
    int displayY = 35;
    for(idx = 0 ; idx <= numOfPictures; idx++){
        display_string_xy(imageFiles[idx], 10, displayY);
        displayY += 10;
        choice[idx + 1] = showPicture;
    }
    max = numOfPictures + 1;
    choice[0] = mainGUI;
}

void showPicture(){
    banner();
    selEnabled = 0;
    imageMode = 1;
    strcpy(picName, imageFiles[position - 1]);
    strcat(picName, ".BMP");

    if ((f_chdir("/pics")) == FR_OK) {
        if (res == FR_OK){
            status_t stat = init_bmp(&imageState, read_image_bytes, 4096);
            if(stat == STATUS_OK) {
                rect4.right = imageState.dibHeader.imageWidth;
                rect4.bottom = imageState.dibHeader.imageHeight;
                display_segment_bmp(20, 20, &rect4, &imageState);
            }
        }
    }
    return;
}

void optionsGUI(){
    banner();

    display_string_xy(">" , 1, curSelectorPos);
    display_string_xy("Back", 10, 25);
    display_string_xy("Brightness Up", 10, 35);
    display_string_xy("Brightness Down", 10, 45);
    max = 3;
    choice[0] = mainGUI;
    choice[1] = brightnessUp;
    choice[2] = brightnessDown;

}

void brightnessUp(){
    brightness += 10;
    lcd_brightness(brightness);
}

void brightnessDown(){
    brightness -= 10;
    lcd_brightness(brightness);
}

int moveSelector(int state) {
    if(selEnabled == 1) {
        int result = os_enc_delta();
        if (result != 0) {
            if (result < 0) { //Moved left
                if (position > 0) {
                    position -= 1;
                    rectangle rect = {0, 9, curSelectorPos, curSelectorPos + 10};
                    fill_rectangle(rect, BLACK);
                    curSelectorPos -= 10;
                    display_string_xy(">", 1, curSelectorPos);
                }
            }
            if (result > 0) { //Moved right
                if (position < (max - 1)) {
                    position += 1;
                    rectangle rect = {0, 9, curSelectorPos, curSelectorPos + 10};
                    fill_rectangle(rect, BLACK);
                    curSelectorPos += 10;
                    display_string_xy(">", 1, curSelectorPos);
                }
            }

        }
    }
    return state;
}

int makeSelection(int state){
    if (get_switch_press(_BV(SWC))) {
        (*choice[position])(); //Perform action selected
    }
    if (get_switch_press(_BV(SWS))) {
        stopSong(); //Stop playback
    }
    if (imageMode == 1){
        if (get_switch_press(_BV(SWW))) {
            picturesGUI(); //Exit picture display
        }
    }
    return state;
}

void banner(){
    //Default for every screen
    clear_screen();
    display_string_xy("fPod Classic", 2, 10);
    fill_rectangle(rect, WHITE_SMOKE);
    fill_rectangle(rect2, WHITE_SMOKE);

    curSelectorPos = 25;
    position = 0;
    selEnabled = 1;
}

void read_image_bytes(bmp_data_request * request)
{
    UINT temp = 0;
    f_lseek(&File, request->dataOffset);
    f_read(&File, request->buffer, request->bufferSize, &temp);
}

