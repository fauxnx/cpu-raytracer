/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __VEC4_H__
#define __VEC4_H__ 1

#include <iostream>
#define _USE_MATH_DEFINES // needed to get M_PI
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>


__declspec(align(32)) class vec4 {
public:

	vec4();
	vec4(__m256d v);
	vec4(double value);
	vec4(double x, double y, double z = 0, double w = 0);
	vec4(double* values_array);
	vec4(const double* values_array);
	vec4(const vec4& copy);
	~vec4();

	void* operator new(size_t i) { return _mm_malloc(i, 32); }
	void operator delete(void* p) { _mm_free(p); }

	inline const vec4& operator+() const;
	inline vec4  operator+(const vec4& v2) const;
	inline vec4& operator+=(const vec4& v2);
	inline vec4  operator+(double value) const;
	inline vec4& operator+=(double value);

	inline vec4  operator-() const;
	inline vec4  operator-(const vec4& v2) const;
	inline vec4& operator-=(const vec4& v2);
	inline vec4  operator-(double value) const;
	inline vec4& operator-=(double value);

	inline vec4  operator*(const vec4& v2) const;
	inline vec4& operator*=(const vec4& v2);
	inline vec4  operator*(double value) const;
	inline vec4& operator*=(double value);

	inline vec4  operator/(const vec4& v2) const;
	inline vec4& operator/=(const vec4& v2);
	inline vec4  operator/(double value) const;
	inline vec4& operator/=(double value);

	inline bool operator==(const vec4& v2) const;
	inline bool operator!=(const vec4& v2) const;
	inline void operator=(const vec4& v2);
	inline void operator=(double value);

	inline double  operator[](int i) const { return _array[i]; }
	inline double& operator[](int i) { return _array[i]; }

	inline vec4 square_root();

	double length() const;
	double squared_length() const;
	void normalize();
	vec4 normalized() const;
	void make_unit_vector();
	void scale(const vec4& scale);

	static vec4           lerp(const vec4& v1, const vec4& v2, double t);
	static vec4 lerp_unclamped(const vec4& v1, const vec4& v2, double t);

	static const vec4 up;
	static const vec4 down;
	static const vec4 right;
	static const vec4 left;
	static const vec4 forward;
	static const vec4 back;
	static const vec4 zero;
	static const vec4 unit;


	union {
		__m256d _register;
		double _array[4];
		struct {
			double x, y, z, w;
		};
		struct {
			double r, g, b, a;
		};
	};
};

inline vec4::vec4() {
	_register = _mm256_setzero_pd();
}

inline vec4::vec4(__m256d v) {
	_register = v;
}

inline vec4::vec4(double x, double y, double z, double w) {
	_register = _mm256_setr_pd(x, y, z, w);
}

inline vec4::vec4(double* values_array) {
	_register = _mm256_loadu_pd(values_array);
}

inline vec4::vec4(const double* values_array) {
	_register = _mm256_loadu_pd(values_array);
}

inline vec4::vec4(double val) {
	_register = _mm256_set1_pd(val);
}

inline vec4::vec4(const vec4& copy) {
	_register = copy._register;
}

inline vec4::~vec4() {}

inline const vec4& vec4::operator+() const {
	return *this;
}

inline vec4 vec4::operator+(const vec4& v2) const {
	return vec4(_mm256_add_pd(_register, v2._register));
}

inline vec4& vec4::operator+=(const vec4& v2) {
	_register = _mm256_add_pd(_register, v2._register);
	return *this;
}

inline vec4 vec4::operator+(double value) const {
	return vec4(_mm256_add_pd(_register, _mm256_set1_pd(value)));
}

inline vec4& vec4::operator+=(double value) {
	_register = _mm256_add_pd(_register, _mm256_set1_pd(value));
	return *this;
}

inline vec4 vec4::operator-() const {
	return vec4(_mm256_sub_pd(_mm256_setzero_pd(), _register));
}

inline vec4 vec4::operator-(const vec4& v2) const {
	return vec4(_mm256_sub_pd(_register, v2._register));
}

inline vec4& vec4::operator-=(const vec4& v2) {
	_register = _mm256_sub_pd(_register, v2._register);
	return *this;
}

inline vec4 vec4::operator-(double value) const {
	return vec4(_mm256_sub_pd(_register, _mm256_set1_pd(value)));
}

inline vec4& vec4::operator-=(double value) {
	_register = _mm256_sub_pd(_register, _mm256_set1_pd(value));
	return *this;
}

inline vec4 vec4::operator*(const vec4& v2) const {
	return vec4(_mm256_mul_pd(_register, v2._register));
}

inline vec4& vec4::operator*=(const vec4& v2) {
	_register = _mm256_mul_pd(_register, v2._register);
	return *this;
}

inline vec4 vec4::operator*(double value) const {
	return vec4(_mm256_mul_pd(_register, _mm256_set1_pd(value)));
}

inline vec4& vec4::operator*=(double value) {
	_register = _mm256_mul_pd(_register, _mm256_set1_pd(value));
	return *this;
}

inline vec4 vec4::operator/(const vec4& v2) const {
	return vec4(_mm256_div_pd(_register, v2._register));
}

inline vec4& vec4::operator/=(const vec4& v2) {
	_register = _mm256_div_pd(_register, v2._register);
	return *this;
}

inline vec4 vec4::operator/(double value) const {
	return vec4(_mm256_div_pd(_register, _mm256_set1_pd(value)));
}

inline vec4& vec4::operator/=(double value) {
	_register = _mm256_div_pd(_register, _mm256_set1_pd(value));
	return *this;
}

inline bool vec4::operator==(const vec4& v2) const {
	__m256d cmp = _mm256_cmp_pd(_register, v2._register, _CMP_EQ_OQ);
	__m256d tmp = _mm256_hadd_pd(cmp, cmp);
	__m128d sum = _mm_add_pd(_mm256_extractf128_pd(tmp, 0), _mm256_extractf128_pd(tmp, 1));
	double  res = 0.0;
	_mm_store_sd(&res, sum);
	return res == 0.0;
}

inline bool vec4::operator!=(const vec4& v2) const {
	__m256d cmp = _mm256_cmp_pd(_register, v2._register, _CMP_EQ_OQ);
	__m256d tmp = _mm256_hadd_pd(cmp, cmp);
	__m128d sum = _mm_add_pd(_mm256_extractf128_pd(tmp, 0), _mm256_extractf128_pd(tmp, 1));
	double  res = 0.0;
	_mm_store_sd(&res, sum);
	return res != 0.0;
}

inline void vec4::operator=(const vec4& v2) {
	_register = v2._register;
}

inline void vec4::operator=(double value) {
	_register = _mm256_set1_pd(value);
}


inline vec4 vec4::square_root() {
	return vec4(_mm256_sqrt_pd(_register));
}

inline double vec4::length() const {
	return sqrt(squared_length());
}

inline double dot(const vec4& v1, const vec4& v2) {
	__m256d mul = _mm256_mul_pd(v1._register, v2._register);
	__m256d tmp = _mm256_hadd_pd(mul, mul);
	__m128d hi128 = _mm256_extractf128_pd(tmp, 1);
	__m128d lo128 = _mm256_extractf128_pd(tmp, 0);
	__m128d dot = _mm_add_pd(lo128, hi128);
	double  res = 0.0;
	_mm_store_sd(&res, dot);
	return res;
	//return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
}

inline double vec4::squared_length() const {
	return dot(*this, *this);
}

inline void vec4::normalize() {
	*this *= 1.0f / length();
}

inline vec4 vec4::normalized() const {
	return vec4(*this * (1.0f / length()));
}

inline void vec4::make_unit_vector() {
	*this *= 1.0f / length();
}


inline double angle(const vec4& v1, const vec4& v2) {
	return acos(dot(v1, v2) / sqrt(v1.normalized().length() * v2.normalized().length()));
}

inline vec4 cross(const vec4& v1, const vec4& v2) {
	/*__m256d tmp0 = _mm256_shuffle_pd(v2._register, v2._register, _MM_SHUFFLE(3, 0, 2, 1));
	__m256d tmp1 = _mm256_shuffle_pd(v1._register, v1._register, _MM_SHUFFLE(3, 0, 2, 1));
					tmp0 = _mm256_mul_pd(tmp0, v1._register);
					tmp1 = _mm256_mul_pd(tmp1, v2._register);
	__m256d tmp2 = _mm256_sub_pd(tmp0, tmp1);
	return vec3(_mm256_shuffle_pd(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1)));*/
	return vec4(
		v1[1] * v2[2] - v1[2] * v2[1],
		v1[2] * v2[0] - v1[0] * v2[2],
		v1[0] * v2[1] - v1[1] * v2[0]);
}

inline void vec4::scale(const vec4& scale) {
	_register = _mm256_mul_pd(_register, scale._register);
}

inline vec4 vec4::lerp(const vec4& v1, const vec4& v2, double t) {
	if (t < 0.0f) return v1;
	if (t > 1.0f) return v2;
	return lerp_unclamped(v1, v2, t);
}

inline vec4 vec4::lerp_unclamped(const vec4& v1, const vec4& v2, double t) {
	return vec4((v1 * (1.0f - t)) + (v2 * t));
}

inline double distance(const vec4& v1, const vec4& v2) {
	return (v1 - v2).length();
}

inline vec4 reflect(const vec4& v, const vec4& n) {
	return v - n * 2.0f * dot(v, n);
}

#endif
