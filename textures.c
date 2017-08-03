#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<strings.h>
#include<string.h>
#include<arpa/inet.h>
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

#define ERROR	-1
#define BUFFER  1024
char * peerID(int);

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

float x = 0;
float y = 0;
float num = 0;
int width  = 0;
int height = 0;
float size = .3;
short BitsPerPixel = 0;
int start_t,end_t;
float total_t;
GLuint text;
GLuint text1;
int t1,t2;
char str_ser[20],str_clnt[20];
int sock;

#define BILLION  1000000000L;

GLfloat rotation_matrix[16];
float rot_z_vel = 50.0, rot_y_vel = 30.0;

/*
NOTES

	- 32 bit program uses 32 bits in its address space
	- any adress will have 32 ones or zeros

*/
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

void DrawAQuad() {

	 glRotatef(num,0,num,0.0);
	 glTranslatef(x,y,0);

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

	 //num+=1;
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
	 glMultMatrixf(rotation_matrix);  
	

	 glClearColor(0, 0, 0, 0);
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//DRAWS OBJECT
	 DrawAQuad();
	glMatrixMode(GL_PROJECTION);
 	glLoadIdentity();
 	glOrtho(0, (float)gwa.width, 0, (float)gwa.height, -1., 1.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	 glXSwapBuffers(dpy, win);
	 glFlush();
}


//just up and down for now to demonstrate 3d and movement without sloppyness
void CheckKeyboard() {
	if(XCheckWindowEvent(dpy,win,KeyPressMask,&xev)) {
		char *key_string = XKeysymToString(XkbKeycodeToKeysym(dpy,xev.xkey.keycode,0,0));
		if(strncmp(key_string, "Left", 4) == 0) {
		/*	x-=0.05;
			num--;
		} else if(strncmp(key_string,"Right", 5) == 0) {
			x+=0.05;
			num++;*/
		} else if(strncmp(key_string,"Up", 2) == 0) {
			//send(sock,"up",2,0);
			write(sock,"up",10);
			  //strncpy(str_clnt,"up\0",10);
			  //write(sock,str_clnt,20);
			  read(sock,str_ser,20);
			  printf("String from Server: %s\n",str_ser);
			  

			  y+=0.05;
		} else if(strncmp(key_string,"Down",4) == 0) {
			//send(sock,"down",2,0);
			//strncpy(str_clnt,"down\0",10);
			write(sock,"down",20);
			read(sock,str_ser,20);
			printf("String from Server: %s\n",str_ser);
			
			y-=0.05;
		}
	}

}

int main(int argc, char *argv[]) {

	struct sockaddr_in remote_server;
	
	char input[BUFFER];
	char output[BUFFER];
	int len;
	int idlen = 10;
	char message[] = "send to the server";
	char *id;
	id = peerID(idlen);
	
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		perror("error");
		exit(-1);
	}
	remote_server.sin_family = AF_INET;
	remote_server.sin_port = htons(atoi("8081"));
	remote_server.sin_addr.s_addr = inet_addr("127.0.0.1");
	bzero(&remote_server.sin_zero, 8);
	
	//if we dont connect to the server close connection, else open window
	if((connect(sock, (struct sockaddr *)&remote_server, sizeof(struct sockaddr_in))) == ERROR) {
		perror("connect");
		exit(-1);
	} else {
		
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
		 
		text = loadTGA("minecraft1.tga");
		text1 = loadTGA("grasstop.tga");
		
		//main loop
	
		//query the time step initial
		struct timespec ts;
		//set a variable called old time and make that equal to the timestep
		clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
		uint64_t oldTime = ts.tv_nsec;
		//we are going to render a frame when 1/60 seconed has passed
		const float desiredTime = 1.0f / 60.0f;
		float tstep = 0;
		float tmpstep = 0;
		int err = 0;
		while(1) {
			//set new time = new timestamp
			clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
			uint64_t newtime = ts.tv_nsec;
			//tmpstep = newtime-oldtime 
			tmpstep = newtime-oldTime;
			//tstep+=tmpstep as long as tmpstep is less then 1
			if(tstep <= 1.0f) {
				tstep += tmpstep;
			}
			oldTime = newtime;
		
			//if tstep has exceeded a 1/60 seconds render cube set tstep=0
			if(tstep >= desiredTime) {
				expose();
				CheckKeyboard();
				tstep = 0;
			}	
		}
	} close(sock);

	 /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */

char *peerID(int len) {
  char nums[11] = "0123456789";
  char *id = malloc(sizeof(char)*len+1);
  for(int i = 0; i < len; i++) {
     id[i] = nums[rand()%9];
  }
  id[len] = '\0';
  return id;
}


