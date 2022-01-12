/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __TEXTURE_H__
#define __TEXTURE_H__ 1

#include "perlin.h"

class texture {
 public:
  virtual vec4 value(double u, double v, const vec4& p) const = 0;
};

class constant_texture : public texture {
 public:
  constant_texture() {}
  constant_texture(vec4 c) : color(c) {}
  virtual vec4 value(double u, double v, const vec4& p) const {
    return color;
  }
  vec4 color;
};

class checker_texture : public texture {
 public:
  checker_texture() {}
  checker_texture(texture *t0, texture *t1) : even(t0), odd(t1) {}
  virtual vec4 value(double u, double v, const vec4& p) const {
    double sines = sin(10*p.x)*sin(10*p.y)*sin(10*p.z);
    if (sines < 0)
      return odd->value(u,v,p);
    else
      return even->value(u,v,p);
  }
  texture *odd;
  texture *even;
};

class noise_texture : public texture {
 public:
  noise_texture() {}
  noise_texture(double sc) : scale(sc) {}
  virtual vec4 value(double u, double v, const vec4& p) const {
    // return vec3(1,1,1)*0.5*(1 + noise.turb(scale * p));
    // return vec3(1,1,1)*noise.turb(scale * p);
    return vec4(1.0,1.0,1.0)*0.5f*(1.0 + sin(scale*p.z + 10.0*noise.turb(p)));
  }
  perlin noise;
  double scale;
};

class image_texture : public texture {
 public:
  image_texture() {}
  image_texture(uint8_t *pixels, int A, int B)
    : data(pixels), width(A), height(B) {}
  virtual vec4 value(double u, double v, const vec4& p) const;
  uint8_t *data;
  int width, height;
};

vec4 image_texture::value(double u, double v, const vec4& p) const {
  int i = static_cast<int>((  u) * width);
  int j = static_cast<int>((1-v) * height-0.001);
  if (i < 0) i = 0;
  if (j < 0) j = 0;
  if (i > width-1)  i = width-1;
  if (j > height-1) j = height-1;
  double r = int(data[3*i + 3*width*j]  ) / 255.0;
  double g = int(data[3*i + 3*width*j+1]) / 255.0;
  double b = int(data[3*i + 3*width*j+2]) / 255.0;
  return vec4(r, g, b);
}

#endif
