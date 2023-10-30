#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <memory.h>

uint64_t** Field = nullptr;
int16_t height = 0;
int16_t width = 0;

std::tuple<int16_t, int16_t, int16_t, int16_t> FieldSize(const std::string& arg) {
  // Получаем полный путь до потенциального файла
  std::filesystem::path fullPath = std::filesystem::current_path() / arg;
  std::string absolute_path = fullPath.string();

  // Пытаемся открыть файл
  std::ifstream inputFiles(absolute_path);
  if (!(inputFiles.is_open())) {
    std::cerr << "Не удалось открыть tsv файл";
    exit(EXIT_FAILURE);
  }

  std::string line;
  int16_t min_cord_x = INT16_MAX;
  int16_t min_cord_y = INT16_MAX;
  int16_t max_cord_x = 0;
  int16_t max_cord_y = 0;

  while (std::getline(inputFiles, line)) {
    std::stringstream ss(line);

    std::string str_cord_x, str_cord_y;
    std::getline(ss, str_cord_x, '\t');
    std::getline(ss, str_cord_y, '\t');

    // Преобразование строковых представлений в числовые значения
    int16_t current_cord_x = std::stoi(str_cord_x);
    int16_t current_cord_y = std::stoi(str_cord_y);

    // Обновление максимальных и минимальных значений
    if (current_cord_x < min_cord_x) min_cord_x = current_cord_x;
    if (current_cord_y < min_cord_y) min_cord_y = current_cord_y;
    if (current_cord_x > max_cord_x) max_cord_x = current_cord_x;
    if (current_cord_y > max_cord_y) max_cord_y = current_cord_y;
  }

  return {min_cord_x, min_cord_y, max_cord_x, max_cord_y};
}

std::tuple<uint64_t**, int16_t, int16_t> FieldCreate(const std::string& arg) {
  // Получаем максимальные и минимальные координаты из FieldSize
  auto [min_cord_x, min_cord_y, max_cord_x, max_cord_y] = FieldSize(arg);

  width = max_cord_x - min_cord_x + 1;
  height = max_cord_y - min_cord_y + 1;

  Field = new uint64_t* [height];
  for (int i = 0; i < height; ++i) {
    Field[i] = new uint64_t[width]();
  }

  std::ifstream inputFiles(arg);
  std::string line;

  while (std::getline(inputFiles, line)) {
    std::stringstream ss(line);

    std::string str_cord_x, str_cord_y, str_count;
    std::getline(ss, str_cord_x, '\t');
    std::getline(ss, str_cord_y, '\t');
    std::getline(ss, str_count, '\t');

    int x = std::stoi(str_cord_x) - min_cord_x; // учитываем сдвиг
    int y = std::stoi(str_cord_y) - min_cord_y; // учитываем сдвиг
    uint64_t count = std::stoull(str_count);

    Field[y][x] = count;
  }

  return {Field, height, width};
}

std::tuple<uint64_t**, int16_t, int16_t> ExpandField(uint64_t** oldField, int16_t oldHeight, int16_t oldWidth) {
  int16_t newHeight = oldHeight + 2;
  int16_t newWidth = oldWidth + 2;
  uint64_t** newField = new uint64_t* [newHeight];

  for (int16_t i = 0; i < newHeight; ++i) {
    newField[i] = new uint64_t[newWidth]();
  }

  for (int16_t i = 0; i < oldHeight; ++i) {
    for (int16_t j = 0; j < oldWidth; ++j) {
      newField[i + 1][j + 1] = oldField[i][j];
    }
  }

  for (int16_t i = 0; i < oldHeight; ++i) {
    delete[] oldField[i];
  }
  delete[] oldField;

  return {newField, newHeight, newWidth};
}

bool PileCollapse(int maxIterations) {
  int currentIteration = 0;
  bool hasChanges = false; // флаг для отслеживания изменений

  int16_t** changes = new int16_t* [height];
  for (int16_t i = 0; i < height; ++i) {
    changes[i] = new int16_t[width];
    memset(changes[i], 0, sizeof(int16_t) * width);
  }

  while (currentIteration < maxIterations) {
    hasChanges = false;
    for (int16_t i = 0; i < height; ++i) {
      for (int16_t j = 0; j < width; ++j) {
        if (Field[i][j] >= 4) {
          hasChanges = true;
          if (i - 1 < 0 || i + 1 >= height || j - 1 < 0 || j + 1 >= width) {
            std::tie(Field, height, width) = ExpandField(Field, height, width);
            // Также увеличим размер массива изменений после расширения поля
            int16_t** newChanges = new int16_t* [height];
            for (int16_t x = 0; x < height; ++x) {
              newChanges[x] = new int16_t[width];
              memset(newChanges[x], 0, sizeof(int16_t) * width);
            }
            for (int16_t x = 0; x < i + 1; ++x) {
              delete[] changes[x];
            }
            delete[] changes;
            changes = newChanges;
            continue;
          }
          changes[i][j] -= 4;
          changes[i - 1][j]++;
          changes[i + 1][j]++;
          changes[i][j - 1]++;
          changes[i][j + 1]++;
        }
      }
    }
    if (!hasChanges) {  // если изменений не было, выходим из цикла
      break;
    }
    for (int16_t i = 0; i < height; ++i) {
      for (int16_t j = 0; j < width; ++j) {
        Field[i][j] += changes[i][j];
        changes[i][j] = 0;  // обнуляем изменения для следующей итерации
      }
    }
    currentIteration++;
  }

  // Освобождаем память
  for (int16_t i = 0; i < height; ++i) {
    delete[] changes[i];
  }
  delete[] changes;
  return hasChanges;
}
