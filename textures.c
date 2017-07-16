#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<math.h>
#include<time.h>
#include<sys/time.h>
#include<X11/Xlib.h>
#include<X11/XKBlib.h>
#include<GL/glx.h>
#include<GL/glext.h>
#include<GL/glu.h>
#include <GL/glut.h>

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

float num = 0;
int width  = 0;
int height = 0;
float size = .3;
short BitsPerPixel = 0;
int start_t,end_t;
float total_t;

int t1,t2;
#define BILLION  1000000000L;

GLfloat rotation_matrix[16];
float                   rot_z_vel = 50.0, rot_y_vel = 30.0;

struct timespec start,stop;

/*
NOTES

	- 32 bit program uses 32 bits in its address space
	- any adress will have 32 ones or zeros

*/
GLuint loadTGA(char *filename) {
	short w = 0;
	short h = 0;
	GLuint texture;
	t1 = 0;
	t2 = 60;

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

void DrawAQuad() {
 
	 GLuint text = loadTGA("minecraft1.tga");
	 GLuint text1 = loadTGA("grasstop.tga");
	 
	 glRotatef(num,num,num,0.0);

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



void expose() {
	 float aspect_ratio;
	 
	 XGetWindowAttributes(dpy,win,&gwa);
	 glViewport(0,0,gwa.width,gwa.height);
	 aspect_ratio = (float)(gwa.width)/(float)(gwa.height);
 	  
	 glEnable(GL_DEPTH_TEST);
 	 glEnable(GL_TEXTURE_2D);
	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();
	 glOrtho(-2.50*aspect_ratio, 2.50*aspect_ratio, -2.50, 2.50, 1., 100.);

	 num+=1;
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
	 glMultMatrixf(rotation_matrix);  
	

	 glClearColor(0, 0, 0, 0);
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//DRAWS OBJECT
	 DrawAQuad();


	 glXSwapBuffers(dpy, win);
	 glFlush();
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

	 t1 = 0;
	 t2 = 0;
	
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
	 //glEnable(GL_DEPTH_TEST); 


	 glGetFloatv(GL_MODELVIEW_MATRIX, rotation_matrix);
	// clock_t ba = clock_gettime(CLOCK_REALTIME,&start);
	 
	 //so far he just got the frequency of each frame.	 
	
	 if(clock_gettime(CLOCK_MONOTONIC_RAW,&start) == -1) {
			perror("Exit");
			exit(EXIT_FAILURE);
	 }
	 
	 long tstep = 0;
	 long tavg = 0;
	 while(1) {
	
	   	if(clock_gettime(CLOCK_MONOTONIC_RAW,&stop) == -1) {
			perror("Exit");
			exit(EXIT_FAILURE);
	 	}
		

		tstep += (stop.tv_nsec - start.tv_nsec);
		printf( "%ld \n",tstep);
		if(tstep >= 16670000) {		
	 		expose();
			//sleep(1);
			tstep = 0;
		} 
			
    	
		if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&start) == -1) {
			perror("Exit"); 
			exit(EXIT_FAILURE);
		}
	        
	 }
		
	 /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */





/*
	while(sysflags & SYS_FLAG_SYSTEM_RUNNING){
		
		timer_update(&timer);
		
		if(timer.tstep >= timer.min_tstep){

			gfx.cube_theta += (float)timer.tstep * 20;
			gfx.cube_theta = (float)fmod(gfx.cube_theta,360);
			
			sys_update(&sys);	
			gfx_update(&gfx);

			timer.tstep = 0;

		}
	}
*/






