/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __HITTABLE_LIST_H__
#define __HITTABLE_LIST_H__ 1

#include "hittable.h"
#include "random.h"

class hittable_list: public hittable {
 public:
  hittable_list() {}
  hittable_list(hittable **l, int n) {list = l; list_size = n; }
  virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& box) const;
  double pdf_value(const vec4& o, const vec4& v) const;
  vec4 random(const vec4& o) const;

  hittable **list;
  int list_size;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  hit_record temp_rec;
  bool hit_anything = false;
  double closest_so_far = t_max;
  for (int i = 0; i < list_size; i++) {
    if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }
  return hit_anything;
}
bool hittable_list::bounding_box(double t0, double t1, aabb& box) const {
  if (list_size < 1) return false;
  aabb temp_box;
  bool first_true = list[0]->bounding_box(t0, t1, temp_box);
  if (!first_true)
    return false;
  else
    box = temp_box;
  for (int i = 1; i < list_size; ++i) {
    if (list[i]->bounding_box(t0, t1, temp_box)) {
      box = surrounding_box(box, temp_box);
    }
    else {
      return false;
    }
  }
  return true;
}

double hittable_list::pdf_value(const vec4& o, const vec4& v) const {
    double weight = 1.0/list_size;
    double sum = 0;
    for (int i = 0; i < list_size; i++)
        sum += weight*list[i]->pdf_value(o, v);
    return sum;
}

vec4 hittable_list::random(const vec4& o) const {
    int index = int(random_double() * (list_size-1));
    return list[index]->random(o);
}

#endif
