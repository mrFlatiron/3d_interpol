#include "simple_vector.h"

simple_vector::simple_vector()
{
  m_data = nullptr;
  m_size = 0;
}

simple_vector::simple_vector (double *data, const int size)
{
  m_data = data;
  m_size = size;
  m_is_owner = false;
}

simple_vector::simple_vector (const int size)
{
  if (size < 0)
    return;
  m_size = size;
  if (size > 0)
    m_data = new double[size];
  for (int i = 0; i < size; i++)
    m_data[i] = 0;
}

simple_vector::simple_vector (const simple_vector &vect)
{
  m_size = vect.size ();
  if (m_size > 0)
    m_data = new double[m_size];

  for (int i = 0; i < m_size; i++)
    m_data[i] = vect[i];
}

simple_vector &simple_vector::operator= (const simple_vector &vector)
{
  if (m_size < vector.size ())
    {
      delete[] m_data;
      m_data = new double[vector.size ()];
    }
  m_size = vector.size ();
  for (int i = 0; i < m_size; i++)
    m_data[i] = vector[i];

  return *this;
}

simple_vector::~simple_vector()
{
    if (m_data && m_is_owner)
  delete[] m_data;
}

int simple_vector::size () const
{
  return m_size;
}

double &simple_vector::operator[] (const int i) const
{
  return m_data[i];
}

double simple_vector::at (const int i) const
{
  return m_data[i];
}

double *simple_vector::data () const
{
  return m_data;
}

void simple_vector::resize (const int size)
{
  if (!m_is_owner)
    {
      m_size = size;
      return;
    }

  if (size != m_size && size > 0)
    {
      if (m_data)
          delete[] m_data;
      m_data = new double[size];
    }
  m_size = size;
}
