#include "camera.h"

template <typename V>
static V interpolate(V c, V w, R dt)
{
  if (c==w) return c;
  R s=std::min(dt,R(1));
  return c+((w-c)*s);
}

Camera::Camera()
  : m_mpSpeed(800),
    m_wZoom(1), m_cZoom(1), m_czSpeed(0), m_mzSpeed(8),
    m_wRotate(0), m_cRotate(0), m_crSpeed(0), m_mrSpeed(360),
    m_age(0)
{}

Camera::Camera(const V2D &pos, R zoom, R rotate)
  : m_wPos(pos), m_cPos(pos), m_mpSpeed(800),
    m_wZoom(1), m_cZoom(zoom), m_czSpeed(0), m_mzSpeed(8),
    m_wRotate(0), m_cRotate(rotate), m_crSpeed(0), m_mrSpeed(360),
    m_age(0)
{}

Camera::~Camera()
{}

void
Camera::step(R dt)
{
  //! the first 4 seconds the camera doesn't move
  m_age+=dt;
  if (m_age<4) return;
  
  V2D wSpeed(m_wPos-m_cPos);
  m_cpSpeed=interpolate(m_cpSpeed, wSpeed, dt*R(4));
  if (m_cpSpeed.norm2()>m_mpSpeed) m_cpSpeed.normalize()*=m_mpSpeed;
  m_cPos+=m_cpSpeed*dt;

  R w(m_wZoom-m_cZoom);
  m_czSpeed=interpolate(m_czSpeed, w, dt*R(6));
  if (m_czSpeed<0) m_czSpeed=std::max(m_czSpeed,-m_mzSpeed);
  else m_czSpeed=std::min(m_czSpeed,m_mzSpeed);
  m_cZoom+=m_czSpeed*dt;

  w=(m_wRotate-m_cRotate);
  m_crSpeed=interpolate(m_crSpeed, w, dt*R(4));
  if (m_crSpeed<0) m_crSpeed=std::max(m_crSpeed,-m_mrSpeed);
  else m_crSpeed=std::min(m_crSpeed,m_mrSpeed);
  m_cRotate+=m_crSpeed*dt;
}
