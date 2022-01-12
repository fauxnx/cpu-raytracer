#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef __PPM_H__
#define __PPM_H__

class ppm {
 public:
  void init();

  uint16_t height;
  uint16_t width;
  uint16_t max_col_val;
  uint8_t *pix;

  ppm();
  ppm(const char* fname);
  void read(const char* fname);
};

void ppm::init() {
  width = 0;
  height = 0;
  max_col_val = 255;
}
ppm::ppm() {
  init();
}
ppm::ppm(const char* fname) {
  init();
  read(fname);
}
void ppm::read(const char* fname) {
  std::ifstream file(fname, std::ios::in | std::ios::binary);
  if (file.is_open()) {

    std::string line;

    std::getline(file, line);
    if (line != "P6")
      return;

    do {
      std::getline(file, line);
    } while (line[0] == '#');

    std::stringstream dimensions(line);

    dimensions >> width;
    dimensions >> height;

    std::getline(file, line);

    std::stringstream max_val(line);

    max_val >> max_col_val;

    pix = (uint8_t*) malloc(3*width*height);
    char aux;
    for (int i = 0; i < 3*width*height; ++i) {
      file.read(&aux, 1);
      pix[i] = (uint8_t) aux;
    }
  } else {
    std::cout << "ERROR: File not found. " << fname << std::endl;
  }
  file.close();
}

#endif
