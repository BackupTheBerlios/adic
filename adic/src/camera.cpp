#include "camera.h"

template <typename V>
static V interpolate(V c, V w, R dt)
{
  if (c==w) return c;
  R s=std::min(dt*2,R(1));
  return c+((w-c)*s);
}

Camera::Camera()
  : m_wZoom(1), m_cZoom(0.1),
    m_wRotate(10), m_cRotate(0)
{}

Camera::Camera(const V2D &pos, R zoom, R rotate)
  : m_wPos(pos), m_cPos(pos),
    m_wZoom(zoom), m_cZoom(zoom),
    m_wRotate(rotate), m_cRotate(rotate)
{}

Camera::~Camera()
{}

void
Camera::step(R dt)
{
  m_cPos=interpolate(m_cPos,m_wPos,dt);
  m_cZoom=interpolate(m_cZoom,m_wZoom,dt*0.1);
  m_cRotate=interpolate(m_cRotate,m_wRotate,dt);
}
