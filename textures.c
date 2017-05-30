#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

Display                 *dpy;
Window                  root;
GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;


int width  = 0;
int height = 0;
float size = .3;
short BitsPerPixel = 0;
float DT;
GLfloat rotation_matrix[16];
float                   rot_z_vel = 50.0, rot_y_vel = 30.0;
///
GLuint loadTGA(char *filename) {
	short w = 0;
	short h = 0;
	GLuint texture;
	
	FILE *file = fopen(filename, "rb");
	for(int i = 0; i < 12; i++) {
		fgetc(file);
	}
	fread(&w,1,2,file);
	fread(&h,1,2,file);
	
	int *data = malloc((w*h*4)*sizeof(int));
	
	fseek(file, 18, SEEK_SET);
	fread(data, w*h*4, 1, file);
	fclose(file);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_BGRA, GL_UNSIGNED_BYTE, data);

	free(data);

	return texture;
}
void RotateCube() {
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 glRotatef(rot_y_vel*DT,0.0,1.0,0.0);
 glMultMatrixf(rotation_matrix);
 glGetFloatv(GL_MODELVIEW_MATRIX,rotation_matrix);
}
void DrawAQuad() {
 glClearColor(0, 0, 0, 0);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glOrtho(-1., 1., -1., 1., 1., 20.);
 glEnable(GL_TEXTURE_2D);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
   
 GLuint text = loadTGA("minecraft1.tga");
 GLuint text1 = loadTGA("grasstop.tga");
 glRotatef(20.0, 1.0, 1.0, 0.0);

 glBindTexture(GL_TEXTURE_2D, text);
 glBegin(GL_QUADS); 
  glTexCoord2f(0.0, 0.0); glVertex3f(-size, -size, -size);
  glTexCoord2f(1.0, 0.0); glVertex3f( size, -size, -size);
  glTexCoord2f(1.0, 1.0); glVertex3f( size,  size, -size);
  glTexCoord2f(0.0, 1.0); glVertex3f(-size,  size, -size);

  glTexCoord2f(0.0, 0.0); glVertex3f(-size, -size, size);
  glTexCoord2f(1.0, 0.0); glVertex3f( size, -size, size);
  glTexCoord2f(1.0, 1.0); glVertex3f( size,  size, size);
  glTexCoord2f(0.0, 1.0); glVertex3f(-size,  size, size);
 glEnd();

 glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(-size, -size, -size);
  glTexCoord2f(1.0, 0.0); glVertex3f(-size, -size, size);
  glTexCoord2f(1.0, 1.0); glVertex3f(-size,  size, size);
  glTexCoord2f(0.0, 1.0); glVertex3f(-size,  size, -size);

  glTexCoord2f(0.0, 0.0); glVertex3f(size, -size, -size);
  glTexCoord2f(1.0, 0.0); glVertex3f(size, -size, size);
  glTexCoord2f(1.0, 1.0); glVertex3f(size,  size, size);
  glTexCoord2f(0.0, 1.0); glVertex3f(size,  size, -size);
 glEnd();
   
 glBindTexture(GL_TEXTURE_2D, text1);
 glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(-size, -size, -size);
  glTexCoord2f(1.0, 0.0); glVertex3f(-size, -size, size);
  glTexCoord2f(1.0, 1.0); glVertex3f( size, -size, size);
  glTexCoord2f(0.0, 1.0); glVertex3f( size, -size, -size);

  glTexCoord2f(0.0, 0.0); glVertex3f(-size,  size, -size);
  glTexCoord2f(1.0, 0.0); glVertex3f(-size,  size,  size);
  glTexCoord2f(1.0, 1.0); glVertex3f( size,  size,  size);
  glTexCoord2f(0.0, 1.0); glVertex3f( size,  size, -size);
 glEnd();

} 

int main(int argc, char *argv[]) {
	short b,h;
 FILE *f = fopen("minecraft.tga","rb");
 for(int i = 0; i < 12; i++){
	fgetc(f);
 }
 fread(&b,1,2,f); 
 printf("%d\n",b);
 fread(&h,1,2,f);
 printf("%d",h);


 dpy = XOpenDisplay(NULL);
 
 if(dpy == NULL) {
        printf("\n\tcannot connect to X server\n\n");
        exit(0);
 }
        
 root = DefaultRootWindow(dpy);

 vi = glXChooseVisual(dpy, 0, att);

 if(vi == NULL) {
        printf("\n\tno appropriate visual found\n\n");
        exit(0);
 } 
 else {
        printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
 }


 cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

 swa.colormap = cmap;
 swa.event_mask = ExposureMask | KeyPressMask;
 
 win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

 XMapWindow(dpy, win);
 XStoreName(dpy, win, "Window");
 
 glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);

 glXMakeCurrent(dpy, win, glc);
 glEnable(GL_DEPTH_TEST); 

 while(1) {
        XNextEvent(dpy, &xev);
        
        if(xev.type == Expose) {
                XGetWindowAttributes(dpy, win, &gwa);
                glViewport(0, 0, gwa.width, gwa.height); 
                DrawAQuad(); 
		RotateCube();
                glXSwapBuffers(dpy, win);
        }
                
        else if(xev.type == KeyPress) {
                glXMakeCurrent(dpy, None, NULL);
                glXDestroyContext(dpy, glc);
                XDestroyWindow(dpy, win);
                XCloseDisplay(dpy);
                exit(0);
        }
    } /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */


