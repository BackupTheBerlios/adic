FUNC(void, glClear,(unsigned int) );
FUNC(void, glMatrixMode,(unsigned int) );
FUNC(void, glLoadIdentity,(void) );
FUNC(void, glColor3f,(float,float,float) );
FUNC(void, glColor4f,(float,float,float,float) );
FUNC(void, glTranslatef,(float,float,float) );
FUNC(void, glScalef,(float,float,float) );
FUNC(void, glBegin,(unsigned int) );
FUNC(void, glVertex2i,(int,int) );
FUNC(void, glVertex2f,(float,float) );
FUNC(void, glEnd,(void) );
FUNC(void, glViewport,(int,int,int,int) );
FUNC(void, glOrtho,(double,double,double,double,double,double) );
FUNC(void, glClearColor,(float,float,float,float) );
FUNC(void, glPushMatrix,(void) );
FUNC(void, glPopMatrix,(void) );
FUNC(void, glGetFloatv,(unsigned int, float *) );
FUNC(void, glGetIntegerv,(unsigned int, int *) );
FUNC(void, glLineWidth,(float) );
FUNC(void, glFlush,(void) );
FUNC(void, glFinish,(void) );
FUNC(void, glEnable,(unsigned int) );
FUNC(void, glDisable,(unsigned int) );
FUNC(void, glTexCoord2f,(float,float) );
FUNC(void, glBindTexture,(unsigned int,unsigned int) );
FUNC(void, glGenTextures,(unsigned int, unsigned int *) );
FUNC(void, glTexParameteri,(unsigned int,unsigned int,int) );
FUNC(void, glTexImage2D,(GLenum target,GLint level,GLint internalFormat,GLsizei width, GLsizei height,GLint border, GLenum format, GLenum type,const GLvoid *pixels ) );
FUNC(void, glRotatef,(float,float,float,float) );
FUNC(void, glShadeModel, (unsigned int) );
FUNC(void, glRasterPos2i, (int,int) );
FUNC(void, glPixelStorei, (unsigned int,unsigned int) );
FUNC(void, glDrawPixels, (int,int,int,int, void *) );
FUNC(void, glPixelZoom, (float,float) );
FUNC(int, glGetError, (void) );
FUNC(void, glBlendFunc, (unsigned int,unsigned int) );