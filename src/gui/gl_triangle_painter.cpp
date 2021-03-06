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
    glColorPointer (3, GL_FLOAT, 0, m_colors);

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
  glColor3f (0.0, 0.0, 0.0);
  glVertexPointer( 3, GL_FLOAT, 0, m_vertices);
  glDrawElements (GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, m_indices);
  glDisableClientState(GL_VERTEX_ARRAY);
}
