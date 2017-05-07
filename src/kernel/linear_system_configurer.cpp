#include "linear_system_configurer.h"
#include <cmath>
#include <cstdio>
#include "sparse_matrix/msr_matrix.h"


linear_system_configurer::linear_system_configurer (const double a0,
                                                    const double a1,
                                                    const double b0,
                                                    const double b1)
{
  if (fabs (a0) < 1e-15
      || fabs (b0) < 1e-15
      || fabs (a1) < 1e-15
      || fabs (b1) < 1e-15
      || fabs (a1 / a0 - b1 / b0) > 1e-15)
    {
      printf ("bad ellipse configuration\n");
      return;
    }
  m_a0 = a0;
  m_a1 = a1;
  m_b0 = b0;
  m_b1 = b1;

  m_c = m_a1 / m_a0;
  m_n = 0;
  m_m = 0;
  m_hphi = 0;
  m_hr = 0;
  m_jacobian_coef = 0;
}

linear_system_configurer::~linear_system_configurer ()
{

}

void linear_system_configurer::set_partition (const int m, const int n)
{
  if (m <= 0 && n <= 0)
    {
      printf ("bad partition\n");
      return;
    }
  m_m = m;
  m_n = n;
  m_hphi = 1. / m;
  m_hr = 1. / n;
  m_jacobian_coef = 2 * M_PI * m_a1 * m_b1 * m_hr * m_hphi / 120  / m_c;
}

void linear_system_configurer::set_system (msr_matrix &out) const
{
  int func_count = (m_m + 1) * (m_n + 1);
  out.set_n (func_count);
  int arr_size = 0;
  arr_size += 3 * 2;
  arr_size += 4 * 2;
  arr_size += 5 * 2 * (m_m - 1);
  arr_size += 5 * 2 * (m_n - 1);
  arr_size += 7 * (m_m - 1) * (m_n - 1);

  out.set_arr_size (arr_size + 1);

  int row;

  row = 0;

  int ja_iter = func_count + 1;
  double val = 0;
  double aa_row[7];
  int ja_row[7];

  for (int i = 0; i <= m_m; i++)
    for (int j = 0; j <= m_n; j++)
      {
        int k = i * (m_n + 1) + j;
        out.aa (k, diagonal_val (i, j));
        out.ja (k, ja_iter);
        int non_zeros = set_row (aa_row, ja_row, i, j);
        for (int l = ja_iter; l < ja_iter + non_zeros; l++)
          {
            out.aa (l, aa_row[l - ja_iter]);
            out.ja (l, ja_row[l - ja_iter]);
          }
        ja_iter += non_zeros;
      }
  out.ja (func_count, arr_size + 1);
}

int linear_system_configurer::get_point_type (const int i, const int j) const
{
  if (i == 0 && j == 0)
    return 7;

  if (i == 0 && j == m_n)
    return 1;

  if (i == m_m && j == 0)
    return 6;

  if (i == m_m && j == m_n)
    return 2;

  if (i == m_m)
    return 4;

  if (i == 0)
    return 9;

  if (j == 0)
    return 8;

  if (j == m_n)
    return 3;

  return 5;
}

double linear_system_configurer::diagonal_val (const int i, const int j) const
{
  if (i == 0 && j == 0)
    return prime_dot_product (2, 2, 1) + prime_dot_product (6, 6, 0);

  if (i == 0 && j == m_n)
    return prime_dot_product (1, 1, m_n);

  if (i == m_m && j == 0)
    return prime_dot_product (4, 4, 0);

  if (i == m_m && j == m_n)
    return prime_dot_product (3, 3, m_n) + prime_dot_product (5, 5, m_n - 1);

  if (i == m_m)
    return prime_dot_product (4, 4, j) + prime_dot_product (3, 3, j) + prime_dot_product (5, 5, j - 1);

  if (i == 0)
    return prime_dot_product (2, 2, j + 1) + prime_dot_product (6, 6, j) + prime_dot_product (1, 1, j);

  if (j == 0)
    return prime_dot_product (4, 4, 0) + prime_dot_product (2, 2, 1) + prime_dot_product (6, 6, 0);

  if (j == m_n)
    return prime_dot_product (3, 3, m_n) + prime_dot_product (5, 5, m_n - 1) + prime_dot_product (1, 1, m_n);

  return prime_dot_product (4, 4, j) + prime_dot_product (2, 2, j + 1) +
      prime_dot_product (6, 6, j) + prime_dot_product (1, 1, j) + prime_dot_product (5, 5, j - 1) +
      prime_dot_product (3, 3, j);
}

int linear_system_configurer::set_row (double *aa_row, int *ja_row, const int i, const int j) const
{
  if (i == 0 && j == 0)
    {
      aa_row[0] = prime_dot_product (2, 1, 1);
      ja_row[0] = 1;
      aa_row[1] = prime_dot_product (6, 4, 0);
      ja_row[1] = m_n + 1;
      aa_row[2] = prime_dot_product (2, 3, 1) + prime_dot_product (6, 5, 0);
      ja_row[2] = m_n + 2;
      return 3;
    }

  if (i == 0 && j == m_n)
    {
      aa_row[0] = prime_dot_product (1, 2, m_n);
      ja_row[0] = m_n - 1;
      aa_row[1] = prime_dot_product (1, 3, m_n);
      ja_row[1] = m_n + 1 + m_n;
      return 2;
    }

  if (i == m_m && j == 0)
    {
      aa_row[0] = prime_dot_product (4, 6, 0);
      ja_row[0] = (m_m - 1) * (m_n + 1);
      aa_row[1] = prime_dot_product (4, 5, 0);
      ja_row[1] = m_m * (m_n + 1) + 1;
      return 2;
    }

  if (i == m_m && j == m_n)
    {
      aa_row[0] = prime_dot_product (2, 3, m_n) + prime_dot_product (6, 5, m_n - 1);
      ja_row[0] = (m_m - 1) * (m_n + 1) + m_n - 1;
      aa_row[1] = prime_dot_product (1, 3, m_n);
      ja_row[1] = ja_row[0] + 1;
      aa_row[2] = prime_dot_product (5, 4, m_n - 1);
      ja_row[2] = m_m * (m_n + 1) + m_n - 1;
      return 3;
    }

  if (i == m_m)
    {
      aa_row[0] = prime_dot_product (3, 2, j) + prime_dot_product (5, 6, j - 1);
      ja_row[0] = (m_m - 1) * (m_n + 1) + j - 1;
      aa_row[1] = prime_dot_product (4, 6, j) + prime_dot_product (3, 1, j);
      ja_row[1] = ja_row[0] + 1;
      aa_row[2] = prime_dot_product (5, 4, j - 1);
      ja_row[2] = m_m * (m_n + 1) + j - 1;
      aa_row[3] = prime_dot_product (4, 5, j);
      ja_row[3] = ja_row[2] + 2;
      return 4;
    }

  if (i == 0)
    {
      aa_row[0] = prime_dot_product (1, 2, j);
      ja_row[0] = j - 1;
      aa_row[1] = prime_dot_product (2, 1, j + 1);
      ja_row[1] = ja_row[0] + 2;
      aa_row[2] = prime_dot_product (6, 4, j) + prime_dot_product (1, 3, j);
      ja_row[2] = (m_n + 1) + j;
      aa_row[3] = prime_dot_product (2, 3, j + 1) + prime_dot_product (6, 5, j);
      ja_row[3] = ja_row[2] + 1;
      return 4;
    }

  if (j == 0)
    {
      aa_row[0] = prime_dot_product (4, 6, 0);
      ja_row[0] = (i - 1) * (m_n + 1);
      aa_row[1] = prime_dot_product (4, 5, 0) + prime_dot_product (2, 1, 1);
      ja_row[1] = i * (m_n + 1) + 1;
      aa_row[2] = prime_dot_product (6, 4, 0);
      ja_row[2] = (i + 1) * (m_n + 1);
      aa_row[3] = prime_dot_product (2, 3, 1) + prime_dot_product (6, 5, 0);
      ja_row[3] = ja_row[2] + 1;
      return 4;
    }

  if (j == m_n)
    {
      aa_row[0] = prime_dot_product (3, 2, m_n) + prime_dot_product (5, 6, m_n - 1);
      ja_row[0] = (i - 1) * (m_n + 1) + m_n - 1;
      aa_row[1] = prime_dot_product (3, 1, m_n);
      ja_row[1] = ja_row[0] + 1;
      aa_row[2] = prime_dot_product (5, 4, m_n - 1) + prime_dot_product (1, 2, m_n);
      ja_row[2] = i * (m_n + 1) + m_n - 1;
      aa_row[3] = prime_dot_product (1, 3, m_n);
      ja_row[3] = (i + 1) * (m_n + 1) + m_n;
      return 4;
    }

  aa_row[0] = prime_dot_product (3, 2, j) + prime_dot_product (5, 6, j - 1);
  ja_row[0] = (i - 1) * (m_n + 1) + j - 1;
  aa_row[1] = prime_dot_product (4, 6, j) + prime_dot_product (3, 1, j);
  ja_row[1] = ja_row[0] + 1;
  aa_row[2] = prime_dot_product (5, 4, j - 1) + prime_dot_product (1, 2, j);
  ja_row[2] = i * (m_n + 1) + j - 1;
  aa_row[3] = prime_dot_product (4, 5, j) + prime_dot_product (2, 1, j + 1);
  ja_row[3] = ja_row[2] + 2;
  aa_row[4] = prime_dot_product (6, 4, j) + prime_dot_product (1, 3, j);
  ja_row[4] = (i + 1) * (m_n + 1) + j;
  aa_row[5] = prime_dot_product (2, 3, j + 1) + prime_dot_product (6, 5, j);
  ja_row[5] = ja_row[4] + 1;
  return 6;
}

double linear_system_configurer::prime_dot_product (const int _type1, const int _type2, const int j) const
{
  int type1, type2;
  if (_type1 > _type2)
    {
      type1 = _type2;
      type2 = _type1;
    }
  else
    {
      type1 = _type1;
      type2 = _type2;
    }

  if (type1 == type2)
    {
      switch (type1)
        {
        case 1:
          return 2 * m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j - 1) + 5);
        case 2:
          return 2 * m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j - 3) + 5);
        case 3:
          return 2 * m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j - 1) + 5);
        case 4:
          return 2 * m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j + 1) + 5);
        case 5:
          return 2 * m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j + 3) + 5);
        case 6:
          return 2 * m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j + 1) + 5);
        }
    }
  if (type1 == 1)
    {
      if (type2 == 2)
        return m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j - 2) + 5);
      if (type2 == 3)
        return m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j - 1) + 5);
    }
  if (type1 == 2 && type2 == 3)
    return m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j - 2) + 5);
  if (type1 == 4)
    {
      if (type2 == 5)
        return m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j + 2) + 5);
      if (type2 == 6)
        return m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j + 1) + 5);
    }
  if (type1 == 5 && type2 == 6)
    return m_jacobian_coef * (m_hr * (m_c - 1) * (5 * j + 2) + 5);

  printf ("bad prime dot product function arguments : (%d, %d)\n", _type1, _type2);
  return -1;
}






