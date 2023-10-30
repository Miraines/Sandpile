#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>

struct ParseResult {
  std::string inputFilePath;
  std::string outputDirPath;
  int maxIter = 0;
  int freq = 0;
};

bool isNumber(const std::string& s) {
  std::stringstream ss(s);
  int n;
  std::string tmp;

  return (ss >> n) && !(ss >> tmp);
}

void parseInput(const std::string& arg, int& i, const int argc, char* argv[], ParseResult& result) {
  if (result.inputFilePath.empty() && i + 1 < argc) {
    result.inputFilePath = argv[++i];
  } else {
    std::cerr << "Произошла ошибка в аргументе -i, --input\n";
    exit(EXIT_FAILURE);
  }
}

void parseOutput(const std::string& arg, int& i, const int argc, char* argv[], ParseResult& result) {
  if (result.outputDirPath.empty() && i + 1 < argc) {
    result.outputDirPath = argv[++i];
  } else {
    std::cerr << "Произошла ошибка в аргументе -o, --output\n";
    exit(EXIT_FAILURE);
  }
}

void parseMaxIter(const std::string& arg, int& i, const int argc, char* argv[], ParseResult& result) {
  if (result.maxIter == 0 && i + 1 < argc && isNumber(argv[i + 1])) {
    result.maxIter = std::stoi(argv[++i]);
  } else {
    std::cerr << "Произошла ошибка в аргументе -m, --max-iter\n";
    exit(EXIT_FAILURE);
  }
}

void parseFreq(const std::string& arg, int& i, const int argc, char* argv[], ParseResult& result) {
  if (result.freq == 0 && i + 1 < argc && isNumber(argv[i + 1])) {
    result.freq = std::stoi(argv[++i]);
  } else {
    std::cerr << "Произошла ошибка в аргументе -f, --freq\n";
    exit(EXIT_FAILURE);
  }
}

ParseResult parseArgs(int argc, char* argv[]) {
  ParseResult result;
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-i" || arg == "--input") {
      parseInput(arg, i, argc, argv, result);
    } else if (arg == "-o" || arg == "--output") {
      parseOutput(arg, i, argc, argv, result);
    } else if (arg == "-m" || arg == "--max-iter") {
      parseMaxIter(arg, i, argc, argv, result);
    } else if (arg == "-f" || arg == "--freq") {
      parseFreq(arg, i, argc, argv, result);
    } else {
      std::cerr << "Неизвестный аргумент: " << arg << "\n";
      exit(EXIT_FAILURE);
    }
  }

  return result;
}