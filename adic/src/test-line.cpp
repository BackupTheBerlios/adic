#include "line.h"
#include "wall.h"
#include <assert.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  R r, dist=0;
  V2D a(atof(argv[1]), atof(argv[2]));
  V2D b(atof(argv[3]), atof(argv[4]));
  V2D c(atof(argv[5]), atof(argv[6]));
  V2D d(atof(argv[7]), atof(argv[8]));
  V2D cp(0,0);
  Line l1(a, b);
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl;
  std::cout << "d: " << d << std::endl;
  std::cout << "v:  " << l1.m_v << std::endl;
  std::cout << "vn: " << l1.m_vn << std::endl;
  std::cout << "l:  " << l1.m_l << std::endl;
  std::cout << "n:  " << l1.m_n << std::endl;
  std::cout << "c-project: " << l1.project(c) << std::endl;
  std::cout << "c-project: " << l1.m_vn*l1.project(c) << std::endl;
  std::cout << "d-project: " << l1.project(d) << std::endl;
  std::cout << "c-dist: " << l1.dist(c) << std::endl;
  std::cout << "d-dist: " << l1.dist(d) << std::endl;
  Line l2(c, d);
  if (l1.collide(l2,cp))
    std::cout << "Lines cross at: " << cp << std::endl;
  else
    std::cout << "Lines do not cross\n";
  std::cout << "cppoint: " << cp << std::endl;
  if (l1.collide(l2,r)) std::cout << "lcol1" << std::endl;
  std::cout << "r1= " << r << std::endl;
  if (l2.collide(l1,r)) std::cout << "lcol2" << std::endl;
  std::cout << "r2= " << r << std::endl;

  Wall w(l1, 1, 0.5);
  Circle k( d, 0.5 );
  if (w.collide(k, cp, dist))
  {
    std::cout << "wall.collide: " << cp << std::endl;
    std::cout << "wall dist:    " << dist << std::endl;
  }
  else
    std::cout << "wall does not collide " << std::endl;
  return 0;
}
