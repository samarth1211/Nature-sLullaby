//	Headers
#include<windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/gl.h>
#include<x32/FreeImage.h>

#include "vmath.h"	
#include "resource.h"

#include"Camera.h"

#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "FreeImage.lib")

//	Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define DELTA 0.0166666666666667f

//namespaces
using namespace vmath;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

//	Global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//	Global variables
FILE *gpfile;
HWND ghwnd = NULL;
bool gbActiveWindow = false;
bool gbFullScreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc = NULL;
HGLRC ghrc = NULL;
float mouse_x, mouse_y;
float xpos = 0.0f, ypos = 0.0f;
float x_right, X_left;
float y_up, Y_down;
float z_far, Z_near;
float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialDiffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLuint gShaderProgramObject_q;
GLuint vao;
GLuint vbo;
GLuint textureSmiley;

GLuint mUniform_q;
GLuint vUniform_q;
GLuint pUniform_q;
GLuint r1Uniform_q;
GLuint r2Uniform_q;
GLuint samplerUniform_q;
GLuint colorUniform_q;
GLuint timeElapsedUniform_q;
GLuint alphaTestUniform_q;
GLuint alphaMultiplierUniform_q;
GLuint quadModeUniform_q;
GLuint toggleLightsUniform_q;
GLuint ld_q, kd_q;
GLuint lightPosition_q;
GLuint windUniform_q;

bool gbEnableWind = false;
bool gbToggleLights = false;
bool gbQuadMode = false;

mat4 perspectiveProjectionMatrix;

// Camera Keys
Camera camera(vmath::vec3(0.0f, 0.0f, 5.0f));
GLfloat g_fLastX = WIN_WIDTH / 2;
GLfloat g_fLastY = WIN_HEIGHT / 2;

GLfloat g_DeltaTime = 0.0f;
GLboolean g_bFirstMouse = true;
GLfloat g_fCurrrentWidth;
GLfloat g_fCurrrentHeight;

int myRand()  // Generate a 4 digit pseudo-random integer
{
	static int next = 3251; // Anything you like here - but not
							// 0000, 0100, 2500, 3792, 7600,
							// 0540, 2916, 5030 or 3009.
	next = ((next * next) / 100) % 10000;
	return next;
}

int myRandInRange(int min, int max)  // max-min must be <10000!!
{
	return myRand() % (max + 1 - min) + min;
}

//WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declarations
	int initialize(void);
	void display(void);
	void uninitialize(void);

	//variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("perspective");
	bool bDone = false;
	int iRet = 0;

	//code
	if (fopen_s(&gpfile, "log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log file cannot be created"), TEXT("Error"), MB_OK);
		exit(0);
	}

	//initialization of wndclass structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbWndExtra = 0;
	wndclass.cbClsExtra = 0;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	//register above class 
	RegisterClassEx(&wndclass);

	//create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("programmable geometry shader triangle"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;
	iRet = initialize();
	if (iRet == -1)
	{
		fprintf(gpfile, "ChoosePixelFormat() Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -2)
	{
		fprintf(gpfile, "SetPixelFormat() Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -3)
	{
		fprintf(gpfile, "wglCreateContext() Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -4)
	{
		fprintf(gpfile, "wglMakeCurrent() Failed\n");
		DestroyWindow(hwnd);
	}
	else
		fprintf(gpfile, "Initialize() Succeded\n");

	//show window
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//gameloop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				//update should be called here
			}
			display();
		}
	}
	uninitialize();
	return((int)msg.wParam);
}

//callback function WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declaration
	void resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);

	//variables
	static GLfloat width, height;

	//code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		g_fCurrrentWidth = LOWORD(lParam);
		g_fCurrrentHeight = HIWORD(lParam);

		width = LOWORD(lParam);
		height = HIWORD(lParam);
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_CHAR:
	{
		switch (LOWORD(wParam))
		{
		case 'q':
			if (gbQuadMode == false)
				gbQuadMode = true;
			else
				gbQuadMode = false;
			break;

		case 'l':
			if (gbToggleLights == false)
				gbToggleLights = true;
			else
				gbToggleLights = false;
			break;

		case 'w':
			gbEnableWind = !gbEnableWind;
			break;
		}
	}
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case VK_UP:
			ypos = ypos + 0.5f;
			break;

		case VK_DOWN:
			ypos = ypos - 0.5f;
			break;

		case VK_LEFT:
			xpos = xpos - 0.5f;
			break;

		case VK_RIGHT:
			xpos = xpos + 0.5f;
			break;

		case 0x46:
			ToggleFullScreen();
			break;

		case 0x41:// A is pressed
			camera.ProcessKeyBoard(E_LEFT, DELTA);
			break;
		case 0x44:// D is pressed
			camera.ProcessKeyBoard(E_RIGHT, DELTA);
			break;
		case 0x57:// W is pressed
			camera.ProcessKeyBoard(E_FORWARD, DELTA);
			break;
		case 0x53:// S is pressed
			camera.ProcessKeyBoard(E_BACKARD, DELTA);
			break;
		}
		break;

	case WM_MOUSEMOVE: // g_fLastX  g_fLastY
	{
		GLfloat xPos = LOWORD(lParam);
		GLfloat yPos = HIWORD(lParam);

		if (g_bFirstMouse)
		{
			g_fLastX = xPos;
			g_fLastY = yPos;

			g_bFirstMouse = false;
		}

		GLfloat xOffset = xPos - g_fLastX;
		GLfloat yOffset = g_fLastY - yPos;

		g_fLastX = xPos;
		g_fLastY = yPos;

		/*g_fLastX = g_fCurrrentWidth / 2;
		g_fLastY = g_fCurrrentHeight / 2;*/

		camera.ProcessMouseMovements(xOffset, yOffset);
	}
	break;

	case WM_MOUSEWHEEL:
	{
		GLfloat xPos = LOWORD(lParam);
		GLfloat yPos = HIWORD(lParam);
		camera.ProcessMouseScrool(xPos, yPos);
	}
	break;
	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

//ToggleFullScreen() function
void ToggleFullScreen(void)
{
	MONITORINFO mi;
	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;
	}
	else
	{
		fprintf(gpfile, "Full Screen to normal\n");
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPED);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

//initialize() function
int initialize(void)
{
	//function declarations
	void resize(int, int);
	void uninitialize(void);
	BOOL loadTexture(GLuint *, TCHAR[]);
	bool loadTextureUsingFreeImage(const char *, GLuint *);

	//vaiable declaraions
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLenum result;
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;
	GLint iProgramLinkStatus = 0;

	GLuint gVertexShaderObject;
	GLuint gGeometryShaderObject_q;
	GLuint gFragmentShaderObject_q;

	//code
	//initialize pfd structure
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		return(-1);
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		return(-2);
	}
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		return(-3);
	}
	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		return(-4);
	}

	result = glewInit();
	if (result != GLEW_OK)
	{
		fprintf(gpfile, "glewInit() failed\n");
		uninitialize();
		DestroyWindow(ghwnd);
	}

	//define vertex shader object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//write vertex shader code
	const GLchar *VertexShaderSourceCode =
		"	#version 430 core								" \
		"												\n	" \
		"	in vec4 vPosition;								" \
		"												\n	" \
		"	uniform mat4 u_m_matrix;						" \
		"	uniform mat4 u_v_matrix;						" \
		"	uniform mat4 u_p_matrix;						" \
		"												\n	" \
		"	void main(void)									" \
		"	{												" \
		"		gl_Position =  u_p_matrix*u_v_matrix*u_m_matrix*vPosition;		" \
		"	}												";

	//specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&VertexShaderSourceCode, NULL);

	//compile the vertex shader
	glCompileShader(gVertexShaderObject);
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "VERTEX SHADER COMPILATION LOG : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	// geometry shader to generate quad
	//define geometry shader object
	gGeometryShaderObject_q = glCreateShader(GL_GEOMETRY_SHADER);
	//write geometry shader code
	const GLchar *GeometryShaderSourceCode_q =
		"	#version 430 core																				" \
		"																								\n	" \
		"	layout(points) in;																				" \
		"	layout(triangle_strip, max_vertices = 12) out;													" \
		"																								\n	" \
		"	uniform mat4 u_m_matrix;																		" \
		"	uniform mat4 u_v_matrix;																		" \
		"	uniform mat4 u_p_matrix;																		" \
		"	uniform mat4 u_r1_matrix;																		" \
		"	uniform mat4 u_r2_matrix;																		" \
		"	uniform float u_time;																		\n	" \
		"	uniform int u_l_key_is_pressed;																\n	" \
		"	uniform vec4 u_light_position;																\n	" \
		"	uniform vec3 u_ld;																			\n	" \
		"	uniform vec3 u_kd;																			\n	" \
		"	uniform int u_enableWind;																	\n	" \
		"																								\n	" \
		"	out vec3 out_color;																			\n	" \
		"	out vec3 out_diffuse_color;																	\n	" \
		"	out vec2 out_texcoord;																		\n	" \
		"	out vec3 out_normal;																		\n	" \
		"																								\n	" \
		"	float PIby180 = 3.14159/180;																\n	" \
		"	float windStrength = 4.0;																	\n	" \
		"	float windPowerDeflection;																	\n	" \
		"	float windPower;																			\n	" \
		"	vec3 primitiveColor[3] = vec3[](vec3(1.0,0.0,0.0), vec3(0.0,1.0,0.0), vec3(0.0,0.0,1.0));	\n	" \
		"	vec4 patchPosition = gl_in[0].gl_Position;													\n	" \
		"	vec4 windDirection = vec4(1.0, 0.0, 1.0, 1.0);												\n	" \
		"																								\n	" \
		"	vec3 vLocalSeed;																			\n	" \
		"																								\n	" \
		"	// This function returns random number between 0 to 1										\n	" \
		"	float randZeroOne()																			\n	" \
		"	{																							\n	" \
		"		uint n = floatBitsToUint(vLocalSeed.y * 214013.0 + 										\n 	" \
		"								 vLocalSeed.x * 2531011.0 +										\n	" \
		"								 vLocalSeed.z * 141251.0);										\n	" \
		"		n = n * (n * n * 15731u + 789221u);														\n	" \
		"		n = (n >> 9u) | 0x3F800000u;															\n	" \
		"		float fRes =  2.0 - uintBitsToFloat(n);													\n	" \
		"																								\n	" \
		"		vLocalSeed = vec3(vLocalSeed.x + 147158.0 * fRes,										\n	" \
		"						  vLocalSeed.y * fRes  + 415161.0 * fRes,								\n	" \
		"						  vLocalSeed.z + 324154.0 * fRes);										\n	" \
		"		return(fRes);																			\n	" \
		"	}																							\n	" \
		"																								\n	" \
		"	int randomInt(int min, int max)																\n	" \
		"	{																							\n	" \
		"		float zeroToOne = randZeroOne();														\n	" \
		"		int index = int(float(min)+zeroToOne*float(max-min));								\n	" \
		"		return(index);																			\n	" \
		"	}																							\n	" \
		"																								\n	" \
		"	mat4 rotateY(float theta)																	\n	" \
		"	{																							\n	" \
		"		theta = theta * PIby180;																\n	" \
		"		return(mat4(cos(theta) , 0.0, sin(theta), 0.0,											\n	" \
		"					0.0, 1.0, 0.0, 0.0,															\n	" \
		"					-sin(theta), 0.0, cos(theta), 0.0,											\n	" \
		"					0.0,  0.0, 0.0, 1.0));														\n	" \
		"	}																							\n	" \
		"																								\n	" \
		"	void main(void)																					" \
		"	{																								" \
		"		vec4 temp_tr_v, temp_tl_v, temp_br_v, temp_bl_v;										\n	" \
		"		vec3 temp_tr_c, temp_tl_c, temp_br_c, temp_bl_c;										\n	" \
		"		vec3 temp_tr_n, temp_tl_n, temp_br_n, temp_bl_n;										\n	" \
		"		vec2 temp_tr_t, temp_tl_t, temp_br_t, temp_bl_t;										\n	" \
		"		vec4 eyeCoordinate;																		\n	" \
		"		vec3 tNorm;																				\n	" \
		"		vec3 viewerVector;																		\n	" \
		"																								\n	" \
		"		mat4 rotationMatrix = rotateY(10.0);													\n	" \
		"		mat4 mvp = u_p_matrix * u_v_matrix * u_m_matrix;											" \
		"		mat4 mv = u_v_matrix * u_m_matrix;														\n	" \
		"		mat3 normalMatrix = mat3(mv);																\n	" \
		"																								\n	" \
		"		if(u_enableWind == 1)																	\n	" \
		"		{																						\n	" \
		"			windDirection = normalize(windDirection);												\n	" \
		"			windPowerDeflection = sin(patchPosition.x/30.0+patchPosition.z/30+u_time*(0.2+windStrength/20.0));	\n	" \
		"			windPower = 1.5 + windPowerDeflection;													\n	" \
		"			if(windPower < 0.0)																		\n	" \
		"				windPower = windPower * 0.2;														\n	" \
		"			else																					\n	" \
		"				windPower = windPower * 0.3;														\n	" \
		"		}																						\n	" \
		"		else																					\n	" \
		"		{																						\n	" \
		"			windPower = 0.0;																		\n	" \
		"		}																						\n	" \
		"																								\n	" \
		"		int quadnumber = 0;																		\n	" \
		"		vLocalSeed = gl_in[0].gl_Position.xyz;							\n	"	\
		"		// select texture index from texture atlas												\n	" \
		"		int texIndex_row = randomInt(0, 3);														\n	" \
		"		int texIndex_column = randomInt(0, 3);															\n	" \
		"																								\n	" \
		"		float tcStartX = float(texIndex_row)*0.25f;													\n	" \
		"		float tcEndX = tcStartX + 0.25f;														\n	" \
		"		//float tcStartY = float(texIndex_column)*0.25f;													\n	" \
		"		//float tcEndY = tcStartY + 0.25f;														\n	" \
		"		float tcStartY = 0;													\n	" \
		"		float tcEndY = 1;														\n	" \
		"																								\n	" \
		"		//quad 0 bottom right vertex															\n	" \
		"		vec4 q0_BR = (gl_in[0].gl_Position +  vec4(1.0, -1.0, 0.0, 0.0));							" \
		"		temp_br_v = q0_BR;																		\n	" \
		"		temp_br_c = primitiveColor[0];															\n	" \
		"		temp_br_n = normalize(vec3(0.0, 1.0, 0.0));												\n	" \
		"		temp_br_t = vec2(tcEndX, 0.0);														\n	" \
		"		gl_Position = mvp * q0_BR;																\n	" \
		"		out_color = temp_br_c;																	\n	" \
		"		out_normal = temp_br_n;																	\n	" \
		"		out_texcoord = temp_br_t;																\n	" \
		"		if(u_l_key_is_pressed == 1)																	\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_br_v;															\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"																								\n	" \
		"		//quad 0 bottom left vertex																\n	" \
		"		vec4 q0_BL = (gl_in[0].gl_Position +  vec4(-1.0, -1.0, 0.0, 0.0));							" \
		"		temp_bl_v = q0_BL;																		\n	" \
		"		temp_bl_c = primitiveColor[0];															\n	" \
		"		temp_bl_n = normalize(vec3(0.0,1.0,0.0));												\n	" \
		"		temp_bl_t = vec2(tcStartX, 0.0);														\n	" \
		"		gl_Position = mvp * q0_BL;																\n	" \
		"		out_color = temp_bl_c;																	\n	" \
		"		out_normal = temp_bl_n;																	\n	" \
		"		out_texcoord = temp_bl_t;																\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_bl_v;														\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"																								\n	" \
		"		//quad 0 top right vertex																\n	" \
		"		vec4 q0_TR = (gl_in[0].gl_Position +  vec4(1.0, 1.0, 0.0, 0.0));							" \
		"		temp_tr_v = (q0_TR + windDirection * windPower);										\n	" \
		"		temp_tr_c = primitiveColor[0];															\n	" \
		"		temp_tr_n = vec3(normalize(temp_br_v - temp_tr_v));									\n	" \
		"		temp_tr_t = vec2(tcEndX, 1.0);															\n	" \
		"		gl_Position = mvp * temp_tr_v;															\n	" \
		"		out_color = temp_tr_c;																	\n	" \
		"		out_normal = temp_tr_n;																	\n	" \
		"		out_texcoord = temp_tr_t;																\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_tr_v;														\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"																								\n	" \
		"		//quad 0 top left vertex																\n	" \
		"		vec4 q0_TL = (gl_in[0].gl_Position +  vec4(-1.0, 1.0, 0.0, 0.0));							" \
		"		temp_tl_v = (q0_TL + windDirection * windPower);										\n	" \
		"		temp_tl_c = primitiveColor[0];															\n	" \
		"		temp_tl_n = normalize(temp_bl_v.xyz - temp_tl_v.xyz);									\n	" \
		"		temp_tl_t = vec2(tcStartX, 1.0);													\n	" \
		"		gl_Position = mvp * temp_tl_v;															\n	" \
		"		out_color = temp_tl_c;																	\n	" \
		"		out_normal = temp_tl_n;																	\n	" \
		"		out_texcoord = temp_tl_t;																\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_tl_v;														\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"		EndPrimitive();																			" \
		"																							\n	" \
		"		// quad 1 bottom right vertex														\n	" \
		"		vec4 q1_BR = (gl_in[0].gl_Position + (u_r1_matrix * vec4(1.0, -1.0, 0.0, 0.0)));		" \
		"		temp_br_v = q1_BR;																	\n	" \
		"		temp_br_c = primitiveColor[1];														\n	" \
		"		temp_br_n = normalize(vec3(0.0, 1.0, 0.0));												\n	" \
		"		temp_br_t = vec2(tcEndX, 0.0);													\n	" \
		"		gl_Position = mvp * temp_br_v;														\n	" \
		"		out_color = temp_br_c;																\n	" \
		"		out_texcoord = temp_br_t;															\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_br_v;														\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"																							\n	" \
		"		// quad 1 bottom left vertex														\n	" \
		"		vec4 q1_BL = (gl_in[0].gl_Position + (u_r1_matrix * vec4(-1.0, -1.0, 0.0, 0.0)));		" \
		"		temp_bl_v = q1_BL;																	\n	" \
		"		temp_bl_c =	primitiveColor[1];														\n	" \
		"		temp_bl_n =	normalize(vec3(0.0, 1.0, 0.0));												\n	" \
		"		temp_bl_t =	vec2(tcStartX, 0.0);													\n	" \
		"		gl_Position = mvp * temp_bl_v;														\n	" \
		"		out_color =	temp_bl_c;																\n	" \
		"		out_normal = temp_bl_n;																\n	" \
		"		out_texcoord = temp_bl_t;															\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_bl_v;														\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"																							\n	" \
		"		// quad 1 top right vertex															\n	" \
		"		vec4 q1_TR = (gl_in[0].gl_Position + (u_r1_matrix * vec4(1.0, 1.0, 0.0, 0.0)));	" \
		"		temp_tr_v = (q1_TR + windDirection * windPower);									\n	" \
		"		temp_tr_c =	primitiveColor[1];														\n	" \
		"		temp_tr_n =	normalize(temp_br_v.xyz - temp_tr_v.xyz);										\n	" \
		"		temp_tr_t =	vec2(tcEndX, 1.0);													\n	" \
		"		gl_Position = mvp * temp_tr_v;														\n	" \
		"		out_color = temp_tr_c;																\n	" \
		"		out_normal = temp_tr_n;																\n	" \
		"		out_texcoord = temp_tr_t;															\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_tr_v;														\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"																							\n	" \
		"		// quad 1 top left vertex															\n	" \
		"		vec4 q1_TL = (gl_in[0].gl_Position + (u_r1_matrix * vec4(-1.0, 1.0, 0.0, 0.0)));	" \
		"		temp_tl_v = (q1_TL + windDirection * windPower);										\n	" \
		"		temp_tl_c =	primitiveColor[1];														\n	" \
		"		temp_tl_n =	normalize(temp_bl_v.xyz - temp_tl_v.xyz);								\n	" \
		"		temp_tl_t =	vec2(tcStartX, 1.0);												\n	" \
		"		gl_Position = mvp * temp_tl_v;														\n	" \
		"		out_color = temp_tl_c;																\n	" \
		"		out_normal = temp_tl_n;																\n	" \
		"		out_texcoord = temp_tl_t;															\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_tl_v;														\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"		EndPrimitive();																			" \
		"																							\n	" \
		"		// quad 2 bottom right vertex														\n	" \
		"		vec4 q2_BR = (gl_in[0].gl_Position + (u_r2_matrix * vec4(1.0, -1.0, 0.0, 0.0)));	" \
		"		temp_br_v = q2_BR;																	\n	" \
		"		temp_br_c =	primitiveColor[2];														\n	" \
		"		temp_br_n = normalize(vec3(0.0, 1.0, 0.0));												\n	" \
		"		temp_br_t = vec2(tcEndX, 0.0);														\n	" \
		"		gl_Position = mvp * temp_br_v;														\n	" \
		"		out_color = temp_br_c;																\n	" \
		"		out_normal = temp_br_n;																\n	" \
		"		out_texcoord = temp_br_t;															\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_br_v;														\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"																							\n	" \
		"		// quad 2 bottom left vertex														\n	" \
		"		vec4 q2_BL = (gl_in[0].gl_Position + (u_r2_matrix * vec4(-1.0, -1.0, 0.0, 0.0)));	" \
		"		temp_bl_v = q2_BL;																	\n	" \
		"		temp_bl_c =	primitiveColor[2];														\n	" \
		"		temp_bl_n = normalize(vec3(0.0, 1.0, 0.0));												\n	" \
		"		temp_bl_t = vec2(tcStartX, 0.0);															\n	" \
		"		gl_Position = mvp * temp_bl_v;														\n	" \
		"		out_color = temp_bl_c;																\n	" \
		"		out_normal = temp_bl_n;																\n	" \
		"		out_texcoord = temp_bl_t;															\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_bl_v;														\n	" \
		"			tNorm = normalize(normalMatrix * out_normal);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"																							\n	" \
		"		// quad 2 top right vertex															\n	" \
		"		vec4 q2_TR = (gl_in[0].gl_Position + (u_r2_matrix * vec4(1.0, 1.0, 0.0, 0.0)));			" \
		"		temp_tr_v = (q2_TR + windDirection * windPower);									\n	" \
		"		temp_tr_c =	primitiveColor[2];														\n	" \
		"		temp_tr_n = (temp_br_v.xyz - temp_tr_v.xyz);										\n	" \
		"		temp_tr_t = vec2(tcEndX, 1.0);														\n	" \
		"		gl_Position = mvp * temp_tr_v;														\n	" \
		"		out_color = temp_tr_c;																\n	" \
		"		out_normal = temp_tr_n;																\n	" \
		"		out_texcoord = temp_tr_t;															\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_tr_v;														\n	" \
		"			tNorm = normalize(normalMatrix * temp_tr_n);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"																							\n	" \
		"		// quad 2 top left vertex															\n	" \
		"		vec4 q2_TL = (gl_in[0].gl_Position + (u_r2_matrix * vec4(-1.0, 1.0, 0.0, 0.0)));	" \
		"		temp_tl_v = (q2_TL + windDirection * windPower);									\n	" \
		"		temp_tl_c = primitiveColor[2];														\n	" \
		"		temp_tl_n = (temp_bl_v.xyz - temp_tl_v.xyz);										\n	" \
		"		temp_tl_t = vec2(tcStartX, 1.0);															\n	" \
		"		gl_Position = mvp * temp_tl_v;														\n	" \
		"		out_color = temp_tl_c;																\n	" \
		"		out_normal = temp_tl_n;																\n	" \
		"		out_texcoord = temp_tl_t;														\n	" \
		"		if (u_l_key_is_pressed == 1)															\n	" \
		"		{																						\n	" \
		"			eyeCoordinate = mv * temp_tl_v;														\n	" \
		"			tNorm = normalize(normalMatrix * temp_tl_n);										\n	" \
		"			viewerVector = normalize(vec3(u_light_position - eyeCoordinate));					\n	" \
		"			out_diffuse_color = u_ld * u_kd * max(dot(tNorm, viewerVector), 0.0);				\n	" \
		"		}																						\n	" \
		"		EmitVertex();																			" \
		"		EndPrimitive();																			" \
		"	}																							";

	//specify above source code to geometry shader object
	glShaderSource(gGeometryShaderObject_q, 1, (const GLchar **)&GeometryShaderSourceCode_q, NULL);

	//compile the geometry shader
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glCompileShader(gGeometryShaderObject_q);
	glGetShaderiv(gGeometryShaderObject_q, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gGeometryShaderObject_q, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gGeometryShaderObject_q, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "GEOMETRY SHADER 2 COMPILATION LOG : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	//define fragment shader object
	gFragmentShaderObject_q = glCreateShader(GL_FRAGMENT_SHADER);
	//write fragment shader code
	const GLchar *FragmentShaderSourceCode_q =
		"	#version 430 core														" \
		"																		\n	" \
		"	in vec3 out_color;													\n	" \
		"	in vec2 out_texcoord;												\n	" \
		"	in vec3 out_normal;													\n	" \
		"	in vec3 out_diffuse_color;											\n	" \
		"																		\n	" \
		"	out vec4 FragColor;														" \
		"																		\n	" \
		"	uniform sampler2D u_sampler;										\n	" \
		"	uniform vec3 u_yellow_color;										\n	" \
		"	uniform float u_alpha_test;											\n	" \
		"	uniform float u_alpha_multiplier;									\n	" \
		"	uniform int u_q_key_is_pressed;										\n	" \
		"	uniform int u_l_key_is_pressed;										\n	" \
		"																		\n	" \
		"	void main(void)														\n	" \
		"	{																	\n	" \
		"		vec4 texColor, finalColor;										\n	" \
		"																		\n	" \
		"		if(u_q_key_is_pressed == 0)										\n	" \
		"		{																\n	" \
		"			texColor = texture(u_sampler, out_texcoord);				\n	" \
		"			float newAlpha = texColor.a * u_alpha_multiplier;			\n	" \
		"																		\n	" \
		"			if(newAlpha < u_alpha_test)									\n	" \
		"				discard;												\n	" \
		"			if(newAlpha > 1.0)											\n	" \
		"				newAlpha = 1.0;											\n	" \
		"																		\n	" \
		"			if(u_l_key_is_pressed == 1)									\n	" \
		"			{															\n	" \
		"				finalColor = vec4(texColor.xyz * u_yellow_color * 2.0 * out_diffuse_color, 1.0);	\n	" \
		"			}															\n	" \
		"			else														\n	" \
		"			{															\n	" \
		"				finalColor = vec4(texColor.xyz * u_yellow_color*3.0, 1.0);	\n	" \
		"			}															\n	" \
		"																		\n	" \
		"			FragColor = vec4(finalColor.xyz, 1.0);						\n	" \
		"		}																\n	" \
		"		else															\n	" \
		"		{																\n	" \
		"			if(u_l_key_is_pressed == 1)									\n	" \
		"			{															\n	" \
		"				finalColor = vec4(out_color * out_diffuse_color, 1.0);	\n	" \
		"			}															\n	" \
		"			else														\n	" \
		"			{															\n	" \
		"				finalColor = vec4(out_color.xyz, 0.5);		\n	" \
		"			}															\n	" \
		"			FragColor = finalColor;							\n	" \
		"		}																\n	" \
		"	}																		";

	//specify above source code to fragmnet shader object
	glShaderSource(gFragmentShaderObject_q, 1, (const GLchar **)&FragmentShaderSourceCode_q, NULL);

	//compile fragment shader
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glCompileShader(gFragmentShaderObject_q);
	glGetShaderiv(gFragmentShaderObject_q, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_q, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_q, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "FRAGMENT SHADER 2 COMPILATION LOG : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	//create shader program object 2
	gShaderProgramObject_q = glCreateProgram();

	//attach vertex shader to the shader program 
	glAttachShader(gShaderProgramObject_q, gVertexShaderObject);
	// attachgeometry shader
	glAttachShader(gShaderProgramObject_q, gGeometryShaderObject_q);
	//attach fragment shader 
	glAttachShader(gShaderProgramObject_q, gFragmentShaderObject_q);

	//prelinking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObject_q, AMC_ATTRIBUTE_POSITION, "vPosition");

	//link shader program
	glLinkProgram(gShaderProgramObject_q);
	//error checking
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderProgramObject_q, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_q, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLint written;
				glGetProgramInfoLog(gShaderProgramObject_q, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "SHADER PROGRAM 2 LINK LOG : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	//postlinking retieving uniform location
	mUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_m_matrix");
	vUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_v_matrix");
	pUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_p_matrix");
	r1Uniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_r1_matrix");
	r2Uniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_r2_matrix");
	samplerUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_sampler");
	colorUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_yellow_color");
	timeElapsedUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_time");
	alphaTestUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_alpha_test");
	alphaMultiplierUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_alpha_multiplier");
	quadModeUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_q_key_is_pressed");
	toggleLightsUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_l_key_is_pressed");
	lightPosition_q = glGetUniformLocation(gShaderProgramObject_q, "u_light_position");
	ld_q = glGetUniformLocation(gShaderProgramObject_q, "u_ld");
	kd_q = glGetUniformLocation(gShaderProgramObject_q, "u_kd");
	windUniform_q = glGetUniformLocation(gShaderProgramObject_q, "u_enableWind");

	GLfloat upper = 5.0f;
	GLfloat lower = -1.0f;
	GLfloat triangleVertices[500 * 3];
	float a = 5.0;

	for (int j = 0; j < 500; j = j + 3)
	{
		//		float num1 = (float)(rand() %	(int)(upper - lower + 1)) + lower;
		float num1 = (((float)rand() / (float)(RAND_MAX)) * a) + lower;
		triangleVertices[j + 0] = num1 + 0.5;
		triangleVertices[j + 1] = 0.0f;
		//float num2 = (float)(rand() % (int)(upper - lower + 1)) + lower;
		float num2 = (((float)rand() / (float)(RAND_MAX)) * a) + lower;
		triangleVertices[j + 2] = num2 + 0.1;
		//fprintf(gpfile, "triangleVertices : %f, %f, %f\n", num1, triangleVertices[j + 1], num2);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//loadTexture(&textureSmiley, MAKEINTRESOURCE(ID_SMILEY));;
	loadTextureUsingFreeImage("grassPack.dds", &textureSmiley);
	//loadTextureUsingFreeImage("grass.dds", &textureSmiley);

	fprintf(gpfile, "random number = %d\n", myRandInRange(0, 5));
	fprintf(gpfile, "random number = %d\n", myRandInRange(0, 5));
	fprintf(gpfile, "random number = %d\n", myRandInRange(0, 5));
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//r,g,b,a	//clear the screen by opengl color

	glClearDepth(1.0f);						//giving existance to depth buffer
	glEnable(GL_DEPTH_TEST);				// enableing depth test
	glDepthFunc(GL_LEQUAL);
	//glEnable(GL_CULL_FACE);
	perspectiveProjectionMatrix = mat4::identity();

	//wramup call to resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

//function to load texture
BOOL loadTexture(GLuint *Textures, TCHAR ImageResourceID[])
{
	//variable declarations
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bStatus = FALSE;

	//code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		ImageResourceID,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		bStatus = TRUE;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, Textures);
		glBindTexture(GL_TEXTURE_2D, *Textures);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			bmp.bmWidth,
			bmp.bmHeight,
			0,
			GL_BGR,
			GL_UNSIGNED_BYTE,
			bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}
	return(bStatus);
}

// load texture using free image
bool loadTextureUsingFreeImage(const char *filename, GLuint *texture)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib(0);
	BYTE *bImageData = NULL;
	int imageWidth = 0;
	int imageHeight = 0;
	int bitsPerPixel = 0;
	int iFormat = 0;

	// Check the file signature and deduce its format
	/*	FreeImage_GetFileType() =>
	Parameters		: const char* filename , int size	the second argument is currently not used by FreeImage)
	Return Value	: one of the predefined FREE_IMAGE_FORMAT constants or a bitmap identification number
	registered by a plugin. returns FIF_UNKNOWN if format is not identified
	*/
	fif = FreeImage_GetFileType(filename, 0);

	// if format is still unknown, try to guess file format from the file extension
	/*	FreeImage_GetFIFFromFilename() =>	This function reads up to 16 bytes and analysing it.
	Parameters		: const char* filename
	Return Value	: one of the predefined FREE_IMAGE_FORMAT constants or a bitmap identification number
	registered by a plugin. Returns FIF_UNKNOWN if format is not identified
	*/
	if (fif == FIF_UNKNOWN)
	{
		fprintf(gpfile, "FreeImage_GetFileType() Failed...\n");
		fprintf(gpfile, "Trying FreeImage_GetFIFFromFilename()...\n");
		fif = FreeImage_GetFIFFromFilename(filename);
	}
	else
	{
		fprintf(gpfile, "FreeImage_GetFileType() Success...\n");
	}

	// If still Unknown, return failure
	if (fif == FIF_UNKNOWN)
	{
		fprintf(gpfile, "Image file format is not known. Exitting Now...\n");
		fprintf(gpfile, "%s Format\n", FreeImage_GetFormatFromFIF(fif));
		return (false);
	}
	else
	{
		fprintf(gpfile, "Image File format found : ");
		fprintf(gpfile, "%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}

	// Check if the plugin has reading capabilities and load the file
	/*	FreeImage_FIFSupportsReading() =>
	Parameters		: FREE_IMAGE FORMAT fif
	Return Value	: Returns TRUE if the plugin belonging to the given FREE_IMAGE_FORMAT can be used to load bitmaps,
	FALSE otherwise.

	FreeImage_Load() => Loads image
	Parameters		: FREE_IMAGE FORMAT fif, const char * filename, format
	Return Value	: returns pointer to image data
	*/
	if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// ok, let's load the file
		dib = FreeImage_Load(fif, filename, 0);
		// unless a bad file format, we are done !
	}

	if (!dib)
	{
		fprintf(gpfile, "FreeImage_Load() Failed. Exitting Now...\n");
		return(false);
	}
	else
	{
		fprintf(gpfile, "FreeImage_Load() Succeeded. Image is loaded successfully. Exitting Now...\n");
	}

	// Retrieve the image data
	bImageData = FreeImage_GetBits(dib);

	// Get image height , width and size of each pixel
	imageWidth = FreeImage_GetWidth(dib);
	imageHeight = FreeImage_GetHeight(dib);
	bitsPerPixel = FreeImage_GetBPP(dib);

	// How much to increase data pointer to get to next pixel data = bitsperpixel
	// Length of one row in data
	unsigned int row_step = bitsPerPixel*imageHeight;

	fprintf(gpfile, "%s width : %d\n", filename, imageWidth);
	fprintf(gpfile, "%s Height : %d\n", filename, imageHeight);
	fprintf(gpfile, "%s Bits Per Pixel : %d\n", filename, bitsPerPixel);

	// if any of these data failed to retieve the information from image return failure
	if (bImageData == NULL || imageWidth == 0 || imageHeight == 0)
	{
		fprintf(gpfile, "Failed to retrieve the image data or image width or image height. Exitting Now...\n");
		return(false);
	}

	iFormat = bitsPerPixel;
	if (iFormat == 32)
		iFormat = GL_RGBA;
	else if (iFormat == 24)
		iFormat = GL_BGR;
	else if (iFormat == 8)
		iFormat = GL_LUMINANCE;

	// generate openGL texture
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		imageWidth,
		imageHeight,
		0,
		iFormat,
		GL_UNSIGNED_BYTE,
		bImageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	// unload image
	FreeImage_Unload(dib);

	return(true);
}

//display function
void display(void)
{
	vmath::mat4 m4PersPectiveProjectionMatrix = vmath::perspective(camera.GetZoom(), (float)g_fCurrrentWidth / (float)g_fCurrrentHeight, 0.1f, 100.0f);

	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 translation;
	mat4 rotation;
	mat4 r1, r2;
	static float angle = 0.0f;
	static float timeElapsed = 0.0f;
	static float alphaTest = 0.1f;
	static float alphaMultiplier = 1.5f;
	float yellowColor[] = { 1.0f, 1.0f, 0.0f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//initialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	model = mat4::identity();
	view = mat4::identity();
	projection = mat4::identity();
	translation = mat4::identity();
	rotation = mat4::identity();
	r1 = mat4::identity();
	r2 = mat4::identity();

	view = vmath::lookat(vec3(xpos, ypos, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	//view= camera.GetViewMatrix();
	translation = translate(0.0f, 0.0f, 0.0f);
	//rotation = rotate(angle, 1.0f, 0.0f, 0.0f);
	model = translation * rotation;
	projection = m4PersPectiveProjectionMatrix;
	r1 = rotate(45.0f, 0.0f, 1.0f, 0.0f);
	r2 = rotate(-45.0f, 0.0f, 1.0f, 0.0f);

	//Use program
	glUseProgram(gShaderProgramObject_q);

	//send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mUniform_q, 1, GL_FALSE, model);
	glUniformMatrix4fv(vUniform_q, 1, GL_FALSE, view);
	glUniformMatrix4fv(pUniform_q, 1, GL_FALSE, projection);
	glUniformMatrix4fv(r1Uniform_q, 1, GL_FALSE, r1);
	glUniformMatrix4fv(r2Uniform_q, 1, GL_FALSE, r2);
	glUniform1f(timeElapsedUniform_q, timeElapsed);

	if (gbQuadMode == true)	// show only quads without grass texture on it
	{
		glUniform1i(quadModeUniform_q, 1);
	}
	else					// show quad textured
	{
		glUniform1i(quadModeUniform_q, 0);
		glUniform1f(alphaTestUniform_q, alphaTest);
		glUniform1f(alphaMultiplierUniform_q, alphaMultiplier);
		glUniform3fv(colorUniform_q, 1, yellowColor);
	}
	if (gbToggleLights == true)	// lights are on
	{
		glUniform1i(toggleLightsUniform_q, 1);
		glUniform3fv(ld_q, 1, lightDiffuse);
		glUniform3fv(kd_q, 1, materialDiffuse);
		glUniform4f(lightPosition_q, 10.0f, 10.0f, 10.0f, 1.0f);
	}
	else						// lights are off
	{
		glUniform1i(toggleLightsUniform_q, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureSmiley);
	glUniform1i(samplerUniform_q, 0);
	if (gbEnableWind == true)
		glUniform1i(windUniform_q, 1);
	else
		glUniform1i(windUniform_q, 0);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, 100);

	//unbind vao
	glBindVertexArray(0);
	glDisable(GL_BLEND);

	//unuse program
	glUseProgram(0);
	SwapBuffers(ghdc);

	angle = angle + 0.02f;
	timeElapsed = timeElapsed + 0.005f;
}

//reize() function
void resize(int win_width, int win_height)
{
	if (win_height == 0)
		win_height = 1;
	glViewport(0, 0, (GLsizei)win_width, (GLsizei)win_height);
	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)win_width / (GLfloat)win_height, 0.1f, 100.0f);
}

//uninitialize() function
void uninitialize(void)
{
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	if (gShaderProgramObject_q)
	{
		GLsizei shaderCount;
		GLsizei shaderNumber;

		glUseProgram(gShaderProgramObject_q);
		glGetProgramiv(gShaderProgramObject_q, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);

		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject_q, shaderCount, &shaderCount, pShaders);
			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				//detach all the shader one by one
				glDetachShader(gShaderProgramObject_q, pShaders[shaderNumber]);
				//delete detached shader
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject_q);
		gShaderProgramObject_q = 0;
		glUseProgram(0);
	}

	if (gbFullScreen == true)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPED);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
	}

	//break the current context
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (gpfile)
	{
		fprintf(gpfile, "log file closed successfully\n");
		fclose(gpfile);
		gpfile = NULL;
	}
}