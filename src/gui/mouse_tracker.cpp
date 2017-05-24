#include "mouse_tracker.h"
#include <QPoint>
#include <QMouseEvent>

mouse_tracker::mouse_tracker ()
{
  m_drag_on = false;
  m_valid = false;
}

mouse_tracker::~mouse_tracker ()
{

}

void mouse_tracker::start_tracking (QMouseEvent *event)
{
  m_drag_on = true;
  m_start_pos = event->pos ();
}

void mouse_tracker::stop_tracking (QMouseEvent *event)
{
  if (!m_drag_on)
    {
      m_valid = false;
      return;
    }

  if (event)
    {
      m_valid = true;
      m_end_pos = event->pos ();
    }

  m_drag_on = false;
  m_valid = false;
}

QPoint mouse_tracker::get_delta (QMouseEvent *event)
{
  if (event)
    {
      QPoint old_start = m_start_pos;
      m_start_pos = event->pos ();
      return m_start_pos - old_start;
    }

  if (m_valid)
    return m_end_pos - m_start_pos;

  return QPoint (0, 0);
}
