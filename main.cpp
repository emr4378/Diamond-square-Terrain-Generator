#include <iostream>
#include <string>
#include <math.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#	include <stdlib.h>
#	include <GL/glut.h>
#elif defined(__APPLE__) && defined(__MACH__)
#	include <GLUT/glut.h>
#else
#   include <GL/glut.h>
#endif

#include "vecmath.h"
#include "terrainGenerator.h"
#include "SOIL.h"

//#define MINECRAFT 1

#define SQRT3 1.73205081


//define keyboard input constants
#define KEY_EXIT 'q'
#define KEY_FORWARD 'w'
#define KEY_BACKWARD 's'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define KEY_UP 32 //Spacebar
#define KEY_DOWN 'z'
#define KEY_RESET 'r'
#define KEY_WIREFRAME 49 //1
#define KEY_SHADED 50 //2

//define constants for user interactivity
#define MOVE_SPEED 14
#define MOVE_MULT 3


#ifdef MINECRAFT
	const int MAX_HEIGHT = 128;
	const int ITERATIONS = 9;
	const float ROUGHNESS = 2;
	const int CUBE_WID = 16;
#else
	const int MAX_HEIGHT = 256;
	const int ITERATIONS = 9;
	const float ROUGHNESS = 5;
	const int CUBE_WID = 16;
#endif

//const string IMAGE_PATH = "Z:\\Dropbox\\3dTerrain\\images\\";
const string IMAGE_PATH = "C:\\Users\\CheEsus\\Dropbox\\3dTerrain\\images\\";


int tTime = 35;

#ifdef MINECRAFT
signed char * map;
#else
float * map;
#endif

int mapSize;
int mapDimension;


Point3 cam(0.0, 200.0, 100.0);
Vector3 gravity(0.0, 0.0, 0.0);
Vector3 forwardV, upV, rightV;
float moveSpeed;
double yaw = 0, pitch = 0;
int oldx, oldy; 
bool keys[256];

GLuint cubeList;

float frustum[6][4];

void extractFrustum()
{
   float   proj[16];
   float   modl[16];
   float   clip[16];
   float   t;

   /* Get the current PROJECTION matrix from OpenGL */
   glGetFloatv( GL_PROJECTION_MATRIX, proj );

   /* Get the current MODELVIEW matrix from OpenGL */
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the numbers for the RIGHT plane */
   frustum[0][0] = clip[ 3] - clip[ 0];
   frustum[0][1] = clip[ 7] - clip[ 4];
   frustum[0][2] = clip[11] - clip[ 8];
   frustum[0][3] = clip[15] - clip[12];

   /* Normalize the result */
   t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
   frustum[0][0] /= t;
   frustum[0][1] /= t;
   frustum[0][2] /= t;
   frustum[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   frustum[1][0] = clip[ 3] + clip[ 0];
   frustum[1][1] = clip[ 7] + clip[ 4];
   frustum[1][2] = clip[11] + clip[ 8];
   frustum[1][3] = clip[15] + clip[12];

   /* Normalize the result */
   t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
   frustum[1][0] /= t;
   frustum[1][1] /= t;
   frustum[1][2] /= t;
   frustum[1][3] /= t;

   /* Extract the BOTTOM plane */
   frustum[2][0] = clip[ 3] + clip[ 1];
   frustum[2][1] = clip[ 7] + clip[ 5];
   frustum[2][2] = clip[11] + clip[ 9];
   frustum[2][3] = clip[15] + clip[13];

   /* Normalize the result */
   t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
   frustum[2][0] /= t;
   frustum[2][1] /= t;
   frustum[2][2] /= t;
   frustum[2][3] /= t;

   /* Extract the TOP plane */
   frustum[3][0] = clip[ 3] - clip[ 1];
   frustum[3][1] = clip[ 7] - clip[ 5];
   frustum[3][2] = clip[11] - clip[ 9];
   frustum[3][3] = clip[15] - clip[13];

   /* Normalize the result */
   t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
   frustum[3][0] /= t;
   frustum[3][1] /= t;
   frustum[3][2] /= t;
   frustum[3][3] /= t;

   /* Extract the FAR plane */
   frustum[4][0] = clip[ 3] - clip[ 2];
   frustum[4][1] = clip[ 7] - clip[ 6];
   frustum[4][2] = clip[11] - clip[10];
   frustum[4][3] = clip[15] - clip[14];

   /* Normalize the result */
   t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
   frustum[4][0] /= t;
   frustum[4][1] /= t;
   frustum[4][2] /= t;
   frustum[4][3] /= t;

   /* Extract the NEAR plane */
   frustum[5][0] = clip[ 3] + clip[ 2];
   frustum[5][1] = clip[ 7] + clip[ 6];
   frustum[5][2] = clip[11] + clip[10];
   frustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
   frustum[5][0] /= t;
   frustum[5][1] /= t;
   frustum[5][2] /= t;
   frustum[5][3] /= t;
}

float sphereInFrustum( float x, float y, float z, float radius )
{
   int p;
   float d;

   for( p = 0; p < 6; p++ )
   {
      d = frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3];
      if( d <= -radius )
         return 0;
   }
   return d + radius;
}

void drawCube(int wid, GLuint side, GLuint top, GLuint bot) {
	glutWireCube(CUBE_WID);
	//Radius = (CUBE_WID / 2) * SQRT3
	/*glPushMatrix();
		for (int i = 0; i < 4; i++) {
			glBindTexture(GL_TEXTURE_2D, side);
			glBegin(GL_POLYGON);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-wid/2, -wid/2, wid/2);
                glTexCoord2f(1.0f, 0.0f); glVertex3f(wid/2, -wid/2, wid/2);
                glTexCoord2f(1.0f, 1.0f); glVertex3f(wid/2, wid/2, wid/2);
                glTexCoord2f(0.0f, 1.0f); glVertex3f(-wid/2, wid/2, wid/2);
			glEnd();
			glRotatef(90, 0, 1, 0);
		}
	glPopMatrix();
	glPushMatrix();
		glRotatef(270, 1, 0, 0);
		glBindTexture(GL_TEXTURE_2D, top);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-wid/2, -wid/2, wid/2);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(wid/2, -wid/2, wid/2);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(wid/2, wid/2, wid/2);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-wid/2, wid/2, wid/2);
		glEnd();

		glRotatef(180, 1, 0, 0);
		glBindTexture(GL_TEXTURE_2D, bot);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-wid/2, -wid/2, wid/2);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(wid/2, -wid/2, wid/2);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(wid/2, wid/2, wid/2);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-wid/2, wid/2, wid/2);
		glEnd();
	glPopMatrix();*/
}

GLuint loadTexture(string filename) {
	GLuint ret = SOIL_load_OGL_texture(
		filename.c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_POWER_OF_TWO
		| SOIL_FLAG_MIPMAPS
		| SOIL_FLAG_MULTIPLY_ALPHA
		| SOIL_FLAG_COMPRESS_TO_DXT
		| SOIL_FLAG_DDS_LOAD_DIRECT
		| SOIL_FLAG_INVERT_Y
    );

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return ret;
}

void display(void)
{
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW); 	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	//set up initial camera vectors
	upV = Vector3(0, 1, 0);
	rightV = Vector3(1, 0, 0);
	forwardV = rightV;

	//rotate right and forward around up for yaw correction (left/right)
	rightV = Vector3::rotate(rightV, upV, yaw);
	forwardV = Vector3::rotate(forwardV, upV, yaw-90);

	//rotate forward around right for pitch correction (up/down)
	forwardV = Vector3::rotate(forwardV, rightV, pitch);

	//take cross product of forward and right to get up vector
	upV = forwardV^rightV;

	//normalize
	upV.normalize();
	rightV.normalize();
	forwardV.normalize();

	//update camera using up, right and forward vectors
	gluLookAt(cam.x, cam.y, cam.z,
				forwardV.x+cam.x, forwardV.y+cam.y, forwardV.z+cam.z,
				upV.x, upV.y, upV.z);

	extractFrustum();

	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(1, 1, 1);

#ifdef MINECRAFT
	glTranslatef(0, -100, 0);
	if (map != NULL) {
		float radius = (CUBE_WID/2) * SQRT3;
		for (int r = 0; r < mapDimension; r++) {
			for (int c = 0; c < mapDimension; c++) {
				float pos[3] = { CUBE_WID*(c-mapDimension/2), CUBE_WID*map[r * mapDimension + c], CUBE_WID*(r-mapDimension/2) };
				if (sphereInFrustum(pos[0], pos[1], pos[2], radius) > 0) {
					glPushMatrix();
						glTranslatef(pos[0], pos[1], pos[2]);
						glCallList(cubeList);
					glPopMatrix();
				}
			}
		}
	}
#else

	glBegin(GL_LINES);
	if (map != NULL) {
		for (int r = 1; r < mapDimension; r++) {
			for (int c = 1; c < mapDimension; c++) {
				
				float pos1[3] = { CUBE_WID*(c-mapDimension/2), CUBE_WID*map[r * mapDimension + c], CUBE_WID*(r-mapDimension/2) };
				float pos2[3] = { CUBE_WID*(c-1-mapDimension/2), CUBE_WID*map[(r) * mapDimension + (c-1)], CUBE_WID*(r-mapDimension/2) };
				float pos3[3] = { CUBE_WID*(c-mapDimension/2), CUBE_WID*map[(r - 1) * mapDimension + (c)], CUBE_WID*(r-1-mapDimension/2) };
				
				glVertex3f(pos1[0], pos1[1], pos1[2]);
				glVertex3f(pos2[0], pos2[1], pos2[2]);

				glVertex3f(pos1[0], pos1[1], pos1[2]);
				glVertex3f(pos3[0], pos3[1], pos3[2]);
			}
		}
	}
	glEnd();
	

#endif


	glDisable(GL_TEXTURE_2D);
	glFlush();
	glutSwapBuffers();
}

void init(void)
{
	//cam = Point3(0.0, 100, 0.0);
	TerrainGenerator gen(ROUGHNESS);
	mapSize = gen.determineLength(ITERATIONS);

#ifdef MINECRAFT
	map = new signed char[mapSize];
#else
	map = new float[mapSize];
#endif

	mapDimension = sqrt((float)mapSize);

	for (int i = 0; i < mapSize; i++) {
		if (i < mapDimension || i > mapSize-mapDimension || i % mapDimension == 0 || (i+1) % mapDimension == 0) {
			map[i] = 1;
		} else {
			map[i] = 0;
		}
	}
	//gen.diamondSquare(map, &mapSize, MAX_HEIGHT, ITERATIONS);

#ifdef MINECRAFT
	gen.diamondSquare<signed char>(map, &mapSize, MAX_HEIGHT);
#else
	gen.diamondSquare<float>(map, &mapSize, MAX_HEIGHT);
#endif
	

	GLuint topTex = loadTexture(IMAGE_PATH + "grass_top_TINY.png");
	GLuint sideTex = loadTexture(IMAGE_PATH + "grass_side_TINY.png");
	GLuint dirtTex = loadTexture(IMAGE_PATH + "dirt_TINY.png");

	if (topTex == 0 || sideTex == 0 || dirtTex == 0) {
		cerr << "ERROR: " << SOIL_last_result() << endl;
		exit(1);
	}

	cubeList = glGenLists(1);
	glNewList(cubeList, GL_COMPILE);
		drawCube(CUBE_WID, sideTex, topTex, dirtTex);
		//glutWireCube(CUBE_WID);
	glEndList();

	//int rowSize = sqrt((float)mapSize);
	//for (int r = 0; r < rowSize; r++) {
	//	for (int c = 0; c < rowSize; c++) {
	//		cout << map[r * rowSize + c] << " ";
	//	}
	//	cout << endl;
	//}


	glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);\
	glShadeModel(GL_FLAT);

	glColor3f(1, 1, 1);
}

void reshape(int w, int h)
{
	oldx = w/2;
	oldy = h/2;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

#ifdef MINECRAFT
	gluPerspective(60.0, (double)w/(double)h, 1, 1000);
#else
	gluPerspective(60.0, (double)w/(double)h, 1, 10000);
#endif
   
}

void timer(int value) {
	if(keys[KEY_FORWARD]) {
		cam.x += sin(yaw * PI/180) * moveSpeed;
		cam.z += cos(yaw * PI/180) * moveSpeed;
	} else if (keys[KEY_BACKWARD]) {
		cam.x -= sin(yaw * PI/180) * moveSpeed;
		cam.z -= cos(yaw * PI/180) * moveSpeed;
	}

	if (keys[KEY_LEFT]) {
		cam.x += cos(yaw * PI/180) * moveSpeed;
		cam.z -= sin(yaw * PI/180) * moveSpeed;
	} else if (keys[KEY_RIGHT]) {
		cam.x -= cos(yaw * PI/180) * moveSpeed;
		cam.z += sin(yaw * PI/180) * moveSpeed;
	}

	if (keys[KEY_UP]) {
		cam.y += moveSpeed;	
	} else if (keys[KEY_DOWN]) {
		cam.y -= moveSpeed;
	}

	if (keys[KEY_EXIT]) {
		exit(0);
	}

	if (keys[KEY_RESET]) {
		init();
	}

	glutPostRedisplay();
	glutTimerFunc(tTime, timer, 0);
}

void keyboard(unsigned char key, int x, int y)
{
	if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
		moveSpeed = MOVE_MULT * MOVE_SPEED;
		if (key >= 65 && key <= 90) {
			key += 32;
		}
	} else {
		moveSpeed = MOVE_SPEED;
	}
	switch (key) {
	default:
		break;
	}
	keys[key] = true;

	glutPostRedisplay();
}

void keyboardUpFunc(unsigned char key, int x, int y) {
	if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
		moveSpeed = 3 * MOVE_SPEED;
		if (key >= 65 && key <= 90) {
			key += 32;
		}
	} else {
		moveSpeed = MOVE_SPEED;
	}
	keys[key] = false;
}

void mouseMoved(int x, int y) {
	float div = 10;
	float diffx = (x-oldx)/div;
	float diffy = (y-oldy)/div;

	yaw -= diffx;
	pitch += diffy;

	if (pitch >= 90) pitch = 90;
	if (pitch <= -90) pitch = -90;

	if (diffx != 0  || diffy != 0) {
		glutPostRedisplay();
		glutWarpPointer(oldx, oldy);
	}
}

void motionFunc(int x, int y) {
	mouseMoved(x, y);
}

void passiveMotionFunc(int x, int y) {
	mouseMoved(x, y);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (500, 500);
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUpFunc);
    glutMotionFunc(motionFunc);
    glutPassiveMotionFunc(passiveMotionFunc);

	//remove cursor and enable fullscreen
	glutSetCursor(GLUT_CURSOR_NONE);
	glutFullScreen();

	glutTimerFunc(tTime, timer, 0);
	glutMainLoop();

	return 0;
}

