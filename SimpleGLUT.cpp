#include "stdafx.h"

// standard
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// glut
#include <GL/glut.h>
#include <vector>
#include <iostream>

//================================
// Keyframe Structure
//================================
struct Keyframe {
	float position[3];     // (x, y, z) position
	float orientation[4];  // Fixed Angles or Quaternion components
};

// Global variables
int g_screenWidth = 1000;
int g_screenHeight = 1000;
int g_frameIndex = 0;   // Current frame
float g_t = 0.0f;       // Interpolation factor (t)
// please change the values below for different executions
int g_splineType = 0;   // 0 for Catmull-Rom, 1 for B-splines
int g_orientationType = 0;  // 0 for Fixed Angles, 1 for Quaternions

// Example keyframes
std::vector<Keyframe> keyframes = {
	 {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
	{{2.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}},
	{{0.0f, 2.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{0.0f, 0.0f, 2.0f}, {0.0f, 0.0f, 0.0f, 0.0f}}

};

//================================
// Catmull-Rom and B-Spline Interpolation
//================================

// Catmull-Rom interpolation formula
float CatmullRom(float p0, float p1, float p2, float p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;
	return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

// B-Spline interpolation formula
float BSpline(float p0, float p1, float p2, float p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;
	return ((-1.0f / 6.0f) * p0 + 0.5f * p1 - 0.5f * p2 + (1.0f / 6.0f) * p3) * t3 +
		(0.5f * p0 - p1 + 0.5f * p2) * t2 + (-0.5f * p0 + 0.5f * p2) * t + (1.0f / 6.0f) * p0 + (2.0f / 3.0f) * p1 + (1.0f / 6.0f) * p2;
}

// Perform interpolation for position
void interpolatePosition(float t, float* pos) {
	int n = keyframes.size();
	int i = g_frameIndex % n;

	// Get 4 control points for spline interpolation
	int p0 = (i - 1 + n) % n;
	int p1 = i;
	int p2 = (i + 1) % n;
	int p3 = (i + 2) % n;

	// Choose interpolation method
	if (g_splineType == 0) {
		// Catmull-Rom Spline
		pos[0] = CatmullRom(keyframes[p0].position[0], keyframes[p1].position[0], keyframes[p2].position[0], keyframes[p3].position[0], t);
		pos[1] = CatmullRom(keyframes[p0].position[1], keyframes[p1].position[1], keyframes[p2].position[1], keyframes[p3].position[1], t);
		pos[2] = CatmullRom(keyframes[p0].position[2], keyframes[p1].position[2], keyframes[p2].position[2], keyframes[p3].position[2], t);
	}
	else {
		// B-Spline
		pos[0] = BSpline(keyframes[p0].position[0], keyframes[p1].position[0], keyframes[p2].position[0], keyframes[p3].position[0], t);
		pos[1] = BSpline(keyframes[p0].position[1], keyframes[p1].position[1], keyframes[p2].position[1], keyframes[p3].position[1], t);
		pos[2] = BSpline(keyframes[p0].position[2], keyframes[p1].position[2], keyframes[p2].position[2], keyframes[p3].position[2], t);
	}
}




// Interpolate orientation
void interpolateQuaternion(float t, float* orientation) {
	int n = keyframes.size();
	int i = g_frameIndex % n;

	// Get 4 control points for quaternion interpolation
	int p0 = (i - 1 + n) % n;
	int p1 = i;
	int p2 = (i + 1) % n;
	int p3 = (i + 2) % n;
	// Temporary array to hold interpolated components
	float interpolated[4];
	//For quaternion interpolation
	for (int j = 0; j < 4; ++j) {
		float p0Val = keyframes[p0].orientation[j];
		float p1Val = keyframes[p1].orientation[j];
		float p2Val = keyframes[p2].orientation[j];
		float p3Val = keyframes[p3].orientation[j];

		if (g_splineType == 0) {
			// Catmull-Rom Spline for quaternion
			orientation[j] = CatmullRom(p0Val, p1Val, p2Val, p3Val, t);
		}
		else {
			// B-Spline for quaternion
			orientation[j] = BSpline(p0Val, p1Val, p2Val, p3Val, t);
		}

	}




}

//================================
// init
//================================
void init(void) {
	glEnable(GL_DEPTH_TEST);
}

//================================
// update
//================================
void update(void) {
	// Update frame index and interpolation factor (g_t)
	g_t += 0.02f;
	if (g_t >= 1.0f) {
		g_t = 0.0f;
		g_frameIndex++;
	}
}

//================================
// render
//================================
void render(void) {
	// Clear buffer
	// clear buffer
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render state
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// light source attributes
	GLfloat LightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightDiffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat LightSpecular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightPosition[] = { 5.0f, 5.0f, 5.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	// surface material attributes
	GLfloat material_Ka[] = { 0.11f, 0.06f, 0.11f, 1.0f };
	GLfloat material_Kd[] = { 0.43f, 0.47f, 0.54f, 1.0f };
	GLfloat material_Ks[] = { 0.33f, 0.33f, 0.52f, 1.0f };
	GLfloat material_Ke[] = { 0.1f , 0.0f , 0.1f , 1.0f };
	GLfloat material_Se = 10;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_Se);

	// Interpolated position
	float pos[3];
	interpolatePosition(g_t, pos);

	// Interpolated orientation
	float orientation[4];

	interpolateQuaternion(g_t, orientation);

	// Set model transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(pos[0], pos[1], pos[2]);
	// Set orientation based on type
	if (g_orientationType == 0) {  // Fixed angles
		glRotatef(orientation[0], 1.0f, 0.0f, 0.0f); // Pitch
		glRotatef(orientation[1], 0.0f, 1.0f, 0.0f); // Yaw
		glRotatef(orientation[2], 0.0f, 0.0f, 1.0f); // Roll
	}
	else { // Quaternions
		// Convert quaternion to rotation (simple implementation)
		float x = orientation[0];
		float y = orientation[1];
		float z = orientation[2];
		float w = orientation[3];
		float angle = 2.0f * acos(w) * 180.0f / M_PI;
		float s = sqrt(1.0f - w * w);
		if (s < 0.001f) {
			// If s close to zero then direction of axis is not important
			glRotatef(angle, x, y, z);
		}
		else {
			glRotatef(angle, x / s, y / s, z / s);
		}
	}
	//camera angle at 10
	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Render teapot
	glutSolidTeapot(1.0f);

	// disable lighting
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	// Swap buffers
	glutSwapBuffers();
}

//================================
// keyboard input
//================================
void keyboard(unsigned char key, int x, int y) {

}

//================================
// reshape : update viewport and projection matrix
//================================
void reshape(int width, int height) {
	g_screenWidth = width;
	g_screenHeight = height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 1.0, 2000.0);
}

//================================
// timer : triggered every 16ms (60 FPS)
//================================
void timer(int value) {

	update();
	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);
}

//================================
// main
//================================
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(g_screenWidth, g_screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Keyframe Animation");

	init();

	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(16, timer, 0);

	glutMainLoop();
	return 0;
}

