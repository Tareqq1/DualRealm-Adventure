#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <cmath>
#include <playsoundapi.h>
#include <glut.h>

bool isJumping = false;
float playerDirectionX = 0.0f;
float playerDirectionZ = -1.0f;
bool isplayervisiblee = true;
bool isFirstPersonView = false;




int WIDTH = 1280;
int HEIGHT = 720;
int score = 0; // Initial score value

GLdouble boyPositionX = 0.0; // Initial X position of the boy model
GLdouble boyPositionY = 0.0; // Initial Y position of the boy model
GLdouble boyPositionZ = 0.0;
float movementSpeed = 0.5f;
float lightAngle = 0.0f; // Initialize the angle for light rotation


int health = 100;
float cameraDistance = 5.0f;
// Distance from the player
   // Height of the camera above the player


float distance(GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2) + pow((z2 - z1), 2));
}
bool cakeVisible[] = { true, true, true, true, true, true, true }; // Visibility of cakes
bool coinVisible[] = { true, true, true, true, true, true, true };
bool carVisible[] = { true, true, true, true, true, true, true }; // Visibility of cars
bool garbageVisible[] = { true, true, true, true, true }; // Visibility of garbages
bool paperVisible[] = { true, true, true, true, true, true, true };
bool keyVisible[] = { true, true, true };

// Positions of cakes and coins (adjust these positions according to your models)
float cakePositions[][3] = {
	{5.0, 0.5, -10.0},
	{-7.0, 0.5, 5.0},
	// Add positions for other cakes in a similar manner
};

float coinPositions[][3] = {
	{7.0, 0.0, -7.0},
	{-5.0, 0.0, 9.0},
	// Add positions for other coins in a similar manner
};

float garbagePositions[][3] = {
	{-7.0, 2.0, -7.0},     // Garbage 1 position
	{15.0, 2.0, -15.0},    // Garbage 2 position
	{-10.0, 2.0, 17.0},    // Garbage 3 position
	{20.0, 2.0, 5.0},      // Garbage 4 position
	{-18.0, 2.0, -20.0}    // Garbage 5 position
};

float carPositions[][3] = {
	{10.0, 2.0, -5.0},     // Car 1 position
	{-8.0, 2.0, 15.0},     // Car 2 position
	{15.0, 2.0, -20.0},    // Car 3 position
	{-20.0, 2.0, 8.0},     // Car 4 position
	{25.0, 2.0, 5.0},      // Car 5 position
	{-18.0, 2.0, -25.0},   // Car 6 position
	{8.0, 2.0, 20.0}       // Car 7 position
};
// Array of paper positions
float paperPositions[][3] = {
	{-4.0, 1.0, -8.0},
	{8.0, 1.0, 12.0},
	{-12.0, 1.0, 4.0}
};

// Array of key positions
float keyPositions[][3] = {
	{10.0, 1.0, -5.0},
	{-15.0, 1.0, 10.0},
	{5.0, 1.0, -15.0}
};

float lockerPositions[][3] = {
	{10.0, 1.0, 5.0},
	{-10.0, 1.0, -10.0},
	{15.0, 1.0, 10.0}
};

float tablePositions[][3] = {
	{2.0, 1.0, 12.0},
	{-8.0, 1.0, -8.0},
	{15.0, 1.0, 15.0}
};

float benchPositions[][3] = {
	{-8.0, 1.0, 2.0},
	{15.0, 1.0, 10.0},
	{-12.0, 1.0, -12.0}
};


// Speed at which the boy moves

GLuint tex;
GLuint tex2;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 500;


class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(0, 10, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables

Model_3DS model_tree;

// Environment 1
Model_3DS Boy;
Model_3DS Bench;
Model_3DS Cake;
Model_3DS Car;
Model_3DS Coin;
Model_3DS House;
Model_3DS LightSource;
Model_3DS garbage;
//Environment 2

Model_3DS Chair;
Model_3DS Keys;
Model_3DS Lockers;
Model_3DS Papers;
Model_3DS Table;
Model_3DS Door;



// Textures
GLTexture tex_ground;
GLTexture tex_ground2;



//=======================================================================
// Lighting Configuration Function
//=======================================================================
bool lightDimmed = false;
GLfloat originalAmbient[4];
GLfloat originalDiffuse[4];
float elapsedTime = 0.0f; // Track elapsed time
float totalDimTime = 10.0f; // Total time to dim the light in seconds
float dimmingRate = 0.1f; // Rate of decrease in intensity per second

void TimerDim(int value) {
	if (value == 1) {
		if (!lightDimmed) {
			// Store the original ambient and diffuse values
			glGetLightfv(GL_LIGHT0, GL_AMBIENT, originalAmbient);
			glGetLightfv(GL_LIGHT0, GL_DIFFUSE, originalDiffuse);
			lightDimmed = true;
		}

		if (lightDimmed && elapsedTime < totalDimTime) {
			// Decrease the light intensity by the defined rate
			GLfloat ambientDim[4];
			GLfloat diffuseDim[4];

			for (int i = 0; i < 4; ++i) {
				ambientDim[i] = originalAmbient[i] - dimmingRate;
				diffuseDim[i] = originalDiffuse[i] - dimmingRate;

				// Ensure the values don't go below zero
				if (ambientDim[i] < 0.0f) ambientDim[i] = 0.0f;
				if (diffuseDim[i] < 0.0f) diffuseDim[i] = 0.0f;
			}

			glLightfv(GL_LIGHT0, GL_AMBIENT, ambientDim);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseDim);

			elapsedTime += 1.0f; // Increment elapsed time by 1 second
			glutPostRedisplay();
			glutTimerFunc(1000, TimerDim, 1); // Call TimerDim again after 1 second for continuous dimming
		}
		else {
			// Reset elapsed time and revert to the original light intensity
			elapsedTime = 0.0f;
			glLightfv(GL_LIGHT0, GL_AMBIENT, originalAmbient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, originalDiffuse);
			lightDimmed = false; // Reset the flag to dim the light again
			glutPostRedisplay();
		}
	}
}

float angle = 0.0f; // Initial angle

void TimerRotate(int value) {
	if (value == 2) {
		angle += 0.5f; // Increment angle by 0.5 degrees
		if (angle >= 360.0f) angle -= 360.0f; // Keep angle within a full circle

		// Set the light's position/angle here using angle (e.g., glRotatef(angle, ...))

		glutPostRedisplay();
		glutTimerFunc(25, TimerRotate, 2); // Rotate again after 25 milliseconds
	}
}


void displayScoreLabelx() {
	glColor3f(0.0, 0.0, 0.0); // White color for text
	glRasterPos3f(-3.5, 5, -6.0); // Adjust position for the label
	const char* label = "Score: "; // Your score label
	for (int i = 0; label[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
	}
}
void displayScoreValuex(int score) {
	glColor3f(0.0, 0.0, 0.0); // White color for text
	glRasterPos3f(-1.5, 5, -6.0); // Adjust position for the score value
	char scoreStr[10]; // Assuming score won't exceed 9 digits
	sprintf(scoreStr, "%d", score); // Convert the score to a string
	for (int i = 0; scoreStr[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, scoreStr[i]);
	}
}

void displayHealthLabelx() {
	glColor3f(0, 0.0, 0.0); // White color for text
	glRasterPos3f(-3.5, 7, -5.0); // Adjust position for the label
	const char* label = "Health: "; // Your health label
	for (int i = 0; label[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
	}
}

void displayHealthValuex(int health) {
	glColor3f(0.0, 0.0, 0.0); // White color for text
	glRasterPos3f(-1.5, 7, -5.0); // Adjust position for the health value
	char healthStr[10]; // Assuming health won't exceed 9 digits
	sprintf(healthStr, "%d", health); // Convert the health to a string
	for (int i = 0; healthStr[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, healthStr[i]);
	}
}


void displayScoreLabel() {
	glColor3f(1.0, 1.0, 1.0); // White color for text
	glRasterPos3f(-3.5, 5, -6.0); // Adjust position for the label
	const char* label = "Score: "; // Your score label
	for (int i = 0; label[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
	}
}
void displayScoreValue(int score) {
	glColor3f(1.0, 01.0, 1.0); // White color for text
	glRasterPos3f(-1.5, 5, -6.0); // Adjust position for the score value
	char scoreStr[10]; // Assuming score won't exceed 9 digits
	sprintf(scoreStr, "%d", score); // Convert the score to a string
	for (int i = 0; scoreStr[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, scoreStr[i]);
	}
}

void displayHealthLabel() {
	glColor3f(1, 1.0, 1.0); // White color for text
	glRasterPos3f(-3.5, 7, -5.0); // Adjust position for the label
	const char* label = "Health: "; // Your health label
	for (int i = 0; label[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
	}
}

void displayHealthValue(int health) {
	glColor3f(1.0, 1.0, 1.0); // White color for text
	glRasterPos3f(-1.5, 7, -5.0); // Adjust position for the health value
	char healthStr[10]; // Assuming health won't exceed 9 digits
	sprintf(healthStr, "%d", health); // Convert the health to a string
	for (int i = 0; healthStr[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, healthStr[i]);
	}
}

void displayScoreLabel1() {
	glColor3f(1.0, 1.0, 1.0); // White color for text
	glRasterPos3f(-0.5, 1, -6); // Adjust position for the label
	const char* label = "Score: "; // Your score label
	for (int i = 0; label[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
	}
}
void displayScoreValue1(int score) {
	glColor3f(1.0, 1.0, 1.0); // White color for text
	glRasterPos3f(0.5, 1, -6.0); // Adjust position for the score value
	char scoreStr[10]; // Assuming score won't exceed 9 digits
	sprintf(scoreStr, "%d", score); // Convert the score to a string
	for (int i = 0; scoreStr[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, scoreStr[i]);
	}
}

void displayHealthLabel1() {
	glColor3f(1.0, 1.0, 1.0); // White color for text
	glRasterPos3f(-0.5, 2, -5.0); // Adjust position for the label
	const char* label = "Health: "; // Your health label
	for (int i = 0; label[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
	}
}

void displayHealthValue1(int health) {
	glColor3f(1.0, 1.0, 1.0); // White color for text
	glRasterPos3f(0.5, 2, -5.0); // Adjust position for the health value
	char healthStr[10]; // Assuming health won't exceed 9 digits
	sprintf(healthStr, "%d", health); // Convert the health to a string
	for (int i = 0; healthStr[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, healthStr[i]);
	}
}
void displayScoreLabel1x() {
	glColor3f(0.0, 0.0, 0.0); // White color for text
	glRasterPos3f(-0.5, 1, -6); // Adjust position for the label
	const char* label = "Score: "; // Your score label
	for (int i = 0; label[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
	}
}
void displayScoreValue1x(int score) {
	glColor3f(0.0, 0.0, 0.0); // White color for text
	glRasterPos3f(0.5, 1, -6.0); // Adjust position for the score value
	char scoreStr[10]; // Assuming score won't exceed 9 digits
	sprintf(scoreStr, "%d", score); // Convert the score to a string
	for (int i = 0; scoreStr[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, scoreStr[i]);
	}
}

void displayHealthLabel1x() {
	glColor3f(0.0, 0.0, 0.0); // White color for text
	glRasterPos3f(-0.5, 2, -5.0); // Adjust position for the label
	const char* label = "Health: "; // Your health label
	for (int i = 0; label[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
	}
}

void displayHealthValue1x(int health) {
	glColor3f(0.0, 0.0, 0.0); // White color for text
	glRasterPos3f(0.5, 2, -5.0); // Adjust position for the health value
	char healthStr[10]; // Assuming health won't exceed 9 digits
	sprintf(healthStr, "%d", health); // Convert the health to a string
	for (int i = 0; healthStr[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, healthStr[i]);
	}
}

void Alternate1() {
	if (isFirstPersonView) {
		isFirstPersonView = false;
	}
	else {
		isFirstPersonView = true;
	}
}
void Alternate() {
	if (isFirstPersonView) {
		Eye.x = boyPositionX;
		Eye.y = boyPositionY + 3;
		Eye.z = boyPositionZ + 9;

		At.x = boyPositionX;
		At.y = boyPositionY;
		At.z = boyPositionZ;
		displayHealthLabel1(); // Display the health label
		displayHealthValue1(health);


		displayScoreLabel1(); // Display the score label
		displayScoreValue1(score);
	}
	else {
		Eye.x = 0;
		Eye.y = 10;
		Eye.z = 20;

		At.x = boyPositionX;
		At.y = boyPositionY;
		At.z = boyPositionZ;
	}
}
/*
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glutTimerFunc(3000, Timer, 1);
} */
//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

const float collisionThreshold = 1.5;




//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	//InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING); // Disable lighting

	glColor3f(0.6, 0.6, 0.6); // Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D); // Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);


	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0); // Set quad normal direction.
	glTexCoord2f(0, 0); // Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-50, 0, -50); // Adjust these coordinates to change the ground size
	glTexCoord2f(5, 0);
	glVertex3f(50, 0, -50); // Adjust these coordinates to change the ground size
	glTexCoord2f(5, 5);
	glVertex3f(50, 0, 50); // Adjust these coordinates to change the ground size
	glTexCoord2f(0, 5);
	glVertex3f(-50, 0, 50); // Adjust these coordinates to change the ground size
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING); // Enable lighting again for other entities coming through the pipeline.

	glColor3f(1, 1, 1); // Set material back to white instead of gray used for the ground texture.
}
void RenderGround2()
{
	glDisable(GL_LIGHTING); // Disable lighting

	glColor3f(0.6, 0.6, 0.6); // Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D); // Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground2.texture[0]);


	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0); // Set quad normal direction.
	glTexCoord2f(0, 0); // Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-50, 0, -50); // Adjust these coordinates to change the ground size
	glTexCoord2f(5, 0);
	glVertex3f(50, 0, -50); // Adjust these coordinates to change the ground size
	glTexCoord2f(5, 5);
	glVertex3f(50, 0, 50); // Adjust these coordinates to change the ground size
	glTexCoord2f(0, 5);
	glVertex3f(-50, 0, 50); // Adjust these coordinates to change the ground size
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING); // Enable lighting again for other entities coming through the pipeline.

	glColor3f(1, 1, 1); // Set material back to white instead of gray used for the ground texture.
}


//=======================================================================
// Display Function
//=======================================================================


// Display the score label

// Inside myDisplay function, after displaying score:
 // Display the health value
void returnToGround(int value) {
	if (value == 0) {
		if (isJumping) {
			boyPositionY -= 2.0; // Decrease the Y position to return to the ground
			isJumping = false;
			glutPostRedisplay();
		}
	}
}

int clickCount = 0;
void myMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isJumping) {
			isJumping = true;
			// Adjust the boy's Y position when jumping
			boyPositionY += 2.0; // You can change the jump height here
			glutTimerFunc(500, returnToGround, 0);
			// Timer to return the boy to the ground after 500 milliseconds
		}
		else {
			clickCount++;
			if (clickCount == 2) {
				isFirstPersonView = true;
				isplayervisiblee = false;
				Alternate();
				clickCount = 0;
			}
		}
	}
}


float distToHouse = distance(boyPositionX, boyPositionY, boyPositionZ, -5.0, 1.0, 6.0); // House position
const float houseCollisionThreshold = 3.5;
bool switchToEnvironment2 = false;

void myDisplay(void)
{
	if (!switchToEnvironment2) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);


		GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
		GLfloat lightPosition[] = { 0.0f, 0.0, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

		// Draw Ground
		RenderGround();

		lightAngle += 0.5f; // Adjust the speed of rotation as needed

		// Set the position of the light source to rotate around a point (0, 0, 0)
		GLfloat light_position[] = {
			10.0f * cos(lightAngle),  // X position
			10.0f,                    // Y position (elevated)
			10.0f * sin(lightAngle),  // Z position
			1.0f                      // Homogeneous coordinate
		};
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		//Environment 1 
		 //Boy
		 // Boy
		glPushMatrix();
		glTranslated(boyPositionX, boyPositionY, boyPositionZ);

		// Calculate the angle of rotation based on player direction
		float rotationAngle = -atan2(playerDirectionX, playerDirectionZ) * 180.0 / 3.14;
		TimerDim(1);

		// Apply rotation
		glRotatef(rotationAngle, 0.0, 1.0, 0.0);

		// Scale and draw the player model
		glScalef(3, 3, 3);
		if (isplayervisiblee) {
			Boy.Draw();
		}


		glPopMatrix();
		/*	if (isJumping) {
				// Update the boy's Y position while jumping
				boyPositionY += 0.1; // You can adjust the rate of ascent here
				// You can add additional conditions to control the jump height or duration
			}
			// Draw the player model*/


		const int CAKE_SCORE = 10;

		for (int i = 0; i < 7; ++i) {

			glPushMatrix();
			glTranslatef(carPositions[i][0], carPositions[i][1], carPositions[i][2]);
			glScalef(0.08, 0.08, 0.08);
			Car.Draw();
			glPopMatrix();
		}


		float distCar1 = distance(boyPositionX, boyPositionY, boyPositionZ, 10, 1, -5);
		float distCar2 = distance(boyPositionX, boyPositionY, boyPositionZ, -8.0, 1, 15.0);
		float distCar3 = distance(boyPositionX, boyPositionY, boyPositionZ, 15.0, 1, -20.0);
		float distCar4 = distance(boyPositionX, boyPositionY, boyPositionZ, -20.0, 1, 8.0);
		float distCar5 = distance(boyPositionX, boyPositionY, boyPositionZ, 25.0, 1, 5.0);
		float distCar6 = distance(boyPositionX, boyPositionY, boyPositionZ, -18.0, 1, -25.0);
		float distCar7 = distance(boyPositionX, boyPositionY, boyPositionZ, 8.0, 1, 20.0);

		if (distCar1 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();

		}
		if (distCar2 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (distCar3 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (distCar4 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (distCar5 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (distCar6 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (distCar7 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}


		float distt = distance(boyPositionX, boyPositionY, boyPositionZ, -5, 1, 6);
		if (distt < collisionThreshold) {
			switchToEnvironment2 = true;

		}


		float distgarbage1 = distance(boyPositionX, boyPositionY, boyPositionZ, -7.0, 1.0, -7.0);
		float distgarbage2 = distance(boyPositionX, boyPositionY, boyPositionZ, 15.0, 1.0, -15.0);
		float distgarbage3 = distance(boyPositionX, boyPositionY, boyPositionZ, -10.0, 1.0, 17.0);
		float distgarbage4 = distance(boyPositionX, boyPositionY, boyPositionZ, 20.0, 1.0, 5.0);
		float distgarbage5 = distance(boyPositionX, boyPositionY, boyPositionZ, -18.0, 1.0, -20.0);

		if (distgarbage1 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (distgarbage2 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}

		if (distgarbage3 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (distgarbage4 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (distgarbage5 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}





		// Draw cakes that are still visible
		for (int i = 0; i < 7; ++i) {
			if (cakeVisible[i]) {
				float dist = distance(boyPositionX, boyPositionY, boyPositionZ, cakePositions[i][0], cakePositions[i][1], cakePositions[i][2]);
				if (dist < collisionThreshold) {
					sndPlaySound(TEXT("sound2.wav"), SND_ASYNC);
					cakeVisible[i] = false;
					score += CAKE_SCORE;// Mark the collided cake as not visible
				}
				else {
					glPushMatrix();
					glTranslatef(cakePositions[i][0], cakePositions[i][1], cakePositions[i][2]);
					// Draw the cake model at its position
					glScalef(0.0004, 0.0004, 0.0004);
					Cake.Draw(); // Replace this line with your cake drawing code
					glPopMatrix();
				}
			}
		}

		// Draw coins that are still visible
		for (int i = 0; i < 7; ++i) {
			if (coinVisible[i]) {
				float dist = distance(boyPositionX, boyPositionY, boyPositionZ, coinPositions[i][0], coinPositions[i][1], coinPositions[i][2]);
				if (dist < collisionThreshold) {
					coinVisible[i] = false;
					sndPlaySound(TEXT("sound2.wav"), SND_ASYNC);
					score += CAKE_SCORE;// Mark the collided coin as not visible
				}
				else {
					glPushMatrix();
					glTranslatef(coinPositions[i][0], coinPositions[i][1], coinPositions[i][2]);
					// Draw the coin model at its position
					glScalef(0.01, 0.01, 0.01);
					Coin.Draw(); // Replace this line with your coin drawing code
					glPopMatrix();
				}
			}
		}
		float distttt = distance(boyPositionX, boyPositionY, boyPositionZ, -5, 1, 6);
		if (distttt < collisionThreshold) {

			sndPlaySound(TEXT("sound3.wav"), SND_ASYNC);
		}
		// LightSource
		glPushMatrix();
		glTranslatef(5, 2, 7);  // Move the LightSource model to a different position
		glScalef(0.01, 0.01, 0.01); // Resize the LightSource model
		LightSource.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-5.0, 1.0, 6.0); // House position at (1.0, 1.0, 1.0)
		glRotatef(90.0f, 90.0f, 1.0f, 0.0f); // Rotate 45 degrees around the y-axis
		glScalef(1.0f, 1.0f, 1.0f); // House scale (adjust as needed)
		House.Draw(); // Draw the house model
		glPopMatrix();


		//glutTimerFunc(3000, Timer, 1);
		/*glPushMatrix();
		glTranslatef(7, 2, 7);  // Move the LightSource model to a different position
		glScalef(0.01, 0.01, 0.01); // Resize the LightSource model
		garbage.Draw();
		glPopMatrix();             */

		glPushMatrix();
		glTranslatef(-7.0, 2.0, -7.0); // Garbage 1 position
		glScalef(0.01, 0.01, 0.01); // Resize the Garbage model
		garbage.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(15.0, 2.0, -15.0); // Garbage 2 position
		glScalef(0.01, 0.01, 0.01); // Resize the Garbage model
		garbage.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-10.0, 2.0, 17.0); // Garbage 3 position
		glScalef(0.01, 0.01, 0.01); // Resize the Garbage model
		garbage.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(20.0, 2.0, 5.0); // Garbage 4 position
		glScalef(0.01, 0.01, 0.01); // Resize the Garbage model
		garbage.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-18.0, 2.0, -20.0); // Garbage 5 position
		glScalef(0.01, 0.01, 0.01); // Resize the Garbage model
		garbage.Draw();
		glPopMatrix();


		displayHealthLabel(); // Display the health label
		displayHealthValue(health);


		displayScoreLabel(); // Display the score label
		displayScoreValue(score);
		if (health == 0) {
			if (!isFirstPersonView) {
				glColor3f(1.0, 1.0, 1.0); // White color for text
				glRasterPos3f(-1.5, 8, -5); // Adjust position for the label
				const char* label = "You Lost: "; // Your health label
				for (int i = 0; label[i] != '\0'; i++) {
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
				}
			}
			else {
				glColor3f(1.0, 1.0, 1.0); // White color for text
				glRasterPos3f(-0.5, 4, -1.5); // Adjust position for the label
				const char* label = "You Lost: "; // Your health label
				for (int i = 0; label[i] != '\0'; i++) {
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
				}

			}
		}


		// Render the scene again after collision handling

		//End of environment 1
		// 
			//sky box
		glPushMatrix();

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);


		glPopMatrix();

		if (isFirstPersonView) {
			displayHealthLabel1(); // Display the health label
			displayHealthValue1(health);
			displayScoreLabel1(); // Display the score label
			displayScoreValue1(score);

		}
		else {
			displayHealthLabel(); // Display the health label
			displayHealthValue(health);
			displayScoreLabel(); // Display the score label
			displayScoreValue(score);
		}

		glutSwapBuffers();
	}


	else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




		GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
		GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

		glLoadIdentity();
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

		lightAngle += 0.5f; // Adjust the speed of rotation as needed

		// Set the position of the light source to rotate around a point (0, 0, 0)
		GLfloat light_position[] = {
			10.0f * cos(lightAngle),  // X position
			10.0f,                    // Y position (elevated)
			10.0f * sin(lightAngle),  // Z position
			1.0f                      // Homogeneous coordinate
		};
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		// Draw Ground
		RenderGround2();


		//Environment 1 
		 //Boy
		 // Boy
		glPushMatrix();
		glTranslated(boyPositionX, boyPositionY, boyPositionZ);

		// Calculate the angle of rotation based on player direction
		float rotationAngle = -atan2(playerDirectionX, playerDirectionZ) * 180.0 / 3.14;


		// Apply rotation
		glRotatef(rotationAngle, 0.0, 1.0, 0.0);

		// Scale and draw the player model
		glScalef(3, 3, 3);
		if (isplayervisiblee) {
			Boy.Draw();
		}

		glPopMatrix();
		/*	if (isJumping) {
				// Update the boy's Y position while jumping
				boyPositionY += 0.1; // You can adjust the rate of ascent here
				// You can add additional conditions to control the jump height or duration
			}

			*/




		float table1 = distance(boyPositionX, boyPositionY, boyPositionZ, 2.0, 1.0, 12.0);
		float table2 = distance(boyPositionX, boyPositionY, boyPositionZ, -8.0, 1.0, -8.0);
		float table3 = distance(boyPositionX, boyPositionY, boyPositionZ, 15.0, 1.0, 15.0);
		float bench1 = distance(boyPositionX, boyPositionY, boyPositionZ, -8.0, 1.0, 2.0);
		float bench2 = distance(boyPositionX, boyPositionY, boyPositionZ, 15.0, 1.0, 10.0);
		float bench3 = distance(boyPositionX, boyPositionY, boyPositionZ, -12.0, 1.0, -12.0);
		float locker1 = distance(boyPositionX, boyPositionY, boyPositionZ, 10.0, 1.0, 5.0);
		float locker2 = distance(boyPositionX, boyPositionY, boyPositionZ, -10.0, 1.0, -10.0);
		float locker3 = distance(boyPositionX, boyPositionY, boyPositionZ, 15.0, 1.0, 10.0);
		float chair1 = distance(boyPositionX, boyPositionY, boyPositionZ, 16.0, 1.0, 5.0);
		float chair22 = distance(boyPositionX, boyPositionY, boyPositionZ, 13.0, 1.0, 5.0);


		float goal = distance(boyPositionX, boyPositionY, boyPositionZ, -12, 1.0, -28.0);

		if (goal < collisionThreshold) {

			glColor3f(1.0, 1.0, 1.0); // White color for text
			glRasterPos3f(-12, 0.5, -23.0); // Adjust position for the label
			const char* label = "You Win: "; // Your health label
			for (int i = 0; label[i] != '\0'; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
			}
		}

		if (chair1 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}

		if (chair22 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}

		if (table1 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (table2 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (table3 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (bench1 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (bench2 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (bench3 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (locker1 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (locker2 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (locker3 < collisionThreshold) {
			boyPositionX = 0;
			boyPositionY = 0;
			boyPositionZ = 0;
			sndPlaySound(TEXT("sound1.wav"), SND_ASYNC);
			if (health > 0) {
				health -= 10;
			}
			else {
				health = 0;
			}
			Alternate();
		}
		if (health == 0) {
			if (!isFirstPersonView) {
				glColor3f(1.0, 1.0, 1.0); // White color for text
				glRasterPos3f(-1.5, 8, -5); // Adjust position for the label
				const char* label = "You Lost: "; // Your health label
				for (int i = 0; label[i] != '\0'; i++) {
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
				}
			}
			else {
				glColor3f(1.0, 1.0, 1.0); // White color for text
				glRasterPos3f(-0.5, 4, -1.5); // Adjust position for the label
				const char* label = "You Lost: "; // Your health label
				for (int i = 0; label[i] != '\0'; i++) {
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
				}

			}
		}

		// Render multiple Lockers
		glPushMatrix();
		glTranslatef(10.0, 1.0, 5.0);
		glScalef(0.01, 0.01, 0.01);
		Lockers.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(13.0, 1.0, 5.0);
		glScalef(0.004, 0.004, 0.004);
		Chair.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(16.0, 1.0, 5.0);
		glScalef(0.004, 0.004, 0.004);
		Chair.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslatef(-10.0, 1.0, -10.0);
		glScalef(0.01, 0.01, 0.01);
		Lockers.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(15.0, 1.0, 10.0);
		glScalef(0.01, 0.01, 0.01);
		Lockers.Draw();
		glPopMatrix();

		/* // Render multiple Papers
		glPushMatrix();
		glTranslatef(-4.0, 1.0, -8.0);
		glScalef(0.001, 0.001, 0.001);
		Papers.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(8.0, 1.0, 12.0);
		glScalef(0.001, 0.001, 0.001);
		Papers.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-12.0, 1.0, 4.0);
		glScalef(0.001, 0.001, 0.001);
		Papers.Draw();
		glPopMatrix();
		*/
		// Render multiple Tables
		glPushMatrix();
		glTranslatef(2.0, 0.0, 12.0);
		glScalef(0.003, 0.003, 0.003);
		Table.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-8.0, 0.0, -8.0);
		glScalef(0.003, 0.003, 0.003);
		Table.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(15.0, 0.0, 15.0);
		glScalef(0.003, 0.003, 0.003);
		Table.Draw();
		glPopMatrix();
		/*
				// Render multiple Keys
				glPushMatrix();
				glTranslatef(10.0, 1.0, -5.0);
				glScalef(0.04, 0.04, 0.04);
				Keys.Draw();
				glPopMatrix();

				glPushMatrix();
				glTranslatef(-15.0, 1.0, 10.0);
				glScalef(0.04, 0.04, 0.04);
				Keys.Draw();
				glPopMatrix();

				glPushMatrix();
				glTranslatef(5.0, 1.0, -15.0);
				glScalef(0.04, 0.04, 0.04);
				Keys.Draw();
				glPopMatrix();
				*/
				// Render multiple Benches
		glPushMatrix();
		glTranslatef(-8.0, 1.0, 2.0);
		glScalef(0.9, 0.9, 0.9);
		Bench.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(15.0, 1.0, 10.0);
		glScalef(0.9, 0.9, 0.9);
		Bench.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-12, 1, -28);
		glScalef(0.5, 0.5, 0.5);
		Door.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-12.0, 1.0, -12.0);
		glScalef(0.9, 0.9, 0.9);
		Bench.Draw();
		glPopMatrix();




		for (int i = 0; i < 3; ++i) {
			if (paperVisible[i]) {
				float dist = distance(boyPositionX, boyPositionY, boyPositionZ, paperPositions[i][0], paperPositions[i][1], paperPositions[i][2]);
				if (dist < collisionThreshold) {
					sndPlaySound(TEXT("sound2.wav"), SND_ASYNC);
					paperVisible[i] = false;
					score += 10;// Mark the collided coin as not visible
				}
				else {
					glPushMatrix();
					glTranslatef(paperPositions[i][0], paperPositions[i][1], paperPositions[i][2]);
					// Draw the coin model at its position
					glScalef(0.001, 0.001, 0.001);
					Papers.Draw(); // Replace this line with your coin drawing code
					glPopMatrix();
				}
			}
		}
		for (int i = 0; i < 3; ++i) {
			if (keyVisible[i]) {
				float dist = distance(boyPositionX, boyPositionY, boyPositionZ, keyPositions[i][0], keyPositions[i][1], keyPositions[i][2]);
				if (dist < collisionThreshold) {
					sndPlaySound(TEXT("sound2.wav"), SND_ASYNC);
					keyVisible[i] = false;
					score += 10;// Mark the collided coin as not visible
				}
				else {
					glPushMatrix();
					glTranslatef(keyPositions[i][0], keyPositions[i][1], keyPositions[i][2]);
					// Draw the coin model at its position
					glScalef(0.04, 0.04, 0.04);
					Keys.Draw(); // Replace this line with your coin drawing code
					glPopMatrix();
				}
			}
		}








		if (isFirstPersonView) {
			displayHealthLabel1(); // Display the health label
			displayHealthValue1(health);
			displayScoreLabel1(); // Display the score label
			displayScoreValue1(score);

		}
		else {
			displayHealthLabel(); // Display the health label
			displayHealthValue(health);
			displayScoreLabel(); // Display the score label
			displayScoreValue(score);
		}


		glPushMatrix();

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex2);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);


		glPopMatrix();



		glutSwapBuffers();
	}
}


//=======================================================================
// Keyboard Function
//=======================================================================
// Variables to track player's direction


void mySpecialKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		// Move player forward
		boyPositionZ -= movementSpeed;
		playerDirectionX = 0.0f;
		playerDirectionZ = 1.0f;
		Eye.z -= movementSpeed; // Change this according to your camera speed
		At.z -= movementSpeed;// Facing negative z-axis
		break;
	case GLUT_KEY_DOWN:
		// Move player backward
		boyPositionZ += movementSpeed;
		playerDirectionX = 0.0f;
		playerDirectionZ = -1.0f; // Facing positive z-axis
		Eye.z += movementSpeed; // Change this according to your camera speed
		At.z += movementSpeed;
		break;
	case GLUT_KEY_LEFT:
		// Move player left
		boyPositionX -= movementSpeed;
		playerDirectionX = -1.0f; // Facing negative x-axis
		playerDirectionZ = 0.0f;
		Eye.x -= movementSpeed; // Change this according to your camera speed
		At.x -= movementSpeed;
		break;
	case GLUT_KEY_RIGHT:
		// Move player right
		boyPositionX += movementSpeed;
		playerDirectionX = 1.0f; // Facing positive x-axis
		playerDirectionZ = 0.0f;
		Eye.x += movementSpeed; // Change this according to your camera speed
		At.x += movementSpeed;

		break;

	default:
		break;
	}
	if (boyPositionX > 50 || boyPositionX < -50 || boyPositionZ > 50 || boyPositionZ < -50) {
		boyPositionX = 0.0f;
		boyPositionY = 0.0f;
		boyPositionZ = 0.0f;
		Alternate();
	}
	// Update the player's orientation here based on the direction of movement

	glutPostRedisplay();
}



void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;

	case 27:
		exit(0);
		break;

	case 'f':
		isFirstPersonView = false;
		isplayervisiblee = true;
		Alternate();

		break;

	case 'y':
		isplayervisiblee = true;
		isFirstPersonView = false;
		Alternate();

		break;


	default:
		break;
	}

	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
/*void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}
*/

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}







//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	// Environment 1


	Boy.Load("Models/Boy/Boy.3DS");
	Car.Load("Models/car/car.3ds");
	Cake.Load("Models/cake/cake.3ds");
	Coin.Load("Models/coin/coin.3ds");
	House.Load("Models/house/house.3ds");
	LightSource.Load("Models/LightS/light.3ds");
	garbage.Load("Models/garbage/garbage.3ds");



	// Environment 2

	Lockers.Load("Models/Lockers/locker.3ds");
	Papers.Load("Models/Paper/Paper.3ds");
	Table.Load("Models/table2/table.3ds");
	Keys.Load("Models/keys/key.3ds");
	Bench.Load("Models/Bench/bench.3DS");
	Door.Load("Models/Door/door.3ds");
	Chair.Load("Models/chair2/chair.3ds");

	tex_ground2.Load("Textures/indoor3.bmp");
	loadBMP(&tex2, "Textures/walllll.bmp", true);

	tex_ground.Load("Textures/street.bmp");
	loadBMP(&tex, "Textures/sky2.bmp", true);










}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(mySpecialKeyboard);



	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	//InitLightSource();
	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}