#ifndef _SRC_S21_DECIMAL_H
#define _SRC_S21_DECIMAL_H

#include <limits.h>

#define MAX_UINT_VAL 4294967295

typedef struct {
  int bits[4];
} s21_decimal;

typedef struct {
  int bits[6];
} s21_siximal;

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_is_less(s21_decimal value_1, s21_decimal value_2);

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);

int s21_is_greater(s21_decimal value_1, s21_decimal value_2);

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);

int s21_mantissa_add(s21_decimal value_1, s21_decimal value_2,
                     s21_decimal *result);

int s21_is_equal(s21_decimal value_1, s21_decimal value_2);

int s21_is_not_equal(s21_decimal a, s21_decimal b);

int s21_from_int_to_decimal(int src, s21_decimal *dst);

int s21_from_float_to_decimal(float src, s21_decimal *dst);

int s21_from_decimal_to_int(s21_decimal src, int *dst);

int s21_from_decimal_to_float(s21_decimal src, float *dst);

int s21_floor(s21_decimal value, s21_decimal *result);

int s21_round(s21_decimal value, s21_decimal *result);

int s21_truncate(s21_decimal value, s21_decimal *result);

int s21_negate(s21_decimal value, s21_decimal *result);

void s21_set_zerro_decimal(s21_decimal *a);

int s21_get_exp(float a);

int s21_get_int_bit(unsigned int src, int index);

int s21_get_bit(s21_decimal src, int index);

void s21_set_bit(s21_decimal *dst, int index);

int s21_check_valid_decimal(s21_decimal src);

int s21_from_dec(s21_decimal src, double *sum);

int s21_up_scale(s21_decimal *src);

int s21_down_scale(s21_decimal *src);

void s21_mantissa_shift(s21_decimal *src, int pos);

int s21_get_scale(s21_decimal src);

void s21_set_scale(s21_decimal *dst, int scale);

int s21_get_major_index(s21_decimal src);

int s21_mantissa_campare(s21_decimal value_1, s21_decimal value_2);

void s21_invert(s21_decimal *src);

void s21_to_siximal(s21_decimal src, s21_siximal *dst);

void s21_from_siximal(s21_siximal src, s21_decimal *dst);

void s21_set_bit_six(s21_siximal *dst, int index);

int s21_get_bit_six(s21_siximal src, int index);

void s21_six_shift(s21_siximal *src, int pos);

void s21_six_add(s21_siximal value_1, s21_siximal value_2, s21_siximal *result);

int s21_get_six_major_index(s21_siximal src);

int s21_six_down_scale(s21_siximal *src);

void s21_set_zerro_six(s21_siximal *src);

int s21_div_mantissa(s21_decimal value_1, s21_decimal value_2,
                     s21_decimal *result);

#endif  //  _SRC_S21_DECIMAL_H
