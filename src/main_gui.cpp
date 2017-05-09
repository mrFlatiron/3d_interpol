#include <QApplication>
#include "gui/main_window.h"

int main (int argc, char *argv[])
{
  QApplication app (argc, argv);

  if (argc < 5)
    {
      printf ("Usage: %s a1 a0 b1 b0", argv[0]);
      return 0;
    }
  double a1 = atof (argv[1]);
  double a0 = atof (argv[2]);
  double b1 = atof (argv[3]);
  double b0 = atof (argv[4]);

  main_window *window = new main_window (a1, a0, b1, b0);

  window->exec ();

  delete window;

  return 0;
}
