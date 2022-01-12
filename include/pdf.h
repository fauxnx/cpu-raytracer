/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __PDF_H__
#define __PDF_H__

#include "onb.h"
#include "random.h"
#include "hittable.h"

class pdf {
 public:
  virtual double value(const vec4& direction) const = 0;
  virtual vec4 generate() const = 0;
};

class cosine_pdf : public pdf {
 public:
  cosine_pdf(const vec4& w) { uvw.build_from_w(w); }
  virtual double value(const vec4& direction) const {
    double cosine = dot(direction.normalized(), uvw.w());
    if (cosine > 0)
      return cosine/double(M_PI);
    else
      return 0;
  }
  virtual vec4 generate() const {
    return uvw.local(random_cosine_direction());
  }

  onb uvw;
};

class hittable_pdf : public pdf {
 public:
  hittable_pdf(hittable *p, const vec4& origin) : ptr(p), o(origin) {}
  virtual double value(const vec4& direction) const {
    return ptr->pdf_value(o, direction);
  }
  virtual vec4 generate() const {
    return ptr->random(o);
  }
  vec4 o;
  hittable *ptr;
};

class mixture_pdf : public pdf {
 public:
  mixture_pdf(pdf *p0, pdf *p1) { p[0] = p0; p[1] = p1; }
  virtual double value(const vec4& direction) const {
    return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
  }
  virtual vec4 generate() const {
    if (random_double() < 0.5)
      return p[0]->generate();
    else
      return p[1]->generate();
  }
  pdf *p[2];
};
#endif
