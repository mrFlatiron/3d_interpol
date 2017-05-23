#ifndef GL_TRIANGLE_PAINTER_H
#define GL_TRIANGLE_PAINTER_H

#include <GL/gl.h>
#include <GL/glu.h>

class gl_triangle_painter
{
private:
  const GLfloat *m_vertices;
  GLuint m_vertex_count;
  const GLuint *m_indices;
  GLuint m_index_count;
  const GLfloat *m_colors;
  bool m_data_valid;
  GLuint m_vertex_buffer;
  GLuint m_index_buffer;
  GLuint m_color_buffer;
public:
  gl_triangle_painter ();
  ~gl_triangle_painter ();

  void set_primary_data (const GLfloat *vertices,
                         const GLuint vertex_count,
                         const GLuint *indices,
                         const GLuint index_count,
                         const GLfloat *colors = nullptr);

  void draw_fill () const;
  void draw_grid () const;
};

#endif // GL_TRIANGLE_PAINTER_H
