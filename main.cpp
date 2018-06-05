#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream.h>

typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef unsigned long LONG;

struct BITMAPFILEHEADER{
  UINT bfType; //Opis formatu pliku. Musi być ‘BM’.
  DWORD bfSize; //Rozmiar pliku BMP w bajtach.
  UINT bfReserved1; //Zarezerwowane. Musi być równe 0.
  UINT bfReserved2; //Zarezerwowane. Musi być równe 0.
  DWORD bfOffBits; //Przesunięcie w bajtach początku danych
}; //obrazu liczone od końca struktury

struct BITMAPINFOHEADER{
 DWORD biSize; //Rozmiar struktury BITMAPINFOHEADER.
 LONG biWidth; //Szerokość bitmapy w pikselach.
 LONG biHeight; //Wysokość bitmapy w pikselach.
 WORD biPlanes; //Ilość płaszczyzn. Musi być 1.
 WORD biBitCount; //Głębia kolorów w bitach na piksel.
 DWORD biCompression; //Rodzaj kompresji (0 – brak).
 DWORD biSizeImage; //Rozmiar obrazu w bajtach. Uwaga może być 0.
 LONG biXPelsPerMeter;//Rozdzielczość pozioma w pikselach na metr.
 LONG biYPelsPerMeter;//Rozdzielczość pionowa w pikselach na metr.
 DWORD biClrUsed; //Ilość używanych kolorów z palety.
 DWORD biClrImportant; //Ilość kolorów z palety niezbędnych do
}; //wyświetlenia obrazu.

struct RGBQUAD{
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
};

void vgaMode(){
  asm{
    mov ax, 0x13
    int 0x10
  }
}
void textMode(){
  asm{
    mov ax, 0x03
    int 0x10
  }
}

void negatyw(unsigned char far* video_memory, unsigned int size){
  for(int i = 0; i < size; i++)
    *(video_memory + i) = ~*(video_memory + i);
}
void rozjasnij(unsigned char far* video_memory, unsigned int size, int jasnosc){
  if(jasnosc <= 255 && jasnosc >= -255){
    for(int i = 0; i < size; i++){
      if(*(video_memory + i) + jasnosc > 255)
        *(video_memory + i) = 255;
      else if(*(video_memory + i) + jasnosc < 0)
        *(video_memory + i) = 0;
      else
        *(video_memory + i) += jasnosc;
    }
  }
}
void kont(unsigned char far* video_memory, unsigned int size, double kont){
    int srednia = 0;
    for(int i = 0; i < size; i++)
      srednia += *(video_memory + i);
    srednia = srednia / size;
    for(int j = 0; j < size; j++){
		int val = kont * (*(video_memory+j) - 128) + 128;
		if(val > 255) val = 255;
		else if(val < 0) val = 0;
		*(video_memory + j) = val;
    }
}
void proguj(unsigned char far* video_memory, unsigned int size, unsigned int prog){
  if(prog <= 255)
    for(int i = 0; i < size; i++)
      if(*(video_memory + i) < prog)
        *(video_memory + i) = 0;
}

int main(){
  unsigned int obrazek, op;
  cout << "Wybierz obrazek: " << endl;
  cout << "1. aero" << endl;
  cout << "2. boat" << endl;
  cout << "3. bridge" << endl;
  cout << "4. lena" << endl;
  cin >> obrazek;
  char* path = "";
  switch(obrazek){
    case 1:
      path = "c:\\cw4\\bmp\\aero.bmp";
      break;
    case 2:
      path = "c:\\cw4\\bmp\\boat.bmp";
      break;
    case 3:
      path = "c:\\cw4\\bmp\\bridge.bmp";
      break;
    case 4:
      path = "c:\\cw4\\bmp\\lena.bmp";
      break;
  }
  cout << "Wybierz opcje: " << endl;
  cout << "1. negatyw" << endl;
  cout << "2. rozjasnianie" << endl;
  cout << "3. kontrast" << endl;
  cout << "4. progowanie" << endl;
  cin >> op;
  int jasnosc = 0;
  double kontrast = 0;
  unsigned int prog = 0;
  switch(op){
    case 2:
      cout << "Podaj jasnosc: ";
      cin >> jasnosc;
      break;
    case 3:
      cout << "Podaj kontrast: ";
      cin >> kontrast;
      break;
    case 4:
      cout << "Podaj prog: ";
      cin >> prog;
  }
  vgaMode();
  unsigned char far* video_memory = (char far*)MK_FP(0xA000, 0x0000);

  FILE *bitmap_file; //Plik bitmapy
  BITMAPFILEHEADER bmfh; //nagłówek nr 1 bitmapy
  BITMAPINFOHEADER bmih; //nagłówek nr 2 bitmapy
  RGBQUAD palette[256];
  unsigned int size;
  size = 320 * 200;

  bitmap_file = fopen(path, "rb");
  fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, bitmap_file);
  fread(&bmih, sizeof(BITMAPINFOHEADER), 1, bitmap_file);
  fread(&palette, sizeof(RGBQUAD), 256, bitmap_file);
  outportb(0x03C8, 0);
  for (int k = 0; k <= 255; k++){
    outp(0x03C9, palette[k].rgbRed * 63 / 256);
    outp(0x03C9, palette[k].rgbGreen * 63 / 256);
    outp(0x03C9, palette[k].rgbBlue * 63 / 256);
  }
  BYTE* buffer = new BYTE[320];
   for(int i = 0; i < 200; i++){
     fread((void*)(buffer), 1, 320, bitmap_file);
     for(int j = 1; j <= 320; j++)
		video_memory[size - (i * 320) - j] = *(buffer + 320 - j);
   }
  fclose(bitmap_file);
  getch();
  switch(op){
    case 1:
      negatyw(video_memory, size);
      break;
    case 2:
      rozjasnij(video_memory, size, jasnosc);
      break;
    case 3:
      kont(video_memory, size, kontrast);
      break;
    case 4:
      proguj(video_memory, size, prog);
      break;
  }
  getch();
  textMode();
  return 0;
}
