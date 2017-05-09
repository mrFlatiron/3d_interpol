#include "least_squares_interpol.h"
#include <cmath>
#include <cstdio>
#include "sparse_matrix/msr_matrix.h"
#include "containers/simple_vector.h"
#include "threads/thread_handler.h"


least_squares_interpol::least_squares_interpol ()
{

}

least_squares_interpol::least_squares_interpol (const double a0,
                                                    const double a1,
                                                    const double b0,
                                                    const double b1, const int m, const int n)
{
  set_ellipse (a0, a1, b0, b1);
  set_partition (m, n);
  m_func = nullptr;
}

least_squares_interpol::~least_squares_interpol ()
{

}

void least_squares_interpol::set_ellipse (const double a0, const double a1, const double b0, const double b1)
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
}

void least_squares_interpol::set_partition (const int m, const int n)
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
  m_jacobian_coef = 2 * M_PI * m_a1 * m_b1;
}

void least_squares_interpol::set_system (msr_matrix &out) const
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


  int ja_iter = func_count + 1;
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

void least_squares_interpol::set_rhs (simple_vector &out, double (*func)(const double, const double))
{
  m_func = func;
  out.resize ((m_n + 1) * (m_m + 1));
  for (int i = 0; i <= m_m; i++)
    for (int j = 0; j <= m_n; j++)
      {
        out[i * (m_n + 1) + j] = rhs_val (i, j);
      }
}

void least_squares_interpol::parallel_set_rhs (thread_handler &handler, simple_vector &shared_out, double (*func)(const double, const double))
{
  int t = handler.t_id ();
  if (t == 0)
    m_func = func;
  handler.barrier_wait ();
  int begin, work;
  handler.divide_work ((m_m + 1) * (m_n + 1), begin, work);

  int i_start = begin / (m_n + 1);
  int j_start = begin - i_start * (m_n + 1);
  int i_end = (begin + work - 1) / (m_n + 1);
  int j_end = (begin + work - 1 - i_end * (m_n + 1));

  for (int i = i_start; i <= i_end; i++)
    for (int j = j_start; j<= j_end; j++)
      shared_out[i * (m_n + 1) + j] = rhs_val (i, j);
  handler.barrier_wait ();
}

void least_squares_interpol::map_to_phir (const double x, const double y, double &phi, double &r) const
{
  double srt = sqrt (x * x / m_a1 / m_a1 + y * y / m_b1 / m_b1);
  r = (m_c * srt - 1) / (m_c - 1);
  double phi0 = acos (x / (m_a1 * srt));
  if (y < 0)
    phi0 = 2 * M_PI - phi0;
  phi = phi0 / 2 / M_PI;
}

void least_squares_interpol::map_to_xy (const double phi, const double r, double &x, double &y) const
{
  x = m_a1 * ((m_c - 1)/m_c * r + 1/m_c) * cos (2 * M_PI * phi);
  y = m_b1 * ((m_c - 1)/m_c * r + 1/m_c) * sin (2 * M_PI * phi);
}

double least_squares_interpol::operator () (const double x, const double y) const
{
  double r, phi;
  map_to_phir (x, y, phi, r);

  return eval_phir (phi, r);
}

double least_squares_interpol::eval_phir (const double phi, const double r) const
{
  int il, ir, jb, jt;
  int i_base, j_base;

  int i[3], j[3];
  int ttype[3];

  if (fabs (phi - 1) < 1e-16)
    {
      il = m_m - 1;
      ir = m_m;
    }
  else
    {
      if (fabs (phi) < 1e-16)
        {
          il = 0;
          ir = 1;
        }
      else
        {
          il = (int)floor (phi / m_hphi);
          ir = il + 1;
        }
    }

  if (fabs (r - 1) < 1e-16)
    {
      jb = m_n - 1;
      jt = m_n;
    }
  else
    {
      if (fabs (r) < 1e-16)
        {
          jb = 0;
          jt = 1;
        }
      else
        {
          jb = (int)floor (r / m_hr);
          jt = jb + 1;
        }
    }

  double check_val = (m_hr/m_hphi) * (phi - il * m_hphi) + jb * m_hr - r;
  if (fabs (check_val) < 1e-16)
    {
      i[0] = il;
      j[0] = jb;
      i[1] = ir;
      j[1] = jt;
      ttype[0] = 2;
      ttype[1] = 3;
      return m_expansion_coefs->at (i[0] * (m_n + 1) + j[0]) * triangle_val (il, jt, ttype[0], phi, r) +
          m_expansion_coefs->at (i[1] * (m_n + 1) + j[1]) * triangle_val (il, jt, ttype[1], phi, r);
    }
  else
    {
      if (check_val > 0)
        {
          i[0] = ir;
          j[0] = jb;
          i[1] = il;
          j[1] = jb;
          i[2] = ir;
          j[2] = jt;
          ttype[0] = 1;
          ttype[1] = 3;
          ttype[2] = 2;
          i_base = ir;
          j_base = jb;
        }
      else
        {
          i[0] = il;
          j[0] = jt;
          i[1] = ir;
          j[1] = jt;
          i[2] = il;
          j[2] = jb;
          ttype[0] = 4;
          ttype[1] = 6;
          ttype[2] = 5;
          i_base = il;
          j_base = jt;
        }
    }
  double retval = 0;
  for (int k = 0; k < 3; k++)
    {
      retval += m_expansion_coefs->at (i[k] * (m_n + 1) + j[k]) * triangle_val (i_base, j_base,
                                                                           ttype[k],
                                                                           phi, r);
    }
  return retval;
}

void least_squares_interpol::set_expansion_coefs (const simple_vector *coefs)
{
  m_expansion_coefs = coefs;
}


double least_squares_interpol::diagonal_val (const int i, const int j) const
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

double least_squares_interpol::rhs_val (const int i, const int j) const
{
  if (i == 0 && j == 0)
    {
      double v1 = triangle_integral (i, j + 1, 2);
      double v2 = triangle_integral (i + 1, j, 6);

      return v1 + v2;
    }

  if (i == 0 && j == m_n)
    {
      double v1 = triangle_integral (i, j, 1);
      return v1;
    }

  if (i == m_m && j == 0)
    {
      double v1 = triangle_integral (i, j, 4);
      return v1;
    }

  if (i == m_m && j == m_n)
    {
      double v1 = triangle_integral (i - 1, j, 3);
      double v2 = triangle_integral (i, j - 1, 5);
      return v1 + v2;
    }

  if (i == m_m)
    {
      double v1 = triangle_integral (i, j, 4);
      double v2 = triangle_integral (i -1, j, 3);
      double v3 = triangle_integral (i, j - 1, 5);
      return v1 + v2 + v3;
    }

  if (i == 0)
    {
      double v1 = triangle_integral (i, j + 1, 2);
      double v2 = triangle_integral (i + 1, j, 6);
      double v3 = triangle_integral (i, j, 1);
      return v1 + v2 + v3;
    }

  if (j == 0)
    {
      double v1 = triangle_integral (i, j, 4);
      double v2 = triangle_integral (i, j+1, 2);
      double v3 = triangle_integral (i+1, j, 6);
      return v1 + v2 + v3;
    }

  if (j == m_n)
    {
      double v1 = triangle_integral (i - 1, j, 3);
      double v2 = triangle_integral (i, j - 1, 5);
      double v3 = triangle_integral (i, j, 1);
      return v1 + v2 + v3;
    }

  double v1 = triangle_integral (i, j, 4);
  double v2 = triangle_integral (i, j+1, 2);
  double v3 = triangle_integral (i+1, j, 6);

  double v4 = triangle_integral (i - 1, j, 3);
  double v5 = triangle_integral (i, j - 1, 5);
  double v6 = triangle_integral (i, j, 1);

  return v1 + v2 + v3 + v4 + v5 + v6;
}

int least_squares_interpol::set_row (double *aa_row, int *ja_row, const int i, const int j) const
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

double least_squares_interpol::prime_dot_product (const int _type1, const int _type2, const int j) const
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
          return 2 * m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j - 1) + 5);
        case 2:
          return 2 * m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j - 3) + 5);
        case 3:
          return 2 * m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j - 1) + 5);
        case 4:
          return 2 * m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j + 1) + 5);
        case 5:
          return 2 * m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j + 3) + 5);
        case 6:
          return 2 * m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j + 1) + 5);
        }
    }
  if (type1 == 1)
    {
      if (type2 == 2)
        return m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j - 2) + 5);
      if (type2 == 3)
        return m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j - 1) + 5);
    }
  if (type1 == 2 && type2 == 3)
    return m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j - 2) + 5);
  if (type1 == 4)
    {
      if (type2 == 5)
        return m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j + 2) + 5);
      if (type2 == 6)
        return m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j + 1) + 5);
    }
  if (type1 == 5 && type2 == 6)
    return m_jacobian_coef * m_hr * m_hphi / 120  / m_c * (m_hr * (m_c - 1) * (5 * j + 2) + 5);

  printf ("bad prime dot product function arguments : (%d, %d)\n", _type1, _type2);
  return -1;
}

double least_squares_interpol::fancy_integral (const int k, const int l,
                                                 const double g1, const double g2,
                                                 const double g3) const
{
  double a = m_hphi / 2;
  double b = m_hr / 2;
  double c = m_c;
  double x1, y1, x2, y2, x3, y3;
  map_to_xy (a * k, b * l, x1, y1);
  double f1 = m_func (x1, y1);
  map_to_xy (a * (k + 1), b * l, x2, y2);
  double f2 = m_func (x2, y2);
  map_to_xy (a * k, b * (l - 1), x3, y3);
  double f3 = m_func (x3, y3);

  return m_jacobian_coef * a * b / 120 / c *

        (
         f3*(5*(g2 + 2*g3) + b*(-1 + c)*(-2*(g2 + 3*g3) + 5*(g2 + 2*g3)*l) + g1*(5 + b*(-1 + c)*(-2 + 5*l))) +
         f2*(5*(2*g2 + g3) + b*(-1 + c)*(-2*(g2 + g3) + 5*(2*g2 + g3)*l) + g1*(5 + b*(-1 + c)*(-1 + 5*l))) +
         f1*(5*(g2 + g3) + b*(-1 + c)*(-g2 - 2*g3 + 5*(g2 + g3)*l) + 2*g1*(5 + b*(-1 + c)*(-1 + 5*l)))
        );
}

double least_squares_interpol::turned_fancy_integral (const int k, const int l,
                                                        const double g1, const double g2,
                                                        const double g3) const
{
  double a = m_hphi / 2;
  double b = m_hr / 2;
  double c = m_c;
  double x1, y1, x2, y2, x3, y3;
  map_to_xy (a * k, b * l, x1, y1);
  double f1 = m_func (x1, y1);
  map_to_xy (a * (k - 1), b * l, x2, y2);
  double f2 = m_func (x2, y2);
  map_to_xy (a * k, b * (l + 1), x3, y3);
  double f3 = m_func (x3, y3);

  return m_jacobian_coef * a * b / 120 / c *
       (
        f2*(5*(2*g2 + g3) + b*(-1 + c)*(2*(g2 + g3) + 5*(2*g2 + g3)*l) + g1*(5 + b*(-1 + c)*(1 + 5*l))) +
        f1*(5*(g2 + g3) + b*(-1 + c)*(g2 + 2*g3 + 5*(g2 + g3)*l) + 2*g1*(5 + b*(-1 + c)*(1 + 5*l))) +
        f3*(5*(g2 + 2*g3) + b*(-1 + c)*(2*g2 + 6*g3 + 5*(g2 + 2*g3)*l) + g1*(5 + b*(-1 + c)*(2 + 5*l)))
        );
}

double least_squares_interpol::triangle_val (const int i, const int j, const int triangle_type, const double phi, const double r) const
{
  switch (triangle_type)
    {
    case 1:
      return 1 + (r - j * m_hr) / m_hr - (phi - i * m_hphi)/m_hphi;
    case 2:
      return - (r - j * m_hr)/ m_hr;
    case 3:
      return (phi - i * m_hphi) / m_hphi;
    case 4:
      return 1 - (r - j * m_hr) / m_hr + (phi - i * m_hphi) / m_hphi;
    case 5:
      return (r - j * m_hr) / m_hr;
    case 6:
      return -(phi - i * m_hphi) / m_hphi;
    }
  return 0;
}

double least_squares_interpol::triangle_integral (const int i, const int j, const int triangle_type) const
{
  int k_base = 2 * i;
  int l_base = 2 * j;
  double val = 0;
  switch (triangle_type)
    {
    case 1:
      val += fancy_integral (k_base, l_base, 1, 0.5, 0.5) +
             fancy_integral (k_base + 1, l_base, 0.5, 0, 0) +
             fancy_integral (k_base, l_base - 1, 0.5, 0, 0) +
             turned_fancy_integral (k_base + 1, l_base - 1, 0, 0.5, 0.5);
      break;
    case 2:
      val += fancy_integral (k_base, l_base, 0, 0, 0.5) +
             fancy_integral (k_base + 1, l_base, 0, 0, 0.5) +
             fancy_integral (k_base, l_base - 1, 0.5, 0.5, 1) +
             turned_fancy_integral (k_base + 1, l_base - 1, 0.5, 0.5, 0);
      break;
    case 3:
      val += fancy_integral (k_base, l_base, 0, 0.5, 0) +
             fancy_integral (k_base + 1, l_base, 0.5, 1, 0.5) +
             fancy_integral (k_base, l_base - 1, 0, 0.5, 0) +
             turned_fancy_integral (k_base + 1, l_base - 1, 0.5, 0, 0.5);
      break;
    case 4:
      val += turned_fancy_integral (k_base, l_base, 1, 0.5, 0.5) +
             turned_fancy_integral (k_base - 1, l_base, 0.5, 0, 0) +
             turned_fancy_integral (k_base, l_base + 1, 0.5, 0, 0) +
             fancy_integral (k_base - 1, l_base + 1, 0, 0.5, 0.5);
      break;
    case 5:
      val += turned_fancy_integral (k_base, l_base , 0, 0, 0.5) +
             turned_fancy_integral (k_base - 1, l_base, 0, 0, 0.5) +
             turned_fancy_integral (k_base, l_base + 1, 0.5, 0.5, 1) +
             fancy_integral (k_base - 1, l_base + 1, 0.5, 0.5, 0);
      break;
    case 6:
      val += turned_fancy_integral (k_base, l_base, 0, 0.5, 0) +
             turned_fancy_integral (k_base - 1, l_base, 0.5, 1, 0.5) +
             turned_fancy_integral (k_base, l_base + 1, 0, 0.5, 0) +
             fancy_integral (k_base - 1, l_base + 1, 0.5, 0, 0.5);
      break;
    }

  return val;
}






