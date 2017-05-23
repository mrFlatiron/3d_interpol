#include "gl_triangle_painter.h"

gl_triangle_painter::gl_triangle_painter ()
{
  m_vertices = nullptr;
  m_vertex_count = 0;
  m_indices = nullptr;
  m_index_count = 0;
  m_colors = nullptr;
  m_data_valid = false;
}

gl_triangle_painter::~gl_triangle_painter()
{

}

void gl_triangle_painter::set_primary_data (const GLfloat *vertices,
                                            const GLuint vertex_count,
                                            const GLuint *indices,
                                            const GLuint index_count,
                                            const GLfloat *colors)
{
  m_vertices = vertices;
  m_vertex_count = vertex_count;
  m_indices = indices;
  m_index_count = index_count;
  m_colors = colors;

//  glGenBuffers(1, &m_vertex_buffer);
//  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
//  glBufferData(GL_ARRAY_BUFFER, m_vertex_count * sizeof (GLfloat), 0, GL_STATIC_DRAW);
//  glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertex_count * sizeof (GLfloat), m_vertices);

//  glGenBuffers(1, &m_index_buffer);
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_count * sizeof (GLuint), 0, GL_STATIC_DRAW);
//  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_index_count * sizeof (GLuint), m_indeces);

//  glGenBuffers (1, &m_color_buffer);
//  glBindBuffer (GL_ARRAY_BUFFER, m_color_buffer);
//  glBufferData (GL_ARRAY_BUFFER, m_vertex_count * sizeof (GLfloat), 0, GL_STATIC_DRAW);

  m_data_valid = true;
}

void gl_triangle_painter::draw_fill () const
{
  glEnableClientState (GL_VERTEX_ARRAY);

  if (m_colors)
    glEnableClientState (GL_COLOR_ARRAY);

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1,1);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glVertexPointer( 3, GL_FLOAT, 0, m_vertices);

  if (m_colors)
    glColorPointer (m_vertex_count, GL_FLOAT, 0, m_colors);

  glDrawElements (GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, m_indices);
  glDisable (GL_POLYGON_OFFSET_FILL);

  if (m_colors)
    glDisableClientState (GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void gl_triangle_painter::draw_grid () const
{
  glEnableClientState (GL_VERTEX_ARRAY);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glLineWidth(1.0f);
  glVertexPointer( 3, GL_FLOAT, 0, m_vertices);
  glDrawElements (GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, m_indices);
  glDisableClientState(GL_VERTEX_ARRAY);
}
