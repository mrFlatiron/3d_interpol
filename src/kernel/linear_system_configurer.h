#ifndef LINEAR_SYSTEM_CONFIGURER_H
#define LINEAR_SYSTEM_CONFIGURER_H

class msr_matrix;

class linear_system_configurer
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
public:
  linear_system_configurer (const double a0, const double a1, const double b0, const double b1);
  ~linear_system_configurer ();
  void set_partition (const int m, const int n);
  void set_system (msr_matrix &out) const;
private:
  int get_point_type (const int i, const int j) const;
  double diagonal_val (const int i, const int j) const;
  int set_row (double *aa_row /*size 7*/, int *ja_row /*size 7*/, const int i, const int j) const;
  double prime_dot_product (const int product_type1, const int product_type2, const int j) const;
};

#endif // LINEAR_SYSTEM_CONFIGURER_H
