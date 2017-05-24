#ifndef MOUSE_TRACKER_H
#define MOUSE_TRACKER_H

#include <QPoint>

class QMouseEvent;
class mouse_tracker
{
private:
  bool m_drag_on;
  bool m_valid;
  QPoint m_start_pos;
  QPoint m_end_pos;
public:
  mouse_tracker ();
  ~mouse_tracker ();
  void start_tracking (QMouseEvent *event);
  void stop_tracking (QMouseEvent *event = nullptr);
  QPoint get_delta (QMouseEvent *event = nullptr);
};

#endif // MOUSE_TRACKER_H
