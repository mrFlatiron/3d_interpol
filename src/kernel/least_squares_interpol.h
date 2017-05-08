#ifndef LEAST_SQUARES_INTERPOL_H
#define LEAST_SQUARES_INTERPOL_H

class msr_matrix;
class simple_vector;
#include <functional>

class least_squares_interpol
{
private:
  double m_a1; //x radius of greater ellipse
  double m_a0; //x radius of smaller ellipse
  double m_b1; //y radius of greater ellipse
  double m_b0; //y radius of smaller ellipse
  //consider new coordinate system (r, phi) (not polar, but somewhat close)
  int m_m;     //parts count to divide r for
  int m_n;     //parts count to divide phi for
  double m_c;  //a1/a0 = b1/b0 = c
  double m_hphi;
  double m_hr;
  double m_jacobian_coef;
  double (*m_func) (const double, const double);
  const simple_vector *m_expansion_coefs;
public:
  least_squares_interpol (const double a0, const double a1,
                          const double b0, const double b1,
                          const int m, const int n);
  ~least_squares_interpol ();
  void set_system (msr_matrix &out) const;
  void set_rhs (simple_vector &out, double (*func) (const double, const double));
  void map_to_phir (const double x, const double y, double &phi, double &r) const;
  void map_to_xy (const double phi, const double r, double &x, double &y) const;
  double operator () (const double x, const double y) const;
  double eval_phir (const double phi, const double r) const;
  void set_expansion_coefs (const simple_vector *coefs);
private:
  void set_partition (const int m, const int n);
  double diagonal_val (const int i, const int j) const;
  double rhs_val (const int i, const int j) const;
  int set_row (double *aa_row /*size 7*/, int *ja_row /*size 7*/, const int i, const int j) const;
  double prime_dot_product (const int product_type1, const int product_type2, const int j) const;
  double fancy_integral (const int k, const int l, const double g1, const double g2, const double g3) const;
  double turned_fancy_integral (const int k, const int l, const double g1, const double g2, const double g3) const;
  double triangle_val (const int i, const int j, const int triangle_type, const double phi, const double r) const;
  double triangle_integral (const int i, const int j, const int triangle_type) const;
};

#endif // LEAST_SQUARES_INTERPOL_H
