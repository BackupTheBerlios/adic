#include "glfont.h"
void 
GLFont::drawTextRow(const std::string &text, bool centered)
{
  int w=texPtr->getWidth(),h=texPtr->getHeight();
  float fw=w,fh=h;
  int dx=w/tilex;
  int dy=h/tiley;
  float fdx=1.0f/tilex;
  float fdy=1.0f/tiley;
  int offset=0;

  if (centered) {
    unsigned nc=text.size();
    unsigned ts=0;
    for (unsigned i=0;i<nc;++i) {
      if (text[i]>=32) ++ts;
    }
    offset=-int(ts)*dx/2;
  }
  
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, texPtr->getTextureID());
  glBegin(GL_QUADS);
  unsigned col=0;
  for (unsigned i=0;i<text.size();++i){
    if (text[i]>=32) {
      float cx=float((int(text[i]-32)%tilex)*dx)/fw;
      float cy=float((int(text[i]-32)/tilex)*dy)/fh;
      int x=col*dx+offset;
      glTexCoord2f(cx,cy);
      glVertex2i(x,dy);
      glTexCoord2f(cx+fdx,cy);
      glVertex2i(x+dx,dy);
      glTexCoord2f(cx+fdx,cy+fdy);
      glVertex2i(x+dx,0);
      glTexCoord2f(cx,cy+fdy);
      glVertex2i(x,0);
      ++col;
      continue;
    }
    if ((text[i]>=11)&&(text[i]<32)&&colors.size()) {
      unsigned t=text[i]-11;
      t%=colors.size();
      glColor3f(colors[t].c[0],colors[t].c[1],colors[t].c[2]);
    }
  }
  glEnd();
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

void 
GLFont::drawText(const std::string &text, bool centered)
{
  std::string::size_type pos(text.find_first_of('\n'));
  if (pos!=std::string::npos) {
    std::string row(text,0,pos);
    drawTextRow(row,centered);
    if (pos+1<text.size()) {
      glTranslatef(0,-texPtr->getHeight()/tiley,0);
      drawText(std::string(text,pos+1),centered);
    }
    return;
  }
  drawTextRow(text,centered);
}
void
GLFont::setColor(unsigned num, const float c[3])
{
  if (num>=colors.size())
    colors.resize(num+1);
  for (unsigned i=0;i<3;++i)
    colors[num].c[i]=c[i];
}

