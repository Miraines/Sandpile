#include "parsing.h"
#include "field.h"
#include "bmp.h"

int main(int argc, char* argv[]) {
  ParseResult result = parseArgs(argc, argv);
  std::tie(Field, height, width) = FieldCreate(result.inputFilePath);

  for (int i = 0; i <= result.maxIter; ++i) {
    if (!PileCollapse(1)) {
      break;  // завершаем цикл, если не было изменений
    }

    std::string fileName = result.outputDirPath + "/output_iter_" + std::to_string(i) + ".bmp";

    if (result.freq == 0) {
      if (i == result.maxIter) {
        CreateBmp(Field, height, width, fileName);
      }
    } else {
      if (i % result.freq == 0) {
        CreateBmp(Field, height, width, fileName);
      }
    }
  }

  // Очищаем память, выделенную под Field
  for (int i = 0; i < height; ++i) {
    delete[] Field[i];
  }
  delete[] Field;

  return 0;
}
