#include "Application.h"

int main(int argc, char **argv)
{
  Application app(argc, argv);
  if (!app.Initialize())
  {
    return 1;
  }
  return app.Run();
}
