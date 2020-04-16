#include "includes.h"
#define HEADER_SIZE   256
void read_wav_fileinformation( char *path,char *filename,unsigned char * header);
int str2int(char * str,int * val_p);

void split_8bit_wavedata(char *from,char *to);
void split_12bit_mono_wavedata(char *from,unsigned short int  *to);
void split_12bit_stereo_wavedata(char *from,unsigned short int *left,unsigned short int *right);

void DisableWavePlayerHardware(void);

void GPIO_for_Push_Button_Start(void);

FRESULT scan_files (char* path);
void play_wave_file( char *path,char *filename);
int SD_TotalSize(void);
char path[12]="0:";
void readFileData(char *path,char *filename);
void printFileDDMMYY(FILINFO file);
void DMA2_Channel3_IRQHandler(void);
void Hardware_for_waveplayer_init(void);
void TIM4_Start(void);

typedef struct 
{
  int format;
  int sample_rate;
  int bits_per_sample;
	int num_channels;
	int block_align;
	int byte_rate ;
  int chunk_size;	
	int sub_chunk2_size;
}wave_format;


/*
data
*/

char music_name[NUMBER_OF_MUSIC_FILES][15];
char music_index = 0;
char skip_music = 0;

char buffer[1024*2]={0};
char Left_Channel[512]={0};
char Right_Channel[512]={0};
unsigned short int Left_Channel_16bit[1024] = {0};
unsigned short int Right_Channel_16bit[1024] = {0};
FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc;         /* file objects */   
FRESULT res;
UINT br;
const char *MonthArray[] = {"January","February","March","April","May","June","July","August","September","October","November","December"};
unsigned char Header[HEADER_SIZE]= {0 };	
int Flag_1,Flag_2 = 0 ;
int HTC_1 = 0 ,TC_1 = 0, HTC_2 = 0 ,TC_2 = 0;
int size;
extern wave_format wave_info;
