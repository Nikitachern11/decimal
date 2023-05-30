
#include "s21_decimal.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  int tmp = -1;
  int mod = 0;
  s21_set_zerro_decimal(result);
  int scale_1 = s21_get_scale(value_1);
  int scale_2 = s21_get_scale(value_2);
  if (scale_1 < scale_2) {
    s21_decimal tmp = value_1;
    value_1 = value_2;
    value_2 = tmp;
    scale_1 = s21_get_scale(value_1);
    scale_2 = s21_get_scale(value_2);
  }
  while (scale_1 > scale_2) {
    if (s21_up_scale(&value_2) == 1) {
      break;
    }
    scale_2 = s21_get_scale(value_2);
  }
  while (scale_1 > scale_2) {
    mod = s21_down_scale(&value_1);
    if (mod) {
      tmp++;
    }
    scale_1 = s21_get_scale(value_1);
  }
  if (mod > 5 || (mod == 5 && (s21_get_bit(value_1, 0) == 1 || tmp > 0))) {
    s21_decimal value_1_copy = value_1;
    s21_decimal one;
    s21_set_zerro_decimal(&one);
    s21_set_bit(&one, 0);
    s21_mantissa_add(value_1_copy, one, &value_1);
    value_1.bits[3] = value_1_copy.bits[3];
  }
  if (s21_get_bit(value_1, 127) && s21_get_bit(value_2, 127)) {
    while (s21_mantissa_add(value_1, value_2, result) && scale_1 > 0) {
      s21_down_scale(&value_1);
      s21_down_scale(&value_2);
      scale_1 = s21_get_scale(value_1);
      scale_2 = s21_get_scale(value_2);
    }
    if (s21_mantissa_add(value_1, value_2, result)) {
      ret = 2;
    } else {
      s21_set_scale(result, scale_1);
      s21_set_bit(result, 127);
    }
  } else if (!s21_get_bit(value_1, 127) && !s21_get_bit(value_2, 127)) {
    while (s21_mantissa_add(value_1, value_2, result) && scale_1 > 0) {
      s21_down_scale(&value_1);
      s21_down_scale(&value_2);
      scale_1 = s21_get_scale(value_1);
      scale_2 = s21_get_scale(value_2);
    }
    if (s21_mantissa_add(value_1, value_2, result)) {
      ret = 1;
    } else {
      s21_set_scale(result, scale_1);
    }
  } else if (s21_mantissa_campare(value_1, value_2) == 1 &&
             s21_get_bit(value_1, 127) == 0) {
    s21_invert(&value_2);
    s21_mantissa_add(value_1, value_2, result);
    s21_set_scale(result, scale_1);
  } else if (s21_mantissa_campare(value_1, value_2) == -1 &&
             s21_get_bit(value_1, 127) == 0) {
    s21_invert(&value_1);
    s21_mantissa_add(value_1, value_2, result);
    s21_set_scale(result, scale_1);
    s21_set_bit(result, 127);
  } else if (s21_mantissa_campare(value_1, value_2) == 1 &&
             s21_get_bit(value_1, 127) == 1) {
    s21_invert(&value_2);
    s21_mantissa_add(value_1, value_2, result);
    s21_set_scale(result, scale_1);
    s21_set_bit(result, 127);
  } else if (s21_mantissa_campare(value_1, value_2) == -1 &&
             s21_get_bit(value_1, 127) == 1) {
    s21_invert(&value_1);
    s21_mantissa_add(value_1, value_2, result);
    s21_set_scale(result, scale_1);
  } else if (s21_mantissa_campare(value_1, value_2) == 0) {
    s21_set_zerro_decimal(result);
  }
  if (ret) s21_set_zerro_decimal(result);
  return ret;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (s21_get_bit(value_2, 127)) {
    value_2.bits[3] = value_2.bits[3] << 1;
    value_2.bits[3] = value_2.bits[3] >> 1;
  } else {
    s21_set_bit(&value_2, 127);
  }
  int ret = s21_add(value_1, value_2, result);
  return ret;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  int mod = 0;
  int tmp = -1;
  if (s21_get_major_index(value_1) == -1 ||
      s21_get_major_index(value_2) == -1) {
    s21_set_zerro_decimal(result);
  } else {
    int scale = s21_get_scale(value_1) + s21_get_scale(value_2);
    s21_set_zerro_decimal(result);
    s21_siximal six_1, six_2, res;
    s21_to_siximal(value_1, &six_1);
    s21_to_siximal(value_2, &six_2);
    s21_to_siximal(*result, &res);
    s21_siximal copy_res = res;
    for (int i = 0; i < 192; i++) {
      if (i) {
        s21_six_shift(&six_1, 1);
      }
      if (s21_get_bit_six(six_2, i)) {
        s21_six_add(copy_res, six_1, &res);
        copy_res = res;
      }
    }
    while ((s21_get_six_major_index(res) > 95 && scale) || scale > 28) {
      mod = s21_six_down_scale(&res);
      if (mod) {
        tmp++;
      }
      scale--;
    }
    if (scale && s21_get_six_major_index(res) == -1) {
      ret = 2;
    }
    int negative = 0;
    if (s21_get_bit(value_1, 127) != s21_get_bit(value_2, 127)) {
      negative = 1;
    }
    if (s21_get_six_major_index(res) > 95) {
      ret = negative ? 2 : 1;
    } else {
      if (mod > 5 || (mod == 5 && (s21_get_bit_six(res, 0) == 1 || tmp > 0))) {
        copy_res = res;
        s21_siximal one;
        s21_set_zerro_six(&one);
        s21_set_bit_six(&one, 0);
        s21_six_add(copy_res, one, &res);
      }
      s21_from_siximal(res, result);
      s21_set_scale(result, scale);
      if (negative) {
        s21_set_bit(result, 127);
      }
    }
  }
  return ret;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  if (s21_get_major_index(value_2) == -1) {
    ret = 3;
  } else {
    int result_sign =
        (s21_get_bit(value_1, 127) - s21_get_bit(value_2, 127)) ? 1 : 0;
    int scale = s21_get_scale(value_1) - s21_get_scale(value_2);
    int scale_1 = s21_get_scale(value_1);
    if (scale < 0) {
      scale_1 += scale;
      if (scale_1 <= 0) {
        scale_1 = scale;
      }
    } else if (scale == 0) {
      scale_1 = 0;
    }
    s21_decimal zero;
    s21_set_zerro_decimal(&zero);
    s21_set_zerro_decimal(result);
    s21_decimal tmp_result;
    s21_set_zerro_decimal(&tmp_result);
    value_1.bits[3] = 0;
    value_2.bits[3] = 0;
    s21_decimal tmp;
    s21_set_zerro_decimal(&tmp);
    int index = s21_get_major_index(value_1);
    s21_decimal copy_tmp;
    while (index >= 0) {
      if (s21_get_bit(value_1, index)) {
        s21_set_bit(&tmp, 0);
      }
      if (s21_mantissa_campare(tmp, value_2) != -1) {
        copy_tmp = tmp;
        s21_sub(copy_tmp, value_2, &tmp);
        s21_set_bit(&tmp_result, 0);
      }
      if (index != 0) {
        s21_mantissa_shift(&tmp_result, 1);
        s21_mantissa_shift(&tmp, 1);
      }
      index--;
      if (s21_is_equal(tmp, zero) && index == -1) {
        *result = tmp_result;
      } else if (s21_is_equal(tmp, zero) == 0 && index == -1) {
        if (scale_1 < 28) {
          if (s21_up_scale(&value_1)) {
            *result = tmp_result;
          } else {
            scale_1++;
            s21_set_scale(&value_1, 0);
            s21_set_zerro_decimal(&tmp_result);
            index = s21_get_major_index(value_1);
            s21_set_zerro_decimal(&tmp);
          }
        }
      }
    }
    s21_set_scale(result, scale_1);
    if (scale >= 0) {
      s21_set_scale(result, scale_1 + scale);
      while (s21_get_scale(*result) > 28) {
        s21_down_scale(result);
      }
    }
    while (s21_get_scale(*result) < 0) {
      if (s21_up_scale(result)) {
        ret = result_sign ? 2 : 1;
        break;
      }
    }
    if (result_sign) {
      s21_set_bit(result, 127);
    }
  }
  return ret;
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  s21_set_zerro_decimal(result);
  if (s21_get_major_index(value_2) == -1) {
    ret = 3;
  } else if (s21_get_major_index(value_1) == -1) {
    ret = 0;
  } else {
    int sign = s21_get_bit(value_1, 127);
    value_1.bits[3] = value_1.bits[3] << 1;
    value_1.bits[3] = value_1.bits[3] >> 1;
    value_2.bits[3] = value_2.bits[3] << 1;
    value_2.bits[3] = value_2.bits[3] >> 1;
    s21_decimal tmp = value_1;
    int scale_1 = s21_get_scale(value_1);
    int scale_2 = s21_get_scale(value_2);
    int flag = 0;
    while (scale_1 < scale_2) {
      if (s21_up_scale(&value_1)) {
        flag = 1;
        break;
      }
      scale_1 = s21_get_scale(value_1);
    }
    if (flag) {
      while (scale_2 > 0) {
        s21_down_scale(&value_2);
        scale_2--;
      }
      ret = 0;
      *result = value_2;
    } else {
      while (s21_is_greater_or_equal(tmp, value_2)) {
        s21_sub(tmp, value_2, result);
        tmp = *result;
      }
      *result = tmp;
    }
    if (sign) s21_set_bit(result, 127);
  }
  if (s21_get_major_index(*result) == -1) s21_set_zerro_decimal(result);
  return ret;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int ret = 0;
  int mod = 0;
  if (s21_get_major_index(value_1) == -1) {
    s21_set_zerro_decimal(&value_1);
  }
  if (s21_get_major_index(value_2) == -1) {
    s21_set_zerro_decimal(&value_2);
  }
  if (s21_get_bit(value_1, 127) && !s21_get_bit(value_2, 127)) {
    ret = 1;
  } else if (!s21_get_bit(value_1, 127) && s21_get_bit(value_2, 127)) {
    ret = 0;
  } else {
    int scale_1 = s21_get_scale(value_1);
    int scale_2 = s21_get_scale(value_2);
    int value_revers = 0;
    if (scale_1 > scale_2) {
      s21_decimal copy_value_1 = value_1;
      value_1 = value_2;
      value_2 = copy_value_1;
      scale_1 = s21_get_scale(value_1);
      scale_2 = s21_get_scale(value_2);
      value_revers = 1;
    }
    while (scale_1 < scale_2) {
      if (s21_up_scale(&value_1)) {
        break;
      }
      scale_1 = s21_get_scale(value_1);
    }
    while (scale_1 < scale_2) {
      mod += s21_down_scale(&value_2);
      scale_2 = s21_get_scale(value_2);
    }
    if (s21_mantissa_campare(value_1, value_2) == -1) {
      ret = 1;
    } else if (s21_mantissa_campare(value_1, value_2) == 0) {
      ret = mod ? 1 : 0;
    } else {
      ret = 0;
    }
    if (s21_get_bit(value_1, 127) && s21_get_bit(value_2, 127)) {
      ret = !ret;
    }
    if (value_revers && s21_mantissa_campare(value_1, value_2)) {
      ret = !ret;
    }
  }
  return ret;
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int ret = 0;
  if (s21_is_less(value_1, value_2) || s21_is_equal(value_1, value_2)) {
    ret = 1;
  }
  return ret;
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  int ret = 1;
  if (s21_is_less_or_equal(value_1, value_2)) {
    ret = 0;
  }
  return ret;
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int ret = 0;
  if (!s21_is_less(value_1, value_2)) {
    ret = 1;
  }
  return ret;
}

int s21_mantissa_add(s21_decimal value_1, s21_decimal value_2,
                     s21_decimal *result) {
  int ret = 0;
  int tmp = 0;
  s21_set_zerro_decimal(result);
  int bit1 = 0, bit2 = 0;
  for (int i = 0; i < 96; i++) {
    bit1 = s21_get_bit(value_1, i);
    bit2 = s21_get_bit(value_2, i);
    if (bit1 && bit2) {
      if (tmp) {
        s21_set_bit(result, i);
      } else {
        tmp = 1;
      }
    } else if (bit1 || bit2) {
      if (tmp == 0) {
        s21_set_bit(result, i);
      }
    } else {
      if (tmp) {
        s21_set_bit(result, i);
        tmp = 0;
      }
    }
  }
  if (tmp) {
    ret = 1;
  }
  return ret;
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int ret = 0;
  if (s21_get_major_index(value_1) == -1) {
    s21_set_zerro_decimal(&value_1);
  }
  if (s21_get_major_index(value_2) == -1) {
    s21_set_zerro_decimal(&value_2);
  }
  if (s21_get_bit(value_1, 127) != s21_get_bit(value_2, 127)) {
    ret = 0;
  } else {
    int scale_1 = s21_get_scale(value_1);
    int scale_2 = s21_get_scale(value_2);
    if (scale_1 > scale_2) {
      s21_decimal copy_value_1 = value_1;
      value_1 = value_2;
      value_2 = copy_value_1;
      scale_1 = s21_get_scale(value_1);
      scale_2 = s21_get_scale(value_2);
    }
    int mod = 0;
    while (scale_1 < scale_2) {
      mod += s21_down_scale(&value_2);
      scale_2 = s21_get_scale(value_2);
    }
    if (s21_mantissa_campare(value_1, value_2) == 0) {
      ret = mod ? 0 : 1;
    }
  }
  return ret;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  int ret = s21_is_equal(a, b);
  ret = ret ? 0 : 1;
  return ret;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  s21_set_zerro_decimal(dst);
  if (src < 0) {
    dst->bits[3] = 0b10000000000000000000000000000000;
    src = -src;
  }
  dst->bits[0] = src;
  return 0;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int ret = 0;
  s21_set_zerro_decimal(dst);
  if (fabs(src) > 79228162514264337593543950335. ||
      (fabs(src) < 1e-28 && src != 0) || isnan(src)) {
    ret = 1;
  } else {
    if (src != 0) {
      int sign = 0;
      if (src < 0) {
        sign = 1;
        src = -src;
      }
      char fl[50] = "";
      sprintf(fl, "%.6e", src);
      char mantissa_fl[10] = "";
      char exp_fl[4] = "";
      mantissa_fl[0] = fl[0];
      for (int i = 1; i < 7; i++) {
        mantissa_fl[i] = fl[i + 1];
      }
      if (src >= 1.) {
        int i = 6;
        while (mantissa_fl[i] == '0') {
          mantissa_fl[i] = '\0';
          i--;
        }
      }
      strncpy(exp_fl, fl + 10, 3);
      int exp = atoi(exp_fl);
      int mantissa = atoi(mantissa_fl);
      s21_from_int_to_decimal(mantissa, dst);
      if (sign) s21_set_bit(dst, 127);
      int scale = 0;
      if (src < 1.) {
        scale = exp + (int)strlen(mantissa_fl) - 1;
      } else {
        scale = -(exp - (int)strlen(mantissa_fl) + 1);
      }
      while (src >= 1. && scale < 0) {
        s21_set_scale(dst, 1);
        if (s21_up_scale(dst)) ret = 1;
        scale++;
      }
      while (scale > 28) {
        s21_set_scale(dst, 1);
        int mod = s21_down_scale(dst);
        if (mod) {
          ret = 1;
          break;
        }
        scale--;
      }
      s21_set_scale(dst, scale);
    }
  }
  if (ret) s21_set_zerro_decimal(dst);
  return ret;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  double sum = 0.;
  int ret = s21_from_dec(src, &sum);
  if ((sum < -2147483648) || (sum > 2147483647)) {
    ret = 1;
  } else {
    *dst = (int)sum;
  }
  return ret;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  double sum = 0.;
  int ret = s21_from_dec(src, &sum);
  *dst = (float)sum;
  return ret;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  int mod_flag = 0;
  int ret = 0;
  int mod = 0;
  int negative = s21_get_bit(value, 127);
  int scale = s21_get_scale(value);
  while (scale) {
    mod = s21_down_scale(&value);
    if (mod) mod_flag = 1;
    scale--;
  }
  if (negative && mod_flag) {
    s21_decimal one = {{1, 0, 0, 0}};
    if (s21_sub(value, one, result) != 0) {
      ret = 1;
    }
  } else {
    *result = value;
  }
  return ret;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int ret = 0;
  int mod = 0;
  int negative = s21_get_bit(value, 127);
  int scale = s21_get_scale(value);
  s21_decimal one = {{1, 0, 0, 0}};
  while (scale) {
    mod = s21_down_scale(&value);
    scale--;
  }
  *result = value;
  if (negative) {
    if (mod >= 5) {
      s21_sub(value, one, result);
    }
  } else {
    if (mod >= 5) {
      s21_add(value, one, result);
    }
  }
  return ret;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  while (s21_get_scale(value)) {
    s21_down_scale(&value);
  }
  *result = value;
  return 0;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  *result = value;
  result->bits[3] = value.bits[3] ^ 0b10000000000000000000000000000000;
  return 0;
}

void s21_set_zerro_decimal(s21_decimal *a) {
  for (int i = 0; i < 4; i++) {
    a->bits[i] = 0;
  }
}

int s21_get_exp(float a) {
  unsigned int fl = *(unsigned int *)&a;
  fl = fl << 1;
  fl = fl >> 24;
  int ret = fl - 127;
  return ret;
}

int s21_get_int_bit(unsigned int src, int index) {
  int mask = 1;
  mask = mask << index;
  return !!(src & mask);
}

int s21_get_bit(s21_decimal src, int index) {
  int mask = 1;
  int bits_num = 0;
  while (index >= 32) {
    index = index - 32;
    bits_num++;
  }
  mask = mask << index;
  return !!(src.bits[bits_num] & mask);
}

void s21_set_bit(s21_decimal *dst, int index) {
  int mask = 1;
  int bits_num = 0;
  while (index >= 32) {
    index = index - 32;
    bits_num++;
  }
  mask = mask << index;
  dst->bits[bits_num] |= mask;
}

int s21_check_valid_decimal(s21_decimal src) {
  int ret = 0;
  for (int i = 96; i <= 96 + 15; i++) {
    if (s21_get_bit(src, i)) {
      ret = 1;
      break;
    }
  }
  for (int i = 96 + 24; i <= 96 + 30; i++) {
    if (s21_get_bit(src, i)) {
      ret = 1;
      break;
    }
  }
  return ret;
}

int s21_from_dec(s21_decimal src, double *sum) {
  int ret = 0;
  if (s21_check_valid_decimal(src)) {
    ret = 1;
  } else {
    *sum = 0;
    for (int i = 0; i < 96; i++) {
      if (s21_get_bit(src, i)) {
        *sum = *sum + pow(2, i);
      }
    }
    int scale = 0;
    int mask = 1;
    for (int i = 0; i < 8; i++) {
      if (s21_get_bit(src, i + 96 + 16)) {
        scale = scale | mask;
      }
      mask = mask << 1;
    }
    while (scale) {
      *sum = *sum / 10;
      scale--;
    }
    if (s21_get_bit(src, 127)) {
      *sum = -*sum;
    }
  }
  return ret;
}

int s21_up_scale(s21_decimal *src) {
  int sign = s21_get_bit(*src, 127);
  int ret = 0;
  int scale = s21_get_scale(*src);
  if (scale < 28) {
    for (int i = 95; i >= 93; i--) {
      if (s21_get_bit(*src, i)) {
        ret = 1;
        break;
      }
    }
    if (ret == 0) {
      s21_decimal x8src = *src;
      s21_decimal x2src = *src;
      s21_mantissa_shift(&x8src, 3);
      s21_mantissa_shift(&x2src, 1);
      s21_decimal src_copy = *src;
      if (s21_mantissa_add(x8src, x2src, &src_copy)) {
        ret = 1;
      } else {
        *src = src_copy;
      }
      s21_set_scale(src, scale + 1);
    }
  }
  if (sign) s21_set_bit(src, 127);
  return ret;
}

int s21_down_scale(s21_decimal *src) {
  int scale = s21_get_scale(*src);
  int index = s21_get_major_index(*src);
  int tmp = 0;
  if (scale) {
    s21_decimal result;
    s21_set_zerro_decimal(&result);
    while (index > 0) {
      if (s21_get_bit(*src, index)) {
        tmp |= 1;
      }
      if (tmp >= 10) {
        tmp -= 10;
        s21_set_bit(&result, 0);
      }
      s21_mantissa_shift(&result, 1);
      tmp = tmp << 1;
      index--;
    }
    if (s21_get_bit(*src, index)) {
      tmp |= 1;
    }
    if (tmp >= 10) {
      s21_set_bit(&result, 0);
      tmp -= 10;
    }
    if (s21_get_major_index(result) == -1) {
      s21_set_zerro_decimal(src);
    } else {
      if (s21_get_bit(*src, 127)) {
        s21_set_bit(&result, 127);
      }
      s21_set_scale(&result, scale - 1);
      *src = result;
    }
  }
  return tmp;
}

void s21_mantissa_shift(s21_decimal *src, int pos) {
  int tmp = 0;
  int tmp1 = 0;
  while (pos > 0) {
    for (int i = 0; i < 3; i++) {
      tmp1 = s21_get_bit(*src, (i + 1) * 32 - 1);
      src->bits[i] = src->bits[i] << 1;
      if (tmp) {
        s21_set_bit(src, i * 32);
      }
      tmp = tmp1;
    }
    pos--;
  }
}

int s21_get_scale(s21_decimal src) {
  src.bits[3] = src.bits[3] << 8;
  return src.bits[3] = src.bits[3] >> 24;
}

void s21_set_scale(s21_decimal *dst, int scale) {
  int sign = s21_get_bit(*dst, 127);
  dst->bits[3] = 0;
  if (sign) s21_set_bit(dst, 127);
  for (int i = 0; i < 8; i++) {
    if (s21_get_int_bit(scale, i)) {
      s21_set_bit(dst, 96 + 16 + i);
    }
  }
}

int s21_get_major_index(s21_decimal src) {
  int ret = -1;
  int i = 95;
  while (i >= 0) {
    if (s21_get_bit(src, i)) {
      ret = i;
      break;
    }
    i--;
  }
  return ret;
}

int s21_mantissa_campare(s21_decimal value_1, s21_decimal value_2) {
  int ret = 0;
  for (int i = 95; i >= 0; i--) {
    if (s21_get_bit(value_1, i) != s21_get_bit(value_2, i)) {
      ret = s21_get_bit(value_1, i) ? 1 : -1;
      break;
    }
  }
  return ret;
}

void s21_invert(s21_decimal *src) {
  s21_decimal copy_src = *src;
  for (int i = 0; i < 3; i++) {
    copy_src.bits[i] = ~copy_src.bits[i];
  }
  s21_decimal num1;
  s21_set_zerro_decimal(&num1);
  num1.bits[0] = 1;
  s21_mantissa_add(copy_src, num1, src);
}

void s21_to_siximal(s21_decimal src, s21_siximal *dst) {
  for (int i = 0; i < 3; i++) {
    dst->bits[i] = src.bits[i];
  }
  for (int i = 3; i < 6; i++) {
    dst->bits[i] = 0;
  }
}

void s21_from_siximal(s21_siximal src, s21_decimal *dst) {
  for (int i = 0; i < 3; i++) {
    dst->bits[i] = src.bits[i];
  }
  dst->bits[3] = 0;
}

void s21_set_bit_six(s21_siximal *dst, int index) {
  int mask = 1;
  int bit = 0;
  while (index >= 32) {
    index = index - 32;
    bit++;
  }
  mask = mask << index;
  for (int i = 0; i < 8; i++) {
    dst->bits[bit] = dst->bits[bit] | mask;
  }
}

void s21_six_shift(s21_siximal *src, int pos) {
  int tmp = 0;
  int tmp1 = 0;
  while (pos > 0) {
    for (int i = 0; i < 6; i++) {
      tmp1 = s21_get_bit_six(*src, (i + 1) * 32 - 1);
      src->bits[i] = src->bits[i] << 1;
      if (tmp) {
        s21_set_bit_six(src, i * 32);
      }
      tmp = tmp1;
    }
    pos--;
  }
}

int s21_get_bit_six(s21_siximal src, int index) {
  int mask = 1;
  int bit = 0;
  while (index >= 32) {
    index = index - 32;
    bit++;
  }
  mask = mask << index;
  int ret = src.bits[bit] & mask;
  return ret;
}

void s21_six_add(s21_siximal value_1, s21_siximal value_2,
                 s21_siximal *result) {
  int tmp = 0;
  for (int i = 0; i < 6; i++) {
    result->bits[i] = 0;
  }
  int bit1 = 0, bit2 = 0;
  for (int i = 0; i < 192; i++) {
    bit1 = s21_get_bit_six(value_1, i);
    bit2 = s21_get_bit_six(value_2, i);
    if (bit1 && bit2) {
      if (tmp) {
        s21_set_bit_six(result, i);
      } else {
        tmp = 1;
      }
    } else if (bit1 || bit2) {
      if (tmp == 0) {
        s21_set_bit_six(result, i);
      }
    } else {
      if (tmp) {
        s21_set_bit_six(result, i);
        tmp = 0;
      }
    }
  }
}

int s21_get_six_major_index(s21_siximal src) {
  int ret = -1;
  int i = 191;
  while (i >= 0) {
    if (s21_get_bit_six(src, i)) {
      ret = i;
      break;
    }
    i--;
  }
  return ret;
}

int s21_six_down_scale(s21_siximal *src) {
  int index = s21_get_six_major_index(*src);
  int tmp = 0;
  s21_siximal result;
  s21_set_zerro_six(&result);
  while (index > 0) {
    if (s21_get_bit_six(*src, index)) {
      tmp |= 1;
    }
    if (tmp >= 10) {
      tmp -= 10;
      s21_set_bit_six(&result, 0);
    }
    s21_six_shift(&result, 1);
    tmp = tmp << 1;
    index--;
  }
  if (s21_get_bit_six(*src, index)) {
    tmp |= 1;
  }
  if (tmp >= 10) {
    s21_set_bit_six(&result, 0);
    tmp -= 10;
  }
  if (s21_get_six_major_index(result) == -1) {
    s21_set_zerro_six(src);
  } else {
    *src = result;
  }
  return tmp;
}

void s21_set_zerro_six(s21_siximal *src) {
  for (int i = 0; i < 6; i++) {
    src->bits[i] = 0;
  }
}
