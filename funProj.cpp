//TODO; implement RGB type in cube class, clean up drawing funcs to make more useful
//TODO; make sure using picknewpath(isRand) correctly when
//todo; looks like when move food with'z', cubes can no longer find food? or smell it? or foodpos invalid? dono...
//TODO; USE void glutKeyboardUpFunc ( void (*func) ( unsigned char key, int x, int y ) ); IN CAM
//TODO; Combine classes esssential to game run(CubeGod, Skybox, animator, ....)


//#include <windows.h>
#include <time.h>               // otherwise get: "clock_t does not name a type", followed by several .... not declared in this scope errors
#include <stdio.h>              // for sprintf
#include <string.h>				// needed for strlen
//#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
//#include <glaux.h>            // This isn't needed. Apparently its outdated and now unsupported, if need bmp load, use nehe glaux replcaement
#include <iostream>             // cout, cin; used to be ....h but eclipse doesnt like that..
#include <fstream>              // files
#include <math.h>
//#include <SFML/Audio.hpp>	// Under windows
#include <SFML/Audio.h>		// Under debian	


using namespace std;
/*****VARIABLES********************************************
*************-*-*-*-*VARIABLES*-*-*-*-*********************
***********************************************************/
#define PI 3.14159265358979
#define EXC "\n(!!)"                                             //EXCELMATION
#define GDD "\n(:D)"											 //GOOD
#define INS "\n(->)"											 //insert?
#define WINWidth 1000											 // dimensions of window
#define WINHeight 700
#define VIEW_DISTANCE 150000										// how far is clipping plane
#define GAME_MAP_SIZE_X 2000                                      	// map size, width/aka X val
#define GAME_MAP_SIZE_Z 2000                                      	// map size, height/aka Z val


int SHADE = 0, gameFoodLocZ, gameFoodLocX, gameMaterialLocX, gameMaterialLocZ;// global food location var

bool fullscreen = false, perspective = true, mousePos[4] = {0}; // Mouse pos; if y < 50, [0] = 1, if x >WINWidth - 50, [1] = 1
int mousePosX = 0, mousePosY = 0, resizeWINWidth = WINWidth, resizeWINHeight = WINHeight;
float ptSize = 0, lineWth = 0;
float xrot = 0.0, yrot = 0, xfa = 0.1, yfa = 0.1;                //Rotation values, and increment factor. (to be increase/decreased)
float lightAmbient[] = { 0.2f, 0.3f, 0.6f, 1.0f };               //Light colour values.
float lightDiffuse[] = { 0.2f, 0.3f, 0.6f, 1.0f };
int polygonMode[2] = {0, 0};
char a;

int cursorX = 0, cursorY = 0;

/*****USR*****************************************************
*************-*-*-*-*USR*-*-*-*-********************
***********************************************************/
#include "ENDIAN.H"
//#include "TGA Loader.h"		// dont need to include in main, needed for skybox only
#include "CommonTypes.h"
#include "MFont.h"
#include "CGUI.h"
#include "MInput.h"
#include "CCamera.h"            // Game camera containing class
#include "MCube.h"				// Class to govern the cubes
#include "MTiming.h"			// Class to govern the cubes
#include "MSkyBox.h"            // Header contains class which loads, renders skybox
//#include <CSound.h>				// CGameSound



/*****HIGH LEVEL VARIABLES*********************************
*************-*-*-*-*VARIABLES*-*-*-*-*********************
***********************************************************/
CKeyboard playerKeyboard;
CCubeGod MainGameCubeGod;                                       // class creates/destroys cube list
CFixedPitchCamera MainGameCamera(GAME_MAP_SIZE_X, 0, GAME_MAP_SIZE_Z);
/*****DEFINITIONS*******************************************
*************-*-*-*-*DEFINITIONS*-*-*-*-********************
***********************************************************/
void inputDistributor(void)
{
	if(playerKeyboard.queryNormalKey(119))	// W
	{
		MainGameCamera.Move(CAMERA_MOVE_FORWARD);
	}
	if(playerKeyboard.queryNormalKey(115))	// S
	{
		MainGameCamera.Move(CAMERA_MOVE_BACKWARD);
	}
	if(playerKeyboard.queryNormalKey(100))	// D
	{
		MainGameCamera.Move(CAMERA_MOVE_RIGHT);
	}
	if(playerKeyboard.queryNormalKey(97))	// A
	{
		MainGameCamera.Move(CAMERA_MOVE_LEFT);
	}

	// Settings:
	if(playerKeyboard.queryNormalKey(99))	// c
	{
		if(SHADE == 0)
			SHADE = 1;
		else
			SHADE = 0;
	}
	if(playerKeyboard.queryNormalKey(112))	// p
	{
		if(perspective == true)
			perspective = false;
		else
			perspective = true;
	}
	if(playerKeyboard.queryNormalKey(113))	// q
	{
		if (glIsEnabled(GL_DEPTH_TEST))
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
		glutPostRedisplay();
	}
	if(playerKeyboard.queryNormalKey(45))	// -
	{
	}
	if(playerKeyboard.queryNormalKey(61))	// =
	{
	}

	if(playerKeyboard.querySpecialKey(GLUT_KEY_RIGHT))
	{
		MainGameCamera.Yaw(0.1);
	}
	if(playerKeyboard.querySpecialKey(GLUT_KEY_LEFT))
	{
		MainGameCamera.Yaw(-0.1);
	}
	if(playerKeyboard.querySpecialKey(GLUT_KEY_UP))
	{
		MainGameCamera.Pitch(0.1);
	}
	if(playerKeyboard.querySpecialKey(GLUT_KEY_DOWN))
	{
		MainGameCamera.Pitch(-0.1);
	}

	// Autorotate:
	MainGameCamera.processCamera(mousePosX, mousePosY);
}

void drawEnvironment(void)													// Costs approx 350 fps
{
	// Draw Sky:
	glColor3f(1.0, 1.0, 1.0);
	MainGameSkyBox.renderSkyBox(MainGameCamera.camPos.x, MainGameCamera.camPos.y, MainGameCamera.camPos.z);

	/*
	   for(int a = GAME_MAP_ZIZE_W; a >= -1*(GAME_MAP_ZIZE_W); a -= 10)
	   {
	// Draw line horizontally:
	glBegin(GL_LINES);
	glVertex3i(-1*(GAME_MAP_ZIZE_W), 0, a);
	glVertex3i(GAME_MAP_ZIZE_W, 0, a);
	glEnd();

	// Draw line vertically
	glBegin(GL_LINES);
	glVertex3i(a, 0, GAME_MAP_ZIZE_W);
	glVertex3i(a, 0, -1*(GAME_MAP_ZIZE_W));
	glEnd();
	}*/

	//   glPushMatrix();
	//   glTranslatef();
	//   glPopMatrix();

	// Draw grid
	// Make sure colour ..
	glColor3f(0.0, 0.0, 0.0);
	glColor3f(0.0, 0.0, 0.0);
	DrawGrid(GAME_MAP_SIZE_Z, GAME_MAP_SIZE_X, 50);

	// Draw Sectors:
	glColor3f(1.0, 0.0, 0.0);   // Red colour
	glPushMatrix();
	glRotatef(90, 1, 0, 0); // Rotate 90 deg via xaxis, end up parallel to xz plane
	DrawStencilRect(0, 0, GAME_MAP_SIZE_X/2, GAME_MAP_SIZE_Z/2);
	glPopMatrix();

	glColor3f(1.0, 0.0, 0.0);   // Red colour
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	DrawStencilRect(GAME_MAP_SIZE_X/2, 0, GAME_MAP_SIZE_X/2, GAME_MAP_SIZE_Z/2);
	glPopMatrix();

	glColor3f(1.0, 0.0, 0.0);   // Red colour
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	DrawStencilRect(0, GAME_MAP_SIZE_Z/2, GAME_MAP_SIZE_X/2, GAME_MAP_SIZE_Z/2);
	glPopMatrix();

	glColor3f(1.0, 0.0, 0.0);   // Red colour
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	DrawStencilRect(GAME_MAP_SIZE_X/2, GAME_MAP_SIZE_Z/2, GAME_MAP_SIZE_X/2, GAME_MAP_SIZE_Z/2);
	glPopMatrix();

	// Set colour back to green:
	glColor3f(1.0, 0.0, 0.0);
}

void setFoodPos(bool isRand, int foodPosX, int foodPosZ)				// Picks random food location
{
	if(isRand)
	{
		// Get num from 1 - 10:
		gameFoodLocZ = rand()%GAME_MAP_SIZE_Z + 1;
		gameFoodLocX = rand()%GAME_MAP_SIZE_X + 1;
	}
	else
	{
		gameFoodLocZ = foodPosZ;
		gameFoodLocX = foodPosX;
	}

// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
//	cout << INS << "Food Pos: Z: " << gameFoodLocZ << " X: " << gameFoodLocX;
}

void setMaterialPos(bool isRand, int materialPosX, int materialPosZ)		// Picks random food location
{
	if(isRand)
	{
		// Get num from 1 - 10:
		gameMaterialLocZ = rand()%GAME_MAP_SIZE_Z + 1;
		gameMaterialLocX = rand()%GAME_MAP_SIZE_X + 1;
	}
	else
	{
		gameMaterialLocZ = materialPosZ;
		gameMaterialLocX = materialPosX;
	}

// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
//	cout << INS << "Food Pos: Z: " << gameFoodLocZ << " X: " << gameFoodLocX;
}

void drawGameResources(void)									// fps cost?
{
	static float rot = 0;
/*
	glPushMatrix();
		glTranslated(gameFoodLocZ, 0, gameFoodLocW);
		glutSolidSphere(1,10,10);										// 10: slices, 10: stacks
	glPopMatrix();

	glPushMatrix();
		glTranslated(gameFoodLocZ + 2, 0, gameFoodLocW);
		glutSolidTorus(0.2,0.8,10,10);
	glPopMatrix();

	glPushMatrix();
		glTranslated(gameFoodLocZ, 0, gameFoodLocW - 2);
		glutWireSphere(1,10,10);
	glPopMatrix();

	glPushMatrix();
		glTranslated(gameFoodLocZ, 0, gameFoodLocW + 2);
		glutWireCone(1,1,10,10);
	glPopMatrix();
*/

	// Draw food:
    glColor3f(0, 0, 0);
    glColor3f(0.3, 1, 0);
	glPushMatrix();
		glTranslated(gameFoodLocX + 1, 0, gameFoodLocZ + 1);			// translate it +1 so not all ontop eachother
		glRotatef(rot, -rot, rot*2,1.0);
		glutWireTorus(2,5,10,10);                                     // innerradius, outerradius, sides, rings
	glPopMatrix();

    glPushMatrix();
		glTranslated(gameFoodLocX - 2, 0, gameFoodLocZ);
		glRotatef(-rot, rot, rot*3,1.0);
        glutSolidCone(5,10,10,10);                                    // base, height, slices, stacks
	glPopMatrix();

    glColor3f(0, 0, 0);
    glColor3f(0, 1, 0);
	glPushMatrix();
		glTranslated(gameFoodLocX, 0, gameFoodLocZ);
		glRotatef(90, 1, 0, 0);
		//glRotatef(rot, 0, rot,0);
        glutSolidCone(CUBE_SMELL_SENSITIVITY,1,10,10);                                    // base, height, slices, stacks
	glPopMatrix();

	// Draw materials:
    glColor3f(0, 0, 0);
    glColor3f(0.9, 0.5, 0.5);
	glPushMatrix();
		glTranslated(gameMaterialLocX + 1, 0, gameMaterialLocZ + 1);	// translate it +1 so not all ontop eachother
		glRotatef(rot, -rot, rot*2,1.0);
		glutWireTorus(2,5,10,10);                               	      // innerradius, outerradius, sides, rings
	glPopMatrix();

    glPushMatrix();
		glTranslated(gameMaterialLocX - 2, 0, gameMaterialLocZ);
		glRotatef(-rot, rot, rot*3,1.0);
		glutWireSphere(1,10,10);
	glPopMatrix();

    glColor3f(0, 0, 0);
    glColor3f(0.5, 0.5, 0.5);
	glPushMatrix();
		glTranslated(gameMaterialLocX, 0, gameMaterialLocZ);
		glRotatef(90, 1, 0, 0);
		//glRotatef(rot, 0, rot,0);
        glutSolidCone(CUBE_SIGHT_RADIUS,1,10,10);               // base, height, slices, stacks
	glPopMatrix();

	// increment rot var, keep rotating
	rot += 0.1;
}

/*****INITIALIZE*******************************************************************************************
**************************-*-*-*-*INITIALIZE*-*-*-*-***************************************************
*******************************************************************************************************/

bool init()
{                                            // Sets the clearing colour, whenever window is cleared
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // it will be this color
    glColor3f(1.0f, 0.0f, 0.0f);             //this will set the drawing colour


    glGetFloatv(GL_POINT_SIZE, &ptSize);     /*Call to glGetFloatv, and pass point size attrib.**Need this value later */
                                             /*to retrive the point size and store in ptSize.  **when we restore it back to origonal.*/

    glGetFloatv(GL_LINE_WIDTH, &lineWth);    //Gets current line width.

    glGetIntegerv(GL_POLYGON_MODE, polygonMode); //Gets polygon mode with that attrib, stores in a two
                                                 //dimensional array. For later when we restore it back.
/****DEPTH***************************************************************************************************
**********DEPTH************************************************************************************/
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	// Seed random # generator.
	srand( (unsigned int) clock());


	cout << INS << "Creating cubes...";
	MainGameCubeGod.createCubes(NUMCUBES);          // Fill master cube list
	MainGameCubeGod.initCubes();                    // fill sector lists
	MainGameCubeGod.tribeInit();                    // set initial tribe vals
	cout << "Done.";

    // Load game skybox:
    cout << INS << "Load skybox...";
	MainGameSkyBox.loadSkyBox();
	cout << INS << "Done.";

	// Load sound:
	cout << INS << "Load sound...";
//	MainGameSound.loadAmbient("Game Files\\Sound\\ambient.ogg");
//	MainGameSound.playAmbient();
	cout << INS << "Done.";

	// Pick food location:
	setFoodPos(true,0,0);
	// Pick food location:
	setMaterialPos(true,0,0);


/*	// ASM TEST
 * 	// AT&T syntax default:
//	asm("mov %ah, %bh");
 *
	asm(
			".intel_syntax noprefix\n" //must add because otherwise must put % infront of registers
//			"mov ebx, _row\n"			// access global vars with "_", must be global
//			"mov eax, _boat\n"			// load eax with address of boat
//			"add eax, ebx\n"
			"mov ah, 02"				//; select interrupt
			"MOV DL,42"	    			//; Character to output
			"INT 21h"					//; Call the interrupt to output "!"
			".att_syntax\n"
		);
*/

	return true;                             //Tell program things went well.
}

void Shutdown()
{
    glPointSize(ptSize);                     //Restore to default size.
    glLineWidth(lineWth);                    //Restore to default width.
    glPolygonMode(GL_FRONT, polygonMode[0]); //Restore this part of mode back to origonal.
	glPolygonMode(GL_BACK, polygonMode[1]);  //And the second.
    //Do other stuff

	glutLeaveMainLoop();					 // tell freeglut to clean up and close down.
   // exit(0);								 // if use freeglut, wont allow it to deallocate and cleanup..
}

void display()                               //Main function that will draw.
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     /*Clear window to previously defined colour ^^^.*/
    glLoadIdentity();                        /*Also specifies what to clear.(COLOR BUFFER)   */

    // CAMERA; specify campos, point to look at, and up vector(?)
    gluLookAt(
		MainGameCamera.camPos.x, MainGameCamera.camPos.y, MainGameCamera.camPos.z,
		MainGameCamera.viewPt.x, MainGameCamera.viewPt.y, MainGameCamera.viewPt.z,
		0.0f, 1.0f, 0.0f);

/*    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -5.0f);
            DrawTriangle();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(5.0f, 0.0f, 0.0f);
        glRotatef(-15.0, 0.0,0.0,1.0);
            DrawTriangle();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-5.0f, 0.0f, 0.0f);
        glRotatef(15.0, 0.0,0.0,1.0);
            DrawTriangle();
    glPopMatrix();
*/
/*    glTranslatef(5.0, 0.0, 0.0);
		DrawTriangle();
*/

	// Draw environment:
	drawEnvironment();

    glColor3f(1, 0, 0);
    glPushMatrix();
        glTranslatef(5.0, 0.0, 0.0);
        glScalef(50,50,50);
        glutWireRhombicDodecahedron();			// freeglut
    //	glutSolidRhombicDodecahedron();			// freeglut
	glPopMatrix();

	// Draw all cubes
	glColor3f(1, 1, 1);
	MainGameCubeGod.cubeLifeCycleandRender();

	// Run tribe routines
	MainGameCubeGod.tribeLifeCycle();

	// Draw tribe artifacts:
	glColor3f(1, 1, 1);
	MainGameCubeGod.tribeRenderArtifacts();

	// Draw food:
	glColor3f(0.5, 0.8, 0.3);
	drawGameResources();

	// Draw strings.
	DrawStrings(resizeWINWidth, resizeWINHeight, VIEW_DISTANCE);

	// Handle input;
	inputDistributor();

	//WaitForQSec(0.01);                              // brings fps from 2000 to 400

	//glutWarpPointer(WINWidth/2, WINHeight/2);

//	glFlush();                               		// Ensures all commands processed  , single buffering
//	glFinish();										// same as glFlush() but this is blocking, doesnt fix flicekring.. apparently useless under double buffers
	glutSwapBuffers();								// needed for double buffering (GLUT_DOUBLE)
	//glutPostRedisplay();							// fix flickering? doesnt do anything
}

void passiveMouse(int x, int y)
{
		// Set current cursor pos
		mousePosX = x;
		mousePosY = y;

		// Auto rotate:
		for(int t = 0; t < 4; t++)
				mousePos[t] = false;

		// Set mouse pos
		if(y < 50)															// Extreme top edge
				mousePos[0] = true;
		if(x > (WINWidth - 50))												// Extreme right edge
				mousePos[1] = true;
		if(y > (WINHeight - 50))											// Extreme bottom edge
				mousePos[2] = true;
		if(x < 50)															// Extreme left edge
				mousePos[3] = true;
}

void mouse(int button, int state, int x, int y)
{
		int mod;

		/*
		   if(SELECT == 1)
		   {
		   if(button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		   return;

		// Set:
		cursorX = x;
		cursorY = y;

		MainGameMenu.pickStartMenu();                                   // Trigger picking.
		}
		*/

		if (button == GLUT_RIGHT_BUTTON)
		{
				mod = glutGetModifiers();

				if (state == GLUT_DOWN)
				{
						cout << "Right button pressed" << endl;

						if(mod == GLUT_ACTIVE_CTRL)
								cout << INS << "mouse click and ctrl";
						else
								cout << INS << "mouse click and no ctrl";
				}
				else
				{
						cout << "Right button lifted "
								<< "at (" << x << "," << y
								<< ")" << endl;
				}
		}
		else if (button == GLUT_LEFT_BUTTON)
		{
				mod = glutGetModifiers();
				if (state == GLUT_DOWN)
				{
						cout << "Left button pressed" << endl;

						if(mod == GLUT_ACTIVE_CTRL)
								cout << INS << "mouse click and ctrl";
						else
								cout << INS << "mouse click and no ctrl";
				}
				else
				{
						cout << "Left button lifted "<< "at (" << x << "," << y	<< ")" << endl;
				}
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{
				mod = glutGetModifiers();

				if(mod == GLUT_ACTIVE_CTRL)
						cout << INS << "mouse click and ctrl";
				else
						cout << INS << "mouse click and no ctrl";
		}

		cout << "{{X:}}" << x << endl;
		cout << "{{Y:}}" << y << endl;
}

void resize(int w, int h)
{
	resizeWINWidth = w;										 // dimensions of window
	resizeWINHeight = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if(w == 0 || h == 0)					    //Prevent division by zero, cant make a window of 0 width..
	{
		w = 50;
		h = 50;
	}

    // Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,(float)w/h,1,VIEW_DISTANCE);

/*
    if(perspective == true)
        gluPerspective(45.0f, w / h, 20.0f, 250.0f);
    else
        glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);
*/

	glMatrixMode(GL_MODELVIEW);
}

void idle()
{
	xrot =+ 0.20f;
	glutPostRedisplay();
}

void normalKeyDown(unsigned char key, int x, int y)      					//key pressed, and mouse pos when pressed.
{
	switch(key)
	{
    case 27:															// (ESC)
        Shutdown();                        								// Exit.
        break;
	case 45:    // -                                                    // Zoom in.
        MainGameCamera.scalePos(-2);
        MainGameSkyBox.editSkyboxScaleFac(0.5);							// Dont want map clipped by skybox so scale it
        playerKeyboard.updateNormalKey(45, Key_DOWN);
        break;
    case 61:    // +                                                    // Zoom out.
        MainGameCamera.scalePos(2);
        MainGameSkyBox.editSkyboxScaleFac(3);							// Dont want map clipped by skybox so scale it
        playerKeyboard.updateNormalKey(61, Key_DOWN);
        break;
	case 97://a
        playerKeyboard.updateNormalKey(97, Key_DOWN);
        break;
    case 99://c
    	playerKeyboard.updateNormalKey(99, Key_DOWN);
        break;
    case 100://d
        playerKeyboard.updateNormalKey(100, Key_DOWN);
		break;
    case 101://e
    	playerKeyboard.updateNormalKey(101, Key_DOWN);
    	break;
	case 112://p
        playerKeyboard.updateNormalKey(112, Key_DOWN);
        break;
    case 113: //q
        playerKeyboard.updateNormalKey(113, Key_DOWN);
        break;
	case 115://s
        playerKeyboard.updateNormalKey(115, Key_DOWN);
        break;
    case 119://w
        playerKeyboard.updateNormalKey(119, Key_DOWN);
        break;
	case 120://x
		setMaterialPos(true,0,0);          //  Pick a new random spot for the material.
		playerKeyboard.updateNormalKey(120, Key_DOWN);
		break;
	case 122://	z
		setFoodPos(true,0,0);              //  Pick a new random spot for the food.
		playerKeyboard.updateNormalKey(122, Key_DOWN);
		break;
	}
}

void specialKeyDown(int key, int x, int y)   //Used for special keys like F1., X,Y OF MOUSE AT PRESS OF KEY
{
	if (key == GLUT_KEY_F1)                   //Check if F1 pressed.
	{
		fullscreen = !fullscreen;             //If F1 pressed, toggle boolean value.

		if (fullscreen)                       //If true, call glut's fullscreen func.
		{
			glutFullScreen();
		}
		else                                  //If not true, make window to default size, and pos.
		{
	//		glutReshapeWindow(WINWidth, WINHeight);      //Resizes.
			glutPositionWindow(50, 50);       //Repositions.
		}
		playerKeyboard.updateSpecialKey(GLUT_KEY_F1, Key_DOWN);
	}
	else if(key == GLUT_KEY_RIGHT)            // Detect keyboard arrow keys and rotate about y or x axis
	{
        MainGameCamera.Yaw(2);
        playerKeyboard.updateSpecialKey(GLUT_KEY_RIGHT, Key_DOWN);
	}
    else if(key == GLUT_KEY_LEFT)
	{
	    MainGameCamera.Yaw(-2);
        playerKeyboard.updateSpecialKey(GLUT_KEY_LEFT, Key_DOWN);
	}
    else if(key == GLUT_KEY_UP)
	{
	    MainGameCamera.Pitch(2);
	    playerKeyboard.updateSpecialKey(GLUT_KEY_UP, Key_DOWN);
	}
    else if(key == GLUT_KEY_DOWN)
	{
        MainGameCamera.Pitch(-2);
	    playerKeyboard.updateSpecialKey(GLUT_KEY_DOWN, Key_DOWN);
	}
    else if(key == GLUT_KEY_F11)
    {
    	//if(glutGet(GLUT_FULL_SCREEN) == 1)
    	glutFullScreenToggle();
    }
}

void normalKeyUp(unsigned char key, int x, int y)
{
	playerKeyboard.updateNormalKey(key, Key_UP);
}

void specialKeyUp(int key, int x, int y)
{
	playerKeyboard.updateSpecialKey(key, Key_UP);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);                           //Initialize glut.

	glutInitWindowPosition(100, 5);                //Specifies window pos.
	glutInitWindowSize(WINWidth, WINHeight);         //Specifies window size

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_BORDERLESS);     //Type of colour, and buffer configuration, borderless = no min/max/close buttons

	glutCreateWindow("Mohammed -  Fun!"); 	         //Name of window.

	glutDisplayFunc(display);                        /*Following functions include the various       */
	glutKeyboardFunc(normalKeyDown);                      /*functions(display,keyboard, special keyboard) */
	glutSpecialFunc(specialKeyDown);                /*into the main loop                            */
    glutKeyboardUpFunc(normalKeyUp);
    glutSpecialUpFunc(specialKeyUp);
    glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(passiveMouse);
    glutIdleFunc(idle);
	if (!init())                                     //Make sure everything went well in initialization.
		return 1;

	glutMainLoop();                                  //The main loop, displays, checks for input.

	return 0;
}
