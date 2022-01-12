/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __SETTINGS_H__
#define __SETTINGS_H__ 1

#include "INIReader.h"

struct settings{
  int window_width;
  int window_height;
  int num_samples;
  double inv_num_samples;
  bool progressive_render;
  bool multithreaded;
  bool output_to_file;
  int scene_index;
};

settings ReadSettings(){
  settings settings;
  INIReader reader("../data/settings.ini");

  settings.window_width  = reader.GetInteger("window","width",  800);
  settings.window_height = reader.GetInteger("window","height", 600);
  settings.num_samples = reader.GetInteger("render","samples", 50);
  settings.inv_num_samples = 1.0 / settings.num_samples;
  settings.progressive_render = reader.GetBoolean("render","progressive_render", true);
  settings.multithreaded = reader.GetBoolean("render","multithreaded", false);
  settings.output_to_file = reader.GetBoolean("general","output_to_file", true);
  settings.scene_index = reader.GetInteger("general","scene_index", 0);
  return settings;
}

#endif
