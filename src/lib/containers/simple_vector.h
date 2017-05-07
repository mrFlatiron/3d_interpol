#ifndef SIMPLE_VECTOR_H
#define SIMPLE_VECTOR_H


class simple_vector
{
private:
  double *m_data;
  int m_size;
public:
  simple_vector ();
  simple_vector (const int size);
  simple_vector (const simple_vector &vect);
  simple_vector &operator= (const simple_vector &vector);
  ~simple_vector ();

  int size () const;
  double &operator[] (const int i) const;
  double at (const int i) const;
  double *data () const;
  void resize (const int size);
};

#endif // SIMPLE_VECTOR_H
