#include "line.h"
#include "wall.h"
#include "polygon.h"
#include <assert.h>
#include <stdlib.h>
#include <vector>

int main(int argc, char* argv[])
{
  std::vector<V2D> vpoly;
  R r;
  int i=1;
  std::cout << "arg " << argc << std::endl;
  for(i=1; i< argc-2; i+=2)
    {
      vpoly.push_back( V2D(atof(argv[i]), atof(argv[i+1])) );
      std::cout << "i " << vpoly.back() << std::endl;
    }

  V2D a(atof(argv[i]), atof(argv[i+1]));

  std::cout << "a: " << a << std::endl;

  Polygon poly(vpoly);

  std::cout << "in polygon: " << poly.inside(a) << std::endl;

  return 0;
}
