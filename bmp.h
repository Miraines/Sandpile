#include <cstdint>
#include <iostream>
#include <string>
#include <fstream>
#include <memory.h>

#pragma pack(push, 1)

// Заголовок файла
struct BITMAPFILEHEADER {
  uint16_t bfType;        // Тип файла, всегда должен быть 0x4D42 для BMP-файлов
  uint32_t bfSize;        // Размер файла в байтах
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;     // Смещение данных изображения от начала файла
};

// Заголовок изображения
struct BITMAPINFOHEADER {
  uint32_t biSize;          // Размер этой структуры в байтах
  int32_t biWidth;         // Ширина изображения в пикселях
  int32_t biHeight;        // Высота изображения в пикселях
  uint16_t biPlanes;        // Количество цветовых плоскостей, всегда 1
  uint16_t biBitCount;      // Количество бит на пиксель
  uint32_t biCompression;
  uint32_t biSizeImage;     // Размер изображения в байтах
  int32_t biXPelsPerMeter; // Горизонтальное разрешение в пикселях на метр
  int32_t biYPelsPerMeter; // Вертикальное разрешение в пикселях на метр
  uint32_t biClrUsed;       // Количество используемых цветов
  uint32_t biClrImportant;  // Количество "важных" цветов
};

#pragma pack(pop) // Возвращаем исходное значение выравнивания

struct Color {
  unsigned char blue;
  unsigned char green;
  unsigned char red;
  unsigned char reserved;
};

Color palette[5] = {
    {255, 255, 255, 0}, // белый
    {0, 255, 0, 0},    // зеленый
    {0, 255, 255, 0},  // желтый
    {255, 0, 255, 0},  // фиолетовый
    {0, 0, 0, 0},      // черный
};

void CreateBmp(uint64_t** Field, uint16_t height, uint16_t width, std::string& name) {
  std::ofstream image_file(name, std::ios::binary);

  if (!image_file) {
    std::cerr << "Error: Не удалось открыть файл " << name;

    return;
  }

  int rowSize = width / 2 + width % 2; // каждый байт содержит 2 пикселя
  int paddingPerRow = (4 - (rowSize % 4)) % 4; // количество байт для выравнивания
  int totalDataSize = (rowSize + paddingPerRow) * height; // общий размер пиксельных данных с учетом выравнивания

  BITMAPFILEHEADER fileHeader = {0};
  BITMAPINFOHEADER infoHeader = {0};

  memset(&fileHeader, 0, sizeof(fileHeader));
  memset(&infoHeader, 0, sizeof(infoHeader));

  fileHeader.bfType = 0x4D42; // "BM" в ASCII
  fileHeader.bfSize = totalDataSize + sizeof(palette) + sizeof(infoHeader) + sizeof(fileHeader);
  fileHeader.bfOffBits = sizeof(palette) + sizeof(infoHeader) + sizeof(fileHeader);

  infoHeader.biSize = sizeof(infoHeader);
  infoHeader.biWidth = width;
  infoHeader.biHeight = height;
  infoHeader.biPlanes = 1;
  infoHeader.biBitCount = 4;
  infoHeader.biCompression = 0;
  infoHeader.biSizeImage = totalDataSize;
  infoHeader.biXPelsPerMeter = 0;
  infoHeader.biYPelsPerMeter = 0;
  infoHeader.biClrUsed = 5;
  infoHeader.biClrImportant = 5;

  image_file.write((char*)&fileHeader, sizeof(fileHeader));
  image_file.write((char*)&infoHeader, sizeof(infoHeader));
  image_file.write((char*)palette, sizeof(palette));

  unsigned char* pixelData =  new unsigned char[totalDataSize];
  memset(pixelData, 0, totalDataSize);
  unsigned char* currentPixel = pixelData;

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; j += 2) {
      uint64_t first_pixel = std::min((long long) Field[i][j], 4ll);
      uint64_t second_pixel = (j + 1 < width) ? std::min((long long) Field[i][j + 1], 4ll) : 4;
      *currentPixel = static_cast<uint8_t> (first_pixel << 4 | second_pixel);
      ++currentPixel;
    }
    currentPixel += paddingPerRow;
  }

  image_file.write((char*)(pixelData), totalDataSize);
  delete[] pixelData;
  image_file.close();
}