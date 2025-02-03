#include "Application.h"

int main(int argc, char **argv)
{
  Application app(argc, argv);
  app.Initialize();
  return app.Run();
}
