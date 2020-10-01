#define STB_IMAGE_IMPLEMENTATION
#include "CommonHeader.h"

#include"Triangle.h"
#include"Terrain.h"
#include"Points.h"

//Various models Main
#include "temp_lake1_grass_coords.h"
#include "BushLocation.h"
#include "PalmLocation.h"
#include "MarigoldLocations.h"
#include "skydome.h"
#include "SunLight.h"
#include "Grass.h"
#include "CubeMap.h"

#include "AnimMesh.h"
#include "AnimModel.h"
#include "FlockingButterfiles.h"
#include "LotusLeafData.h"
//model load
#include "models/Mesh.h"
#include "models/Model.h"
#include "LoadShaders.h"
#include "Data.h"


// Font Rendering
#include "audioManager.h"
#include "soundSource.h"
#include "Credits.h"

#include <vector>

#include <IL\il.h>


#define WIN_WIDTH	800
#define WIN_HEIGHT	600


#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"FreeImage.lib")
#pragma comment(lib,"assimp-vc140-mt.lib")
#pragma comment(lib,"DevIL.lib")
#pragma comment(lib,"ILU.lib")
#pragma comment(lib,"ILUT.lib")
#pragma comment(lib,"OpenAL32.lib")
#pragma comment(lib,"freetype.lib")

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

bool g_bWindowActive = false;
HWND g_hwnd = NULL;
HDC  g_hdc = NULL;
HGLRC g_hrc = NULL;

WINDOWPLACEMENT wpPrev;
DWORD dwStyle;
bool g_bFullScreen = false;


// Projection
glm::mat4 g_PersPectiveProjectionMatrix;
glm::mat4 g_PersPectiveProjectionMatrix_SkyBox;

// Camera Keys

GLfloat g_fLastX = WIN_WIDTH / 2;
GLfloat g_fLastY = WIN_HEIGHT / 2;

GLfloat g_DeltaTime = 0.0f;
GLboolean g_bFirstMouse = true;
GLfloat g_fCurrrentWidth;
GLfloat g_fCurrrentHeight;

GLsizei giWinWidth;
GLsizei giWinHeight;

//FILE *g_pFile = NULL;

glm::vec3 g_v3NightAmbient = glm::vec3(0.25f, 0.25f, 0.25f);
//glm::vec3 g_v3NightAmbient =  glm::vec3(0.45f, 0.45f, 0.45f);
glm::vec3 g_v3NightDiffuse = glm::vec3(0.25f, 0.25f, 0.25f);
glm::vec3 g_v3NightSpecular = glm::vec3(0.25f, 0.25f, 0.25f);
glm::vec3 g_v3NightDirection = glm::vec3(-968.825684f, -911.897095f, -824.769104f);

CTerrain* g_pTerrain = NULL;
CSkyDome* g_pSkyDome = NULL;
CubeMap* g_pCubeMap = NULL;
CFlockingButterflies* g_pFlockingButterfiles = NULL;


//Camera mainCamera(glm::vec3(0.0f, 35.0f, 5.0f));
Camera* mainCamera = NULL;

// Shadow Framebuffer Start
#define FRUSTUM_DEPTH 1000.0f
//#define DEPTH_TEXTURE_SIZE 2048
#define DEPTH_TEXTURE_SIZE 4096
//#define DEPTH_TEXTURE_SIZE 8192
GLuint g_ShadowFramebuffer;
GLuint g_Texture_ShadowDepthTexure;
// Shadow Program Object
GLuint g_ShaderProgramObject_depth;
// Shadow Uniform 
GLint g_depth_U_Model_Matrix;
GLint g_depth_U_View_Matrix;
GLint g_depth_U_Projection_Matrix;

glm::vec3 g_v3LightPosition = glm::vec3(0.0f);
glm::mat4 g_m4LightViewMatrix = glm::mat4(1.0f);
glm::mat4 g_m4LightProjectionMatrix = glm::mat4(1.0f);

const glm::mat4 ScaleBiasMatrix = glm::mat4(glm::vec4(0.5f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.5f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.5f, 0.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
// Shadow Framebuffer Stop

// Skydome variable
GLuint gShaderProgramObject_SkyDome;
// SkyDome FBO Start
GLuint OcclusionFBO;
GLuint OcclusionMapTexture;
// SkyDome FBO Stop

glm::vec3 g_v3CameraPosition;
glm::vec3 g_v3CameraFront;
glm::vec3 g_v3CameraUp;
glm::vec3 g_v3CameraLook;
glm::vec3 g_v3CameraRight;

// HDR Start
// HDR - > all cleaned up
GLuint gShaderObjectHDR;
GLuint gFrameBufferObjectHDR;
GLuint gRenderBufferObjectDepth;
GLuint gColorBufferTexture;

GLuint gVAOHDRQuad;
GLuint gVBOHDR;

GLfloat fExposure = 1.0f;
GLfloat g_fHDRAlphaValue = 1.0f;

void InitializeQuadHDR(void);
void RenderQuadHDR(void);
// HDR Stop


// MSAA Start
// MSAA FBO -> all cleaned up
GLuint gShaderObjectMSAA;
GLuint gFBOMSAA;
GLuint gRBOMSAA;
GLuint gTextureMSAA;
// MSAA Stop 

float g_fTime = 0.0f;


GLuint g_terrainShaderProg = 0;
GLint g_UniformTerrain_XPixelOffset = 0;
GLint g_UniformTerrain_YPixelOffset = 0;
GLint g_UniformTerrain_ViewPosition = 0;
GLint g_UniformTerrain_shadowBias = 0;
GLint g_UniformTerrain_lightView_Matrix = 0;
GLint g_UniformTerrain_lightprojrction_Matrix = 0;

GLint g_UniformTerrain_sunAmbient = 0;
GLint g_UniformTerrain_sunSpecular = 0;
GLint g_UniformTerrain_sunDiffuse = 0;
GLint g_UniformTerrain_sunDirection = 0;

GLint g_UniformTerrain_ClipPlane = -1; // Required for water

CSunLight* g_pSunLight = NULL;

//Marigold
ModelLoad::Model* Marigold = NULL;
GLuint g_UBO_InstancingMarigold = -1;
#define	NUM_INSTANCES_MARIGOLD  615


// Palm Tree - straight , tilted, bent
ModelLoad::Model* PalmStraight = NULL;
GLuint g_UBO_InstancingPlamST = -1;
ModelLoad::Model* PalmTilted = NULL;
GLuint g_UBO_InstancingPlamTILT = -1;
ModelLoad::Model* PalmBent = NULL;
GLuint g_UBO_InstancingPlamBEN = -1;
#define	NUM_INSTANCES_PALMTREE  10

//Tree
glm::vec3 TreesPositions1[] =
{


	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(00.0f, 0.0f, 150.0f),
	glm::vec3(0.0f, 0.0f, 300.0f),
	glm::vec3(00.0f, 0.0f, 450.0f),
	glm::vec3(00.0f, 0.0f, 600.0f),
	glm::vec3(0.0f, 0.0f, 750.0f),
	glm::vec3(50.0f, 0.0f, 900.0f),
	glm::vec3(000.0f, 0.0f, 1150.0f),
	glm::vec3(00.0f, 0.0f, 1200.0f),
	glm::vec3(00.0f, 0.0f, 1300.0f),
};


///Palm tree
glm::vec3 TreesPositions2[] =
{
	glm::vec3(136.917419,32.886059,778.106079),	//palm tree position near lake 1
	glm::vec3(50.852745,34.480873,1095.618774),
	glm::vec3(58.759624,21.876902,1056.16345),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(-100.0f, 0.0f, 0.0f),
	glm::vec3(-220.0f, 0.0f, 0.0f),
	glm::vec3(-300.0f, 0.0f, 0.0f),
	glm::vec3(-400.0f, 0.0f, 0.0f),
	glm::vec3(-500.0f, 0.0f, 0.0f),
	glm::vec3(-600.0f, 0.0f, 0.0f),
	glm::vec3(-700.0f, 0.0f, 50.0f),
	glm::vec3(-800.0f, 0.0f, 0.0f),
	glm::vec3(-900.0f, 0.0f, 0.0f),
};


//TreeModel *treeModel;


/* Water Code Start */
#define WAVE_SPEED 0.003f;


GLuint g_ShaderProgramObject_Water = 0;

// Uniforms
GLint g_Uniform_Model_Matrix_Water = 0;
GLint g_Uniform_View_Matrix_Water = 0;
GLint g_Uniform_Projection_Matrix_Water = 0;
GLint g_uniform_MoveFactor_Water = 0;
GLint g_uniform_CamPos_Water = -1;
GLint g_uniform_LightPosition_Water = -1;
GLint g_uniform_LightColor_Water = -1;
// sampler
GLint g_uniform_TextureSampler_FB_Reflection = 1;
GLint g_uniform_TextureSampler_FB_Refraction = -1;
GLint g_uniform_TextureSampler_FB_DuDv = -1;
GLint g_uniform_TextureSampler_FB_Normal = -1;
GLint g_uniform_TextureSampler_FB_Depth = -1;


// Texture
GLuint g_Tex_WaterNormal;
GLuint g_Tex_WaterDuDv;

// Framebuffer
GLuint g_FrameBuffer_Reflection;
GLuint g_ColorTexture_Reflection;
GLuint g_DepthTexture_Reflection;


GLuint g_FrameBuffer_Refraction;
GLuint g_ColorTexture_Refraction;
GLuint g_DepthTexture_Refraction;

const GLenum g_DrawBuffers[] = { GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT };


GLfloat g_fWaterMoveFactor = 0.0f;


GLuint g_VertexArrayObject_Water = 0;
GLuint g_VertexBufferObject_Position_Water = 0;
GLuint g_VertexBufferObject_TexCoords_Water = 0;


// Water Code Stop 


/*	Animated Model Loading Stuff	*/
AnimModel* mySunFlower;
GLuint g_UBO_InstancingSunFlower = -1;
#define	NUM_INSTANCES_SUNFLOWER  5

glm::vec3 SunFlowerPositions[] =
{
	glm::vec3(-610.821167f, 13.719143f, -154.846970f),	// Flower 1
	glm::vec3(-667.880554f, 28.519143f, -183.268799f),	// Flower 2
	glm::vec3(-656.345276f, 31.5f, 		-160.988129f),	// Flower 3
	glm::vec3(-628.897461f, 16.719143f, -136.101624f),	// Flower 4
	glm::vec3(-691.820435f, 14.719143f, -164.064804f),	// Flower 5
};

glm::vec3 SunFlowerScaleVal[] =
{
	glm::vec3(3.0f, 3.0f, 3.0f),	// Flower 1
	glm::vec3(1.8f, 1.8f, 1.8f),	// Flower 2
	glm::vec3(2.1f, 2.1f, 2.1f),	// Flower 3
	glm::vec3(2.8f, 2.8f, 2.8f),	// Flower 4
	glm::vec3(3.0f, 3.0f, 3.0f),	// Flower 5
};

GLuint g_ShaderProgramObject_Model_Sunflower = 0;
// Uniforms
GLint g_Uniform_Model_Model_Matrix_Sunflower = 0;
GLint g_Uniform_Model_View_Matrix_Sunflower = 0;
GLint g_Uniform_Model_Projection_Matrix_Sunflower = 0;
GLint g_Uniform_Model_LD_Sunflower = 0;
GLint g_Uniform_Model_LA_Sunflower = 0;
GLint g_Uniform_Model_LS_Sunflower = 0;

GLint g_Uniform_Model_KD_Sunflower = 0;
GLint g_Uniform_Model_KA_Sunflower = 0;
GLint g_Uniform_Model_KS_Sunflower = 0;

GLint g_Uniform_Model_lightPosition_animModel_Sunflower = 0;
GLint g_Uniform_Model_ViewPosition_Sunflower = 0;
GLint g_Uniform_Model_DiffuseMapSampler_Sunflower = 0;
GLint g_Uniform_Model_NormalMapSampler_Sunflower = 0;
GLint g_Uniform_Model_MaterialShininess_Sunflower = 0;
GLint g_Uniform_Model_Animation_Sunflower = 0;

GLuint g_ShaderProgramObject_Model_Bee = 0;

// Uniforms
GLint g_Uniform_Model_Model_Matrix_Bee = 0;
GLint g_Uniform_Model_View_Matrix_Bee = 0;
GLint g_Uniform_Model_Projection_Matrix_Bee = 0;
GLint g_Uniform_Model_LD_Bee = 0;
GLint g_Uniform_Model_LA_Bee = 0;
GLint g_Uniform_Model_LS_Bee = 0;

GLint g_Uniform_Model_KD_Bee = 0;
GLint g_Uniform_Model_KA_Bee = 0;
GLint g_Uniform_Model_KS_Bee = 0;

GLint g_Uniform_Model_lightPosition_animModel_Bee = 0;
GLint g_Uniform_Model_ViewPosition_Bee = 0;
GLint g_Uniform_Model_DiffuseMapSampler_Bee = 0;
GLint g_Uniform_Model_NormalMapSampler_Bee = 0;
GLint g_Uniform_Model_MaterialShininess_Bee = 0;
GLint g_Uniform_Model_Animation_Bee = 0;


AnimModel* myHoneyBee;
AnimModel* myNiceLotus0;
AnimModel* myNiceLotus1;



bool g_bLight = false;
bool g_bAnimationControl = false;
GLfloat lightAmbient_animModel[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse_animModel[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular_animModel[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition_animModel[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat material_ambient_animModel[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse_animModel[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular_animModel[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess_animModel = 50.0f;
double g_dBeeAnimateTime = 0.0;
double g_dSunFlowerAnimateTime = 0.1;

/*	Animated Model Loading Stuff	*/

/*		Lotus Code Start		*/


GLuint g_ShaderProgramObject_Model_LotusFlower = 0;

// Uniforms

GLuint g_UBO_InstancingNiceLotus1 = -1;
#define	NUM_INSTANCES_NICELOTUS1  5

glm::vec3 NiceLotus1Positions[] =
{
	glm::vec3(447.590820, 12.1f, 799.615112),	// Flower 1
	glm::vec3(485.091125, 12.1f, 1014.680237),	// Flower 2
	glm::vec3(439.730042, 12.1f, 508.287964),	// Flower 3
	glm::vec3(198.220093, 12.1f, 951.219910),	// Flower 4
	glm::vec3(261.527527, 12.1f, 637.014038),	// Flower 5
};

glm::vec3 NiceLotus1Scale[] =
{
	glm::vec3(1.6,1.6,1.6), // Flower 1
	glm::vec3(1.6,1.6,1.6), // Flower 2
	glm::vec3(1.6,1.6,1.6), // Flower 3
	glm::vec3(1.6,1.6,1.6), // Flower 4
	glm::vec3(1.6,1.6,1.6), // Flower 5
};

GLint g_Uniform_Model_Model_Matrix_LotusFlower = 0;
GLint g_Uniform_Model_View_Matrix_LotusFlower = 0;
GLint g_Uniform_Model_Projection_Matrix_LotusFlower = 0;

GLint g_Uniform_Model_ViewPosition_LotusFlower = 0;
GLint g_Uniform_Model_DiffuseMapSampler_LotusFlower = 0;
GLint g_Uniform_Model_NormalMapSampler_LotusFlower = 0;
GLint g_Uniform_Model_Animation_LotusFlower = 0;


// Shaders
GLuint g_ShaderProgramObject_LotusLeaf = 0;

// All Vertex Buffers
GLuint  g_VertexArrayObject_LotusLeaf0 = 0;
GLuint g_VertexBufferObject_LotusLeaf0_Position = 0;
GLuint g_VertexBufferObject_LotusLeaf0_Normal = 0;
GLuint g_VertexBufferObject_LotusLeaf0_TexCoord = 0;
GLuint g_VertexBufferObject_LotusLeaf0_Elements = 0;

GLuint  g_VertexArrayObject_LotusLeaf1 = 0;
GLuint g_VertexBufferObject_LotusLeaf1_Position = 0;
GLuint g_VertexBufferObject_LotusLeaf1_Normal = 0;
GLuint g_VertexBufferObject_LotusLeaf1_TexCoord = 0;
GLuint g_VertexBufferObject_LotusLeaf1_Elements = 0;

GLuint  g_VertexArrayObject_LotusLeaf2 = 0;
GLuint g_VertexBufferObject_LotusLeaf2_Position = 0;
GLuint g_VertexBufferObject_LotusLeaf2_Normal = 0;
GLuint g_VertexBufferObject_LotusLeaf2_TexCoord = 0;
GLuint g_VertexBufferObject_LotusLeaf2_Elements = 0;

// Uniforms
GLuint g_UBO_InstancingLotusLeafs0 = -1;
#define	NUM_INSTANCES_LOTUSLEAFS0  5
glm::vec3 LotusLeaf0Positions[] =
{
	glm::vec3(447.590820, 14.85f, 799.615112),	// Leaf 1
	glm::vec3(485.091125, 14.85f, 1014.680237),	// Leaf 2
	glm::vec3(439.730042, 14.85f, 508.287964),	// Leaf 3
	glm::vec3(198.220093, 14.85f, 951.219910),	// Leaf 4
	glm::vec3(261.527527, 14.85f, 637.014038),	// Leaf 5
};

glm::vec3 LotusLeaf0Scale[] =
{
	glm::vec3(2.0,2.0,2.0), // Leaf 1
	glm::vec3(2.0,2.0,2.0), // Leaf 2
	glm::vec3(2.0,2.0,2.0), // Leaf 3
	glm::vec3(2.0,2.0,2.0), // Leaf 4
	glm::vec3(2.0,2.0,2.0), // Leaf 5
};

GLuint g_Uniform_LotusLeaf_ModelMatrix = 0;
GLuint g_Uniform_LotusLeaf_ViewMatrix = 0;
GLuint g_Uniform_LotusLeaf_ProjectionMatrix = 0;
// sampler
GLuint g_uniform_LotusLeaf_DiffuseMapSampler;
GLuint g_uniform_LotusLeaf_BumpMapSampler;
GLuint g_uniform_LotusLeaf_SpecularMapSampler;

GLuint g_LotusLeaf_DiffuseTexture;
GLuint g_LotusLeaf_BumpTexture;
GLuint g_LotusLeaf_SpecularTexture;
/*		Lotus Code Stop 		*/


/*Particle System Start*/
// Shaders
GLuint g_ShaderProgramObject_Particle1 = 0;
GLuint g_ShaderProgramObject_Particle2 = 0;

// All Vertex Buffers
// for traveling in +ve direction
GLuint g_VertexArrayObject_Particle;
GLuint g_VertexBufferObject_ParticlePosition;
GLuint g_VertexBufferObject_ParticleColor;
GLuint g_VertexBufferObject_ParticleVelocity;
GLuint g_VertexBufferObject_ParticleStartTime;

// for traveling in -ve direction
GLuint  g_VertexArrayObject_ParticleInv;
GLuint g_VertexBufferObject_ParticleInvPosition;
GLuint g_VertexBufferObject_ParticleInvColor;
GLuint g_VertexBufferObject_ParticleInvVelocity;
GLuint g_VertexBufferObject_ParticleInvStartTime;

GLint g_arrayWidth_Particle, g_arrayHeight_Particle;
GLfloat* g_pfVerts_Particle = NULL;
GLfloat* g_pfColors_Particle = NULL;
GLfloat* g_pfVelocities_Particle = NULL;
GLfloat* g_pfStartTimes_Particle = NULL;

GLfloat g_pfParticleMoveTime_Particle1 = 0.0f;
GLfloat g_pfParticleMoveTime_Particle2 = 0.0f;

GLfloat g_pfParticleAnimeTime_Particle1 = -0.5f; // for blending
GLfloat g_pfParticleAnimeTime_Particle2 = -0.3f; // for blending
GLfloat g_pfParticleAnimeTime_Particle3 = -0.5f; // for blending
GLfloat g_pfParticleAnimeTime_Particle4 = -0.3f; // for blending

bool g_bDoneBlending1 = true;
bool g_bDoneBlending2 = false;
bool g_bDoneBlending3 = false;
bool g_bDoneBlending4 = true;


// for traveling in -ve direction
GLfloat* g_pfVelocities_ParticleInv = NULL;

// Uniforms
GLint g_Uniform_ModelMatrix_Particle1;
GLint g_Uniform_ViewMatrix_Particle1;
GLint g_Uniform_ProjectionMatrix_Particle1;
GLint g_Uniform_Time_Particle1;
GLint g_Uniform_Anime_Particle1;


GLint g_Uniform_ModelMatrix_Particle2;
GLint g_Uniform_ViewMatrix_Particle2;
GLint g_Uniform_ProjectionMatrix_Particle2;
GLint g_Uniform_Time_Particle2;
GLint g_Uniform_Anime_Particle2;
/*Particle System Stop */


GLfloat g_Angle_CubeMap_Rotation = 100.0f;
/*	All Control Variables Start 	*/


bool g_bIsDemoStarted = false;


GLfloat g_universalTimer = 0.0f;

GLfloat fireFly_waitTime = 0.0f;

// Camera Update Variables Start
GLuint SceneCounter = 1;

glm::vec2 CalculateYawPitch(glm::vec3 point_1, glm::vec3 point_2);

// Scene 1
// Sunrise and rotation
#define SUNRISE_TIMER_1	0.763994f
#define SUNRISE_POSITION_1 glm::vec3(-811.576233, 97.081673, -728.034973)
#define SUNRISE_YAW_1 -192.699936f
#define SUNRISE_FINAL_YAW_1 -273.300171f
#define SUNRISE_PITCH_1 0.0f
#define SUNRISE_ROTATION_SPEED_1 0.1f
// movement to sunflower view
#define FINAL_YAW_1		-270.471558f
#define FINAL_PITCH_1	-17.547005f
#define FINAL_POSITION_1 glm::vec3(-622.273071,132.553391,-292.651276)
#define YAW_INCREMENT_FACTOR_1 500.0f
#define PITCH_INCREMENT_FACTOR_1 500.0f

// Scene 2
// movement sunrise to sunfower
glm::vec2 g_angles_Scene;

GLfloat pitchIncrement;
GLfloat yawIncrement;

// Scene 3
// Bee Variables
glm::vec3 bee_path_constant = { -618.396851f, 92.936745f, -168.458038 };
glm::vec3 bee_pos_parabola = { 0.0f, 0.0f, 0.0f };
GLfloat bee_pos_parabola_angle_degrees = 0.0f;
GLfloat start_Bee_X = 180.615722f;
GLfloat start_Bee_Z = 45.683857f;
GLfloat Bee_Z_Increment = 45.683857f / 100.0f;
GLfloat p4 = 0.0f;
GLfloat Get_Bee_X(GLfloat Z);
GLfloat Get_Parabola_4P(GLfloat X, GLfloat Z);

vector<glm::vec3> bee_parabola;

GLfloat bee_wait_time = 0.0f;
GLfloat bee_wait_time_interval = 0.25f;

// Scene 4
#define FINAL_YAW_4	-337.044922f
#define FINAL_PITCH_4 9.852985f

#define FINAL_POSITION_4 glm::vec3(FINAL_POSITION_1.x, FINAL_POSITION_1.y, FINAL_POSITION_1.z)

GLfloat butterfly_scene_wait_time = 0.0f;

// Scene 5
int numberStepsCamToBeeCurve = 0;
int CamToBeeCurvePosition = 0;

#define CURVE_1_TOTAL_POINTS							1000

GLint CURVE_1_PART_1_POINTS = CURVE_1_TOTAL_POINTS / 5; // 200
GLfloat CURVE_1_PART_1_START_YAW = FINAL_YAW_4;
GLfloat CURVE_1_PART_1_START_PITCH = FINAL_PITCH_4;
GLfloat CURVE_1_PART_1_END_YAW = -335.900238f;
GLfloat CURVE_1_PART_1_END_PITCH = 20.000003f;
GLfloat CURVE_1_PART_1_YAW_INCREMENT = (CURVE_1_PART_1_END_YAW - CURVE_1_PART_1_START_YAW) / (GLfloat)CURVE_1_PART_1_POINTS; ;
GLfloat CURVE_1_PART_1_PITCH_INCREMENT = (CURVE_1_PART_1_END_PITCH - CURVE_1_PART_1_START_PITCH) / (GLfloat)CURVE_1_PART_1_POINTS; ;

GLint CURVE_1_PART_2_POINTS = CURVE_1_TOTAL_POINTS / 2; // 500
GLfloat CURVE_1_PART_2_START_YAW = CURVE_1_PART_1_END_YAW;
GLfloat CURVE_1_PART_2_START_PITCH = CURVE_1_PART_1_END_PITCH;
GLfloat CURVE_1_PART_2_END_YAW = -180.999725f;
GLfloat CURVE_1_PART_2_END_PITCH = -32.799984f;
GLfloat CURVE_1_PART_2_YAW_INCREMENT = (CURVE_1_PART_2_END_YAW - CURVE_1_PART_2_START_YAW) / (GLfloat)(CURVE_1_PART_2_POINTS - CURVE_1_PART_1_POINTS);
GLfloat CURVE_1_PART_2_PITCH_INCREMENT = (CURVE_1_PART_2_END_PITCH - CURVE_1_PART_2_START_PITCH) / (GLfloat)(CURVE_1_PART_2_POINTS - CURVE_1_PART_1_POINTS);

GLint CURVE_1_PART_3_POINTS = CURVE_1_TOTAL_POINTS * 3 / 4; // 750	
GLfloat CURVE_1_PART_3_START_YAW = CURVE_1_PART_2_END_YAW;
GLfloat CURVE_1_PART_3_START_PITCH = CURVE_1_PART_2_END_PITCH;
GLfloat CURVE_1_PART_3_END_YAW = -179.000305f;
GLfloat CURVE_1_PART_3_END_PITCH = -27.599991f;
GLfloat CURVE_1_PART_3_YAW_INCREMENT = (CURVE_1_PART_3_END_YAW - CURVE_1_PART_3_START_YAW) / (GLfloat)(CURVE_1_PART_3_POINTS - CURVE_1_PART_2_POINTS);
GLfloat CURVE_1_PART_3_PITCH_INCREMENT = (CURVE_1_PART_3_END_PITCH - CURVE_1_PART_3_START_PITCH) / (GLfloat)(CURVE_1_PART_3_POINTS - CURVE_1_PART_2_POINTS);

GLint CURVE_1_PART_4_POINTS = CURVE_1_TOTAL_POINTS; // 1000	
GLfloat CURVE_1_PART_4_START_YAW = CURVE_1_PART_3_END_YAW;
GLfloat CURVE_1_PART_4_START_PITCH = CURVE_1_PART_3_END_PITCH;
GLfloat CURVE_1_PART_4_END_YAW = -307.500305f;
GLfloat CURVE_1_PART_4_END_PITCH = -12.799990f;
GLfloat CURVE_1_PART_4_YAW_INCREMENT = (CURVE_1_PART_4_END_YAW - CURVE_1_PART_4_START_YAW) / (GLfloat)(CURVE_1_PART_4_POINTS - CURVE_1_PART_3_POINTS);
GLfloat CURVE_1_PART_4_PITCH_INCREMENT = (CURVE_1_PART_4_END_PITCH - CURVE_1_PART_4_START_PITCH) / (GLfloat)(CURVE_1_PART_4_POINTS - CURVE_1_PART_3_POINTS);

GLfloat Curve_1_YAW_Angle = CURVE_1_PART_1_START_YAW;
GLfloat Curve_1_Pitch_Angle = CURVE_1_PART_1_START_PITCH;

#define CAM_BEE_FINAL_POSITION glm::vec3(-419.225433f, 110.520264f, 134.322006f)
glm::vec3 cam_path_control_points[] = {
	glm::vec3(FINAL_POSITION_4.x, FINAL_POSITION_4.y, FINAL_POSITION_4.z),
	glm::vec3(320.864838f, 470.759583f, -16.659210f),
	glm::vec3(-662.555054f, 74.881302f, -227.996857f),
	glm::vec3(-518.725464f, 68.520264f, -78.177994f),
	glm::vec3(-191.252243f, 274.677704f, -127.355148f)
};

vector<glm::vec3> cam_to_bee_curve;

vector<glm::vec3> generateBezierCurve(glm::vec3 points[4], int numSteps);
GLuint vao_cam_to_bee_curve;
GLuint vbo_cam_to_bee_curve;
GLuint vao_cam_to_bee_control_points;
GLuint vbo_cam_to_bee_control_points;

// Scene 6
int numberStepsBeeToLake = 0;
int beeToLakePosition = 0;

#define CURVE_2_TOTAL_POINTS							1000

#define BEE_END_YAW -33.429108f
#define BEE_END_PITCH 14.286167f

GLint CURVE_2_PART_1_POINTS = CURVE_2_TOTAL_POINTS / 5; // 200
GLfloat CURVE_2_PART_1_START_YAW = 360.0f + CURVE_1_PART_4_END_YAW;
GLfloat CURVE_2_PART_1_START_PITCH = CURVE_1_PART_4_END_PITCH;
GLfloat CURVE_2_PART_1_END_YAW = 65.499847f;
GLfloat CURVE_2_PART_1_END_PITCH = 15.599983f;
GLfloat CURVE_2_PART_1_YAW_INCREMENT = (CURVE_2_PART_1_END_YAW - CURVE_2_PART_1_START_YAW) / (GLfloat)CURVE_2_PART_1_POINTS; ;
GLfloat CURVE_2_PART_1_PITCH_INCREMENT = (CURVE_2_PART_1_END_PITCH - CURVE_2_PART_1_START_PITCH) / (GLfloat)CURVE_2_PART_1_POINTS; ;

GLint CURVE_2_PART_2_POINTS = CURVE_2_TOTAL_POINTS / 2; // 500
GLfloat CURVE_2_PART_2_START_YAW = CURVE_2_PART_1_END_YAW;
GLfloat CURVE_2_PART_2_START_PITCH = CURVE_2_PART_1_END_PITCH;
GLfloat CURVE_2_PART_2_END_YAW = 68.299927f;
GLfloat CURVE_2_PART_2_END_PITCH = 6.699984f;
GLfloat CURVE_2_PART_2_YAW_INCREMENT = (CURVE_2_PART_2_END_YAW - CURVE_2_PART_2_START_YAW) / (GLfloat)(CURVE_2_PART_2_POINTS - CURVE_2_PART_1_POINTS);
GLfloat CURVE_2_PART_2_PITCH_INCREMENT = (CURVE_2_PART_2_END_PITCH - CURVE_2_PART_2_START_PITCH) / (GLfloat)(CURVE_2_PART_2_POINTS - CURVE_2_PART_1_POINTS);

GLint CURVE_2_PART_3_POINTS = CURVE_2_TOTAL_POINTS * 3 / 4; // 750	
GLfloat CURVE_2_PART_3_START_YAW = CURVE_2_PART_2_END_YAW;
GLfloat CURVE_2_PART_3_START_PITCH = CURVE_2_PART_2_END_PITCH;
GLfloat CURVE_2_PART_3_END_YAW = 46.421665f;
GLfloat CURVE_2_PART_3_END_PITCH = 3.399985f;
GLfloat CURVE_2_PART_3_YAW_INCREMENT = (CURVE_2_PART_3_END_YAW - CURVE_2_PART_3_START_YAW) / (GLfloat)(CURVE_2_PART_3_POINTS - CURVE_2_PART_2_POINTS);
GLfloat CURVE_2_PART_3_PITCH_INCREMENT = (CURVE_2_PART_3_END_PITCH - CURVE_2_PART_3_START_PITCH) / (GLfloat)(CURVE_2_PART_3_POINTS - CURVE_2_PART_2_POINTS);

GLint CURVE_2_PART_4_POINTS = CURVE_2_TOTAL_POINTS; // 1000	
GLfloat CURVE_2_PART_4_START_YAW = CURVE_2_PART_3_END_YAW;
GLfloat CURVE_2_PART_4_START_PITCH = CURVE_2_PART_3_END_PITCH;
GLfloat CURVE_2_PART_4_END_YAW = BEE_END_YAW;
GLfloat CURVE_2_PART_4_END_PITCH = BEE_END_PITCH;
GLfloat CURVE_2_PART_4_YAW_INCREMENT = (CURVE_2_PART_4_END_YAW - CURVE_2_PART_4_START_YAW) / (GLfloat)(CURVE_2_PART_4_POINTS - CURVE_2_PART_3_POINTS);
GLfloat CURVE_2_PART_4_PITCH_INCREMENT = (CURVE_2_PART_4_END_PITCH - CURVE_2_PART_4_START_PITCH) / (GLfloat)(CURVE_2_PART_4_POINTS - CURVE_2_PART_3_POINTS);

GLfloat Curve_2_YAW_Angle = CURVE_2_PART_1_START_YAW;
GLfloat Curve_2_Pitch_Angle = CURVE_2_PART_1_START_PITCH;

#define BEE_CONSTANT_ANGLE 300.0f
#define BEE_START_POSITION_MARI_TO_LAKE glm::vec3(-518.725464f, 68.520264f, -78.177994f)
#define BEE_END_POSITION_LAKE glm::vec3(171.445786f, 52.154884f, 822.021423f)
//#define BEE_END_POSITION_LAKE glm::vec3(204.377060f, 28.873436f, 764.429321f)
//glm::vec3 bee_to_lake_control_points[] = {
//	bee_start_position_mari_to_lake,
//	glm::vec3(-219.785461f, 20.520264f, 234.177994f),
//	glm::vec3(-167.785464f, 30.520264f, 927.177979f),
//	bee_end_position_lake  ///
//};
glm::vec3 bee_to_lake_control_points[] = {
	BEE_START_POSITION_MARI_TO_LAKE,
	glm::vec3(-219.785461f, 17.520264f, 234.177994f),
	glm::vec3(-165.285461f, 93.020264f, 911.677979f),
	BEE_END_POSITION_LAKE
};
vector<glm::vec3> bee_to_lake_curve;
GLuint vao_bee_to_lake_curve;
GLuint vbo_bee_to_lake_curve;
GLuint vao_bee_to_lake_control_points;
GLuint vbo_bee_to_lake_control_points;

// Scene 7
// bee to lotus
glm::vec3 bee_to_lotus_points[2] = {
	BEE_END_POSITION_LAKE,
	glm::vec3(448.518677f, 52.154884f, 798.074646f)
};
// SAM05 : set bee-Lotus Pos here
GLfloat BEE_TO_LOTUS_FINAL_HEIGHT = LotusLeaf0Positions[0].y;
glm::vec2 bee_to_lotus_angles;
glm::vec3 bee_to_lotus_front;

// Scene 8
GLfloat BEE_FLY_AWAY_HEIGHT = BEE_TO_LOTUS_FINAL_HEIGHT + 20.0f;
glm::vec3 bee_flyaway_points[2] = {
	bee_to_lotus_points[1],
	bee_to_lotus_points[1] + glm::vec3(0.0f, 0.0f, 1000.0f)
};

glm::vec2 bee_to_flyaway_angles;
glm::vec3 bee_to_flyaway_front;

GLfloat Scene8_TotalSteps = 100.0f;

// End

// AnimationControl Variablesd  Start
bool g_bSunIsRunning = true;
bool g_bAnimateSunflower = false;
bool g_bShowButterFlies = false;
bool g_bShowFireFlies = false;
// AnimationControl Variablesd  Stop 

GLuint lineProgam;
GLuint model_line_uniform;
GLuint view_line_uniform;
GLuint perspective_line_uniform;

GLuint vao_sunrise_sunflower;
GLuint vao_bee_parabola;
GLuint vao_sunflower_butterfly;
GLuint vao_bee_meet_point;
GLuint vao_butterfly_meet_point;
GLuint vao_bee_to_lake;

/*	All Control Variables Stop  	*/
int numChangeValue = 0;


/*	All Audio Variables Start	*/

AudioManager* pMasterAudio = NULL;
ALuint g_iAudioBufferId1;

SoundSource* g_pHeavenAndEarthSrc = NULL;
bool g_bPlayAudioOnce = false;

/*	All Audio Variables Stop	*/


/* All Font rendering Code Start */
CCredits * pCredits = NULL;
/* All Font rendering Code Stop */



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	//int UnInitialize(void);
	int Initialize(void);
	void Update(void);
	void Render(void);

	// Windowing Elelments
	WNDCLASSEX wndclass;
	MSG msg;
	HWND hwnd = NULL;
	TCHAR szClassName[] = TEXT("Sam_OGL");
	RECT windowRect;

	// Game Loop Control
	bool bDone = false;

	// Initialization Status
	int iInitRet = 0;


	SecureZeroMemory((void*)&wndclass, sizeof(wndclass));
	wndclass.cbSize = sizeof(wndclass);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = MainWndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);

	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, TEXT("Issue...!!!"), TEXT("Could Not RegisterClass() "), MB_OK | MB_ICONERROR);
		exit(EXIT_FAILURE);
	}

	if ((fopen_s(&g_pFile, "SamLogFile.txt", "w+")) == 0)
	{
		fprintf_s(g_pFile, "File Opened Successfully. \n");
	}
	else
	{
		MessageBox(NULL, TEXT("Issue...!!!"), TEXT("Could not open File"), MB_OK | MB_ICONERROR);
		exit(EXIT_FAILURE);
	}

	SecureZeroMemory((void*)&windowRect, sizeof(windowRect));
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.bottom = WIN_HEIGHT;
	windowRect.right = WIN_WIDTH;
	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szClassName,
		TEXT("First_OpenGL_Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("Issue...!!!"), TEXT("Could Not CreateWindow() "), MB_OK | MB_ICONERROR);
		exit(EXIT_FAILURE);
	}

	g_hwnd = hwnd;

	iInitRet = Initialize();
	switch (iInitRet)
	{
	case INIT_AUDIO_LOAD_FAILED:
		break;
	case INIT_AUDIO_MANAGER_INIT_FAILED:
		fprintf_s(g_pFile, "Failed to Initalize OpenAL \n");
		DestroyWindow(hwnd);
		break;
	case INIT_COMPUTE_SHADER_COMPILATION_FAILED:
		fprintf_s(g_pFile, "Failed to Compile Compute Shader \n");
		DestroyWindow(hwnd);
		break;
	case INIT_MODEL_LOADING_FAILED:
		fprintf_s(g_pFile, "Failed to Init Model Loading \n");
		DestroyWindow(hwnd);
		break;
	case INIT_FRAMEBUFFER_FAILED:
		fprintf_s(g_pFile, "Failed to Init Framebuffers \n");
		DestroyWindow(hwnd);
		break;

	case INIT_RESOURCE_LOAD_FAILED:
		fprintf_s(g_pFile, "Failed to load resources \n");
		DestroyWindow(hwnd);
		break;
	case INIT_GEOMETRY_SHADER_COMPILATION_FAILED:
		fprintf_s(g_pFile, "Failed to Compile geometry Shader \n");
		DestroyWindow(hwnd);
		break;
	case INIT_TESSILATION_EVALUATION_SHADER_COMPILATION_FAILED:
		fprintf_s(g_pFile, "Failed to Compile Tesselation Evaluation Shader \n");
		DestroyWindow(hwnd);
		break;
	case INIT_TESSILATION_CONTROL_SHADER_COMPILATION_FAILED:
		fprintf_s(g_pFile, "Failed to Compile Tesselation Control Shader \n");
		DestroyWindow(hwnd);
		break;
	case INIT_VERTEX_SHADER_COMPILATION_FAILED:
		fprintf_s(g_pFile, "Failed to Compile vertex Shader \n");
		DestroyWindow(hwnd);
		break;
	case INIT_FRAGMENT_SHADER_COMPILATION_FAILED:
		fprintf_s(g_pFile, "Failed to Compile fragment Shader \n");
		DestroyWindow(hwnd);
		break;
	case INIT_LINK_SHADER_PROGRAM_FAILED:
		fprintf_s(g_pFile, "Failed to Link Shader Program Object \n");
		DestroyWindow(hwnd);
		break;
	case INIT_FAIL_GLEW_INIT:
		fprintf_s(g_pFile, "Failed to glewInit \n");
		DestroyWindow(hwnd);
		break;
	case INIT_FAIL_BRIDGE_CONTEX_SET:
		fprintf_s(g_pFile, "Failed to wglMakeCurrent \n");
		DestroyWindow(hwnd);
		break;
	case INIT_FAIL_BRIDGE_CONTEX_CREATION:
		fprintf_s(g_pFile, "Failed to wglCreateContext \n");
		DestroyWindow(hwnd);
		break;
	case INIT_FAIL_SET_PIXEL_FORMAT:
		fprintf_s(g_pFile, "Failed to set Pixel Format \n");
		DestroyWindow(hwnd);
		break;
	case INIT_FAIL_NO_PIXEL_FORMAT:
		fprintf_s(g_pFile, "Failed to get PixelFormat \n");
		DestroyWindow(hwnd);
		break;
	case INIT_FAIL_NO_HDC:
		fprintf_s(g_pFile, "Failed to Get HDC \n");
		DestroyWindow(hwnd);
		break;
	case INIT_ALL_OK:
		fprintf_s(g_pFile, "Initialize Complete \n");
		break;
	default:
		fprintf_s(g_pFile, "Failed UnKnown Reasons \n");
		DestroyWindow(hwnd);
		break;
	}




	ShowWindow(hwnd, SW_SHOWNORMAL);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


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
			if (g_bWindowActive)
			{
				Update();
			}
			// Show all Animations
			Render();

		}
	}


	//UnInitialize();

	return ((int)msg.wParam);
}


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int UnInitialize(void);
	void FullScreen(void);
	bool Resize(int, int);
	void updateCurve(WPARAM wParam, vector<glm::vec3> & curve);


	switch (iMsg)
	{
	case WM_CREATE:
		PostMessage(hwnd, WM_KEYDOWN, (WPARAM)0x46, (LPARAM)NULL);
		break;

	case WM_SETFOCUS:
		g_bWindowActive = true;
		break;

	case WM_KILLFOCUS:
		g_bWindowActive = false;
		break;

	case WM_KEYDOWN:

		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case VK_SPACE:
			if (g_bIsDemoStarted == false)
			{
				g_bIsDemoStarted = true;

			}

			break;

		case 0x41:// A is pressed  

			mainCamera->ProcessKeyBoard(E_LEFT, DELTA);

			break;
		case 0x44:// D is pressed

			mainCamera->ProcessKeyBoard(E_RIGHT, DELTA);
			break;
		case 0x57:// W is pressed

			mainCamera->ProcessKeyBoard(E_FORWARD, DELTA);
			break;
		case 0x53:// S is pressed

			mainCamera->ProcessKeyBoard(E_BACKARD, DELTA);
			break;

		case 0x50:
		{
			glm::vec3 posSet = mainCamera->GetCameraPosition();
			posSet.y = g_pTerrain->GetHeightAt(posSet);
			fprintf_s(g_pFile, "%f,%f,%f\n", posSet.x, posSet.y, posSet.z);

			/*glm::vec3 tempVal = g_pSunLight->getAmbient();
			fprintf_s(g_pFile, "SunLight Ambient => %f,%f,%f\n", tempVal.r, tempVal.g, tempVal.b);

			ZeroMemory((void*)&tempVal,sizeof(glm::vec3));
			tempVal = g_pSunLight->getDiffuse();
			fprintf_s(g_pFile, "SunLight Diffudse => %f,%f,%f\n", tempVal.r, tempVal.g, tempVal.b);

			ZeroMemory((void*)&tempVal, sizeof(glm::vec3));
			tempVal = g_pSunLight->getSpecular();
			fprintf_s(g_pFile, "SunLight Specular => %f,%f,%f\n", tempVal.r, tempVal.g, tempVal.b);

			ZeroMemory((void*)&tempVal, sizeof(glm::vec3));
			tempVal = g_pSunLight->getDirection();
			fprintf_s(g_pFile, "SunLight Direction => %f,%f,%f\n", tempVal.x, tempVal.y, tempVal.z);*/

			// printy all camera properties
			glm::vec3 cameraPos = mainCamera->GetCameraPosition();
			fprintf_s(g_pFile, "Camera Position => %f,%f,%f\n", cameraPos.x, cameraPos.y, cameraPos.z);
			fprintf_s(g_pFile, "Cam Yaw   = %f\n", mainCamera->GetYaw());
			fprintf_s(g_pFile, "Cam Roll  = %f\n", mainCamera->GetRoll());
			fprintf_s(g_pFile, "Cam Pitch = %f\n", mainCamera->GetPitch());

			fprintf_s(g_pFile, "Universal Timer => %f\n", g_universalTimer);

			fprintf_s(g_pFile, "Cubemap Angle => %f\n", g_Angle_CubeMap_Rotation);

			fprintf_s(g_pFile, "Cam path control Points:\n");
			fprintf_s(g_pFile, "\n%f %f %f\n", cam_path_control_points[0].x, cam_path_control_points[0].y, cam_path_control_points[0].z);
			fprintf_s(g_pFile, "%f %f %f\n", cam_path_control_points[1].x, cam_path_control_points[1].y, cam_path_control_points[1].z);
			fprintf_s(g_pFile, "%f %f %f\n", cam_path_control_points[2].x, cam_path_control_points[2].y, cam_path_control_points[2].z);
			fprintf_s(g_pFile, "%f %f %f\n\n", cam_path_control_points[3].x, cam_path_control_points[3].y, cam_path_control_points[3].z);

			fprintf_s(g_pFile, "Bee to Lake control Points:\n");
			fprintf_s(g_pFile, "\n%f %f %f\n", bee_to_lake_control_points[0].x, bee_to_lake_control_points[0].y, bee_to_lake_control_points[0].z);
			fprintf_s(g_pFile, "%f %f %f\n", bee_to_lake_control_points[1].x, bee_to_lake_control_points[1].y, bee_to_lake_control_points[1].z);
			fprintf_s(g_pFile, "%f %f %f\n", bee_to_lake_control_points[2].x, bee_to_lake_control_points[2].y, bee_to_lake_control_points[2].z);
			fprintf_s(g_pFile, "%f %f %f\n\n", bee_to_lake_control_points[3].x, bee_to_lake_control_points[3].y, bee_to_lake_control_points[3].z);


		}
		break;


		case 0x5A://z or Z
				  //g_bSunIsRunning = (g_bSunIsRunning) ? false : true;
			break;

			// Arraow Keys
		case VK_UP:
			//triangleCamera.ProcessKeyBoard(E_UP, DELTA);
			mainCamera->ProcessKeyBoard(E_UP, DELTA);
			break;
		case VK_DOWN:
			//triangleCamera.ProcessKeyBoard(E_DOWN, DELTA);
			mainCamera->ProcessKeyBoard(E_DOWN, DELTA);
			break;
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case 0x46: // 'f' or 'F'
				   //MessageBox(hwnd, TEXT("F is pressed"), TEXT("Status"), MB_OK);
			FullScreen();
			break;
			//grass
		case 'L':
		case 'l':
			/*if (gbToggleLights == false)
			gbToggleLights = true;
			else
			gbToggleLights = false;
			break;*/
			//Wind or Breeze in grass
		case 'b':
		case 'B':
			//gbEnableWind = !gbEnableWind;
			break;

		case 'V':
		case 'v':
			break;

		case '1':
			numChangeValue = 0;
			break;

		case '2':
			numChangeValue = 1;
			break;

		case '3':
			numChangeValue = 2;
			break;

		case '4':
			numChangeValue = 3;
			break;

		case 'u':
		case 'U':
		case 'j':
		case 'J':
		case 'h':
		case 'H':
		case 'k':
		case 'K':
		case 't':
		case 'T':
		case 'y':
		case 'Y':
			updateCurve(wParam, bee_to_lake_curve);
			break;
		default:
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

			g_bFirstMouse = GL_FALSE;
		}

		GLfloat xOffset = xPos - g_fLastX;
		GLfloat yOffset = g_fLastY - yPos;

		/*g_fLastX = xPos;
		g_fLastY = yPos;*/

		g_fLastX = g_fCurrrentWidth / 2;
		g_fLastY = g_fCurrrentHeight / 2;


		mainCamera->ProcessMouseMovements(xOffset, yOffset, GL_TRUE);

	}
	break;

	case WM_MOUSEWHEEL:
	{
		GLfloat xPos = LOWORD(lParam);
		GLfloat yPos = HIWORD(lParam);

		mainCamera->ProcessMouseScrool(xPos, yPos);
	}
	break;

	case WM_SIZE:
		g_fCurrrentWidth = LOWORD(lParam);
		g_fCurrrentHeight = HIWORD(lParam);

		giWinWidth = LOWORD(lParam);
		giWinHeight = HIWORD(lParam);
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		return(0);
		//break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		UnInitialize();
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

int Initialize(void)
{

	int LoadGLTextures(GLuint * texture, TCHAR imageResourceId[]);
	GLuint LoadPngTexture(const char* pathToFile);
	void CreatePoints_FireFly(GLint iWidth, GLint iHeight);
	bool Resize(int, int);


	int iPixelIndex = 0;
	PIXELFORMATDESCRIPTOR pfd;


	GLenum err = NULL; // GLEW Error codes
	int iProgramErrors = 0;

	//Particle System
	GLuint iVertexShaderObject_Particle1 = 0;
	GLuint iGeometryShaderObject_Particle1 = 0;
	GLuint iFragmentShaderObject_Particle1 = 0;

	GLuint   iVertexShaderObject_Particle2 = 0;
	GLuint iGeometryShaderObject_Particle2 = 0;
	GLuint iFragmentShaderObject_Particle2 = 0;

	// CubeMap
	GLuint iShaderProgramObject_CubeMap = 0;
	GLuint iVertexShaderObject_CubeMap = 0;
	GLuint iFragmentShaderObject_CubeMap = 0;


	// Lotus Leaves
	GLuint iVertexShaderObject_LotusLeaf = 0;
	GLuint iFragmentShaderObject_LotusLeaf = 0;

	// Animated Model Shader Programs
	GLuint iVertexShaderObject_AnimatedModel_SunFlower = 0;
	GLuint iFragmentShaderObject_AnimatedModel_SunFlower = 0;

	GLuint iVertexShaderObject_AnimatedModel_Bee = 0;
	GLuint iFragmentShaderObject_AnimatedModel_Bee = 0;

	GLuint iVertexShaderObject_AnimatedModel_LotusFlower = 0;
	GLuint iFragmentShaderObject_AnimatedModel_LotusFlower = 0;

	// Water variables
	GLuint iVertexShaderObject_Water = 0;
	GLuint iFragmentShaderObject_Water = 0;


	// Shadow Variables
	GLuint iVertexShaderObject_depth;
	GLuint iFragmentShaderObject_depth;

	// Skydome Variables
	GLuint iVertexShaderObject_SkyDome;
	GLuint iFragmentShaderObject_SkyDome;

	GLuint iVertexShaderObject_HDR;
	GLuint iFragmentShaderObject_HDR;

	SecureZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	g_hdc = GetDC(g_hwnd);
	if (g_hdc == NULL)
	{
		return INIT_FAIL_NO_HDC;
	}

	iPixelIndex = ChoosePixelFormat(g_hdc, &pfd);
	if (iPixelIndex == 0)
	{
		return INIT_FAIL_NO_PIXEL_FORMAT;
	}

	if (SetPixelFormat(g_hdc, iPixelIndex, &pfd) == FALSE)
	{
		return INIT_FAIL_SET_PIXEL_FORMAT;
	}

	g_hrc = wglCreateContext(g_hdc);
	if (g_hrc == NULL)
	{
		return INIT_FAIL_BRIDGE_CONTEX_CREATION;
	}

	if (wglMakeCurrent(g_hdc, g_hrc) == FALSE)
	{
		return INIT_FAIL_BRIDGE_CONTEX_SET;
	}

	// Enables Feature Required for Programable Pipeline
	err = glewInit();
	if (err != GLEW_OK)
	{
		return INIT_FAIL_GLEW_INIT;
	}

	// GL information Start
	fprintf_s(g_pFile, "SHADER_INFO : Vendor is : %s\n", glGetString(GL_VENDOR));
	fprintf_s(g_pFile, "SHADER_INFO : Renderer is : %s\n", glGetString(GL_RENDER));
	fprintf_s(g_pFile, "SHADER_INFO : OpenGL Version is : %s\n", glGetString(GL_VERSION));
	fprintf_s(g_pFile, "SHADER_INFO : GLSL Version is : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	//fprintf_s(g_pFile, "SHADER_INFO : Extention is : %s \n", glGetString(GL_EXTENSIONS));
	// GL information End

	iProgramErrors = InitializeTriangle();
	if (iProgramErrors != INIT_ALL_OK)
	{
		return iProgramErrors;
	}
	g_pTerrain = new CTerrain();


	//modifiedTerrain.raw
	//std::string sHeightMapFileName = "TerrainData/modifiedTerrain.raw";
	std::string sHeightMapFileName = "TerrainData/modified terrain.raw";
	iProgramErrors = g_pTerrain->LoadHeightmap(sHeightMapFileName, 16, 257, 257);
	if (iProgramErrors != INIT_ALL_OK)
	{
		fprintf_s(g_pFile, "TERRAIN : LoadHeightMap() Failed\n");
		return iProgramErrors;
	}


	iProgramErrors = g_pTerrain->SetUpTerrain();
	if (iProgramErrors != INIT_ALL_OK)
	{
		fprintf_s(g_pFile, "TERRAIN : SetUpTerrain() Failed\n");
		return iProgramErrors;
	}

	iProgramErrors = InitializePoints(g_pTerrain);
	if (iProgramErrors != INIT_ALL_OK)
	{
		fprintf_s(g_pFile, "POINTS : InitializePoints() Failed\n");
		return iProgramErrors;
	}

	g_terrainShaderProg = g_pTerrain->GetShaderProgram();
	// Get al Uniforms here
	glUseProgram(0);
	glUseProgram(g_pTerrain->GetShaderProgram());
	g_UniformTerrain_XPixelOffset = glGetUniformLocation(g_terrainShaderProg, "uXPixelOffset");
	g_UniformTerrain_YPixelOffset = glGetUniformLocation(g_terrainShaderProg, "uYPixelOffset");
	g_UniformTerrain_ViewPosition = glGetUniformLocation(g_terrainShaderProg, "u_veiw_position");
	g_UniformTerrain_shadowBias = glGetUniformLocation(g_terrainShaderProg, "u_Light_scaleBias_matrix");
	g_UniformTerrain_lightprojrction_Matrix = glGetUniformLocation(g_terrainShaderProg, "u_Light_projection_matrix");
	g_UniformTerrain_lightView_Matrix = glGetUniformLocation(g_terrainShaderProg, "u_Light_view_matrix");

	g_UniformTerrain_sunAmbient = glGetUniformLocation(g_terrainShaderProg, "SunLight.ambient");
	g_UniformTerrain_sunSpecular = glGetUniformLocation(g_terrainShaderProg, "SunLight.specular");
	g_UniformTerrain_sunDiffuse = glGetUniformLocation(g_terrainShaderProg, "SunLight.diffuse");
	g_UniformTerrain_sunDirection = glGetUniformLocation(g_terrainShaderProg, "SunLight.direction");

	g_UniformTerrain_ClipPlane = glGetUniformLocation(g_terrainShaderProg, "u_Plane");
	glUseProgram(0);

	//mainCamera = new Camera(glm::vec3(1429.089233, 10.0f+g_pTerrain->GetHeightAt(glm::vec3(0.0, 0.0, 0.0)), -665.074524));

	//-380.662933,39.640911,-126.081200
	//mainCamera = new Camera(glm::vec3(-380.662933, 15.0f + g_pTerrain->GetHeightAt(glm::vec3(-380.662933, 0.0, -126.081200)), -126.081200));  -813.355042,42.379829,-732.573547
	// mainCamera = new Camera(glm::vec3(194.951309, 39.061901, 771.507141)); // Directly at Lake

	mainCamera = new Camera(glm::vec3(-813.355042, 15.0f + g_pTerrain->GetHeightAt(glm::vec3(-813.355042, 0.0, -732.573547)), -732.573547));

	//mainCamera = new Camera(glm::vec3(-558.973389, 15.0f + g_pTerrain->GetHeightAt(glm::vec3(-558.973389, 0.0, -482.023926)), -482.023926));

	//camera Position for  Flower Field : -49.183632, 44.357899, -800.866455
	// camera position for Lake View	:  630.146973,44.327904,402.961121
	// Cam Lake back : 593.565552,47.741188,623.833923
	// Cam Lake Back 2 : 194.951309,39.061901,771.507141


	//initialize grass
	bool statusGrass = InitGrass(g_pTerrain);
	if (statusGrass == false)
	{
		fprintf_s(g_pFile, "POINTS : InitializePoints() Failed\n");
		return -1;
	}


	//Palm Models
	PalmStraight = new ModelLoad::Model("models/Palm02_straight1/straightPalm1.obj");

	// normal code
	GLint progID = PalmStraight->GetProgram();
	GLint iLocationInProg;
	iLocationInProg = glGetUniformBlockIndex(progID, "perInstanceData");
	glUniformBlockBinding(progID, iLocationInProg, 0);

	glm::mat4* m4TempMatrix_palmST = (glm::mat4*)calloc(NUM_INSTANCES_PALMTREE, sizeof(glm::mat4));

	for (size_t i = 0; i < NUM_INSTANCES_PALMTREE; i++)
	{
		glm::mat4 translationMatrix = glm::mat4(1.0f);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		translationMatrix = glm::translate(glm::mat4(1.0f), PalmStraight_Pos[i]);
		scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(10.0f, 10.0f, 10.0f));
		modelMatrix = modelMatrix * translationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;

		m4TempMatrix_palmST[i] = modelMatrix;
	}


	glGenBuffers(1, &g_UBO_InstancingPlamST);
	glBindBuffer(GL_UNIFORM_BUFFER, g_UBO_InstancingPlamST);
	glBufferData(GL_UNIFORM_BUFFER, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4), m4TempMatrix_palmST, GL_DYNAMIC_DRAW);// filling the data with Marices
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// use it just before drawing to avoid conflicts
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamST, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));

	progID = PalmStraight->GetProgramShadow();
	iLocationInProg = glGetUniformBlockIndex(progID, "perInstanceData");
	glUniformBlockBinding(progID, iLocationInProg, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamST, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));


	progID = -1;
	iLocationInProg = -1;

	PalmTilted = new ModelLoad::Model("models/Palm03_Tilted/tiltedPalm.obj");
	progID = PalmTilted->GetProgram();
	iLocationInProg = glGetUniformBlockIndex(progID, "perInstanceData");
	glUniformBlockBinding(progID, iLocationInProg, 0);
	glm::mat4* m4TempMatrix_palmTILED = (glm::mat4*)calloc(NUM_INSTANCES_PALMTREE, sizeof(glm::mat4));
	for (size_t i = 0; i < NUM_INSTANCES_PALMTREE; i++)
	{
		glm::mat4 translationMatrix = glm::mat4(1.0f);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		translationMatrix = glm::translate(glm::mat4(1.0f), PalmTilted_Pos[i]);
		scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(10.0f, 10.0f, 10.0f));
		modelMatrix = modelMatrix * translationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;

		m4TempMatrix_palmTILED[i] = modelMatrix;
	}

	glGenBuffers(1, &g_UBO_InstancingPlamTILT);
	glBindBuffer(GL_UNIFORM_BUFFER, g_UBO_InstancingPlamTILT);
	glBufferData(GL_UNIFORM_BUFFER, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4), m4TempMatrix_palmTILED, GL_DYNAMIC_DRAW);// filling the data with Marices
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// use it just before drawing to avoid conflicts
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamTILT, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));

	progID = -1;
	iLocationInProg = -1;

	PalmBent = new ModelLoad::Model("models/Palm04_bent/bentPalm.obj");
	progID = PalmBent->GetProgram();


	iLocationInProg = glGetUniformBlockIndex(progID, "perInstanceData");
	glUniformBlockBinding(progID, iLocationInProg, 0);
	glm::mat4* m4TempMatrix_palmBENT = (glm::mat4*)calloc(NUM_INSTANCES_PALMTREE, sizeof(glm::mat4));
	for (size_t i = 0; i < NUM_INSTANCES_PALMTREE; i++)
	{
		glm::mat4 translationMatrix = glm::mat4(1.0f);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		translationMatrix = glm::translate(glm::mat4(1.0f), PalmBent_Pos[i]);
		scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(10.0f, 10.0f, 10.0f));
		modelMatrix = modelMatrix * translationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;

		m4TempMatrix_palmBENT[i] = modelMatrix;
	}

	glGenBuffers(1, &g_UBO_InstancingPlamBEN);
	glBindBuffer(GL_UNIFORM_BUFFER, g_UBO_InstancingPlamBEN);
	glBufferData(GL_UNIFORM_BUFFER, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4), m4TempMatrix_palmBENT, GL_DYNAMIC_DRAW);// filling the data with Marices
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// use it just before drawing to avoid conflicts
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamTILT, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));

	progID = -1;
	iLocationInProg = -1;

	//Bush Models
	/*BushType1 = new ModelLoad::Model("models/BushType01/bush1.obj");
	BushType2 = new ModelLoad::Model("models/BushType02/BushType2.obj");
	BushType3 = new ModelLoad::Model("models/BushType03/BushType3.obj");
	BushType4 = new ModelLoad::Model("models/BushType04/BushType4.obj");*/


	//Marigold 
	Marigold = new ModelLoad::Model("models/Marigold/marigold.obj");
	progID = Marigold->GetProgram();
	iLocationInProg;

	iLocationInProg = glGetUniformBlockIndex(progID, "perInstanceData");
	glUniformBlockBinding(progID, iLocationInProg, 0);

	//generate model matrix
	glm::mat4* m4TempMatrix = (glm::mat4*)calloc(NUM_INSTANCES_MARIGOLD, sizeof(glm::mat4));
	for (size_t i = 0; i < NUM_INSTANCES_MARIGOLD; i++)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 ScaleMatrix = glm::mat4(1.0f);
		glm::mat4 TranslationMatrix = glm::mat4(1.0f);
		glm::mat4 RotationMatrix = glm::mat4(1.0f);

		TranslationMatrix = glm::translate(glm::mat4(1.0f), MarigoldLocation[i]);
		if (MarigoldScaleValue[i] < 0.6f)
			MarigoldScaleValue[i] = 0.6f;

		ScaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(4.0f * MarigoldScaleValue[i], 4.0f * MarigoldScaleValue[i], 4.0f * MarigoldScaleValue[i]));
		RotationMatrix = glm::rotate(MarigoldScaleValue[i] * 360.0f, glm::vec3(0.0, 1.0, 0.0));
		modelMatrix = modelMatrix * TranslationMatrix;
		modelMatrix = modelMatrix * ScaleMatrix;
		modelMatrix = modelMatrix * RotationMatrix;
		m4TempMatrix[i] = modelMatrix;
	}

	// put in buffer
	glGenBuffers(1, &g_UBO_InstancingMarigold);
	glBindBuffer(GL_UNIFORM_BUFFER, g_UBO_InstancingMarigold);
	glBufferData(GL_UNIFORM_BUFFER, NUM_INSTANCES_MARIGOLD * sizeof(glm::mat4), m4TempMatrix, GL_DYNAMIC_DRAW);// filling the data with Marices
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// use it just before drawing to avoid conflicts
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingMarigold, 0, NUM_INSTANCES_MARIGOLD * sizeof(glm::mat4));
	progID = -1;
	iLocationInProg = -1;


	if (Marigold == nullptr)
	{
		return INIT_MODEL_LOADING_FAILED;
	}

	if (PalmStraight == nullptr || PalmTilted == nullptr || PalmBent == nullptr)
	{
		return INIT_MODEL_LOADING_FAILED;
	}

	/*	Line Shader Start	*/
	GLuint vso_line, fso_line;

	const GLchar* vssc_line = {
		"#version 430 core\n"
		"\n"
		"in vec4 vPosition;\n"
		"in vec4 vColor;\n"
		"\n"
		"uniform mat4 u_m;\n"
		"uniform mat4 u_v;\n"
		"uniform mat4 u_p;\n"
		"\n"
		"out vec4 outColor;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = u_p * u_v * u_m * vPosition;\n"
		"	outColor = vColor;\n"
		"}\n"
	};

	const GLchar* fssc_line = {
		"#version 430 core\n"
		"\n"
		"in vec4 outColor;\n"
		"\n"
		"out vec4 FragColor;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	FragColor = outColor;\n"
		"	FragColor.a = 1.0;\n"
		"}\n"
	};

	vso_line = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vso_line, 1, (GLchar**)&vssc_line, NULL);
	glCompileShader(vso_line);

	fso_line = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fso_line, 1, (GLchar**)&fssc_line, NULL);
	glCompileShader(fso_line);

	lineProgam = glCreateProgram();
	glAttachShader(lineProgam, vso_line);
	glAttachShader(lineProgam, fso_line);

	glBindAttribLocation(lineProgam, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(lineProgam, AMC_ATTRIBUTE_COLOR, "vColor");

	glLinkProgram(lineProgam);

	model_line_uniform = glGetUniformLocation(lineProgam, "u_m");
	view_line_uniform = glGetUniformLocation(lineProgam, "u_v");
	perspective_line_uniform = glGetUniformLocation(lineProgam, "u_p");

	glLinkProgram(0);

	glCreateVertexArrays(1, &vao_sunrise_sunflower);
	glBindVertexArray(vao_sunrise_sunflower);

	GLuint vbo_sunrise_sunflower;
	glCreateBuffers(1, &vbo_sunrise_sunflower);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_sunrise_sunflower);

	GLfloat points[] = {
		-811.576233f, 97.081673f, -728.034973f,
		FINAL_POSITION_1.x, FINAL_POSITION_1.y, FINAL_POSITION_1.z,

		bee_path_constant.x - 10000.0f, bee_path_constant.y, bee_path_constant.z,
		bee_path_constant.x + 10000.0f, bee_path_constant.y, bee_path_constant.z,

		bee_path_constant.x, bee_path_constant.y - 10000.0f, bee_path_constant.z,
		bee_path_constant.x, bee_path_constant.y + 10000.0f, bee_path_constant.z,

		bee_path_constant.x, bee_path_constant.y, bee_path_constant.z - 10000.0f,
		bee_path_constant.x, bee_path_constant.y, bee_path_constant.z + 10000.0f,

		bee_to_lotus_points[0].x, bee_to_lotus_points[0].y, bee_to_lotus_points[0].z,
		bee_to_lotus_points[1].x, bee_to_lotus_points[1].y, bee_to_lotus_points[1].z
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, points, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	/*	Line Shader Stop 	*/

	// Setup for Skybox Start
	/*Vertex Shader Start*/
	iVertexShaderObject_SkyDome = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexShaderSourceCode_SkyDome =
		"#version 430 core" \
		"\n" \
		"layout(location = 0) in vec4 vPos;" \
		"layout(location = 3) in vec2 vTexture0_Coord;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 vPosition;" \
		"void main(void)" \
		"{" \
		"	vPosition = vPos;" \
		"	gl_Position = u_mvp_matrix * vPos;" \
		"}";

	glShaderSource(iVertexShaderObject_SkyDome, 1, (const GLchar**)&vertexShaderSourceCode_SkyDome, NULL);
	glCompileShader(iVertexShaderObject_SkyDome);

	GLint iShaderCompileStatus = 0;
	GLint iGLInfoLogLength = 0;
	GLchar* szInfoLog = NULL;

	glGetShaderiv(iVertexShaderObject_SkyDome, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_SkyDome, GL_INFO_LOG_LENGTH, &iGLInfoLogLength);
		if (iGLInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iGLInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_SkyDome, iGLInfoLogLength, &written, szInfoLog);
				fprintf(g_pFile, "SKYDOME : Vertex Shader Compilation Log :: %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Vertex Shader Stop */

	/*Fragment Shader Start*/
	iFragmentShaderObject_SkyDome = glCreateShader(GL_FRAGMENT_SHADER);

	// Write Fragment Shader code.
	const GLchar* fragmentShaderSourceCode_SkyDome =
		"#version 430 core" \
		"\n" \
		"out vec4 FragColor; " \
		"in vec4 vPosition; " \

		"uniform float g; " \
		"uniform float g2; " \

		"uniform vec3 v3CameraPos; " \
		"uniform vec3 v3LightPos; " \
		"uniform vec3 v3InvWaveLength; " \
		"uniform float fCameraHeight; " \
		"uniform float fCameraHeight2; " \
		"uniform float fOuterRadius; " \
		"uniform float fOuterRadius2; " \
		"uniform float fInnerRadius; " \
		"uniform float fInnerRadius2; " \
		"uniform float fKrESun; " \
		"uniform float fKmESun; " \
		"uniform float fKr4PI; " \
		"uniform float fKm4PI; " \
		"uniform float fScale; " \
		"uniform float fScaleDepth; " \
		"uniform float fScaleOverScaleDepth; " \
		"uniform int Samples; " \
		"uniform float alpha; " \

		"float scale(float fCos)" \
		"{" \
		"	float x = 1.0 - fCos; " \
		"	return(fScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))))); " \
		"}" \

		"void main(void)" \
		"{" \

		"	vec3 v3Direction; " \
		"	vec3 PrimaryColor; " \
		"	vec3 SecondaryColor; " \


		//Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
		"	vec3 v3Pos = vPosition.xyz; " \
		"	vec3 v3Ray = v3Pos - v3CameraPos; " \
		"	float fFar = length(v3Ray); " \
		"	v3Ray /= fFar; " \

		//Calculate the ray's starting position, then calculate its scattering offset
		"	vec3 v3Start = v3CameraPos; " \
		"	float fHeight = length(v3Start); " \
		"	float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight)); " \
		"	float fStartAngle = dot(v3Ray, v3Start) / fHeight; " \
		"	float fStartOffset = fDepth * scale(fStartAngle); " \

		//Initialize the scattering loop variables
		"	float fSampleLength = fFar / Samples; " \
		"	float fScaledLength = fSampleLength * fScale; " \
		"	vec3 v3SampleRay = v3Ray * fSampleLength; " \
		"	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5; " \

		//Now loop through the sample rays
		"	vec3 v3FrontColor = vec3(0.0); " \
		"	for (int i = 0; i < Samples; i++)" \
		"	{" \
		"		float fHeight = length(v3SamplePoint); " \
		"		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight)); " \
		"		float fLightAngle = dot(v3LightPos, v3SamplePoint) / fHeight; " \
		"		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight; " \
		"		float fScatter = (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle))); " \
		"		vec3 v3Attenuate = exp(-fScatter * (v3InvWaveLength * fKr4PI + fKm4PI)); " \
		"		v3FrontColor += v3Attenuate * (fDepth * fScaledLength); " \
		"		v3SamplePoint += v3SampleRay; " \
		"	}" \

		//Finally, scale the Mie and Rayleight colors and
		"	SecondaryColor = v3FrontColor * fKmESun; " \
		"	PrimaryColor = v3FrontColor * (v3InvWaveLength * fKrESun); " \
		"	v3Direction = v3CameraPos - v3Pos; " \

		"	float fCos = dot(v3LightPos, v3Direction) / length(v3Direction); " \
		"	float fRayleighPhase = 1.0 + fCos * fCos; " \
		"	float fMeiPhase = (1.0 - g2) / (2.0 + g2) * (1.0 + fCos * fCos) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5); " \
		"	vec3 Color = 1.0 - exp(-1.5 * (fRayleighPhase * PrimaryColor + fMeiPhase * SecondaryColor)); " \
		//FragColor = vec4(pow(Color, vec3(1.0 / 2.2)), 1.0); " \ // Gamma correction
		"	FragColor = vec4(Color, alpha); " \
		"}";


	glShaderSource(iFragmentShaderObject_SkyDome, 1, (const GLchar**)&fragmentShaderSourceCode_SkyDome, NULL);

	glCompileShader(iFragmentShaderObject_SkyDome);

	iShaderCompileStatus = 0;
	iGLInfoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_SkyDome, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_SkyDome, GL_INFO_LOG_LENGTH, &iGLInfoLogLength);
		if (iGLInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iGLInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_SkyDome, iGLInfoLogLength, &written, szInfoLog);
				fprintf(g_pFile, "SKYDOME : Fragment Shader Compilation Log :: %s \n", szInfoLog);
				free(szInfoLog);
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Fragment Shader Stop */

	gShaderProgramObject_SkyDome = glCreateProgram();
	glAttachShader(gShaderProgramObject_SkyDome, iVertexShaderObject_SkyDome);
	glAttachShader(gShaderProgramObject_SkyDome, iFragmentShaderObject_SkyDome);
	glBindAttribLocation(gShaderProgramObject_SkyDome, AMC_ATTRIBUTE_POSITION, "vPosition");
	glLinkProgram(gShaderProgramObject_SkyDome);
	GLint iProgramLinkStatus = 0;
	iGLInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderProgramObject_SkyDome, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_SkyDome, GL_INFO_LOG_LENGTH, &iGLInfoLogLength);
		if (iGLInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iGLInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_SkyDome, iGLInfoLogLength, &written, szInfoLog);
				fprintf(g_pFile, "SKYDOME : Link status Log :: %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
			}
		}
	}
	// Setup for Skybox Stop 

	// SetupFor HDR Start
	/*Vertex Shader Start*/
	iVertexShaderObject_HDR = glCreateShader(GL_VERTEX_SHADER);

	// Write Vertex Shader code.
	const GLchar* vertexShaderSourceCode_HDR =
		"#version 430 core" \
		"\n" \
		"layout(location = 0) in vec3 vPosition;" \
		"layout(location = 5) in vec2 vTexCoords;" \
		"out vec2 out_texcoord;" \
		"void main(void)" \
		"{" \
		"	out_texcoord = vTexCoords;" \
		"	gl_Position = vec4(vPosition, 1.0);" \
		"}";

	glShaderSource(iVertexShaderObject_HDR, 1, (const GLchar**)&vertexShaderSourceCode_HDR, NULL);
	glCompileShader(iVertexShaderObject_HDR);
	iShaderCompileStatus = 0;
	iGLInfoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject_HDR, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_HDR, GL_INFO_LOG_LENGTH, &iGLInfoLogLength);
		if (iGLInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iGLInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_HDR, iGLInfoLogLength, &written, szInfoLog);
				fprintf(g_pFile, "HDR : Vertex Shader Compilation Log :: %s \n", szInfoLog);
				free(szInfoLog);
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Vertex Shader Stop */

	/*Fragment Shader Start */
	iFragmentShaderObject_HDR = glCreateShader(GL_FRAGMENT_SHADER);

	// Write Fragment Shader code.
	const GLchar* fragmentShaderSourceCode_HDR =
		"#version 430 core" \
		"\n" \
		"out vec4 FragColor;" \
		"in vec2 out_texcoord;" \
		"uniform sampler2D HDRTexture;" \
		"uniform float u_AlphaVal;" \
		"uniform float exposure;" \
		"const float gamma = 2.2;" \
		"const float A = 0.15;" \
		"const float B = 0.50;" \
		"const float C = 0.10;" \
		"const float D = 0.20;" \
		"const float E = 0.02;" \
		"const float F = 0.30;" \
		"const float W = 11.2;" \
		"void main(void)" \
		"{" \
		"	vec3 HDRColor = texture(HDRTexture, out_texcoord).rgb;" \
		"	FragColor = vec4(HDRColor, u_AlphaVal);" \
		"}";

	glShaderSource(iFragmentShaderObject_HDR, 1, (const GLchar**)&fragmentShaderSourceCode_HDR, NULL);

	glCompileShader(iFragmentShaderObject_HDR);

	iShaderCompileStatus = 0;
	iGLInfoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_HDR, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_HDR, GL_INFO_LOG_LENGTH, &iGLInfoLogLength);
		if (iGLInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iGLInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_HDR, iGLInfoLogLength, &written, szInfoLog);
				fprintf(g_pFile, "HDR : Fragment Shader Compilation Log :: %s \n", szInfoLog);
				free(szInfoLog);
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Fragment Shader Stop  */

	/*	Shader Program Start	*/
	gShaderObjectHDR = glCreateProgram();

	glAttachShader(gShaderObjectHDR, iVertexShaderObject_HDR);
	glAttachShader(gShaderObjectHDR, iFragmentShaderObject_HDR);

	//glBindAttribLocation(gShaderObjectHDR, AMC_ATTRIBUTE_POSITION, "vPosition");
	//glBindAttribLocation(gShaderObjectHDR, AMC_ATTRIBUTE_TEXTURE0, "vTexCoords");

	//Link the Shader Program
	glLinkProgram(gShaderObjectHDR);

	iProgramLinkStatus = 0;
	iGLInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderObjectHDR, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderObjectHDR, GL_INFO_LOG_LENGTH, &iGLInfoLogLength);
		if (iGLInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iGLInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderObjectHDR, iGLInfoLogLength, &written, szInfoLog);
				fprintf(g_pFile, "HDR : Link status Log :: %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
	}
	/*	Shader Program Stop 	*/

	//Init HDR
	InitializeQuadHDR();
	g_pSkyDome = new CSkyDome();

	g_pSkyDome->SetShaderProgramObject(gShaderProgramObject_SkyDome);
	g_pSkyDome->InitializeSky(g_pSkyDome->GetShaderProgramObject());
	// SetupFor HDR Start

	// setup for shadow  Start
	/*Vertex Shader Start*/
	iVertexShaderObject_depth = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexShaderDepthSourceCode = "#version 450 core"	\
		"\n" \
		"layout (location = 0)in vec4 vPosition;\n" \
		"uniform mat4 u_scene_model_matrix;\n" \
		"uniform mat4 u_Light_view_matrix;\n" \
		"uniform mat4 u_Light_projection_matrix;\n" \
		"void main(void)" \
		"{\n" \
		"	mat4 shadowMatrix = u_Light_projection_matrix * u_Light_view_matrix * u_scene_model_matrix;\n"	\
		"	gl_Position = shadowMatrix * vPosition;\n" \
		"}";

	glShaderSource(iVertexShaderObject_depth, 1, (const GLchar**)&vertexShaderDepthSourceCode, NULL);

	// Compile it
	glCompileShader(iVertexShaderObject_depth);
	GLint iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject_depth, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_depth, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_depth, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Vertex Shader Depth Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Vertex Shader End*/

	/*Fragment Shader Start*/
	iFragmentShaderObject_depth = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragmentShaderDepthSourceCode = "#version 450 core"	\
		"\n"	\
		"layout (location = 0)out vec4 FragColor;"	\
		"void main(void)"	\
		"{"	\
		"	FragColor = vec4(1.0);"	\
		"}";

	glShaderSource(iFragmentShaderObject_depth, 1, (const GLchar**)&fragmentShaderDepthSourceCode, NULL);
	glCompileShader(iFragmentShaderObject_depth);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_depth, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_depth, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_depth, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf(g_pFile, "ERROR: Fragment Shader Depth Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Fragment Shader End*/
	/* Shader Program Start */
	g_ShaderProgramObject_depth = glCreateProgram();
	glAttachShader(g_ShaderProgramObject_depth, iVertexShaderObject_depth);
	glAttachShader(g_ShaderProgramObject_depth, iFragmentShaderObject_depth);
	glBindAttribLocation(g_ShaderProgramObject_depth, AMC_ATTRIBUTE_POSITION, "vPosition");
	glLinkProgram(g_ShaderProgramObject_depth);

	GLint iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	glGetProgramiv(g_ShaderProgramObject_depth, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_depth, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(g_ShaderProgramObject_depth, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Linking Shader Program Objects Failed %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
			}
		}
	}
	/* Shader Program End */
	// get all uniforms
	g_depth_U_Model_Matrix = glGetUniformLocation(g_ShaderProgramObject_depth, "u_scene_model_matrix");
	g_depth_U_View_Matrix = glGetUniformLocation(g_ShaderProgramObject_depth, "u_Light_view_matrix");
	g_depth_U_Projection_Matrix = glGetUniformLocation(g_ShaderProgramObject_depth, "u_Light_projection_matrix");

	// Setup all Framebuffers and Textures required for scenes

	/*	Shadow Start	*/
	glGenTextures(1, &g_Texture_ShadowDepthTexure);
	glBindTexture(GL_TEXTURE_2D, g_Texture_ShadowDepthTexure);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glm::vec4 borderColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(borderColor));
	glBindTexture(GL_TEXTURE_2D, 0);

	// Prepare depth texture
	glGenFramebuffers(1, &g_ShadowFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, g_ShadowFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_Texture_ShadowDepthTexure, 0);
	glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	GLenum frameBufferStaus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (frameBufferStaus != GL_FRAMEBUFFER_COMPLETE)
	{
		MessageBox(g_hwnd, TEXT("Incomplete framebuffer"), TEXT("FBO Error"), MB_OK | MB_ICONERROR);
		fprintf_s(g_pFile, "FRAMEBUFFER : g_ShadowFramebuffer In-Complete Failed\n");
		return INIT_FRAMEBUFFER_FAILED;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// setup for shadow  Stop 
	/*	Shadow Stop 	*/

	/*		MSAA FBO Start		*/
	glGenFramebuffers(1, &gFBOMSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, gFBOMSAA);

	// create a multisampled color attachment
	glGenTextures(1, &gTextureMSAA);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gTextureMSAA);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB8, giWinWidth, giWinHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, gTextureMSAA, 0);

	glGenRenderbuffers(1, &gRBOMSAA);
	glBindRenderbuffer(GL_RENDERBUFFER, gRBOMSAA);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, giWinWidth, giWinHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gRBOMSAA);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(g_pFile, "Framebuffer gRBOMSAA not complete msaa!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/*		MSAA FBO Stop 		*/

	/*		HDR FBO Start		*/
	glGenFramebuffers(1, &gFrameBufferObjectHDR);
	glBindFramebuffer(GL_FRAMEBUFFER, gFrameBufferObjectHDR);

	glGenTextures(1, &gColorBufferTexture);
	glBindTexture(GL_TEXTURE_2D, gColorBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, giWinWidth, giWinHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// create depth buffer (renderbuffer)
	glGenRenderbuffers(1, &gRenderBufferObjectDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, gRenderBufferObjectDepth);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, giWinWidth, giWinHeight); // Only depth component was not working => Check it out...!!
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, giWinWidth, giWinHeight);

	// attach buffers
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gColorBufferTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gRenderBufferObjectDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{

		fprintf(g_pFile, "Framebuffer not complete!\n");

	}
	/*		HDR FBO Stop 		*/

	/*		Occlusion FBO Start	*/
	glGenFramebuffers(1, &OcclusionFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, OcclusionFBO);
	// create a texture for occlusion map
	glGenTextures(1, &OcclusionMapTexture);
	glBindTexture(GL_TEXTURE_2D, OcclusionMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, giWinWidth, giWinHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OcclusionMapTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/*		Occlusion FBO Stop 		*/

	g_pSunLight = new CSunLight();
	g_pSunLight->setAmbient(glm::vec3(0.2f, 0.2f, 0.2f));
	g_pSunLight->setDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	g_pSunLight->setSpecular(glm::vec3(1.0f, 1.0f, 1.0f));
	g_pSunLight->setDirection(glm::vec3(0.0f, 1.0f, 0.0f));

	/****  Water Code Start   ****/
	iVertexShaderObject_Water = glCreateShader(GL_VERTEX_SHADER);
	/*const GLchar *vertexShaderSourceCode_Water = "#version 450 core"	\
	"\n" \
	"layout (location = 0)in vec4 vPosition;\n" \
	"layout (location = 5)in vec2 vTexCoords;\n" \
	"uniform mat4 u_model_matrix,u_view_matrix,u_projection_matrix;\n" \
	"uniform vec3 cameraPosition;\n"	\
	"uniform vec3 u_LightPosition;\n"	\
	"layout (location = 0)out vec4 clipSpace;\n"	\
	"layout (location = 1)out vec2 out_TexCoords;\n" \
	"layout (location = 2)out vec3 out_toCameraVector;\n" \
	"layout (location = 3)out vec3 out_fromLightVector;\n" \
	"const float tilling = 6.0;\n"	\
	"void main(void)" \
	"{" \
	"	vec4 worldPosition = u_model_matrix * vPosition;\n"	\
	"	clipSpace = u_projection_matrix * u_view_matrix * worldPosition;\n"	\
	"	gl_Position =  clipSpace;\n" \
	"	out_TexCoords = vTexCoords * tilling;\n"	\
	"	out_TexCoords = vTexCoords;\n"	\
	"	out_toCameraVector = cameraPosition - worldPosition.xyz;\n"	\
	"	out_fromLightVector = worldPosition.xyz - u_LightPosition;\n"	\
	"}";*/

	const GLchar* vertexShaderSourceCode_Water = "#version 450 core"	\
		"\n" \
		"layout (location = 0)in vec4 vPosition;\n" \
		"layout (location = 5)in vec2 vTexCoords;\n" \
		"uniform mat4 u_model_matrix,u_view_matrix,u_projection_matrix;\n" \
		"uniform vec3 cameraPosition;\n"	\
		"uniform vec3 u_LightPosition;\n"	\
		"layout (location = 0)out vec2 out_TexCoords;\n" \
		"layout (location = 1)out vec4 clipSpace;\n"	\
		"layout (location = 2)out vec3 out_toCameraVector;\n" \
		"layout (location = 3)out vec3 out_fromLightVector;\n" \
		"const float tilling = 6.0;\n"	\
		"void main(void)" \
		"{" \
		"	vec4 worldPosition = u_model_matrix * vPosition;\n"	\
		"	clipSpace = u_projection_matrix * u_view_matrix * worldPosition;\n"	\
		"	gl_Position = clipSpace;\n"	\
		"	out_TexCoords = vTexCoords * tilling;\n"	\
		"	out_toCameraVector = cameraPosition - worldPosition.xyz;\n"	\
		"	out_fromLightVector = worldPosition.xyz - u_LightPosition;\n"	\
		"}";

	glShaderSource(iVertexShaderObject_Water, 1, (const GLchar**)&vertexShaderSourceCode_Water, NULL);

	// Compile it
	glCompileShader(iVertexShaderObject_Water);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject_Water, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_Water, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_Water, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Vertex Shader Water Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}

	/*Vertex Shader End*/

	/*Fragment Shader Start*/
	iFragmentShaderObject_Water = glCreateShader(GL_FRAGMENT_SHADER);
	/*const GLchar *fragmentShaderSourceCode_Water = "#version 450 core"	\
	"\n"	\
	"layout (location = 0)in vec4 clipSpace;\n"	\
	"layout (location = 1)in vec2 out_TexCoords;\n" \
	"layout (location = 2)in vec3 out_toCameraVector;\n" \
	"layout (location = 3)in vec3 out_fromLightVector;\n" \
	"layout (location = 0)out vec4 FragColor;"	\
	"uniform sampler2D u_texture_reflection;"	\
	"uniform sampler2D u_texture_refraction;"	\
	"uniform sampler2D u_texture_dudvMap;"	\
	"uniform sampler2D u_texture_normalMap;"	\
	"uniform sampler2D u_texture_depthMap;"	\
	"uniform float u_fMoveFactor;\n"	\
	"uniform vec3 u_LightColor;\n"	\
	"const float waveStrength = 0.02;\n"	\
	"const float shineDamper = 20.0;\n"	\
	"const float reflectivity = 0.6;\n"	\
	"const float nearPlane = 0.1;\n"	\
	"const float farPlane = 1000.0;\n"	\
	"void main(void)"	\
	"{"	\
	"	vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;\n"	\
	"	vec2 refractionTexcoord = vec2(ndc.x,ndc.y);\n" \
	"	vec2 reflectionTexcoord = vec2(ndc.x,-ndc.y);\n" \
	"	float depth = texture(u_texture_depthMap,refractionTexcoord).r;\n"	\
	"	float floorDistance = 2.0 * nearPlane * farPlane/(farPlane+nearPlane - (2.0*depth-1.0)*(farPlane-nearPlane));\n"	\
	"	depth = gl_FragCoord.z;\n"	\
	"	float waterDistance = 2.0 * nearPlane * farPlane/(farPlane+nearPlane - (2.0*depth-1.0)*(farPlane-nearPlane));\n"	\
	"	float waterDepth = floorDistance-waterDistance;\n"	\
	"	vec2 distortedTexcoords = texture(u_texture_dudvMap,vec2(out_TexCoords.x+u_fMoveFactor,out_TexCoords.y)).rg*0.1;\n"	\
	"	distortedTexcoords = out_TexCoords+vec2(distortedTexcoords.x,distortedTexcoords.y+u_fMoveFactor);\n"	\
	"	vec2 totalDistortion = (texture(u_texture_dudvMap,distortedTexcoords).rg*2.0-1.0) * waveStrength *clamp(waterDepth/20.0,0.0,1.0);\n"	\
	"	refractionTexcoord += totalDistortion;\n"	\
	"	refractionTexcoord = clamp(refractionTexcoord,0.001,0.999);\n"	\
	"	reflectionTexcoord += totalDistortion;\n"	\
	"	reflectionTexcoord.x = clamp(reflectionTexcoord.x,0.001,0.999);\n"	\
	"	reflectionTexcoord.y = clamp(reflectionTexcoord.y,-0.999,-0.001);\n"	\
	"	vec4 reflectionColor = texture(u_texture_reflection,reflectionTexcoord);\n"	\
	"	vec4 refractionColor = texture(u_texture_refraction,refractionTexcoord);\n"	\
	"	vec4 normalMapColor = texture(u_texture_normalMap,distortedTexcoords);\n"	\
	"	vec3 normal = vec3(normalMapColor.r *2.0-1.0,normalMapColor.b * 3.0,normalMapColor.g*2.0-1.0);\n"	\
	"	normal = normalize(normal);\n"	\
	"	vec3 viewVector = normalize(out_toCameraVector);\n"	\
	"	float refractiveFactor = dot(viewVector,normal);\n"	\
	"	refractiveFactor = pow(refractiveFactor,5.0);\n"	\
	"	vec3 reflectedLight  = reflect(normalize(out_fromLightVector),normal);\n"	\
	"	float specular = max(dot(reflectedLight,viewVector),0.0);\n"	\
	"	specular = pow(specular,shineDamper);\n"	\
	"	vec3 specularHighlights = u_LightColor * specular * reflectivity * clamp(waterDepth/5.0,0.0,1.0);\n"	\
	"	FragColor = mix(reflectionColor,refractionColor,refractiveFactor);\n"	\
	"	FragColor = mix(FragColor,vec4(0.0,0.3,0.5,1.0),0.2) + vec4(specularHighlights,0.0);\n"	\
	"   FragColor.a = clamp(waterDepth/5.0,0.0,1.0);\n"	\
	"	FragColor = texture(u_texture_refraction,totalDistortion);\n"	\
	"}";*/
	const GLchar* fragmentShaderSourceCode_Water =
		"#version 450 core"	\
		"\n"	\
		"layout (location = 0)in vec2 out_TexCoords;\n" \
		"layout (location = 1)in vec4 clipSpace;\n"	\
		"layout (location = 2)in vec3 out_toCameraVector;\n" \
		"layout (location = 3)in vec3 out_fromLightVector;\n" \
		"layout (location = 0)out vec4 FragColor;\n"	\
		"uniform sampler2D u_texture_reflection;"	\
		"uniform sampler2D u_texture_refraction;"	\
		"uniform sampler2D u_texture_dudvMap;"	\
		"uniform sampler2D u_texture_normalMap;"	\
		"uniform sampler2D u_texture_depthMap;"	\
		"const float waveStrength = 0.02;\n"	\
		"const float shineDamper = 20.0;\n"	\
		"const float reflectivity = 0.6;\n"	\
		"const float nearPlane = 0.1;\n"	\
		"const float farPlane = 1000.0;\n"	\
		"uniform float u_fMoveFactor;\n"	\
		"uniform vec3 u_LightColor;\n"	\
		"void main ()\n"	\
		"{\n"	\
		"	vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;\n"	\
		"	vec2 refractionTexcoord = vec2(ndc.x,ndc.y);\n" \
		"	vec2 reflectionTexcoord = vec2(ndc.x,-ndc.y);\n" \
		"	float depth = texture(u_texture_depthMap,refractionTexcoord).r;\n"	\
		"	float floorDistance = 2.0 * nearPlane * farPlane/(farPlane+nearPlane - (2.0*depth-1.0)*(farPlane-nearPlane));\n"	\
		"	depth = gl_FragCoord.z;\n"	\
		"	float waterDistance = 2.0 * nearPlane * farPlane/(farPlane+nearPlane - (2.0*depth-1.0)*(farPlane-nearPlane));\n"	\
		"	float waterDepth = floorDistance-waterDistance;\n"	\
		"	vec2 distortedTexcoords = texture(u_texture_dudvMap,vec2(out_TexCoords.x+u_fMoveFactor,out_TexCoords.y)).rg*0.1;\n"	\
		"	distortedTexcoords = out_TexCoords+vec2(distortedTexcoords.x,distortedTexcoords.y+u_fMoveFactor);\n"	\
		"	vec2 totalDistortion = (texture(u_texture_dudvMap,distortedTexcoords).rg*2.0-1.0) * waveStrength * clamp(waterDepth/20.0,0.0,1.0);\n"	\
		"	refractionTexcoord += totalDistortion;\n"	\
		"	refractionTexcoord = clamp(refractionTexcoord,0.001,0.999);\n"	\
		"	reflectionTexcoord += totalDistortion;\n"	\
		"	reflectionTexcoord.x = clamp(reflectionTexcoord.x,0.001,0.999);\n"	\
		"	reflectionTexcoord.y = clamp(reflectionTexcoord.y,-0.999,-0.001);\n"	\
		"	vec4 reflectionColor = texture(u_texture_reflection,reflectionTexcoord);\n"	\
		"	vec4 refractionColor = texture(u_texture_refraction,refractionTexcoord);\n"	\
		"	vec4 normalMapColor = texture(u_texture_normalMap,totalDistortion);\n"	\
		"	vec3 normal = vec3(normalMapColor.r *2.0-1.0,normalMapColor.b * 5.0,normalMapColor.g*2.0-1.0);\n"	\
		"	normal = normalize(normal);\n"	\
		"	vec3 viewVector = normalize(out_toCameraVector);\n"	\
		"	float refractiveFactor = dot(viewVector,normal);\n"	\
		"	refractiveFactor = pow(refractiveFactor,10.0);\n"	\
		"	vec3 reflectedLight  = reflect(normalize(out_fromLightVector),normal);\n"	\
		"	float specular = max(dot(reflectedLight,viewVector),0.0);\n"	\
		"	specular = pow(specular,shineDamper);\n"	\
		"	vec3 specularHighlights = u_LightColor * specular * reflectivity * clamp(waterDepth/5.0,0.0,1.0);\n"	\
		"	FragColor = mix(reflectionColor,refractionColor,refractiveFactor);\n"	\
		"	FragColor = mix(FragColor,vec4(0.0,0.3,0.5,1.0),0.2) + vec4(specularHighlights,0.0);\n"	\
		"   FragColor.a = clamp(waterDepth/5.0,0.0,1.0);\n"	\
		"	//FragColor = vec4(waterDepth/50.0);\n"
		"}";

	// distortion1 gave good result
	/*
	"	vec2 distortion1 = (texture(u_texture_dudvMap,vec2(out_TexCoords.x + u_fMoveFactor,out_TexCoords.y)).rg*2.0-1.0) * waveStrength;\n"	\
	"	vec2 distortion2 = (texture(u_texture_dudvMap,vec2(-out_TexCoords.x+u_fMoveFactor,out_TexCoords.y+u_fMoveFactor)).rg*2.0-1.0) * waveStrength;\n"	\
	"	vec2 totalDistortion = distortion1 + distortion2;\n"	\
	*/

	glShaderSource(iFragmentShaderObject_Water, 1, (const GLchar**)&fragmentShaderSourceCode_Water, NULL);
	glCompileShader(iFragmentShaderObject_Water);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_Water, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_Water, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_Water, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf(g_pFile, "ERROR: Fragment Shader Water Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/*Fragment Shader End*/

	/* Shader Program Start */
	g_ShaderProgramObject_Water = glCreateProgram();
	glAttachShader(g_ShaderProgramObject_Water, iVertexShaderObject_Water);
	glAttachShader(g_ShaderProgramObject_Water, iFragmentShaderObject_Water);
	glBindAttribLocation(g_ShaderProgramObject_Water, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(g_ShaderProgramObject_Water, AMC_ATTRIBUTE_TEXTURE0, "vTexCoords");
	glLinkProgram(g_ShaderProgramObject_Water);

	iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	glGetProgramiv(g_ShaderProgramObject_Water, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_Water, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(g_ShaderProgramObject_Water, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Linking Shader Program Objects Water Failed %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}

	/*Setup Uniforms Start*/
	g_Uniform_Model_Matrix_Water = glGetUniformLocation(g_ShaderProgramObject_Water, "u_model_matrix");
	g_Uniform_Projection_Matrix_Water = glGetUniformLocation(g_ShaderProgramObject_Water, "u_projection_matrix");
	g_Uniform_View_Matrix_Water = glGetUniformLocation(g_ShaderProgramObject_Water, "u_view_matrix");

	g_uniform_TextureSampler_FB_Reflection = glGetUniformLocation(g_ShaderProgramObject_Water, "u_texture_reflection");
	g_uniform_TextureSampler_FB_Refraction = glGetUniformLocation(g_ShaderProgramObject_Water, "u_texture_refraction");
	g_uniform_TextureSampler_FB_DuDv = glGetUniformLocation(g_ShaderProgramObject_Water, "u_texture_dudvMap");
	g_uniform_TextureSampler_FB_Normal = glGetUniformLocation(g_ShaderProgramObject_Water, "u_texture_normalMap");
	g_uniform_TextureSampler_FB_Depth = glGetUniformLocation(g_ShaderProgramObject_Water, "u_texture_depthMap");

	g_uniform_MoveFactor_Water = glGetUniformLocation(g_ShaderProgramObject_Water, "u_fMoveFactor");
	g_uniform_CamPos_Water = glGetUniformLocation(g_ShaderProgramObject_Water, "cameraPosition");
	g_uniform_LightColor_Water = glGetUniformLocation(g_ShaderProgramObject_Water, "u_LightColor");
	g_uniform_LightPosition_Water = glGetUniformLocation(g_ShaderProgramObject_Water, "u_LightPosition");


	const GLfloat quadVertices[] =
	{
		+1280.0f,15.0f,-1280.0f,
		-1280.0f,15.0f,-1280.0f,
		-1280.0f,15.0f,+1280.0f,

		+1280.0f,15.0f,-1280.0f,
		-1280.0f,15.0f,+1280.0f,
		+1280.0f,15.0f,+1280.0f,

	};

	const GLfloat quadTexcoords[] =
	{
		1.0f,1.0f,
		0.0f,1.0f,
		0.0f,0.0f,

		1.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f,

	};

	glGenVertexArrays(1, &g_VertexArrayObject_Water);//VAO
	glBindVertexArray(g_VertexArrayObject_Water);

	glGenBuffers(1, &g_VertexBufferObject_Position_Water);// vbo position
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_Position_Water);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_TexCoords_Water); // vbo color
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_TexCoords_Water);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexcoords), quadTexcoords, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	// Reflection
	glGenFramebuffers(1, &g_FrameBuffer_Reflection);
	glBindFramebuffer(GL_FRAMEBUFFER, g_FrameBuffer_Reflection);


	glGenTextures(1, &g_ColorTexture_Reflection);
	glBindTexture(GL_TEXTURE_2D, g_ColorTexture_Reflection);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, 1920, 1080);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenTextures(1, &g_DepthTexture_Reflection);
	glBindTexture(GL_TEXTURE_2D, g_DepthTexture_Reflection);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 1920, 1080);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_ColorTexture_Reflection, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, g_DepthTexture_Reflection, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDrawBuffers(2, g_DrawBuffers);

	// Refraction
	glGenFramebuffers(1, &g_FrameBuffer_Refraction);
	glBindFramebuffer(GL_FRAMEBUFFER, g_FrameBuffer_Refraction);

	glGenTextures(1, &g_ColorTexture_Refraction);
	glBindTexture(GL_TEXTURE_2D, g_ColorTexture_Refraction);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, 1920, 1080);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &g_DepthTexture_Refraction);
	glBindTexture(GL_TEXTURE_2D, g_DepthTexture_Refraction);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 1920, 1080);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_ColorTexture_Refraction, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, g_DepthTexture_Refraction, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDrawBuffers(2, g_DrawBuffers);


	LoadGLTextures(&g_Tex_WaterNormal, MAKEINTRESOURCE(ID_BITMAP_NORMAL));
	LoadGLTextures(&g_Tex_WaterDuDv, MAKEINTRESOURCE(ID_BITMAP_DUDV));
	/****  Water Code Stop    ****/

	/********** Animated Model Loading Code Start ************/
	iVertexShaderObject_AnimatedModel_SunFlower = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexShaderSourceCode_ModelSunFlower = "#version 450 core"	\
		"\n" \
		"in vec3 vPosition;\n" \
		"in vec4 vColor;\n" \
		"in vec3 vNormal;\n" \
		"in vec2 vTexCoord;\n" \
		"in vec3 vTangent;\n" \
		"in vec3 vBitangent;\n" \
		"in vec4 s_vWeights;\n" \
		"in ivec4 s_vIDs;\n" \
		"const int MAX_BONES = 100;\n"	\
		"layout (location = 0)out vec3 out_transformed_normals;\n" \
		"layout (location = 1)out vec3 out_light_direction;\n" \
		"layout (location = 2)out vec3 out_viewer_vector;\n" \
		"layout (location = 3)out vec2 out_TexCoords;\n" \
		"uniform mat4 gBones[MAX_BONES];\n" \
		"uniform mat4 u_model_matrix;\n" \
		"uniform mat4 u_view_matrix;\n" \
		"uniform mat4 u_projection_matrix;\n" \
		"uniform int is_Animated;\n" \
		"uniform vec3 u_light_position;\n"\
		"uniform vec3 u_view_position;\n" \
		"uniform int u_LKeyPressed;\n" \
		"layout (std140, binding = 0) uniform perInstanceData\n"	\
		"{\n"	\
		"	mat4 u_instanceModelMatrix[615];\n"	\
		"};\n"	\
		"void main(void)\n" \
		"{\n" \
		"	mat4 modelMatrix = u_instanceModelMatrix[gl_InstanceID];"	\
		"	mat4 BMatrix;\n"	\
		"	BMatrix =  gBones[s_vIDs[0]] * s_vWeights[0];\n"	\
		"	BMatrix += gBones[s_vIDs[1]] * s_vWeights[1];\n"	\
		"	BMatrix += gBones[s_vIDs[2]] * s_vWeights[2];\n"	\
		"	BMatrix += gBones[s_vIDs[3]] * s_vWeights[3];\n"	\
		"	vec4 eye_coordinates=u_view_matrix * modelMatrix * vec4(vPosition, 1.0);\n" \
		"	out_transformed_normals = mat3(u_view_matrix*modelMatrix) * vec3(vNormal.x,vNormal.y,vNormal.z);\n"	\
		"	out_light_direction = vec3(u_light_position) - eye_coordinates.xyz;\n"	\
		"	out_viewer_vector = -eye_coordinates.xyz;\n"	\
		"	out_TexCoords = vTexCoord;\n"	\
		"	if(is_Animated == 1)"	\
		"	{\n"	\
		"		gl_Position =  u_projection_matrix * u_view_matrix * modelMatrix * BMatrix * vec4(vPosition,1.0);\n"	\
		"	}\n"	\
		"	else"	\
		"	{\n"	\
		"		gl_Position =  u_projection_matrix * u_view_matrix * modelMatrix * vec4(vPosition,1.0);\n"	\
		"	}\n"	\
		"}";

	glShaderSource(iVertexShaderObject_AnimatedModel_SunFlower, 1, (const GLchar**)&vertexShaderSourceCode_ModelSunFlower, NULL);

	// Compile it
	glCompileShader(iVertexShaderObject_AnimatedModel_SunFlower);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject_AnimatedModel_SunFlower, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_AnimatedModel_SunFlower, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_AnimatedModel_SunFlower, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Vertex Shader Model Sunflowere Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}

	/*Vertex Shader End*/

	/*Fragment Shader Start*/
	iFragmentShaderObject_AnimatedModel_SunFlower = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragmentShaderSourceCode_ModelSunFlower = "#version 450 core"	\
		"\n"	\
		"layout (location = 0)in vec3 out_transformed_normals;\n" \
		"layout (location = 1)in vec3 out_light_direction;\n" \
		"layout (location = 2)in vec3 out_viewer_vector;\n" \
		"layout (location = 3)in vec2 out_TexCoords;\n" \
		"layout (location = 0)out vec4 FragColor;\n"	\
		"uniform sampler2D diffuseMap;\n" \
		"uniform sampler2D normalMap;\n" \
		"uniform int isTexture;\n" \
		"uniform int isTransperent;\n" \
		"uniform vec3 material_diffuse_animModel;\n" \
		"uniform vec3 material_ambient_animModel;\n" \
		"uniform vec3 material_specular_animModel;\n" \
		"uniform vec3 u_La;\n" \
		"uniform vec3 u_Ld;\n" \
		"uniform vec3 u_Ls;\n" \
		"uniform vec3 u_Ka;\n" \
		"uniform vec3 u_Kd;\n" \
		"uniform vec3 u_Ks;\n" \
		"uniform float u_material_shininess_animModel;\n" \
		"uniform int u_LKeyPressed;\n" \
		"void main(void)"	\
		"{"	\
		"	vec3 phong_ads_color = vec3(1.0);\n" \
		"	vec3 color = vec3(1.0);\n" \
		"	if(isTexture == 1)"\
		"	{\n"\
		"		color = texture(diffuseMap, out_TexCoords).rgb ;\n"\
		"	}\n"\
		"	phong_ads_color = vec3(1.0) * color;\n"	\
		"	FragColor = vec4(phong_ads_color,1.0);\n"	\
		"}";

	glShaderSource(iFragmentShaderObject_AnimatedModel_SunFlower, 1, (const GLchar**)&fragmentShaderSourceCode_ModelSunFlower, NULL);
	glCompileShader(iFragmentShaderObject_AnimatedModel_SunFlower);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_AnimatedModel_SunFlower, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_AnimatedModel_SunFlower, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_AnimatedModel_SunFlower, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf(g_pFile, "ERROR: Fragment Shader Model Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/*Fragment Shader End*/

	/* Shader Program Start */
	g_ShaderProgramObject_Model_Sunflower = glCreateProgram();
	glAttachShader(g_ShaderProgramObject_Model_Sunflower, iVertexShaderObject_AnimatedModel_SunFlower);
	glAttachShader(g_ShaderProgramObject_Model_Sunflower, iFragmentShaderObject_AnimatedModel_SunFlower);
	glBindAttribLocation(g_ShaderProgramObject_Model_Sunflower, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(g_ShaderProgramObject_Model_Sunflower, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(g_ShaderProgramObject_Model_Sunflower, AMC_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(g_ShaderProgramObject_Model_Sunflower, AMC_ATTRIBUTE_TEXTURE0, "vTexCoord");
	glBindAttribLocation(g_ShaderProgramObject_Model_Sunflower, AMC_ATTRIBUTE_TANGENT, "aTangent");
	glBindAttribLocation(g_ShaderProgramObject_Model_Sunflower, AMC_ATTRIBUTE_BITANGENT, "aBitangent");
	glBindAttribLocation(g_ShaderProgramObject_Model_Sunflower, AMC_ATTRIBUTE_ID, "s_vIDs");
	glBindAttribLocation(g_ShaderProgramObject_Model_Sunflower, AMC_ATTRIBUTE_WEIGHTS, "s_vWeights");
	glLinkProgram(g_ShaderProgramObject_Model_Sunflower);

	iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	glGetProgramiv(g_ShaderProgramObject_Model_Sunflower, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_Model_Sunflower, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(g_ShaderProgramObject_Model_Sunflower, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Linking Shader Model Program Objects Failed %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/* Shader Program End */

	/*	Setup uniforms	*/
	g_Uniform_Model_Model_Matrix_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_model_matrix");
	g_Uniform_Model_View_Matrix_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_view_matrix");
	g_Uniform_Model_Projection_Matrix_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_projection_matrix");
	g_Uniform_Model_LD_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_Ld");
	g_Uniform_Model_LA_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_La");
	g_Uniform_Model_LS_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_Ls");

	g_Uniform_Model_KD_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_Kd");
	g_Uniform_Model_KA_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_Ka");
	g_Uniform_Model_KS_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_Ks");

	g_Uniform_Model_lightPosition_animModel_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_light_position");
	g_Uniform_Model_ViewPosition_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_view_position");
	g_Uniform_Model_DiffuseMapSampler_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "diffuseMap");
	g_Uniform_Model_NormalMapSampler_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "normalMap");
	g_Uniform_Model_MaterialShininess_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "u_material_shininess_animModel");
	g_Uniform_Model_Animation_Sunflower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "is_Animated");

	g_UBO_InstancingSunFlower = glGetUniformLocation(g_ShaderProgramObject_Model_Sunflower, "perInstanceData");
	glUniformBlockBinding(progID, iLocationInProg, 0);

	//generate model matrix
	glm::mat4* m4TempMatrixSunFlower = (glm::mat4*)calloc(NUM_INSTANCES_SUNFLOWER, sizeof(glm::mat4));
	for (size_t i = 0; i < NUM_INSTANCES_SUNFLOWER; i++)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 ScaleMatrix = glm::mat4(1.0f);
		glm::mat4 TranslationMatrix = glm::mat4(1.0f);
		glm::mat4 RotationMatrix = glm::mat4(1.0f);

		TranslationMatrix = glm::translate(glm::mat4(1.0f), SunFlowerPositions[i]);


		ScaleMatrix = glm::scale(glm::mat4(1.0), SunFlowerScaleVal[i]);

		RotationMatrix = glm::rotate(glm::mat4(1.0), glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		RotationMatrix *= glm::rotate(glm::mat4(1.0), glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		modelMatrix = modelMatrix * TranslationMatrix;
		modelMatrix = modelMatrix * ScaleMatrix;
		modelMatrix = modelMatrix * RotationMatrix;
		m4TempMatrixSunFlower[i] = modelMatrix;
	}

	// put in buffer
	glGenBuffers(1, &g_UBO_InstancingSunFlower);
	glBindBuffer(GL_UNIFORM_BUFFER, g_UBO_InstancingSunFlower);
	glBufferData(GL_UNIFORM_BUFFER, NUM_INSTANCES_SUNFLOWER * sizeof(glm::mat4), m4TempMatrixSunFlower, GL_DYNAMIC_DRAW);// filling the data with Marices
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// use it just before drawing to avoid conflicts
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingSunFlower, 0, NUM_INSTANCES_SUNFLOWER * sizeof(glm::mat4));

	mySunFlower = new AnimModel();
	mySunFlower->IsModelHavingAnimation = true;
	if (mySunFlower->IsModelHavingAnimation)
	{
		mySunFlower->initShaders(g_ShaderProgramObject_Model_Sunflower);
	}

	mySunFlower->loadModel("models/SunFlower/Sunflower_2_0.dae");

	if (m4TempMatrixSunFlower)
	{
		free(m4TempMatrixSunFlower);
		m4TempMatrixSunFlower = NULL;
	}

	iVertexShaderObject_AnimatedModel_Bee = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexShaderSourceCode_ModelBee = "#version 450 core"	\
		"\n" \
		"in vec3 vPosition;\n" \
		"in vec4 vColor;\n" \
		"in vec3 vNormal;\n" \
		"in vec2 vTexCoord;\n" \
		"in vec3 vTangent;\n" \
		"in vec3 vBitangent;\n" \
		"in vec4 s_vWeights;\n" \
		"in ivec4 s_vIDs;\n" \
		"const int MAX_BONES = 100;\n"	\
		"layout (location = 0)out vec3 out_transformed_normals;\n" \
		"layout (location = 1)out vec3 out_light_direction;\n" \
		"layout (location = 2)out vec3 out_viewer_vector;\n" \
		"layout (location = 3)out vec2 out_TexCoords;\n" \
		"uniform mat4 gBones[MAX_BONES];\n" \
		"uniform mat4 u_model_matrix;\n" \
		"uniform mat4 u_view_matrix;\n" \
		"uniform mat4 u_projection_matrix;\n" \
		"uniform int is_Animated;\n" \
		"uniform vec3 u_light_position;\n"\
		"uniform vec3 u_view_position;\n" \
		"uniform int u_LKeyPressed;\n" \
		"void main(void)\n" \
		"{\n" \
		"	mat4 BMatrix;\n"	\
		"	BMatrix =  gBones[s_vIDs[0]] * s_vWeights[0];\n"	\
		"	BMatrix += gBones[s_vIDs[1]] * s_vWeights[1];\n"	\
		"	BMatrix += gBones[s_vIDs[2]] * s_vWeights[2];\n"	\
		"	BMatrix += gBones[s_vIDs[3]] * s_vWeights[3];\n"	\
		"	vec4 eye_coordinates=u_view_matrix * u_model_matrix * vec4(vPosition, 1.0);\n" \
		"	out_transformed_normals = mat3(u_view_matrix*u_model_matrix) * vec3(vNormal.x,vNormal.y,vNormal.z);\n"	\
		"	out_light_direction = vec3(u_light_position) - eye_coordinates.xyz;\n"	\
		"	out_viewer_vector = -eye_coordinates.xyz;\n"	\
		"	out_TexCoords = vTexCoord;\n"	\
		"	if(is_Animated == 1)"	\
		"	{\n"	\
		"		gl_Position =  u_projection_matrix * u_view_matrix * u_model_matrix * BMatrix * vec4(vPosition,1.0);\n"	\
		"	}\n"	\
		"	else"	\
		"	{\n"	\
		"		gl_Position =  u_projection_matrix * u_view_matrix * u_model_matrix * vec4(vPosition,1.0);\n"	\
		"	}\n"	\
		"}";

	glShaderSource(iVertexShaderObject_AnimatedModel_Bee, 1, (const GLchar**)&vertexShaderSourceCode_ModelBee, NULL);

	// Compile it
	glCompileShader(iVertexShaderObject_AnimatedModel_Bee);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject_AnimatedModel_Bee, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_AnimatedModel_Bee, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_AnimatedModel_Bee, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Vertex Shader Model Animated Bee Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}

	/*Vertex Shader End*/

	/*Fragment Shader Start*/
	iFragmentShaderObject_AnimatedModel_Bee = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragmentShaderSourceCode_ModelBee = "#version 450 core"	\
		"\n"	\
		"layout (location = 0)in vec3 out_transformed_normals;\n" \
		"layout (location = 1)in vec3 out_light_direction;\n" \
		"layout (location = 2)in vec3 out_viewer_vector;\n" \
		"layout (location = 3)in vec2 out_TexCoords;\n" \
		"layout (location = 0)out vec4 FragColor;\n"	\
		"uniform sampler2D diffuseMap;\n" \
		"uniform sampler2D normalMap;\n" \
		"uniform int isTexture;\n" \
		"uniform int isTransperent;\n" \
		"uniform vec3 material_diffuse_animModel;\n" \
		"uniform vec3 material_ambient_animModel;\n" \
		"uniform vec3 material_specular_animModel;\n" \
		"uniform vec3 u_La;\n" \
		"uniform vec3 u_Ld;\n" \
		"uniform vec3 u_Ls;\n" \
		"uniform vec3 u_Ka;\n" \
		"uniform vec3 u_Kd;\n" \
		"uniform vec3 u_Ks;\n" \
		"uniform float u_material_shininess_animModel;\n" \
		"uniform int u_LKeyPressed;\n" \
		"void main(void)"	\
		"{"	\
		"	vec3 phong_ads_color = vec3(1.0);\n" \
		"	vec3 color = vec3(1.0);\n" \
		"	if(isTexture == 1)"\
		"	{\n"\
		"		color = texture(diffuseMap, out_TexCoords).rgb * texture(normalMap, out_TexCoords).rgb;\n"\
		"	}\n"\
		"		phong_ads_color = vec3(1.0) * color;\n"	\
		"	FragColor = vec4(phong_ads_color,1.0);\n"	\
		"}";

	glShaderSource(iFragmentShaderObject_AnimatedModel_Bee, 1, (const GLchar**)&fragmentShaderSourceCode_ModelBee, NULL);
	glCompileShader(iFragmentShaderObject_AnimatedModel_Bee);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_AnimatedModel_Bee, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_AnimatedModel_Bee, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_AnimatedModel_Bee, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf(g_pFile, "ERROR: Fragment Shader Model Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/*Fragment Shader End*/

	/* Shader Program Start */
	g_ShaderProgramObject_Model_Bee = glCreateProgram();
	glAttachShader(g_ShaderProgramObject_Model_Bee, iVertexShaderObject_AnimatedModel_Bee);
	glAttachShader(g_ShaderProgramObject_Model_Bee, iFragmentShaderObject_AnimatedModel_Bee);
	glBindAttribLocation(g_ShaderProgramObject_Model_Bee, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(g_ShaderProgramObject_Model_Bee, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(g_ShaderProgramObject_Model_Bee, AMC_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(g_ShaderProgramObject_Model_Bee, AMC_ATTRIBUTE_TEXTURE0, "vTexCoord");
	glBindAttribLocation(g_ShaderProgramObject_Model_Bee, AMC_ATTRIBUTE_TANGENT, "aTangent");
	glBindAttribLocation(g_ShaderProgramObject_Model_Bee, AMC_ATTRIBUTE_BITANGENT, "aBitangent");
	glBindAttribLocation(g_ShaderProgramObject_Model_Bee, AMC_ATTRIBUTE_ID, "s_vIDs");
	glBindAttribLocation(g_ShaderProgramObject_Model_Bee, AMC_ATTRIBUTE_WEIGHTS, "s_vWeights");
	glLinkProgram(g_ShaderProgramObject_Model_Bee);

	iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	glGetProgramiv(g_ShaderProgramObject_Model_Bee, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_Model_Bee, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(g_ShaderProgramObject_Model_Bee, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Linking Shader Model Program Objects Failed %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/* Shader Program End */

	/*	Setup uniforms	*/
	g_Uniform_Model_Model_Matrix_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_model_matrix");
	g_Uniform_Model_View_Matrix_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_view_matrix");
	g_Uniform_Model_Projection_Matrix_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_projection_matrix");
	g_Uniform_Model_LD_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_Ld");
	g_Uniform_Model_LA_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_La");
	g_Uniform_Model_LS_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_Ls");

	g_Uniform_Model_KD_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_Kd");
	g_Uniform_Model_KA_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_Ka");
	g_Uniform_Model_KS_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_Ks");

	g_Uniform_Model_lightPosition_animModel_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_light_position");
	g_Uniform_Model_ViewPosition_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_view_position");
	g_Uniform_Model_DiffuseMapSampler_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "diffuseMap");
	g_Uniform_Model_NormalMapSampler_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "normalMap");
	g_Uniform_Model_MaterialShininess_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "u_material_shininess_animModel");
	g_Uniform_Model_Animation_Bee = glGetUniformLocation(g_ShaderProgramObject_Model_Bee, "is_Animated");

	myHoneyBee = new AnimModel();
	myHoneyBee->IsModelHavingAnimation = true;

	if (myHoneyBee->IsModelHavingAnimation)
	{
		myHoneyBee->initShaders(g_ShaderProgramObject_Model_Bee);
	}

	myHoneyBee->loadModel("models/honeyBeeUP/exportedHoneyBeeFlying.fbx");

	/********** Animated Model Loading Code Stop  ************/


	/********** Lotus Code Start  ************/
	/*Vertex Shader Start*/
	iVertexShaderObject_LotusLeaf = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexShaderSourceCode = "#version 450 core"	\
		"\n" \
		"in vec4 vPosition;\n" \
		"in vec3 vNormal;\n" \
		"in vec2 vTexCoord;\n" \
		"layout (location = 0)out vec3 out_Normal;\n" \
		"layout (location = 1)out vec2 out_TexCoord;\n" \
		"uniform mat4 u_model_matrix,u_view_matrix,u_projection_matrix;\n" \
		"layout (std140, binding = 0) uniform perInstanceData\n"	\
		"{\n"	\
		"	mat4 u_instanceModelMatrix[10];\n"	\
		"};\n"	\
		"void main(void)" \
		"{\n" \
		"	mat4 model = u_instanceModelMatrix[gl_InstanceID];\n"	\
		"	gl_Position =  u_projection_matrix * u_view_matrix * model * vPosition;\n" \
		"	out_Normal = mat3(transpose(inverse(model))) * vNormal;\n"	\
		"	out_TexCoord = vTexCoord;\n"	\
		"}";

	glShaderSource(iVertexShaderObject_LotusLeaf, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// Compile it
	glCompileShader(iVertexShaderObject_LotusLeaf);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject_LotusLeaf, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_LotusLeaf, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_LotusLeaf, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Vertex Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}

	/*Vertex Shader End*/

	/*Fragment Shader Start*/
	iFragmentShaderObject_LotusLeaf = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragmentShaderSourceCode = "#version 450 core"	\
		"\n"	\
		"layout (location = 0)in vec3 out_Normal;\n" \
		"layout (location = 1)in vec2 out_TexCoord;\n" \
		"layout (location = 0)out vec4 FragColor;\n"	\
		"layout(binding=0)uniform sampler2D u_texture_diffuse_sampler;\n"	\
		"layout(binding=1)uniform sampler2D u_texture_bump_sampler;\n"	\
		"layout(binding=2)uniform sampler2D u_texture_specular_sampler;\n"	\
		"void main(void)\n"	\
		"{\n"	\
		"	vec4 diffuseColor = texture(u_texture_diffuse_sampler,out_TexCoord);\n"	\
		"	vec4 bumpColor = texture(u_texture_bump_sampler,out_TexCoord);\n"	\
		"	vec4 specularColor = texture(u_texture_specular_sampler,out_TexCoord);\n"	\
		"	FragColor = diffuseColor*bumpColor*specularColor;\n"	\
		"	//FragColor = vec4(1.0);\n"	\
		"}";

	glShaderSource(iFragmentShaderObject_LotusLeaf, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(iFragmentShaderObject_LotusLeaf);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_LotusLeaf, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_LotusLeaf, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_LotusLeaf, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf(g_pFile, "ERROR: Fragment Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/*Fragment Shader End*/

	/* Shader Program Start */
	g_ShaderProgramObject_LotusLeaf = glCreateProgram();
	glAttachShader(g_ShaderProgramObject_LotusLeaf, iVertexShaderObject_LotusLeaf);
	glAttachShader(g_ShaderProgramObject_LotusLeaf, iFragmentShaderObject_LotusLeaf);
	glBindAttribLocation(g_ShaderProgramObject_LotusLeaf, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(g_ShaderProgramObject_LotusLeaf, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(g_ShaderProgramObject_LotusLeaf, AMC_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(g_ShaderProgramObject_LotusLeaf, AMC_ATTRIBUTE_TEXTURE0, "vTexCoord");
	glLinkProgram(g_ShaderProgramObject_LotusLeaf);

	iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	glGetProgramiv(g_ShaderProgramObject_LotusLeaf, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_LotusLeaf, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(g_ShaderProgramObject_LotusLeaf, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Linking Shader Program Objects Failed %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/* Shader Program End */

	/*Setup Uniforms Start*/
	g_Uniform_LotusLeaf_ModelMatrix = glGetUniformLocation(g_ShaderProgramObject_LotusLeaf, "u_model_matrix");
	g_Uniform_LotusLeaf_ProjectionMatrix = glGetUniformLocation(g_ShaderProgramObject_LotusLeaf, "u_projection_matrix");
	g_Uniform_LotusLeaf_ViewMatrix = glGetUniformLocation(g_ShaderProgramObject_LotusLeaf, "u_view_matrix");

	g_uniform_LotusLeaf_DiffuseMapSampler = glGetUniformLocation(g_ShaderProgramObject_LotusLeaf, "u_texture_diffuse_sampler");
	g_uniform_LotusLeaf_BumpMapSampler = glGetUniformLocation(g_ShaderProgramObject_LotusLeaf, "u_texture_bump_sampler");
	g_uniform_LotusLeaf_SpecularMapSampler = glGetUniformLocation(g_ShaderProgramObject_LotusLeaf, "u_texture_specular_sampler");

	g_UBO_InstancingLotusLeafs0 = glGetUniformLocation(g_ShaderProgramObject_LotusLeaf, "perInstanceData");
	glUniformBlockBinding(g_ShaderProgramObject_LotusLeaf, g_UBO_InstancingLotusLeafs0, 0);


	//generate model matrix
	glm::mat4* m4TempMatrixLotyusLeaf = (glm::mat4*)calloc(NUM_INSTANCES_LOTUSLEAFS0, sizeof(glm::mat4));
	for (size_t i = 0; i < NUM_INSTANCES_LOTUSLEAFS0; i++)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 ScaleMatrix = glm::mat4(1.0f);
		glm::mat4 TranslationMatrix = glm::mat4(1.0f);
		glm::mat4 RotationMatrix = glm::mat4(1.0f);

		TranslationMatrix = glm::translate(glm::mat4(1.0f), LotusLeaf0Positions[i]);
		ScaleMatrix = glm::scale(glm::mat4(1.0), LotusLeaf0Scale[i]);
		RotationMatrix = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		modelMatrix = modelMatrix * TranslationMatrix;
		modelMatrix = modelMatrix * ScaleMatrix;
		modelMatrix = modelMatrix * RotationMatrix;
		m4TempMatrixLotyusLeaf[i] = modelMatrix;
	}

	// put in buffer
	glGenBuffers(1, &g_UBO_InstancingLotusLeafs0);
	glBindBuffer(GL_UNIFORM_BUFFER, g_UBO_InstancingLotusLeafs0);
	glBufferData(GL_UNIFORM_BUFFER, NUM_INSTANCES_LOTUSLEAFS0 * sizeof(glm::mat4), m4TempMatrixLotyusLeaf, GL_DYNAMIC_DRAW);// filling the data with Marices
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// use it just before drawing to avoid conflicts
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingLotusLeafs0, 0, NUM_INSTANCES_LOTUSLEAFS0 * sizeof(glm::mat4));


	if (m4TempMatrixLotyusLeaf)
	{
		free(m4TempMatrixLotyusLeaf);
		m4TempMatrixLotyusLeaf = 0;
	}
	/*Setup Uniforms End*/

	/* Fill Buffers Start*/
	glGenVertexArrays(1, &g_VertexArrayObject_LotusLeaf0);//VAO
	glBindVertexArray(g_VertexArrayObject_LotusLeaf0);

	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf0_Position);// vbo position
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf0_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fVertices_Lotus0), fVertices_Lotus0, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf0_Normal);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf0_Normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fNormal_Lotus0), fNormal_Lotus0, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf0_TexCoord);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf0_TexCoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fTexCoord_Lotus0), fTexCoord_Lotus0, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//Elements  
	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf0_Elements);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf0_Elements);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uiIndices_Lotus0), uiIndices_Lotus0, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	glGenVertexArrays(1, &g_VertexArrayObject_LotusLeaf1);//VAO
	glBindVertexArray(g_VertexArrayObject_LotusLeaf1);

	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf1_Position);// vbo position
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf1_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fVertices_Lotus1), fVertices_Lotus1, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf1_Normal);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf1_Normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fNormal_Lotus1), fNormal_Lotus1, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf1_TexCoord);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf1_TexCoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fTexCoord_Lotus1), fTexCoord_Lotus1, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//Elements  
	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf1_Elements);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf1_Elements);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uiIndices_Lotus1), uiIndices_Lotus1, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	glGenVertexArrays(1, &g_VertexArrayObject_LotusLeaf2);//VAO
	glBindVertexArray(g_VertexArrayObject_LotusLeaf2);

	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf2_Position);// vbo position
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf2_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fVertices_Lotus0), fVertices_Lotus0, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf2_Normal);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf2_Normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fNormal_Lotus2), fNormal_Lotus2, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf2_TexCoord);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf2_TexCoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fTexCoord_Lotus2), fTexCoord_Lotus2, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//Elements  
	glGenBuffers(1, &g_VertexBufferObject_LotusLeaf2_Elements);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_LotusLeaf2_Elements);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uiIndices_Lotus2), uiIndices_Lotus2, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	/* Fill Buffers End*/

	ilInit();
	g_LotusLeaf_BumpTexture = LoadPngTexture("models/NiceLotus/LotusLeafTexOnly/Leaf_normal.png");
	g_LotusLeaf_DiffuseTexture = LoadPngTexture("models/NiceLotus/LotusLeafTexOnly/Leaf_diffuse.png");
	g_LotusLeaf_SpecularTexture = LoadPngTexture("models/NiceLotus/LotusLeafTexOnly/Leaf_reflection.png");


	/*	Lotus Flower Start	*/
	iVertexShaderObject_AnimatedModel_LotusFlower = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexShaderSourceCode_ModelLotusFlower = "#version 450 core"	\
		"\n" \
		"in vec3 vPosition;\n" \
		"in vec4 vColor;\n" \
		"in vec3 vNormal;\n" \
		"in vec2 vTexCoord;\n" \
		"in vec3 vTangent;\n" \
		"in vec3 vBitangent;\n" \
		"in vec4 s_vWeights;\n" \
		"in ivec4 s_vIDs;\n" \
		"const int MAX_BONES = 100;\n"	\
		"layout (location = 0)out vec3 out_normals;\n" \
		"layout (location = 1)out vec2 out_TexCoords;\n" \
		"layout (location = 2)out vec3 out_FragPos;\n" \
		"uniform mat4 gBones[MAX_BONES];\n" \
		"uniform mat4 u_model_matrix;\n" \
		"uniform mat4 u_view_matrix;\n" \
		"uniform mat4 u_projection_matrix;\n" \
		"uniform int is_Animated;\n" \
		"uniform vec3 u_light_position;\n"\
		"uniform vec3 u_view_position;\n" \
		"uniform int u_LKeyPressed;\n" \
		"layout (std140, binding = 0) uniform perInstanceData\n"	\
		"{\n"	\
		"	mat4 u_instanceModelMatrix[10];\n"	\
		"};\n"	\
		"void main(void)\n" \
		"{\n" \
		"	mat4 model = u_instanceModelMatrix[gl_InstanceID];\n"	\
		"	mat4 BMatrix;\n"	\
		"	BMatrix =  gBones[s_vIDs[0]] * s_vWeights[0];\n"	\
		"	BMatrix += gBones[s_vIDs[1]] * s_vWeights[1];\n"	\
		"	BMatrix += gBones[s_vIDs[2]] * s_vWeights[2];\n"	\
		"	BMatrix += gBones[s_vIDs[3]] * s_vWeights[3];\n"	\
		"	out_normals = mat3(transpose(inverse(model))) * vNormal;\n"	\
		"	out_TexCoords = vTexCoord;\n"	\
		"	out_FragPos = vec3(model * vec4(vPosition.xyz, 1.0));\n"	\
		"	if(is_Animated == 1)"	\
		"	{\n"	\
		"		gl_Position =  u_projection_matrix * u_view_matrix * model * BMatrix * vec4(vPosition,1.0);\n"	\
		"	}\n"	\
		"	else"	\
		"	{\n"	\
		"		gl_Position =  u_projection_matrix * u_view_matrix * model * vec4(vPosition,1.0);\n"	\
		"	}\n"	\
		"}";

	glShaderSource(iVertexShaderObject_AnimatedModel_LotusFlower, 1, (const GLchar**)&vertexShaderSourceCode_ModelLotusFlower, NULL);

	// Compile it
	glCompileShader(iVertexShaderObject_AnimatedModel_LotusFlower);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject_AnimatedModel_LotusFlower, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_AnimatedModel_LotusFlower, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_AnimatedModel_LotusFlower, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Vertex Shader Model LotusFlower Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}

	/*Vertex Shader End*/

	/*Fragment Shader Start*/
	iFragmentShaderObject_AnimatedModel_LotusFlower = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragmentShaderSourceCode_LotusFlower = "#version 450 core"	\
		"\n"	\
		"layout (location = 0)in vec3 out_normals;\n" \
		"layout (location = 1)in vec2 out_TexCoords;\n" \
		"layout (location = 2)in vec3 out_FragPos;\n" \
		"layout (location = 0)out vec4 FragColor;\n"	\
		"uniform sampler2D diffuseMap;\n" \
		"uniform sampler2D normalMap;\n" \
		"uniform int isTexture;\n" \
		"uniform int isTransperent;\n" \
		"struct Light\n"	\
		"{\n"	\
		"	vec3 direction;\n"	\
		"	vec3 ambient;\n"	\
		"	vec3 diffuse;\n"	\
		"	vec3 specular;\n"	\
		"};\n"	\
		"uniform Light SunLight;\n"	\
		"uniform vec3 u_view_position;\n"	\
		"vec3 CalculateSunLight(Light SunLight, vec3 FinalTerrainColor, vec3 normalized_normal, vec3 fragpos, vec3 view_direction);\n"	\
		"void main(void)\n"	\
		"{\n"	\
		"	vec3 phong_ads_color = vec3(1.0);\n" \
		"	vec3 color = vec3(1.0);\n" \
		"	vec3 normalized_normal = normalize(out_normals);\n"	\
		"	vec3 view_direction = normalize(u_view_position-out_FragPos);\n"	\
		"	if(isTexture == 1)\n"\
		"	{\n"\
		"		color = 2.0*texture(diffuseMap, out_TexCoords).rgb * texture(normalMap, out_TexCoords).rgb;\n"\
		"	}\n"\
		"	phong_ads_color = CalculateSunLight(SunLight,color.rgb,normalized_normal,out_FragPos,view_direction);\n"	\
		"	//phong_ads_color *= 1.5;\n"	\
		"	FragColor = vec4(phong_ads_color,1.0);\n"	\
		"}\n"	\
		"vec3 CalculateSunLight(Light SunLight, vec3 FinalTerrainColor, vec3 normalized_normal, vec3 fragpos, vec3 view_direction)\n"	\
		"{\n"	\
		"	vec3 ambient = SunLight.ambient * FinalTerrainColor;\n"	\
		"	vec3 light_direction = normalize(-SunLight.direction);\n"	\
		"	float diffuse_multiplier = max(dot(normalized_normal, light_direction), 0.0);\n"	\
		"	vec3 diffuse = SunLight.diffuse * diffuse_multiplier * FinalTerrainColor;\n"	\
		"	vec3 half_vector = normalize(light_direction + view_direction);\n"	\
		"	float specular_multiplier = pow(max(dot(normalized_normal, half_vector), 0.0), 50.0);\n"	\
		"	vec3 specular = SunLight.specular * specular_multiplier * FinalTerrainColor;\n"	\
		"	return(ambient + diffuse);\n"	\
		"}\n";

	glShaderSource(iFragmentShaderObject_AnimatedModel_LotusFlower, 1, (const GLchar**)&fragmentShaderSourceCode_LotusFlower, NULL);
	glCompileShader(iFragmentShaderObject_AnimatedModel_LotusFlower);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_AnimatedModel_LotusFlower, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_AnimatedModel_LotusFlower, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_AnimatedModel_LotusFlower, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf(g_pFile, "ERROR: Fragment Shader Model Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/*Fragment Shader End*/

	/* Shader Program Start */
	g_ShaderProgramObject_Model_LotusFlower = glCreateProgram();
	glAttachShader(g_ShaderProgramObject_Model_LotusFlower, iVertexShaderObject_AnimatedModel_LotusFlower);
	glAttachShader(g_ShaderProgramObject_Model_LotusFlower, iFragmentShaderObject_AnimatedModel_LotusFlower);
	glBindAttribLocation(g_ShaderProgramObject_Model_LotusFlower, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(g_ShaderProgramObject_Model_LotusFlower, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(g_ShaderProgramObject_Model_LotusFlower, AMC_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(g_ShaderProgramObject_Model_LotusFlower, AMC_ATTRIBUTE_TEXTURE0, "vTexCoord");
	glBindAttribLocation(g_ShaderProgramObject_Model_LotusFlower, AMC_ATTRIBUTE_TANGENT, "aTangent");
	glBindAttribLocation(g_ShaderProgramObject_Model_LotusFlower, AMC_ATTRIBUTE_BITANGENT, "aBitangent");
	glBindAttribLocation(g_ShaderProgramObject_Model_LotusFlower, AMC_ATTRIBUTE_ID, "s_vIDs");
	glBindAttribLocation(g_ShaderProgramObject_Model_LotusFlower, AMC_ATTRIBUTE_WEIGHTS, "s_vWeights");
	glLinkProgram(g_ShaderProgramObject_Model_LotusFlower);

	iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	glGetProgramiv(g_ShaderProgramObject_Model_LotusFlower, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_Model_LotusFlower, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(g_ShaderProgramObject_Model_LotusFlower, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Linking Shader Model Program Objects Failed %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/* Shader Program End */

	/*	Setup uniforms	*/
	g_Uniform_Model_Model_Matrix_LotusFlower = glGetUniformLocation(g_ShaderProgramObject_Model_LotusFlower, "u_model_matrix");
	g_Uniform_Model_View_Matrix_LotusFlower = glGetUniformLocation(g_ShaderProgramObject_Model_LotusFlower, "u_view_matrix");
	g_Uniform_Model_Projection_Matrix_LotusFlower = glGetUniformLocation(g_ShaderProgramObject_Model_LotusFlower, "u_projection_matrix");

	//g_Uniform_Model_lightPosition_animModel_LotusFlower = glGetUniformLocation(g_ShaderProgramObject_Model_LotusFlower, "u_light_position");
	g_Uniform_Model_ViewPosition_LotusFlower = glGetUniformLocation(g_ShaderProgramObject_Model_LotusFlower, "u_view_position");
	g_Uniform_Model_DiffuseMapSampler_LotusFlower = glGetUniformLocation(g_ShaderProgramObject_Model_LotusFlower, "diffuseMap");
	g_Uniform_Model_NormalMapSampler_LotusFlower = glGetUniformLocation(g_ShaderProgramObject_Model_LotusFlower, "normalMap");
	g_Uniform_Model_Animation_LotusFlower = glGetUniformLocation(g_ShaderProgramObject_Model_LotusFlower, "is_Animated");


	g_UBO_InstancingNiceLotus1 = glGetUniformLocation(g_ShaderProgramObject_Model_LotusFlower, "perInstanceData");
	glUniformBlockBinding(g_ShaderProgramObject_Model_LotusFlower, g_UBO_InstancingNiceLotus1, 0);

	//generate model matrix
	glm::mat4* m4TempMatrix_NiceLotus1 = (glm::mat4*)calloc(NUM_INSTANCES_NICELOTUS1, sizeof(glm::mat4));
	for (size_t i = 0; i < NUM_INSTANCES_NICELOTUS1; i++)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 ScaleMatrix = glm::mat4(1.0f);
		glm::mat4 TranslationMatrix = glm::mat4(1.0f);
		glm::mat4 RotationMatrix = glm::mat4(1.0f);

		TranslationMatrix = glm::translate(glm::mat4(1.0f), NiceLotus1Positions[i]);

		ScaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(NiceLotus1Scale[i]));
		RotationMatrix = glm::rotate(RotationMatrix, -190.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		RotationMatrix = glm::rotate(RotationMatrix, 100.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		modelMatrix = modelMatrix * TranslationMatrix;
		modelMatrix = modelMatrix * ScaleMatrix;
		modelMatrix = modelMatrix * RotationMatrix;
		m4TempMatrix_NiceLotus1[i] = modelMatrix;
	}

	// put in buffer
	glGenBuffers(1, &g_UBO_InstancingNiceLotus1);
	glBindBuffer(GL_UNIFORM_BUFFER, g_UBO_InstancingNiceLotus1);
	glBufferData(GL_UNIFORM_BUFFER, NUM_INSTANCES_NICELOTUS1 * sizeof(glm::mat4), m4TempMatrix_NiceLotus1, GL_DYNAMIC_DRAW);// filling the data with Marices
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingNiceLotus1, 0, NUM_INSTANCES_NICELOTUS1 * sizeof(glm::mat4));

	//myNiceLotus0 = new AnimModel();
	myNiceLotus1 = new AnimModel();

	//myNiceLotus0->IsModelHavingAnimation = false;
	myNiceLotus1->IsModelHavingAnimation = false;

	//myNiceLotus0->loadModel("models/NiceLotus/Lotus0/Lotus0.obj");//
	myNiceLotus1->loadModel("models/NiceLotus/Lotus1/Lotus1.obj");//

																  /*	LotusFlower End	*/
																  /********** Lotus Code Stop   ************/



																  /********** CubeMap Code Start   ************/
																  //Init cube map
																  /*Vertex Shader Start*/
	iVertexShaderObject_CubeMap = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexShaderSourceCode_CubeMap =
		"#version 430 core" \
		"\n" \
		"layout(location = 0) in vec3 aPosition;\n" \
		"layout(location = 0) out vec3 TexCoords;\n" \
		"uniform mat4 uProjectionMatrix;\n"	\
		"uniform mat4 uViewMatrix;\n"	\
		"void main(void)" \
		"{" \
		"	TexCoords = aPosition;\n"	\
		"	vec4 Position = uProjectionMatrix * uViewMatrix * vec4(aPosition, 1.0);\n"	\
		"	gl_Position = Position.xyww;\n"	\
		"}";

	glShaderSource(iVertexShaderObject_CubeMap, 1, (const GLchar**)&vertexShaderSourceCode_CubeMap, NULL);
	glCompileShader(iVertexShaderObject_CubeMap);

	iShaderCompileStatus = 0;
	iGLInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(iVertexShaderObject_CubeMap, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_CubeMap, GL_INFO_LOG_LENGTH, &iGLInfoLogLength);
		if (iGLInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iGLInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_CubeMap, iGLInfoLogLength, &written, szInfoLog);
				fprintf(g_pFile, "CubeMap : Vertex Shader Compilation Log :: %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Vertex Shader Stop */
	/*Fragment Shader Start*/
	iFragmentShaderObject_CubeMap = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragmentShaderSourceCode_CubeMap =
		"#version 430 core" \
		"\n" \
		"layout(location = 0) out vec4 FragColor;\n"	\
		"layout(location = 0) in vec3 TexCoords;\n"	\
		"uniform samplerCube Cubemap;\n"	\
		"uniform vec3 fogColor;\n"	\
		"const float lowerLimit = 0.0;\n"	\
		"const float upperLimit = 0.4;\n"	\
		"void main()\n"	\
		"{\n"	\
		"	vec4 finalColor = texture(Cubemap, TexCoords);\n"	\
		"	float factor=(TexCoords.y-lowerLimit)/(upperLimit-lowerLimit);\n"	\
		"	factor = clamp(factor,0.0,1.0);\n"	\
		"	FragColor = mix(vec4(fogColor,1.0), finalColor, factor);\n"	\
		"}\n";

	glShaderSource(iFragmentShaderObject_CubeMap, 1, (const GLchar**)&fragmentShaderSourceCode_CubeMap, NULL);

	glCompileShader(iFragmentShaderObject_CubeMap);

	iShaderCompileStatus = 0;
	iGLInfoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_CubeMap, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_CubeMap, GL_INFO_LOG_LENGTH, &iGLInfoLogLength);
		if (iGLInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iGLInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_CubeMap, iGLInfoLogLength, &written, szInfoLog);
				fprintf(g_pFile, "CUBEMAP : Fragment Shader Compilation Log :: %s \n", szInfoLog);
				free(szInfoLog);
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Fragment Shader Stop */

	/*	Shader Program Start	*/
	iShaderProgramObject_CubeMap = glCreateProgram();
	glAttachShader(iShaderProgramObject_CubeMap, iVertexShaderObject_CubeMap);
	glAttachShader(iShaderProgramObject_CubeMap, iFragmentShaderObject_CubeMap);

	//Link the Shader Program
	glLinkProgram(iShaderProgramObject_CubeMap);

	iProgramLinkStatus = 0;
	iGLInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(iShaderProgramObject_CubeMap, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(iShaderProgramObject_CubeMap, GL_INFO_LOG_LENGTH, &iGLInfoLogLength);
		if (iGLInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iGLInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(iShaderProgramObject_CubeMap, iGLInfoLogLength, &written, szInfoLog);
				fprintf(g_pFile, "CUBEMAP : Link status Log :: %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
	}

	/*	Shader Program Stop 	*/
	g_pCubeMap = new CubeMap();
	g_pCubeMap->InitializeLightCube(iShaderProgramObject_CubeMap);
	/********** CubeMap Code Stop    ************/

	/********** ButteerFly Code Start    ************/
	g_pFlockingButterfiles = new CFlockingButterflies();

	if (g_pFlockingButterfiles)
	{
		g_pFlockingButterfiles->InitializeFlockingButterflies();
	}
	/********** ButteerFly Code Stop    ************/


	/********** Particle Code Start    ************/
	/*Vertex Shader Start*/
	iVertexShaderObject_Particle1 = glCreateShader(GL_VERTEX_SHADER);
	// VS Source Code
	const GLchar* chVertexShaderSourceCode_Particle = "#version 450 core"	\
		"\n"	\
		"in vec4 vPosition;\n"	\
		"in vec4 vColor;\n"	\
		"in vec3 Velocity;\n"	\
		"in float StartTime;\n"	\
		"out vec4 vs_out_Color;\n"	\
		"out float vs_out_t;\n"
		"out float vs_out_STtime;\n"	\
		"uniform float Time;\n"	\
		"uniform mat4 u_model_matrix;\n"	\
		"uniform mat4 u_view_matrix;\n"	\
		"uniform mat4 u_projection_matrix;\n"	\
		"void main(void)\n"	\
		"{\n"	\
		"	vec4 vert;\n"	\
		"	float t=Time-StartTime;\n"	\
		"	if(t>=0.0)\n"	\
		"	{\n"	\
		"		vert = vPosition-vec4(Velocity*t*1.0,0.0);\n"	\
		"		//vert.x += t / 50.0;\n"	\
		"		vert.z *= 2.0*sin(t*0.25) * cos(t*0.25);\n"	\
		"		vert.y *= 2.0*sin(t*0.25) * cos(t*0.25);\n"	\
		"		vs_out_Color = vColor;\n"	\
		"	}\n"	\
		"	gl_Position = vert;\n"	\
		"	vs_out_t = t ;\n"
		"	vs_out_STtime = StartTime;\n"	\
		"}\n";

	glShaderSource(iVertexShaderObject_Particle1, 1, (const GLchar**)&chVertexShaderSourceCode_Particle, NULL);
	// Compile VS
	glCompileShader(iVertexShaderObject_Particle1);
	iInfoLogLength = 0;
	GLint iCompileStatus = 0;
	char* chErrorMessage = NULL;
	glGetShaderiv(iVertexShaderObject_Particle1, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_Particle1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			chErrorMessage = (char*)malloc(iInfoLogLength);
			if (chErrorMessage != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_Particle1, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
				fprintf_s(g_pFile, "Vertex Shader Particles Error : %s \n", chErrorMessage);
				free(chErrorMessage); chErrorMessage = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
				/*Scene2_UnInitialize(EXIT_FAILURE);
				exit(EXIT_FAILURE);*/
			}
		}
	}
	/*Vertex Shader End*/

	/*Geomerty Shader Start*/
	iGeometryShaderObject_Particle1 = glCreateShader(GL_GEOMETRY_SHADER);
	const GLchar* geometryShaderSourceCode_FireFly = "#version 450 core"	\
		"\n"	\
		"layout (points)in;\n" \
		"layout (triangle_strip,max_vertices = 4)out;\n" \
		"in vec4 vs_out_Color[];\n"	\
		"in float vs_out_t[];\n"
		"in float vs_out_STtime[];\n"	\
		"uniform mat4 u_model_matrix,u_view_matrix,u_projection_matrix;\n" \
		"float particle_size = 2.0;\n"	\
		"out vec4  gs_out_Color;\n"	\
		"out float gs_out_t;\n"
		"out float gs_out_STtime;\n"	\
		"out vec2  out_VertOffset;\n"	\
		"void main(void)\n"	\
		"{\n"	\
		"	vec4 pos = u_view_matrix * u_model_matrix * gl_in[0].gl_Position;\n"	\
		"	out_VertOffset = vec2(-1.0,-1.0) * particle_size;\n"	\
		"	gs_out_Color = vs_out_Color[0];\n"	\
		"	gs_out_t = vs_out_t[0];\n"	\
		"	gs_out_STtime = vs_out_STtime[0];\n"	\
		"	gl_Position = u_projection_matrix*(pos+vec4(out_VertOffset,0,0));\n"	\
		"	EmitVertex();\n"	\
		"	out_VertOffset = vec2(1.0,-1.0) * particle_size;\n"	\
		"	gs_out_Color = vs_out_Color[0];\n"	\
		"	gs_out_t = vs_out_t[0];\n"	\
		"	gs_out_STtime = vs_out_STtime[0];\n"	\
		"	gl_Position = u_projection_matrix*(pos+vec4(out_VertOffset,0,0));\n"	\
		"	EmitVertex();\n"	\
		"	out_VertOffset = vec2(-1.0,1.0) * particle_size;\n"	\
		"	gs_out_Color = vs_out_Color[0];\n"	\
		"	gs_out_t = vs_out_t[0];\n"	\
		"	gs_out_STtime = vs_out_STtime[0];\n"	\
		"	gl_Position = u_projection_matrix*(pos+vec4(out_VertOffset,0,0));\n"	\
		"	EmitVertex();\n"	\
		"	out_VertOffset = vec2(1.0,1.0) * particle_size;\n"	\
		"	gs_out_Color = vs_out_Color[0];\n"	\
		"	gs_out_t = vs_out_t[0];\n"	\
		"	gs_out_STtime = vs_out_STtime[0];\n"	\
		"	gl_Position = u_projection_matrix*(pos+vec4(out_VertOffset,0,0));\n"	\
		"	EmitVertex();\n"	\
		"	EndPrimitive();\n"	\
		"}\n";

	glShaderSource(iGeometryShaderObject_Particle1, 1, (const GLchar**)&geometryShaderSourceCode_FireFly, NULL);

	// Compile Source Code
	glCompileShader(iGeometryShaderObject_Particle1);
	iInfoLogLength = 0;
	iCompileStatus = 0;
	chErrorMessage = NULL;
	glGetShaderiv(iGeometryShaderObject_Particle1, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iGeometryShaderObject_Particle1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			chErrorMessage = (char*)malloc(iInfoLogLength);
			if (chErrorMessage != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iGeometryShaderObject_Particle1, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
				fprintf_s(g_pFile, "Error : Geometry Shader Model Compilation Log : %s \n", chErrorMessage);
				free(chErrorMessage);
				chErrorMessage = NULL;
				return INIT_GEOMETRY_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Geomerty Shader Stop */

	/*Fragment Shader Start*/
	iFragmentShaderObject_Particle1 = glCreateShader(GL_FRAGMENT_SHADER);
	// FS Source Code
	const GLchar* chFragmentShaderSourceCode_Inlet = "#version 450 core"	\
		"\n"	\
		"in vec4  gs_out_Color;"	\
		"in float gs_out_t;"
		"in float gs_out_STtime;\n"	\
		"in vec2  out_VertOffset;\n"	\
		"out vec4 FragColor;\n"	\
		"uniform float u_animate_time;\n"	\
		"void main(void)\n"	\
		"{\n"	\
		"	float alpha;\n"	\
		"	float s = 2.0/(dot(out_VertOffset,out_VertOffset));\n"	\
		"	alpha = u_animate_time * s;\n"	\
		"	if( (gs_out_t <= 0.0)  || (s < 0.85 ) )\n"	\
		"	{\n"	\
		"		discard;\n"	\
		"	}\n"	\
		"	FragColor = s*vec4(gs_out_Color.r, gs_out_Color.g, gs_out_Color.b, alpha);\n"	\
		"}\n";
	glShaderSource(iFragmentShaderObject_Particle1, 1, (const GLchar**)&chFragmentShaderSourceCode_Inlet, NULL);

	//float s = 100.0*(1.0/(1.0 + 15.0*dot(out_VertOffset,out_VertOffset))- 1/16.0);
	//alpha = u_animate_time/16.0 * s * 0.2;
	//((gs_out_t <= 0.0)  || (gs_out_t >= 15.0) ) ||(s < 0.55 )

	iInfoLogLength = 0;
	iCompileStatus = 0;
	chErrorMessage = NULL;

	glCompileShader(iFragmentShaderObject_Particle1);
	glGetShaderiv(iFragmentShaderObject_Particle1, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_Particle1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			chErrorMessage = (char*)malloc(iInfoLogLength);
			glGetShaderInfoLog(iFragmentShaderObject_Particle1, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
			fprintf_s(g_pFile, "Fragment Shader Particle Error : %s \n ", chErrorMessage);
			free(chErrorMessage); chErrorMessage = NULL;
			//exit(EXIT_FAILURE);
			return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
		}
	}
	/*Fragment Shader End*/

	/* Shader Program Start */
	g_ShaderProgramObject_Particle1 = glCreateProgram();
	// Attach all Shaders
	glAttachShader(g_ShaderProgramObject_Particle1, iVertexShaderObject_Particle1);
	glAttachShader(g_ShaderProgramObject_Particle1, iGeometryShaderObject_Particle1);
	glAttachShader(g_ShaderProgramObject_Particle1, iFragmentShaderObject_Particle1);
	// Bind Attributes in Shader

	glBindAttribLocation(g_ShaderProgramObject_Particle1, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(g_ShaderProgramObject_Particle1, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(g_ShaderProgramObject_Particle1, AMC_VELOCITY_ARRAY, "Velocity");
	glBindAttribLocation(g_ShaderProgramObject_Particle1, AMC_START_TIME_ARRAY, "StartTime");
	//Link Shader
	glLinkProgram(g_ShaderProgramObject_Particle1);

	GLint iLinkStatus = 0;
	iInfoLogLength = 0;
	chErrorMessage = NULL;
	glGetProgramiv(g_ShaderProgramObject_Particle1, GL_LINK_STATUS, &iLinkStatus);

	if (iLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_Particle1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			chErrorMessage = (char*)malloc(iInfoLogLength);
			if (chErrorMessage != NULL)
			{
				glGetProgramInfoLog(g_ShaderProgramObject_Particle1, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
				fprintf_s(g_pFile, "Error In ShaderProgram Particle: %s \n", chErrorMessage);
				free(chErrorMessage); chErrorMessage = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
			}
		}
	}
	/* Shader Program End */

	/*Setup Uniforms Start*/
	g_Uniform_ModelMatrix_Particle1 = glGetUniformLocation(g_ShaderProgramObject_Particle1, "u_model_matrix");
	g_Uniform_ViewMatrix_Particle1 = glGetUniformLocation(g_ShaderProgramObject_Particle1, "u_view_matrix");
	g_Uniform_ProjectionMatrix_Particle1 = glGetUniformLocation(g_ShaderProgramObject_Particle1, "u_projection_matrix");
	g_Uniform_Time_Particle1 = glGetUniformLocation(g_ShaderProgramObject_Particle1, "Time");
	g_Uniform_Anime_Particle1 = glGetUniformLocation(g_ShaderProgramObject_Particle1, "u_animate_time");
	/*Setup Uniforms End*/

	/*Vertex Shader Start*/
	iVertexShaderObject_Particle2 = glCreateShader(GL_VERTEX_SHADER);
	// VS Source Code
	const GLchar* chVertexShaderSourceCode_Particle2 = "#version 450 core"	\
		"\n"	\
		"in vec4 vPosition;\n"	\
		"in vec4 vColor;\n"	\
		"in vec3 Velocity;\n"	\
		"in float StartTime;\n"	\
		"out vec4 vs_out_Color;\n"	\
		"out float vs_out_t;\n"
		"out float vs_out_STtime;\n"	\
		"uniform float Time;\n"	\
		"uniform mat4 u_model_matrix;\n"	\
		"uniform mat4 u_view_matrix;\n"	\
		"uniform mat4 u_projection_matrix;\n"	\
		"void main(void)\n"	\
		"{\n"	\
		"	vec4 vert;\n"	\
		"	float t=Time-StartTime;\n"	\
		"	if(t>=0.0)\n"	\
		"	{\n"	\
		"		vert = vPosition-vec4(Velocity*t*2.5,0.0);\n"	\
		"		//vert.y += t / 50.0;\n"	\
		"		vert.x = 0.0;\n"	\
		"		vert.yz *= 1.5*sin(t*0.3) * cos(t*0.5);\n"	\
		"		if(vert.y < 0.0 )\n"	\
		"		{\n"	\
		"			vert.y = -vert.y;\n"	\
		"		}\n"	\
		"		vs_out_Color = vColor;\n"	\
		"	}"	\
		"	gl_Position = vert;\n"	\
		"	vs_out_t = t ;\n"
		"	vs_out_STtime = StartTime;\n"	\
		"}\n";

	glShaderSource(iVertexShaderObject_Particle2, 1, (const GLchar**)&chVertexShaderSourceCode_Particle2, NULL);
	// Compile VS
	glCompileShader(iVertexShaderObject_Particle2);
	iInfoLogLength = 0;
	iCompileStatus = 0;
	chErrorMessage = NULL;
	glGetShaderiv(iVertexShaderObject_Particle2, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_Particle2, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			chErrorMessage = (char*)malloc(iInfoLogLength);
			if (chErrorMessage != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_Particle2, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
				fprintf_s(g_pFile, "Vertex Shader Particles 2 Error : %s \n", chErrorMessage);
				free(chErrorMessage); chErrorMessage = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;

			}
		}
	}
	/*Vertex Shader End*/

	/*Geomerty Shader Start*/
	iGeometryShaderObject_Particle2 = glCreateShader(GL_GEOMETRY_SHADER);
	const GLchar* geometryShaderSourceCode_FireFly2 = "#version 450 core"	\
		"\n"	\
		"layout (points)in;\n" \
		"layout (triangle_strip,max_vertices = 4)out;\n" \
		"in vec4 vs_out_Color[];\n"	\
		"in float vs_out_t[];\n"
		"in float vs_out_STtime[];\n"	\
		"uniform mat4 u_model_matrix,u_view_matrix,u_projection_matrix;\n" \
		"float particle_size = 50.0;\n"	\
		"out vec4  gs_out_Color;\n"	\
		"out float gs_out_t;\n"
		"out float gs_out_STtime;\n"	\
		"out vec2  out_VertOffset;\n"	\
		"void main(void)\n"	\
		"{\n"	\
		"	vec4 pos = u_view_matrix * u_model_matrix * gl_in[0].gl_Position;\n"	\
		"	out_VertOffset = vec2(-1.0,-1.0) * particle_size;\n"	\
		"	gs_out_Color = vs_out_Color[0];\n"	\
		"	gs_out_t = vs_out_t[0];\n"	\
		"	gs_out_STtime = vs_out_STtime[0];\n"	\
		"	gl_Position = u_projection_matrix*(pos+vec4(out_VertOffset,0,0));\n"	\
		"	EmitVertex();\n"	\
		"	out_VertOffset = vec2(1.0,-1.0) * particle_size;\n"	\
		"	gs_out_Color = vs_out_Color[0];\n"	\
		"	gs_out_t = vs_out_t[0];\n"	\
		"	gs_out_STtime = vs_out_STtime[0];\n"	\
		"	gl_Position = u_projection_matrix*(pos+vec4(out_VertOffset,0,0));\n"	\
		"	EmitVertex();\n"	\
		"	out_VertOffset = vec2(-1.0,1.0) * particle_size;\n"	\
		"	gs_out_Color = vs_out_Color[0];\n"	\
		"	gs_out_t = vs_out_t[0];\n"	\
		"	gs_out_STtime = vs_out_STtime[0];\n"	\
		"	gl_Position = u_projection_matrix*(pos+vec4(out_VertOffset,0,0));\n"	\
		"	EmitVertex();\n"	\
		"	out_VertOffset = vec2(1.0,1.0) * particle_size;\n"	\
		"	gs_out_Color = vs_out_Color[0];\n"	\
		"	gs_out_t = vs_out_t[0];\n"	\
		"	gs_out_STtime = vs_out_STtime[0];\n"	\
		"	gl_Position = u_projection_matrix*(pos+vec4(out_VertOffset,0,0));\n"	\
		"	EmitVertex();\n"	\
		"	EndPrimitive();\n"	\
		"}\n";

	glShaderSource(iGeometryShaderObject_Particle2, 1, (const GLchar**)&geometryShaderSourceCode_FireFly2, NULL);

	// Compile Source Code
	glCompileShader(iGeometryShaderObject_Particle2);
	iInfoLogLength = 0;
	iCompileStatus = 0;
	chErrorMessage = NULL;
	glGetShaderiv(iGeometryShaderObject_Particle2, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iGeometryShaderObject_Particle2, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			chErrorMessage = (char*)malloc(iInfoLogLength);
			if (chErrorMessage != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iGeometryShaderObject_Particle2, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
				fprintf_s(g_pFile, "Error : Geometry Shader Particles 2 Compilation Log : %s \n", chErrorMessage);
				free(chErrorMessage);
				chErrorMessage = NULL;
				return INIT_GEOMETRY_SHADER_COMPILATION_FAILED;
			}
		}
	}
	/*Geomerty Shader Stop */

	/*Fragment Shader Start*/
	iFragmentShaderObject_Particle2 = glCreateShader(GL_FRAGMENT_SHADER);
	// FS Source Code
	const GLchar* chFragmentShaderSourceCode_Inlet2 = "#version 450 core"	\
		"\n"	\
		"in vec4  gs_out_Color;"	\
		"in float gs_out_t;"
		"in float gs_out_STtime;\n"	\
		"in vec2  out_VertOffset;\n"	\
		"out vec4 FragColor;\n"	\
		"uniform float u_animate_time;\n"	\
		"void main(void)\n"	\
		"{\n"	\
		"	float alpha;\n"	\
		"	//float s = 100.0*(1.0/(1.0 + 15.0*dot(out_VertOffset,out_VertOffset))- 1/16.0);\n"	\
		"	float s = 2.0/(dot(out_VertOffset,out_VertOffset));\n"	\
		"	//alpha = u_animate_time/16.0 * s * 0.2;\n"	\
		"	alpha = u_animate_time * s;\n"	\
		"	if(   (gs_out_t <= 0.0) ||(s < 0.85 ) )\n"	\
		"	{\n"	\
		"		discard;\n"	\
		"	}\n"	\
		"	FragColor = s*vec4(gs_out_Color.r, gs_out_Color.g, gs_out_Color.b, alpha);\n"	\
		"}\n";
	glShaderSource(iFragmentShaderObject_Particle2, 1, (const GLchar**)&chFragmentShaderSourceCode_Inlet2, NULL);

	//|| (gs_out_t >= 15.0) )

	iInfoLogLength = 0;
	iCompileStatus = 0;
	chErrorMessage = NULL;

	glCompileShader(iFragmentShaderObject_Particle2);
	glGetShaderiv(iFragmentShaderObject_Particle2, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_Particle2, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			chErrorMessage = (char*)malloc(iInfoLogLength);
			glGetShaderInfoLog(iFragmentShaderObject_Particle2, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
			fprintf_s(g_pFile, "Fragment Shader Particles 2 Error : %s \n ", chErrorMessage);
			free(chErrorMessage); chErrorMessage = NULL;
			//exit(EXIT_FAILURE);
			return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
		}
	}
	/*Fragment Shader End*/

	/* Shader Program Start */
	g_ShaderProgramObject_Particle2 = glCreateProgram();
	// Attach all Shaders
	glAttachShader(g_ShaderProgramObject_Particle2, iVertexShaderObject_Particle2);
	glAttachShader(g_ShaderProgramObject_Particle2, iGeometryShaderObject_Particle2);
	glAttachShader(g_ShaderProgramObject_Particle2, iFragmentShaderObject_Particle2);
	// Bind Attributes in Shader

	glBindAttribLocation(g_ShaderProgramObject_Particle2, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(g_ShaderProgramObject_Particle2, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(g_ShaderProgramObject_Particle2, AMC_VELOCITY_ARRAY, "Velocity");
	glBindAttribLocation(g_ShaderProgramObject_Particle2, AMC_START_TIME_ARRAY, "StartTime");
	//Link Shader
	glLinkProgram(g_ShaderProgramObject_Particle2);

	iLinkStatus = 0;
	iInfoLogLength = 0;
	chErrorMessage = NULL;
	glGetProgramiv(g_ShaderProgramObject_Particle2, GL_LINK_STATUS, &iLinkStatus);

	if (iLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_Particle2, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			chErrorMessage = (char*)malloc(iInfoLogLength);
			if (chErrorMessage != NULL)
			{
				glGetProgramInfoLog(g_ShaderProgramObject_Particle2, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
				fprintf_s(g_pFile, "Error In ShaderProgram Particle: %s \n", chErrorMessage);
				free(chErrorMessage); chErrorMessage = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
			}
		}
	}
	/* Shader Program End */

	/*Setup Uniforms Start*/
	g_Uniform_ModelMatrix_Particle2 = glGetUniformLocation(g_ShaderProgramObject_Particle2, "u_model_matrix");
	g_Uniform_ViewMatrix_Particle2 = glGetUniformLocation(g_ShaderProgramObject_Particle2, "u_view_matrix");
	g_Uniform_ProjectionMatrix_Particle2 = glGetUniformLocation(g_ShaderProgramObject_Particle2, "u_projection_matrix");
	g_Uniform_Time_Particle2 = glGetUniformLocation(g_ShaderProgramObject_Particle2, "Time");
	g_Uniform_Anime_Particle2 = glGetUniformLocation(g_ShaderProgramObject_Particle2, "u_animate_time");
	/*Setup Uniforms End*/

	/* Fill Buffers Start*/
	CreatePoints_FireFly(5, 5);

	// Positive Side  Buffer
	glGenVertexArrays(1, &g_VertexArrayObject_Particle);
	glBindVertexArray(g_VertexArrayObject_Particle);

	glGenBuffers(1, &g_VertexBufferObject_ParticlePosition);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_ParticlePosition);
	glBufferData(GL_ARRAY_BUFFER, g_arrayWidth_Particle * g_arrayWidth_Particle * 3 * sizeof(float), g_pfVerts_Particle, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_ParticleColor);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_ParticleColor);
	glBufferData(GL_ARRAY_BUFFER, g_arrayWidth_Particle * g_arrayWidth_Particle * 3 * sizeof(float), g_pfColors_Particle, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_ParticleVelocity);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_ParticleVelocity);
	glBufferData(GL_ARRAY_BUFFER, g_arrayWidth_Particle * g_arrayWidth_Particle * 3 * sizeof(float), g_pfVelocities_Particle, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_VELOCITY_ARRAY, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_VELOCITY_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_ParticleStartTime);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_ParticleStartTime);
	glBufferData(GL_ARRAY_BUFFER, g_arrayWidth_Particle * g_arrayWidth_Particle * sizeof(float), g_pfStartTimes_Particle, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_START_TIME_ARRAY, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_START_TIME_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	// Negative Side  Buffer
	glGenVertexArrays(1, &g_VertexArrayObject_ParticleInv);
	glBindVertexArray(g_VertexArrayObject_ParticleInv);

	glGenBuffers(1, &g_VertexBufferObject_ParticleInvPosition);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_ParticleInvPosition);
	glBufferData(GL_ARRAY_BUFFER, g_arrayWidth_Particle * g_arrayWidth_Particle * 3 * sizeof(float), g_pfVerts_Particle, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_ParticleInvColor);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_ParticleInvColor);
	glBufferData(GL_ARRAY_BUFFER, g_arrayWidth_Particle * g_arrayWidth_Particle * 3 * sizeof(float), g_pfColors_Particle, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_ParticleInvVelocity);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_ParticleInvVelocity);
	glBufferData(GL_ARRAY_BUFFER, g_arrayWidth_Particle * g_arrayWidth_Particle * 3 * sizeof(float), g_pfVelocities_ParticleInv, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_VELOCITY_ARRAY, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_VELOCITY_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_VertexBufferObject_ParticleInvStartTime);
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_ParticleInvStartTime);
	glBufferData(GL_ARRAY_BUFFER, g_arrayWidth_Particle * g_arrayWidth_Particle * sizeof(float), g_pfStartTimes_Particle, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_START_TIME_ARRAY, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_START_TIME_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	/* Fill Buffers End*/
	/********** Particle Code Stop     ************/

	/* All Font rendering Code Start */
	pCredits = new CCredits();
	pCredits->initialize();
	/* All Font rendering Code Stop  */

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);

	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);

	g_PersPectiveProjectionMatrix = glm::mat4(1.0f);
	g_PersPectiveProjectionMatrix_SkyBox = glm::mat4(1.0f);

	Resize(WIN_WIDTH, WIN_HEIGHT);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (m4TempMatrix)
	{
		free(m4TempMatrix);
		m4TempMatrix = NULL;
	}

	if (m4TempMatrix_palmBENT)
	{
		free(m4TempMatrix_palmBENT);
		m4TempMatrix_palmBENT = NULL;
	}

	if (m4TempMatrix_palmTILED)
	{
		free(m4TempMatrix_palmTILED);
		m4TempMatrix_palmTILED = NULL;
	}

	if (m4TempMatrix_palmST)
	{
		free(m4TempMatrix_palmST);
		m4TempMatrix_palmST = NULL;
	}

	if (g_pfColors_Particle != NULL)
	{
		free(g_pfColors_Particle);
		g_pfColors_Particle = NULL;
	}

	if (g_pfVelocities_Particle != NULL)
	{
		free(g_pfVelocities_Particle);
		g_pfVelocities_Particle = NULL;
	}

	if (g_pfVelocities_ParticleInv != NULL)
	{
		free(g_pfVelocities_ParticleInv);
		g_pfVelocities_ParticleInv = NULL;
	}

	if (g_pfStartTimes_Particle != NULL)
	{
		free(g_pfStartTimes_Particle);
		g_pfStartTimes_Particle = NULL;
	}

	p4 = Get_Parabola_4P(start_Bee_X, start_Bee_Z);
	GLfloat tempZ = start_Bee_Z;
	while (tempZ >= 0.0f)
	{
		bee_pos_parabola.x = Get_Bee_X(tempZ);
		bee_pos_parabola.z = tempZ;

		tempZ = tempZ - Bee_Z_Increment;

		bee_parabola.push_back(bee_path_constant - bee_pos_parabola);
	}

	glCreateVertexArrays(1, &vao_bee_parabola);
	glBindVertexArray(vao_bee_parabola);

	GLuint vbo_bee_parabola;
	glCreateBuffers(1, &vbo_bee_parabola);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bee_parabola);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * bee_parabola.size(), bee_parabola.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bee_parabola);

	glBindVertexArray(0);




	cam_to_bee_curve = generateBezierCurve(cam_path_control_points, CURVE_1_TOTAL_POINTS);

	glCreateVertexArrays(1, &vao_cam_to_bee_curve);
	glBindVertexArray(vao_cam_to_bee_curve);

	glCreateBuffers(1, &vbo_cam_to_bee_curve);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cam_to_bee_curve);
	glBufferData(GL_ARRAY_BUFFER, cam_to_bee_curve.size() * 3 * sizeof(GLfloat), cam_to_bee_curve.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glCreateVertexArrays(1, &vao_cam_to_bee_control_points);
	glBindVertexArray(vao_cam_to_bee_control_points);
	glCreateBuffers(1, &vbo_cam_to_bee_control_points);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cam_to_bee_control_points);
	GLfloat tpoints[] = {
		cam_path_control_points[0].x, cam_path_control_points[0].y, cam_path_control_points[0].z,
		cam_path_control_points[1].x, cam_path_control_points[1].y, cam_path_control_points[1].z,
		cam_path_control_points[2].x, cam_path_control_points[2].y, cam_path_control_points[2].z,
		cam_path_control_points[3].x, cam_path_control_points[3].y, cam_path_control_points[3].z,
		cam_path_control_points[4].x, cam_path_control_points[4].y, cam_path_control_points[4].z
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 15, tpoints, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	bee_to_lake_curve = generateBezierCurve(bee_to_lake_control_points, CURVE_2_TOTAL_POINTS);

	glCreateVertexArrays(1, &vao_bee_to_lake_curve);
	glBindVertexArray(vao_bee_to_lake_curve);

	glCreateBuffers(1, &vbo_bee_to_lake_curve);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bee_to_lake_curve);
	glBufferData(GL_ARRAY_BUFFER, bee_to_lake_curve.size() * 3 * sizeof(GLfloat), bee_to_lake_curve.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glCreateVertexArrays(1, &vao_bee_to_lake_control_points);
	glBindVertexArray(vao_bee_to_lake_control_points);
	glCreateBuffers(1, &vbo_bee_to_lake_control_points);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bee_to_lake_control_points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 15, bee_to_lake_control_points, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	if (SceneCounter == 1)
	{
		mainCamera->SetCameraPosition(SUNRISE_POSITION_1);
		mainCamera->SetYaw(SUNRISE_YAW_1);
		mainCamera->SetPitch(SUNRISE_PITCH_1);
		mainCamera->UpdateCameraVectors();
	}
	else if (SceneCounter == 2)
	{
		mainCamera->SetCameraPosition(SUNRISE_POSITION_1);
		mainCamera->SetYaw(SUNRISE_FINAL_YAW_1);
		mainCamera->SetPitch(SUNRISE_PITCH_1);

		mainCamera->UpdateCameraVectors();

		g_angles_Scene = CalculateYawPitch(
			SUNRISE_POSITION_1,
			FINAL_POSITION_1
		);

		yawIncrement = (FINAL_YAW_1 - SUNRISE_FINAL_YAW_1) / YAW_INCREMENT_FACTOR_1;
		pitchIncrement = (FINAL_PITCH_1 - SUNRISE_PITCH_1) / PITCH_INCREMENT_FACTOR_1;
	}
	else if (SceneCounter == 3)
	{
		mainCamera->SetCameraPosition(FINAL_POSITION_1);
		mainCamera->SetYaw(FINAL_YAW_1);
		mainCamera->SetPitch(FINAL_PITCH_1);

		g_bAnimateSunflower = false;
		g_bSunIsRunning = false;
		g_bShowButterFlies = true;

		mainCamera->UpdateCameraVectors();
	}
	else if (SceneCounter == 5)
	{
		mainCamera->SetCameraPosition(FINAL_POSITION_4);
		mainCamera->SetYaw(FINAL_YAW_4);
		mainCamera->SetYaw(FINAL_PITCH_4);

		mainCamera->UpdateCameraVectors();

		numberStepsCamToBeeCurve = (int)cam_to_bee_curve.size() - 1;
	}
	else if (SceneCounter == 6)
	{
		numberStepsBeeToLake = (int)bee_to_lake_curve.size() - 2;

		// preperation
		bee_to_lotus_angles = CalculateYawPitch(bee_to_lotus_points[0], bee_to_lotus_points[1]);

		bee_to_lotus_front = glm::vec3(
			glm::cos(bee_to_lotus_angles.y) * glm::cos(bee_to_lotus_angles.x),
			glm::sin(bee_to_lotus_angles.y),
			glm::cos(bee_to_lotus_angles.y) * glm::sin(bee_to_lotus_angles.x)
		);
	}

	/*Audio  Setup Start*/
	pMasterAudio = new AudioManager();
	ALboolean bAudioManagerrInitialize = pMasterAudio->initialize();
	if (!bAudioManagerrInitialize)
	{
		return INIT_AUDIO_MANAGER_INIT_FAILED;
	}


	pMasterAudio->setListenerPosition(0.0f, 0.0f, 0.0f);
	pMasterAudio->setListenerVelocity(0.0f, 0.0f, 0.0f);

	//g_iAudioBufferId1
	alGenBuffers(1, &g_iAudioBufferId1);

	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
	{
		fprintf_s(g_pFile, "Not able to create audio buffer, error code : %d\n", error);
		return INIT_AUDIO_LOAD_FAILED;
	}
	error = AL_NO_ERROR;

	ALboolean waveDataLoaded = pMasterAudio->loadWaveAudio("HeavenAndEarth.wav", g_iAudioBufferId1);

	if (!waveDataLoaded)
	{
		fprintf_s(g_pFile, "Not able to Load audio file.\n");
		return INIT_AUDIO_LOAD_FAILED;
	}


	g_pHeavenAndEarthSrc = new SoundSource();

	g_pHeavenAndEarthSrc->setPosition3f(0.0f, 0.0f, 0.0f);


	/*Audio  Setup Stop */


	return INIT_ALL_OK;
}

// Particle System

void CreatePoints_FireFly(GLint iWidth, GLint iHeight)
{
	GLfloat* vptr = NULL, * cptr = NULL, * velptr = NULL, * stptr = NULL, * invVelPtr = NULL;
	GLfloat i, j;

	if (g_pfVerts_Particle != NULL)
	{
		free(g_pfVerts_Particle);
	}

	if (g_pfColors_Particle != NULL)
	{
		free(g_pfColors_Particle);
	}

	if (g_pfVelocities_Particle != NULL)
	{
		free(g_pfVelocities_Particle);
	}

	if (g_pfStartTimes_Particle != NULL)
	{
		free(g_pfStartTimes_Particle);
	}

	if (g_pfVelocities_ParticleInv != NULL)
	{
		free(g_pfVelocities_ParticleInv);
	}


	g_pfVerts_Particle = (GLfloat*)malloc(iWidth * iHeight * 3 * (sizeof(GLfloat)));
	g_pfColors_Particle = (GLfloat*)malloc(iWidth * iHeight * 3 * (sizeof(GLfloat)));
	g_pfVelocities_Particle = (GLfloat*)malloc(iWidth * iHeight * 3 * (sizeof(GLfloat)));
	g_pfStartTimes_Particle = (GLfloat*)malloc(iWidth * iHeight * (sizeof(GLfloat)));

	g_pfVelocities_ParticleInv = (GLfloat*)malloc(iWidth * iHeight * 3 * (sizeof(GLfloat)));

	vptr = g_pfVerts_Particle;
	cptr = g_pfColors_Particle;
	velptr = g_pfVelocities_Particle;
	stptr = g_pfStartTimes_Particle;

	invVelPtr = g_pfVelocities_ParticleInv;

	for (i = 0.5f / iWidth - 0.5f; i < 0.5f; i = i + 1.0f / iWidth)
	{
		for (j = 0.5f / iHeight - 0.5f; j < 0.5f; j = j + 1.0f / iHeight)
		{

			/**(vptr + 0) = 0.0f;//55.0f;
			*(vptr + 1) = (15.0f * ((GLfloat)rand() / (GLfloat)RAND_MAX)) - 4.0f;
			*(vptr + 2) = ((GLfloat)rand() / (GLfloat)RAND_MAX) * 0.5f + 0.5f;
			vptr += 3;*/


			*(vptr + 0) = 0.0f;//55.0f;
			*(vptr + 1) = (50.0f * ((GLfloat)rand() / (GLfloat)RAND_MAX)) - 4.0f;
			*(vptr + 2) = 350.0f * (((GLfloat)rand() / (GLfloat)RAND_MAX) * 0.5f + 0.5f);
			//*(vptr + 2) = 750.0 * (((GLfloat)rand() / (GLfloat)RAND_MAX) * 0.5f + 0.5f)-380.0f;
			vptr += 3;

			*cptr = 0.07f;
			*(cptr + 1) = 0.53f;
			*(cptr + 2) = 0.03f;
			cptr += 3;

			*velptr = 3.0f;
			*(velptr + 1) = 0.5f;
			*(velptr + 2) = 0.0f;
			velptr += 3;

			*invVelPtr = -3.0f;
			*(invVelPtr + 1) = 0.5f;
			*(invVelPtr + 2) = 0.0f;
			invVelPtr += 3;

			*stptr = ((float)rand() / (GLfloat)RAND_MAX) * 10.0f;
			stptr++;

		}
	}

	g_arrayWidth_Particle = iWidth;
	g_arrayHeight_Particle = iHeight;

}



// to set in water
int LoadGLTextures(GLuint* texture, TCHAR imageResourceId[])
{
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	int iStatus = FALSE;
	GLfloat Anisotropy = 4.0f;

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitmap)
	{
		fprintf_s(g_pFile, "EXEC : Inside LoadGLTextures - Image Obtained\n");
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);

		glGenTextures(1, texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Anisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Anisotropy);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);

		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap);

	}

	return iStatus;
}


GLuint LoadPngTexture(const char* pathToFile)
{
	GLuint textureID = NULL;
	GLfloat Anisotropy = 4.0f;

	//Generate and set current image ID
	ILuint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);

	//Load image
	if (!ilLoadImage((ILstring)pathToFile))
	{
		return NULL;
	}

	glGenTextures(1, &textureID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Anisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Anisotropy);

	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


void Update(void)
{

	if (g_bIsDemoStarted == true)
	{
		// Water
		g_fWaterMoveFactor = g_fWaterMoveFactor + WAVE_SPEED;
		//g_fWaterMoveFactor = g_fWaterMoveFactor % 1;

		g_Angle_Pyramid = g_Angle_Pyramid + 0.5f;
		if (g_Angle_Pyramid >= 360.0f)
			g_Angle_Pyramid = 0;


		if (g_pSkyDome->IsNight())
		{
			/*if (SceneCounter > 6)
			{
				if (g_Angle_CubeMap_Rotation <= 100.0f && g_Angle_CubeMap_Rotation >= 50.0f)
				{
					g_bSunIsRunning = false;
				}
				else
				{
					g_bSunIsRunning = true;
				}
			}*/

			g_Angle_CubeMap_Rotation = g_Angle_CubeMap_Rotation - 0.01f;
			if (g_Angle_CubeMap_Rotation <= 0.0f)
			{
				g_Angle_CubeMap_Rotation = 0.0f;
			}
		}

		/*glm::vec3 posSet = mainCamera->GetCameraPosition();
		posSet.y = g_pTerrain->GetHeightAt(posSet);
		mainCamera->SetCameraPosition(posSet);*/

		g_dBeeAnimateTime = g_dBeeAnimateTime + 0.0055f;

		if (g_bAnimateSunflower)
		{
			g_dSunFlowerAnimateTime = g_dSunFlowerAnimateTime + 0.0055f;
			if (g_dSunFlowerAnimateTime > 8.476500)
			{
				g_dSunFlowerAnimateTime = 8.476500;
				g_bShowButterFlies = true;
				g_bAnimateSunflower = false;
			}
		}


		g_fTime = g_fTime + 50.0f;

		if (g_bShowButterFlies)
		{
			if (g_pFlockingButterfiles)
			{
				g_pFlockingButterfiles->Update();
			}
		}

		// Uiniversal Timer
		g_universalTimer = g_universalTimer + 0.001f;



		/********** Particle Code Start    ************/
		if (g_pSkyDome->IsNight())
		{
			// Particle Movement
			g_pfParticleMoveTime_Particle1 += 0.0025f;// Particle System Time
			if (g_pfParticleMoveTime_Particle1 > 50.0f)
			{
				g_pfParticleMoveTime_Particle1 = 0.0f;
			}

			g_pfParticleMoveTime_Particle2 += 0.0025f;
			if (g_pfParticleMoveTime_Particle2 > 50.0f)
			{
				g_pfParticleMoveTime_Particle2 = 0.0f;
			}


			//Particle blending 
			if (g_bDoneBlending1)
			{
				g_pfParticleAnimeTime_Particle1 -= 0.07f;
				if (g_pfParticleAnimeTime_Particle1 <= -1.0f)
				{
					g_pfParticleAnimeTime_Particle1 = -1.0f;
					g_bDoneBlending1 = false;
				}
			}
			else
			{
				g_pfParticleAnimeTime_Particle1 += 0.05f;
				if (g_pfParticleAnimeTime_Particle1 >= 1.0f)
				{
					g_pfParticleAnimeTime_Particle1 = 1.0f;
					g_bDoneBlending1 = true;
				}
			}


			if (g_bDoneBlending2)
			{
				g_pfParticleAnimeTime_Particle2 -= 0.07f;
				if (g_pfParticleAnimeTime_Particle2 <= -1.0f)
				{
					g_pfParticleAnimeTime_Particle2 = -1.0f;
					g_bDoneBlending2 = false;
				}
			}
			else
			{
				g_pfParticleAnimeTime_Particle2 += 0.05f;
				if (g_pfParticleAnimeTime_Particle2 >= 1.0f)
				{
					g_pfParticleAnimeTime_Particle2 = 1.0f;
					g_bDoneBlending2 = true;
				}
			}


			if (g_bDoneBlending3)
			{
				g_pfParticleAnimeTime_Particle3 -= 0.07f;
				if (g_pfParticleAnimeTime_Particle3 <= -1.0f)
				{
					g_pfParticleAnimeTime_Particle3 = -1.0f;
					g_bDoneBlending3 = false;
				}
			}
			else
			{

				g_pfParticleAnimeTime_Particle3 += 0.05f;
				if (g_pfParticleAnimeTime_Particle3 >= 1.0f)
				{
					g_pfParticleAnimeTime_Particle3 = 1.0f;
					g_bDoneBlending3 = true;
				}
			}



			if (g_bDoneBlending4)
			{
				g_pfParticleAnimeTime_Particle4 -= 0.07f;
				if (g_pfParticleAnimeTime_Particle4 <= -1.0f)
				{
					g_pfParticleAnimeTime_Particle4 = -1.0f;
					g_bDoneBlending4 = false;
				}
			}
			else
			{

				g_pfParticleAnimeTime_Particle4 += 0.05f;
				if (g_pfParticleAnimeTime_Particle4 >= 1.0f)
				{
					g_pfParticleAnimeTime_Particle4 = 1.0f;
					g_bDoneBlending4 = true;
				}
			}
		}
		/********** Particle Code Stop     ************/


		// camera : sunrise

		if (SceneCounter == 1) // Sunrise Scene
		{
			if (g_universalTimer < SUNRISE_TIMER_1)
			{
				mainCamera->SetCameraPosition(SUNRISE_POSITION_1);
				mainCamera->SetYaw(SUNRISE_YAW_1);
				mainCamera->SetPitch(SUNRISE_PITCH_1);
			}
			else
			{
				if (mainCamera->GetYaw() >= SUNRISE_FINAL_YAW_1)
				{
					mainCamera->SetCameraPosition(SUNRISE_POSITION_1);
					mainCamera->SetYaw(mainCamera->GetYaw() - SUNRISE_ROTATION_SPEED_1);
					mainCamera->SetPitch(SUNRISE_PITCH_1);
				}
				else
				{
					SceneCounter = 2;

					g_angles_Scene = CalculateYawPitch(
						SUNRISE_POSITION_1,
						FINAL_POSITION_1
					);

					yawIncrement = (FINAL_YAW_1 - mainCamera->GetYaw()) / YAW_INCREMENT_FACTOR_1;
					pitchIncrement = (FINAL_PITCH_1 - mainCamera->GetPitch()) / PITCH_INCREMENT_FACTOR_1;
				}
			}

			mainCamera->UpdateCameraVectors();
		}

		if (SceneCounter == 2)
		{
			if (mainCamera->GetCameraPosition().x <= FINAL_POSITION_1.x)
			{
				// prepare movement front 
				glm::vec3 front = glm::vec3(
					glm::cos(g_angles_Scene.y) * glm::cos(g_angles_Scene.x),
					glm::sin(g_angles_Scene.y),
					glm::cos(g_angles_Scene.y) * glm::sin(g_angles_Scene.x)
				);

				glm::vec3 newPosition = mainCamera->GetCameraPosition() + (front);

				mainCamera->SetCameraPosition(glm::vec3(newPosition));

				GLfloat currentYaw = mainCamera->GetYaw();
				GLfloat currentPitch = mainCamera->GetPitch();

				if (currentYaw < FINAL_YAW_1)
				{
					mainCamera->SetYaw(mainCamera->GetYaw() + yawIncrement);
				}

				if (currentPitch > FINAL_PITCH_1)
				{
					mainCamera->SetPitch(mainCamera->GetPitch() + pitchIncrement);
				}

				mainCamera->UpdateCameraVectors();
			}
			else
			{
				SceneCounter = 3;

				yawIncrement = 0.0f;
				pitchIncrement = 0.0f;

				g_bSunIsRunning = false;

				g_bAnimateSunflower = true;

				p4 = Get_Parabola_4P(start_Bee_X, start_Bee_Z);
			}
		}

		if (SceneCounter == 3 && g_bShowButterFlies)
		{
			if (start_Bee_Z > 0.0f)
			{
				glm::vec3 firstPoint = bee_pos_parabola;

				bee_pos_parabola.x = Get_Bee_X(start_Bee_Z);
				bee_pos_parabola.y = 0.0f;
				bee_pos_parabola.z = start_Bee_Z;

				start_Bee_Z = start_Bee_Z - Bee_Z_Increment;

				bee_pos_parabola = bee_path_constant - bee_pos_parabola;

				bee_pos_parabola_angle_degrees = glm::degrees(CalculateYawPitch(firstPoint, bee_pos_parabola).x);

				bee_wait_time = g_universalTimer;
			}
			else if (g_universalTimer > bee_wait_time + bee_wait_time_interval)
			{
				SceneCounter = 4;

				yawIncrement = (FINAL_YAW_4 - mainCamera->GetYaw()) / 500.0f;
				pitchIncrement = (FINAL_PITCH_4 - mainCamera->GetPitch()) / 500.0f;
			}
		}

		if (SceneCounter == 4)
		{
			if (mainCamera->GetYaw() > FINAL_YAW_4)
			{
				mainCamera->SetYaw(mainCamera->GetYaw() + yawIncrement);
				mainCamera->SetPitch(mainCamera->GetPitch() + pitchIncrement);
			}
			else if (FINAL_POSITION_4.x > mainCamera->GetCameraPosition().x)
			{
				butterfly_scene_wait_time = g_universalTimer;
			}
			else if (g_universalTimer > butterfly_scene_wait_time + 0.02f)
			{
				SceneCounter = 5;

				mainCamera->SetYaw(FINAL_YAW_4);
				mainCamera->SetPitch(FINAL_PITCH_4);

				numberStepsCamToBeeCurve = (int)cam_to_bee_curve.size() - 1;
				CamToBeeCurvePosition = 0;
			}
		}

		// Done
		if (SceneCounter == 5)
		{
			int pos = beeToLakePosition + 50;
			if (pos > CURVE_1_TOTAL_POINTS)
			{
				bee_pos_parabola = bee_to_lake_curve[pos];
			}
			else
			{
				bee_pos_parabola = bee_to_lake_curve[pos];
			}

			if (numberStepsCamToBeeCurve > CamToBeeCurvePosition)
			{
				if (CamToBeeCurvePosition >= 0 && CamToBeeCurvePosition < CURVE_1_PART_1_POINTS)
				{
					Curve_1_YAW_Angle = Curve_1_YAW_Angle + CURVE_1_PART_1_YAW_INCREMENT;
					Curve_1_Pitch_Angle = Curve_1_Pitch_Angle + CURVE_1_PART_1_PITCH_INCREMENT;

					mainCamera->SetYaw(Curve_1_YAW_Angle);
					mainCamera->SetPitch(Curve_1_Pitch_Angle);

					bee_wait_time = g_universalTimer + 5.5f;
					++CamToBeeCurvePosition;
				}
				else if (CamToBeeCurvePosition >= CURVE_1_PART_1_POINTS && CamToBeeCurvePosition < CURVE_1_PART_2_POINTS && bee_wait_time < g_universalTimer)
				{
					Curve_1_YAW_Angle = Curve_1_YAW_Angle + CURVE_1_PART_2_YAW_INCREMENT;
					Curve_1_Pitch_Angle = Curve_1_Pitch_Angle + CURVE_1_PART_2_PITCH_INCREMENT;

					mainCamera->SetYaw(Curve_1_YAW_Angle);
					mainCamera->SetPitch(Curve_1_Pitch_Angle);

					++CamToBeeCurvePosition;
				}
				else if (CamToBeeCurvePosition >= CURVE_1_PART_2_POINTS && CamToBeeCurvePosition < CURVE_1_PART_3_POINTS)
				{
					Curve_1_YAW_Angle = Curve_1_YAW_Angle + CURVE_1_PART_3_YAW_INCREMENT;
					Curve_1_Pitch_Angle = Curve_1_Pitch_Angle + CURVE_1_PART_3_PITCH_INCREMENT;

					mainCamera->SetYaw(Curve_1_YAW_Angle);
					mainCamera->SetPitch(Curve_1_Pitch_Angle);

					++CamToBeeCurvePosition;
				}
				else if (CamToBeeCurvePosition >= CURVE_1_PART_3_POINTS && CamToBeeCurvePosition < CURVE_1_PART_4_POINTS)
				{
					Curve_1_YAW_Angle = Curve_1_YAW_Angle + CURVE_1_PART_4_YAW_INCREMENT;
					Curve_1_Pitch_Angle = Curve_1_Pitch_Angle + CURVE_1_PART_4_PITCH_INCREMENT;

					mainCamera->SetYaw(Curve_1_YAW_Angle);
					mainCamera->SetPitch(Curve_1_Pitch_Angle);

					++CamToBeeCurvePosition;
				}

				mainCamera->SetCameraPosition(cam_to_bee_curve[CamToBeeCurvePosition]);

				mainCamera->UpdateCameraVectors();
			}
			else
			{
				SceneCounter = 6;

				g_bSunIsRunning = true;// enable sky move

				numberStepsBeeToLake = (int)bee_to_lake_curve.size() - 2;
				beeToLakePosition = 0;

				// preperation
				bee_to_lotus_angles = CalculateYawPitch(bee_to_lotus_points[0], bee_to_lotus_points[1]);

				bee_to_lotus_front = glm::vec3(
					glm::cos(bee_to_lotus_angles.y) * glm::cos(bee_to_lotus_angles.x),
					glm::sin(bee_to_lotus_angles.y),
					glm::cos(bee_to_lotus_angles.y) * glm::sin(bee_to_lotus_angles.x)
				);
			}
		}

		// 
		if (SceneCounter == 6)
		{
			int pos = beeToLakePosition + 50;
			if (pos < CURVE_1_TOTAL_POINTS)
			{
				bee_pos_parabola = bee_to_lake_curve[pos];

				bee_pos_parabola_angle_degrees = Curve_2_YAW_Angle + 10.0f;
			}
			else
			{
				if (bee_pos_parabola.x <= bee_to_lotus_points[1].x)
				{
					bee_pos_parabola = bee_pos_parabola + bee_to_lotus_front;
					bee_pos_parabola_angle_degrees = 30.0f - CalculateYawPitch(BEE_END_POSITION_LAKE, bee_to_lotus_points[1]).x;
				}
			}

			if (numberStepsBeeToLake > beeToLakePosition)
			{
				//glm::vec2 angles = CalculateYawPitch(bee_to_lake_curve[beeToLakePosition], bee_to_lake_curve[beeToLakePosition + 1]);

				//fprintf(g_pFile, "%d\t\t%f\t%f\t%f\t\t%f\t%f\n", beeToLakePosition, bee_to_lake_curve[beeToLakePosition].x, bee_to_lake_curve[beeToLakePosition].y, bee_to_lake_curve[beeToLakePosition].z, glm::degrees(angles.x), glm::degrees(angles.y));

				//mainCamera->SetYaw(glm::degrees(angles.x));

				//if (beeToLakePosition > 850)
				//{
				//	mainCamera->SetPitch(glm::degrees(angles.y)/* - 40.0f * (GLfloat)(beeToLakePosition - 800) / 199.0f*/);
				//}
				//else if (beeToLakePosition < 200)
				//{
				//	mainCamera->SetPitch(glm::degrees(angles.y));
				//}
				//else
				//{
				//	if (beeToLakePosition < 250)
				//	{
				//		mainCamera->SetPitch(glm::degrees(angles.y) + 20.0f * ((GLfloat)beeToLakePosition - 199.0f) / 50.0f);
				//	}
				//	else
				//	{
				//		mainCamera->SetPitch(glm::degrees(angles.y) + 20.0f);
				//	}
				//}
				//mainCamera->SetCameraPosition(bee_to_lake_curve[beeToLakePosition]);

				//mainCamera->UpdateCameraVectors();

				//++beeToLakePosition;

				if (beeToLakePosition >= 0 && beeToLakePosition < CURVE_2_PART_1_POINTS)
				{
					mainCamera->SetYaw(Curve_2_YAW_Angle);
					mainCamera->SetPitch(Curve_2_Pitch_Angle);

					Curve_2_YAW_Angle = Curve_2_YAW_Angle + CURVE_2_PART_1_YAW_INCREMENT;
					Curve_2_Pitch_Angle = Curve_2_Pitch_Angle + CURVE_2_PART_1_PITCH_INCREMENT;

					++beeToLakePosition;
				}
				else if (beeToLakePosition >= CURVE_2_PART_1_POINTS && beeToLakePosition < CURVE_2_PART_2_POINTS)
				{
					mainCamera->SetYaw(Curve_2_YAW_Angle);
					mainCamera->SetPitch(Curve_2_Pitch_Angle);

					Curve_2_YAW_Angle = Curve_2_YAW_Angle + CURVE_2_PART_2_YAW_INCREMENT;
					Curve_2_Pitch_Angle = Curve_2_Pitch_Angle + CURVE_2_PART_2_PITCH_INCREMENT;

					++beeToLakePosition;
				}
				else if (beeToLakePosition >= CURVE_2_PART_2_POINTS && beeToLakePosition < CURVE_2_PART_3_POINTS)
				{
					mainCamera->SetYaw(Curve_2_YAW_Angle);
					mainCamera->SetPitch(Curve_2_Pitch_Angle);

					Curve_2_YAW_Angle = Curve_2_YAW_Angle + CURVE_2_PART_3_YAW_INCREMENT;
					Curve_2_Pitch_Angle = Curve_2_Pitch_Angle + CURVE_2_PART_3_PITCH_INCREMENT;

					++beeToLakePosition;
					g_bShowButterFlies = false;
				}
				else if (beeToLakePosition >= CURVE_2_PART_3_POINTS && beeToLakePosition < CURVE_2_PART_4_POINTS)
				{
					mainCamera->SetYaw(Curve_2_YAW_Angle);
					mainCamera->SetPitch(Curve_2_Pitch_Angle);

					Curve_2_YAW_Angle = Curve_2_YAW_Angle + CURVE_2_PART_4_YAW_INCREMENT;
					Curve_2_Pitch_Angle = Curve_2_Pitch_Angle + CURVE_2_PART_4_PITCH_INCREMENT;

					++beeToLakePosition;
				}

				mainCamera->SetCameraPosition(bee_to_lake_curve[beeToLakePosition]);

				mainCamera->UpdateCameraVectors();
			}
			else
			{
				SceneCounter = 7;
			}
		}

		if (SceneCounter == 7)
		{
			if (bee_pos_parabola.x <= bee_to_lotus_points[1].x)
			{
				bee_pos_parabola = bee_pos_parabola + bee_to_lotus_front;
			}
			else
			{
				if (bee_pos_parabola.y > 0.0f)
				{
					bee_pos_parabola.y = bee_pos_parabola.y - 0.1f;
				}
				else
				{
					bee_pos_parabola_angle_degrees = bee_pos_parabola_angle_degrees + 90.0f;
					SceneCounter = 8;

					bee_to_flyaway_angles = CalculateYawPitch(bee_flyaway_points[0], bee_flyaway_points[1]);

					bee_to_flyaway_front = glm::vec3(
						glm::cos(bee_to_flyaway_angles.y) * glm::cos(bee_to_flyaway_angles.x),
						glm::sin(bee_to_flyaway_angles.y),
						glm::cos(bee_to_flyaway_angles.y) * glm::sin(bee_to_flyaway_angles.x)
					);
				}
			}
		}

		if (SceneCounter == 8 && g_pSkyDome->IsDay() && !g_pSkyDome->IsNight())
		{
			if (bee_pos_parabola.y < BEE_FLY_AWAY_HEIGHT)
			{
				bee_pos_parabola.y = bee_pos_parabola.y + 0.1f;
			}
			else if (bee_pos_parabola.z <= bee_flyaway_points[1].z)
			{
				bee_pos_parabola = bee_pos_parabola + bee_to_flyaway_front;
			}
			else
			{
				SceneCounter = 9;
			}
		}

		if (SceneCounter == 9)
		{
			g_fHDRAlphaValue = g_fHDRAlphaValue - 0.01f;
			if (g_fHDRAlphaValue <= 0.0f)
			{
				g_fHDRAlphaValue = 0.0f;
			}
		}

		/*glm::vec3 front = {
		glm::cos(glm::radians(mainCamera->GetPitch())) * glm::cos(glm::radians(mainCamera->GetYaw())),
		glm::sin(glm::radians(mainCamera->GetPitch())),
		glm::cos(glm::radians(mainCamera->GetPitch())) * glm::sin(glm::radians(mainCamera->GetYaw()))
		};*/
	}
	else
	{
		// splash Screen
		/* All Font rendering Code Start */
		if (pCredits != NULL)
		{
			//pCredits->update();
		}
		/* All Font rendering Code Stop  */
	}



}


GLfloat Get_Bee_X(GLfloat Z)
{
	GLfloat answer = glm::pow(Z, 2) / p4;
	return answer;
}

glm::vec2 CalculateYawPitch(glm::vec3 point_1, glm::vec3 point_2)
{
	glm::vec2 ret;

	GLfloat len_x = point_2.x - point_1.x;
	GLfloat len_y = point_2.y - point_1.y;
	GLfloat len_z = point_2.z - point_1.z;

	// pitch
	ret.y = glm::atan(len_y / len_z);

	// yay
	ret.x = glm::atan(len_z / len_x);

	return(ret);
}

void Render(void)
{

	void SetDayLight(GLuint ShaderObject);
	void SetNightLight(GLuint ShaderObject);



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 translationMatrix = glm::mat4(1.0f);
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	glm::mat4 modelViewProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 viewMat_SkyBox = glm::mat4(1.0f);


	if (g_bIsDemoStarted == true)
	{
		// Start Audio
		if (g_bPlayAudioOnce == false)
		{
			g_pHeavenAndEarthSrc->play(g_iAudioBufferId1);
			g_bPlayAudioOnce = true;
		}

		g_v3LightPosition = g_pSkyDome->GetSunWPos();
		g_m4LightViewMatrix = glm::lookAt(g_v3LightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		g_m4LightProjectionMatrix = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, FRUSTUM_DEPTH);

		g_v3CameraPosition = mainCamera->GetCameraPosition();

		viewMatrix = mainCamera->GetViewMatrix();


		viewMat_SkyBox = glm::mat4(glm::mat3(mainCamera->GetViewMatrix()));
		viewMat_SkyBox *= glm::rotate(glm::mat4(1.0), glm::radians(g_Angle_CubeMap_Rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		// Calculate Light properties as per the skyDome
		g_pSunLight->setAmbient(glm::vec3(g_pSkyDome->GetAmbientIntensity()));
		g_pSunLight->setDiffuse(glm::vec3(g_pSkyDome->GetDiffuseIntensity()));
		g_pSunLight->setSpecular(glm::vec3(g_pSkyDome->GetDiffuseIntensity()));
		g_pSunLight->setDirection(glm::vec3(-g_pSkyDome->GetSunWPos()));



		/* Framebuffer for depth Start  */
		// Fill the depth texture
		g_v3LightPosition = glm::vec3(0.0);
		g_m4LightViewMatrix = glm::lookAt(g_v3LightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		g_m4LightProjectionMatrix = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, FRUSTUM_DEPTH);
		glBindFramebuffer(GL_FRAMEBUFFER, g_ShadowFramebuffer);
		glClearDepth(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(2.0f, 4.0f);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		// Render the terrain
		modelMatrix = glm::mat4(1.0f);
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix = modelMatrix * translationMatrix;
		// Render Terrain
		// use modelMatrix,lightViewMatrix,lightProjectionMatrix
		glUseProgram(g_ShaderProgramObject_depth);

		glUniformMatrix4fv(g_depth_U_Model_Matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(g_depth_U_Projection_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightProjectionMatrix));
		glUniformMatrix4fv(g_depth_U_View_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightViewMatrix));

		modelMatrix = glm::mat4(1.0f);
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix = modelMatrix * translationMatrix;
		glUniformMatrix4fv(g_depth_U_Model_Matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		g_pTerrain->RenderTerrainShadow();
		glUseProgram(0);
		/*
		// Render trees
		PalmStraight->UseProgramShadow();
		// set uniforms
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamST, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		GLint locationID = glGetUniformLocation(PalmStraight->GetProgramShadow(), "u_Light_projection_matrix");
		glUniformMatrix4fv(locationID, 1, GL_FALSE, glm::value_ptr(g_m4LightProjectionMatrix));

		locationID = glGetUniformLocation(PalmStraight->GetProgramShadow(), "u_Light_view_matrix");
		glUniformMatrix4fv(locationID, 1, GL_FALSE, glm::value_ptr(g_m4LightViewMatrix));
		PalmStraight->DrawForShadow(PalmStraight->GetProgramShadow(), g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glUseProgram(0);*/

		glCullFace(GL_BACK);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/* Framebuffer for depth Stop   */

		glViewport(0, 0, giWinWidth, giWinHeight);
		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);



		/*** Water Code Start ***/
		mainCamera->InvertCamera();
		viewMatrix = mainCamera->GetViewMatrix();
		// Draw on Reflection Framebuffer Start
		glEnable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, g_FrameBuffer_Reflection);
		glViewport(0, 0, giWinWidth, giWinHeight);
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f, 0.75f, 1.0f, 1.0f)));
		glClearBufferfv(GL_DEPTH, 0, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

		// Render SkyBox Here  

		g_pCubeMap->DrawCubemap(viewMat_SkyBox, g_PersPectiveProjectionMatrix_SkyBox);
		//g_pCubeMap->DrawCubemap(pCameraCubeMap->GetViewMatrix(), g_PersPectiveProjectionMatrix_SkyBox);
		//g_pCubeMap->DrawCubemap(glm::mat4(glm::mat3(mainCamera->GetViewMatrix())), g_PersPectiveProjectionMatrix_SkyBox);

		modelMatrix = glm::mat4(1.0f);
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(g_v3CameraPosition.x, g_v3CameraPosition.y - g_pSkyDome->GetInnerRadius(), g_v3CameraPosition.z));
		modelMatrix = modelMatrix * translationMatrix;
		modelViewProjectionMatrix = g_PersPectiveProjectionMatrix_SkyBox * viewMatrix * modelMatrix;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		g_pSkyDome->RenderSky(g_v3CameraPosition, modelViewProjectionMatrix, false);
		glDisable(GL_BLEND);
		// Render terrain here
		modelMatrix = glm::mat4(1.0f);
		glUseProgram(g_pTerrain->GetShaderProgram());
		// set all light uniforms
		glUniform1f(g_UniformTerrain_XPixelOffset, 1.0f / DEPTH_TEXTURE_SIZE);
		glUniform1f(g_UniformTerrain_YPixelOffset, 1.0f / DEPTH_TEXTURE_SIZE);
		glUniform3fv(g_UniformTerrain_ViewPosition, 1, glm::value_ptr(mainCamera->GetCameraPosition()));
		glUniformMatrix4fv(g_UniformTerrain_shadowBias, 1, GL_FALSE, glm::value_ptr(ScaleBiasMatrix));
		glUniformMatrix4fv(g_UniformTerrain_lightprojrction_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightProjectionMatrix));
		glUniformMatrix4fv(g_UniformTerrain_lightView_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightViewMatrix));

		if (g_pSkyDome->IsNight())
		{
			glUniform3fv(g_UniformTerrain_sunAmbient, 1, glm::value_ptr(g_v3NightAmbient));
			glUniform3fv(g_UniformTerrain_sunSpecular, 1, glm::value_ptr(g_v3NightSpecular));
			glUniform3fv(g_UniformTerrain_sunDiffuse, 1, glm::value_ptr(g_v3NightDiffuse));
			glUniform3fv(g_UniformTerrain_sunDirection, 1, glm::value_ptr(g_v3NightDirection));
		}
		else
		{
			glUniform3fv(g_UniformTerrain_sunAmbient, 1, glm::value_ptr(g_pSunLight->getAmbient()));
			glUniform3fv(g_UniformTerrain_sunSpecular, 1, glm::value_ptr(g_pSunLight->getSpecular()));
			glUniform3fv(g_UniformTerrain_sunDiffuse, 1, glm::value_ptr(g_pSunLight->getDiffuse()));
			glUniform3fv(g_UniformTerrain_sunDirection, 1, glm::value_ptr(g_pSunLight->getDirection()));
		}

		glUniform4fv(g_UniformTerrain_ClipPlane, 1, glm::value_ptr(glm::vec4(0.0f, 1.0f, 0.0f, -300.0f))); // -ve for reflection

		g_pTerrain->RenderTerrain(modelMatrix, viewMatrix, g_pTerrain->m4TerrainPerspectiveProjectionMatrix, g_Texture_ShadowDepthTexure);

		glUseProgram(0);

		// Tree
		PalmStraight->UseProgram();
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(PalmStraight->GetProgram());
		}
		else
		{
			SetDayLight(PalmStraight->GetProgram());
		}


		GLint id = -1;
		id = glGetUniformLocation(PalmStraight->GetProgram(), "projection");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(PalmStraight->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamST, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		glDisable(GL_CULL_FACE);
		PalmStraight->Draw(g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glEnable(GL_CULL_FACE);
		glUseProgram(0);

		// Tilt trees
		PalmTilted->UseProgram();
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(PalmTilted->GetProgram());
		}
		else
		{
			SetDayLight(PalmTilted->GetProgram());
		}
		//SetDayLight(PalmTilted->GetProgram());
		id = -1;
		id = glGetUniformLocation(PalmTilted->GetProgram(), "projection");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(PalmTilted->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamTILT, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		glDisable(GL_CULL_FACE);
		PalmTilted->Draw(g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glEnable(GL_CULL_FACE);
		glUseProgram(0);

		// Bent trees
		PalmBent->UseProgram();
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(PalmBent->GetProgram());
		}
		else
		{
			SetDayLight(PalmBent->GetProgram());
		}
		//SetDayLight(PalmBent->GetProgram());
		id = -1;
		id = glGetUniformLocation(PalmBent->GetProgram(), "projection");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(PalmBent->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamBEN, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		glDisable(GL_CULL_FACE);
		PalmBent->Draw(g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glEnable(GL_CULL_FACE);
		glUseProgram(0);

		/*
		//Lotus start
		//LotusLeaf_0
		modelMatrix = glm::mat4(1.0);
		rotationMatrix = glm::mat4(1.0);
		scaleMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0);

		glUseProgram(g_ShaderProgramObject_LotusLeaf);
		glUniformMatrix4fv(g_Uniform_LotusLeaf_ViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(g_Uniform_LotusLeaf_ProjectionMatrix, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_LotusLeaf_DiffuseTexture);
		glUniform1i(g_uniform_LotusLeaf_DiffuseMapSampler, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, g_LotusLeaf_BumpTexture);
		glUniform1i(g_uniform_LotusLeaf_BumpMapSampler, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, g_LotusLeaf_SpecularTexture);
		glUniform1i(g_uniform_LotusLeaf_SpecularMapSampler, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingLotusLeafs0_reflection, 0, NUM_INSTANCES_LOTUSLEAFS0 * sizeof(glm::mat4));

		glBindVertexArray(g_VertexArrayObject_LotusLeaf0);
		glDrawArraysInstanced(GL_TRIANGLES, 0, LOTUSLEAF0_SIZE, NUM_INSTANCES_LOTUSLEAFS0);
		glBindVertexArray(0);
		glUseProgram(0);

		//LotusFlower Start
		modelMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);


		glUseProgram(g_ShaderProgramObject_Model_LotusFlower);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(447.590820, 13.0f, 799.615112));
		rotationMatrix = glm::rotate(rotationMatrix, -190.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, 100.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1.5f));

		modelMatrix = modelMatrix * rotationMatrix * scaleMatrix;

		glUniform3fv(g_Uniform_Model_LD_LotusFlower, 1, lightDiffuse_animModel);
		glUniform3fv(g_Uniform_Model_LA_LotusFlower, 1, lightAmbient_animModel);
		glUniform3fv(g_Uniform_Model_LS_LotusFlower, 1, lightSpecular_animModel);

		glUniform3fv(g_Uniform_Model_lightPosition_animModel_LotusFlower, 1, lightPosition_animModel);
		glUniform3fv(g_Uniform_Model_ViewPosition_LotusFlower, 1, glm::value_ptr(mainCamera->GetCameraPosition()));
		glUniform1f(g_Uniform_Model_MaterialShininess_LotusFlower, material_shininess_animModel);

		glUniformMatrix4fv(g_Uniform_Model_Model_Matrix_LotusFlower, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(g_Uniform_Model_View_Matrix_LotusFlower, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(g_Uniform_Model_Projection_Matrix_LotusFlower, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));
		glUniform1i(g_Uniform_Model_Animation_LotusFlower, false);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingNiceLotus1, 0, NUM_INSTANCES_NICELOTUS1 * sizeof(glm::mat4));

		myNiceLotus1->drawInstanced(g_ShaderProgramObject_Model_LotusFlower, g_bAnimationControl, 0.0, NUM_INSTANCES_NICELOTUS1);
		glUseProgram(0);

		//LotusFlower Stop
		//Lotus End
		*/

		// Render Grass Here
		modelMatrix = glm::mat4(1.0f);
		glDisable(GL_CULL_FACE);
		//displayGrass(modelMatrix, viewMatrix, g_pTerrain->m4TerrainPerspectiveProjectionMatrix, g_pSunLight);
		glEnable(GL_CULL_FACE);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_CLIP_DISTANCE0);
		// Draw on Reflection Framebuffer Stop 

		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);


		mainCamera->InvertCamera();

		viewMatrix = glm::mat4(1.0);
		viewMatrix = mainCamera->GetViewMatrix();

		// Draw on Refraction Framebuffer Start
		glBindFramebuffer(GL_FRAMEBUFFER, g_FrameBuffer_Refraction);
		glViewport(0, 0, giWinWidth, giWinHeight);
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f, 0.75f, 1.0f, 1.0f)));
		glClearBufferfv(GL_DEPTH, 0, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

		// Render SkyBox Here

		//g_pCubeMap->DrawCubemap(glm::mat4(glm::mat3(mainCamera->GetViewMatrix())), g_PersPectiveProjectionMatrix_SkyBox);
		//g_pCubeMap->DrawCubemap(pCameraCubeMap->GetViewMatrix(), g_PersPectiveProjectionMatrix_SkyBox);
		g_pCubeMap->DrawCubemap(viewMat_SkyBox, g_PersPectiveProjectionMatrix_SkyBox);

		modelMatrix = glm::mat4(1.0f);
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(g_v3CameraPosition.x, g_v3CameraPosition.y - g_pSkyDome->GetInnerRadius(), g_v3CameraPosition.z));
		modelMatrix = modelMatrix * translationMatrix;
		modelViewProjectionMatrix = g_PersPectiveProjectionMatrix_SkyBox * viewMatrix * modelMatrix;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		g_pSkyDome->RenderSky(g_v3CameraPosition, modelViewProjectionMatrix, false);
		glDisable(GL_BLEND);
		// Render terrain here
		modelMatrix = glm::mat4(1.0f);
		glUseProgram(g_pTerrain->GetShaderProgram());
		// set all light uniforms
		glUniform1f(g_UniformTerrain_XPixelOffset, 1.0f / DEPTH_TEXTURE_SIZE);
		glUniform1f(g_UniformTerrain_YPixelOffset, 1.0f / DEPTH_TEXTURE_SIZE);
		glUniform3fv(g_UniformTerrain_ViewPosition, 1, glm::value_ptr(mainCamera->GetCameraPosition()));
		glUniformMatrix4fv(g_UniformTerrain_shadowBias, 1, GL_FALSE, glm::value_ptr(ScaleBiasMatrix));
		glUniformMatrix4fv(g_UniformTerrain_lightprojrction_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightProjectionMatrix));
		glUniformMatrix4fv(g_UniformTerrain_lightView_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightViewMatrix));

		if (g_pSkyDome->IsNight())
		{
			glUniform3fv(g_UniformTerrain_sunAmbient, 1, glm::value_ptr(g_v3NightAmbient));
			glUniform3fv(g_UniformTerrain_sunSpecular, 1, glm::value_ptr(g_v3NightSpecular));
			glUniform3fv(g_UniformTerrain_sunDiffuse, 1, glm::value_ptr(g_v3NightDiffuse));
			glUniform3fv(g_UniformTerrain_sunDirection, 1, glm::value_ptr(g_v3NightDirection));
		}
		else
		{
			glUniform3fv(g_UniformTerrain_sunAmbient, 1, glm::value_ptr(g_pSunLight->getAmbient()));
			glUniform3fv(g_UniformTerrain_sunSpecular, 1, glm::value_ptr(g_pSunLight->getSpecular()));
			glUniform3fv(g_UniformTerrain_sunDiffuse, 1, glm::value_ptr(g_pSunLight->getDiffuse()));
			glUniform3fv(g_UniformTerrain_sunDirection, 1, glm::value_ptr(g_pSunLight->getDirection()));
		}
		glUniform4fv(g_UniformTerrain_ClipPlane, 1, glm::value_ptr(glm::vec4(0.0f, -1.0f, 0.0f, 15.0f))); // +ve for refraction

		g_pTerrain->RenderTerrain(modelMatrix, viewMatrix, g_pTerrain->m4TerrainPerspectiveProjectionMatrix, g_Texture_ShadowDepthTexure);

		glUseProgram(0);

		// Trees
		PalmStraight->UseProgram();
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(PalmStraight->GetProgram());
		}
		else
		{
			SetDayLight(PalmStraight->GetProgram());
		}
		//SetDayLight(PalmStraight->GetProgram());
		id = -1;

		id = glGetUniformLocation(PalmStraight->GetProgram(), "projection");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(PalmStraight->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamST, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		glDisable(GL_CULL_FACE);
		PalmStraight->Draw(g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glEnable(GL_CULL_FACE);
		glUseProgram(0);
		// Tilt trees

		PalmTilted->UseProgram();
		SetDayLight(PalmTilted->GetProgram());
		id = -1;

		id = glGetUniformLocation(PalmTilted->GetProgram(), "projection");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(PalmTilted->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamTILT, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		glDisable(GL_CULL_FACE);
		PalmTilted->Draw(g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glEnable(GL_CULL_FACE);

		glUseProgram(0);

		// Bent trees
		PalmBent->UseProgram();
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(PalmBent->GetProgram());
		}
		else
		{
			SetDayLight(PalmBent->GetProgram());
		}
		//SetDayLight(PalmBent->GetProgram());
		id = -1;

		id = glGetUniformLocation(PalmBent->GetProgram(), "projection");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(PalmBent->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamBEN, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		PalmBent->Draw(g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glUseProgram(0);

		//Lotus start
		//LotusLeaf_0
		modelMatrix = glm::mat4(1.0);
		rotationMatrix = glm::mat4(1.0);
		scaleMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0);

		glUseProgram(g_ShaderProgramObject_LotusLeaf);
		glUniformMatrix4fv(g_Uniform_LotusLeaf_ViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(g_Uniform_LotusLeaf_ProjectionMatrix, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_LotusLeaf_DiffuseTexture);
		glUniform1i(g_uniform_LotusLeaf_DiffuseMapSampler, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, g_LotusLeaf_BumpTexture);
		glUniform1i(g_uniform_LotusLeaf_BumpMapSampler, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, g_LotusLeaf_SpecularTexture);
		glUniform1i(g_uniform_LotusLeaf_SpecularMapSampler, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingLotusLeafs0, 0, NUM_INSTANCES_LOTUSLEAFS0 * sizeof(glm::mat4));

		glBindVertexArray(g_VertexArrayObject_LotusLeaf0);
		glDrawArraysInstanced(GL_TRIANGLES, 0, LOTUSLEAF0_SIZE, NUM_INSTANCES_LOTUSLEAFS0);
		glBindVertexArray(0);
		glUseProgram(0);

		//LotusFlower Start
		modelMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);


		glUseProgram(g_ShaderProgramObject_Model_LotusFlower);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(447.590820, 13.0f, 799.615112));
		rotationMatrix = glm::rotate(rotationMatrix, -190.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, 100.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1.5f));

		modelMatrix = modelMatrix * rotationMatrix * scaleMatrix;

		//glUniform3fv(g_Uniform_Model_lightPosition_animModel_LotusFlower, 1, lightPosition_animModel);
		glUniform3fv(g_Uniform_Model_ViewPosition_LotusFlower, 1, glm::value_ptr(mainCamera->GetCameraPosition()));

		glUniformMatrix4fv(g_Uniform_Model_Model_Matrix_LotusFlower, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(g_Uniform_Model_View_Matrix_LotusFlower, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(g_Uniform_Model_Projection_Matrix_LotusFlower, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));
		glUniform1i(g_Uniform_Model_Animation_LotusFlower, false);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingNiceLotus1, 0, NUM_INSTANCES_NICELOTUS1 * sizeof(glm::mat4));
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(g_ShaderProgramObject_Model_LotusFlower);
		}
		else
		{
			SetDayLight(g_ShaderProgramObject_Model_LotusFlower);
		}
		myNiceLotus1->drawInstanced(g_ShaderProgramObject_Model_LotusFlower, g_bAnimationControl, 0.0, NUM_INSTANCES_NICELOTUS1);
		glUseProgram(0);

		//LotusFlower Stop 
		//Lotus End

		// Render Grass Here
		modelMatrix = glm::mat4(1.0f);
		glDisable(GL_CULL_FACE);
		displayGrass(modelMatrix, viewMatrix, g_pTerrain->m4TerrainPerspectiveProjectionMatrix, g_pSunLight);
		glEnable(GL_CULL_FACE);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Draw on Refraction Framebuffer Stop 

		// def fb
		/*** Water Code Stop  ***/

		glViewport(0, 0, giWinWidth, giWinHeight);
		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		modelViewProjectionMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();
		/*		Occlusion FBO	Start	*/
		glBindFramebuffer(GL_FRAMEBUFFER, OcclusionFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		modelMatrix = glm::mat4(1.0f);
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix = modelMatrix * translationMatrix;
		glUseProgram(g_pTerrain->GetShaderProgram());
		glUniformMatrix4fv(g_UniformTerrain_shadowBias, 1, GL_FALSE, glm::value_ptr(ScaleBiasMatrix));
		glUniformMatrix4fv(g_UniformTerrain_lightprojrction_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightProjectionMatrix));
		glUniformMatrix4fv(g_UniformTerrain_lightView_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightViewMatrix));
		g_pTerrain->RenderTerrain(modelMatrix, viewMatrix, g_pTerrain->m4TerrainPerspectiveProjectionMatrix, g_Texture_ShadowDepthTexure);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/*		Occlusion FBO	Stop	*/

		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		modelViewProjectionMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();

		/*		MSAA Framebuffer Start		*/
		glBindFramebuffer(GL_FRAMEBUFFER, gFBOMSAA);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (SceneCounter == 0)
		{
			modelMatrix = glm::mat4(1.0f);

			glLineWidth(2.0f);

			glUseProgram(lineProgam);

			glUniformMatrix4fv(model_line_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(view_line_uniform, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(perspective_line_uniform, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));

			glBindVertexArray(vao_sunrise_sunflower);

			glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)));
			glDrawArrays(GL_LINES, 0, 2);

			glLineWidth(5.0f);
			glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
			glDrawArrays(GL_LINES, 2, 2);

			glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
			glDrawArrays(GL_LINES, 4, 2);

			glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
			glDrawArrays(GL_LINES, 6, 2);

			glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
			glDrawArrays(GL_LINES, 8, 2);

			glLineWidth(1.0f);

			glBindVertexArray(0);

			glBindVertexArray(vao_bee_parabola);

			glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)));
			for (int i = 0; i < bee_parabola.size() - 2; i++)
			{
				glDrawArrays(GL_LINES, i, 2);
			}

			glBindVertexArray(0);


			glBindVertexArray(vao_cam_to_bee_control_points);
			glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)));
			glDrawArrays(GL_LINES, 0, 2);
			glDrawArrays(GL_LINES, 1, 2);
			glDrawArrays(GL_LINES, 2, 2);
			glPointSize(10.0f);
			glDrawArrays(GL_POINTS, 4, 1);
			glPointSize(1.0f);
			glBindVertexArray(0);

			glBindVertexArray(vao_cam_to_bee_curve);

			for (int i = 0; i < cam_to_bee_curve.size() - 2; i++)
			{
				if (i < 200)
				{
					glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
				}
				else if (i < 500)
				{
					glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
				}
				else if (i < 750)
				{
					glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
				}
				else
				{
					glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
				}

				glDrawArrays(GL_LINES, i, 2);
			}

			glBindVertexArray(0);


			glBindVertexArray(vao_bee_to_lake_control_points);
			glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)));
			glDrawArrays(GL_LINES, 0, 2);
			glDrawArrays(GL_LINES, 1, 2);
			glDrawArrays(GL_LINES, 2, 2);
			glBindVertexArray(0);

			glBindVertexArray(vao_bee_to_lake_curve);
			for (int i = 0; i < bee_to_lake_curve.size() - 2; i++)
			{
				if (i < 200)
				{
					glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
				}
				else if (i < 500)
				{
					glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
				}
				else if (i < 750)
				{
					glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
				}
				else
				{
					glVertexAttrib4fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
				}

				glDrawArrays(GL_LINES, i, 2);
			}
			glBindVertexArray(0);

			glUseProgram(0);
		}

		modelMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 300.0f + g_pTerrain->GetHeightAt(glm::vec3(0.0f, 0.0f, 0.0f)), 0.0f));
		modelMatrix = modelMatrix * translationMatrix;

		if (g_bShowButterFlies)
		{
			glDisable(GL_CULL_FACE);
			g_pFlockingButterfiles->RenderFlockingButterflies(modelMatrix, viewMatrix, g_pTerrain->m4TerrainPerspectiveProjectionMatrix);
			glEnable(GL_CULL_FACE);
		}


		//g_pCubeMap->DrawCubemap(glm::mat4(glm::mat3(mainCamera->GetViewMatrix())), g_PersPectiveProjectionMatrix_SkyBox);
		//g_pCubeMap->DrawCubemap(pCameraCubeMap->GetViewMatrix(), g_PersPectiveProjectionMatrix_SkyBox);
		g_pCubeMap->DrawCubemap(viewMat_SkyBox, g_PersPectiveProjectionMatrix_SkyBox);

		modelMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(g_v3CameraPosition.x, g_v3CameraPosition.y - g_pSkyDome->GetInnerRadius(), g_v3CameraPosition.z));
		modelMatrix = modelMatrix * translationMatrix;
		modelViewProjectionMatrix = g_PersPectiveProjectionMatrix_SkyBox * viewMatrix * modelMatrix;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		g_pSkyDome->RenderSky(g_v3CameraPosition, modelViewProjectionMatrix, false);
		glDisable(GL_BLEND);

		modelMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();

		glUseProgram(g_pTerrain->GetShaderProgram());
		// set all light uniforms
		glUniform1f(g_UniformTerrain_XPixelOffset, 1.0f / DEPTH_TEXTURE_SIZE);
		glUniform1f(g_UniformTerrain_YPixelOffset, 1.0f / DEPTH_TEXTURE_SIZE);

		glUniform3fv(g_UniformTerrain_ViewPosition, 1, glm::value_ptr(mainCamera->GetCameraPosition()));
		glUniformMatrix4fv(g_UniformTerrain_shadowBias, 1, GL_FALSE, glm::value_ptr(ScaleBiasMatrix));
		glUniformMatrix4fv(g_UniformTerrain_lightprojrction_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightProjectionMatrix));
		glUniformMatrix4fv(g_UniformTerrain_lightView_Matrix, 1, GL_FALSE, glm::value_ptr(g_m4LightViewMatrix));

		if (g_pSkyDome->IsNight())
		{
			glUniform3fv(g_UniformTerrain_sunAmbient, 1, glm::value_ptr(g_v3NightAmbient));
			glUniform3fv(g_UniformTerrain_sunSpecular, 1, glm::value_ptr(g_v3NightSpecular));
			glUniform3fv(g_UniformTerrain_sunDiffuse, 1, glm::value_ptr(g_v3NightDiffuse));
			glUniform3fv(g_UniformTerrain_sunDirection, 1, glm::value_ptr(g_v3NightDirection));
		}
		else
		{
			glUniform3fv(g_UniformTerrain_sunAmbient, 1, glm::value_ptr(g_pSunLight->getAmbient()));
			glUniform3fv(g_UniformTerrain_sunSpecular, 1, glm::value_ptr(g_pSunLight->getSpecular()));
			glUniform3fv(g_UniformTerrain_sunDiffuse, 1, glm::value_ptr(g_pSunLight->getDiffuse()));
			glUniform3fv(g_UniformTerrain_sunDirection, 1, glm::value_ptr(g_pSunLight->getDirection()));
		}

		/*glUniform3f(g_UniformTerrain_sunAmbient, 1.0f, 1.0f, 1.0f);
		glUniform3f(g_UniformTerrain_sunSpecular, 1.0f, 1.0f, 1.0f);
		glUniform3f(g_UniformTerrain_sunDiffuse, 1.0f, 1.0f, 1.0f);
		glUniform3f(g_UniformTerrain_sunDirection, 100.0f, 900.0f, 0.0f);*/

		g_pTerrain->RenderTerrain(modelMatrix, viewMatrix, g_pTerrain->m4TerrainPerspectiveProjectionMatrix, g_Texture_ShadowDepthTexure);


		glUseProgram(0);


		glm::mat4 viewMatrixTree = glm::mat4(1.0f);
		viewMatrixTree = viewMatrix;

		// ****** Draw Palm Tree *****
		// straight trees

		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();

		PalmStraight->UseProgram();
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(PalmStraight->GetProgram());
		}
		else
		{
			SetDayLight(PalmStraight->GetProgram());
		}
		//SetDayLight(PalmStraight->GetProgram());
		id = -1;

		id = glGetUniformLocation(PalmStraight->GetProgram(), "projection");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(PalmStraight->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamST, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		glDisable(GL_CULL_FACE);
		PalmStraight->Draw(g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glEnable(GL_CULL_FACE);
		glUseProgram(0);
		// Tilt trees
		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();

		PalmTilted->UseProgram();
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(PalmTilted->GetProgram());
		}
		else
		{
			SetDayLight(PalmTilted->GetProgram());
		}
		//SetDayLight(PalmTilted->GetProgram());
		id = -1;

		id = glGetUniformLocation(PalmTilted->GetProgram(), "projection");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(PalmTilted->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamTILT, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		glDisable(GL_CULL_FACE);
		PalmTilted->Draw(g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glEnable(GL_CULL_FACE);
		glUseProgram(0);

		// Bent trees
		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();

		PalmBent->UseProgram();
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(PalmBent->GetProgram());
		}
		else
		{
			SetDayLight(PalmBent->GetProgram());
		}
		//SetDayLight(PalmBent->GetProgram());
		id = -1;

		id = glGetUniformLocation(PalmBent->GetProgram(), "projection");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(PalmBent->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));


		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingPlamBEN, 0, NUM_INSTANCES_PALMTREE * sizeof(glm::mat4));
		glDisable(GL_CULL_FACE);
		PalmBent->Draw(g_DeltaTime, NUM_INSTANCES_PALMTREE);
		glEnable(GL_CULL_FACE);
		glUseProgram(0);


		//  Render call for marigold
		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();

		Marigold->UseProgram();
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(Marigold->GetProgram());
		}
		else
		{
			SetDayLight(Marigold->GetProgram());
		}
		//SetDayLight(Marigold->GetProgram());
		id = -1;

		id = glGetUniformLocation(Marigold->GetProgram(), "projection");
		//glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(g_pTerrain->m4TerrainPerspectiveProjectionMatrix));
		id = glGetUniformLocation(Marigold->GetProgram(), "view");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		id = glGetUniformLocation(Marigold->GetProgram(), "model");
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingMarigold, 0, NUM_INSTANCES_MARIGOLD * sizeof(glm::mat4));
		Marigold->Draw(g_DeltaTime, NUM_INSTANCES_MARIGOLD);
		glUseProgram(0);

		/*** Animated Model Loading Start ***/

		/*Sunflower Stat */
		modelMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0);
		rotationMatrix = glm::mat4(1.0);
		scaleMatrix = glm::mat4(1.0);
		viewMatrix = mainCamera->GetViewMatrix();

		glUseProgram(g_ShaderProgramObject_Model_Sunflower);


		glUniform3fv(g_Uniform_Model_LD_Sunflower, 1, lightDiffuse_animModel);
		glUniform3fv(g_Uniform_Model_LA_Sunflower, 1, lightAmbient_animModel);
		glUniform3fv(g_Uniform_Model_LS_Sunflower, 1, lightSpecular_animModel);

		glUniform3fv(g_Uniform_Model_lightPosition_animModel_Sunflower, 1, glm::value_ptr(g_pSunLight->getDirection()));
		glUniform3fv(g_Uniform_Model_ViewPosition_Sunflower, 1, glm::value_ptr(mainCamera->GetCameraPosition()));

		glUniform1f(g_Uniform_Model_MaterialShininess_Sunflower, material_shininess_animModel);

		//glUniformMatrix4fv(g_Uniform_Model_Model_Matrix_Sunflower, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(g_Uniform_Model_View_Matrix_Sunflower, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(g_Uniform_Model_Projection_Matrix_Sunflower, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));

		glUniform1i(g_Uniform_Model_Animation_Sunflower, true);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingSunFlower, 0, NUM_INSTANCES_SUNFLOWER * sizeof(glm::mat4));
		mySunFlower->drawInstanced(g_ShaderProgramObject_Model_Sunflower, g_bAnimationControl, g_dSunFlowerAnimateTime, NUM_INSTANCES_SUNFLOWER);

		glUseProgram(0);
		/*Sunflower Stop */

		/********** Particle System Code Start    ************/
		modelMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);

		if (g_pSkyDome->IsNight() && (g_universalTimer > (fireFly_waitTime + 0.005f)))
		{
			modelMatrix = glm::mat4(1.0);
			viewMatrix = glm::mat4(1.0);
			rotationMatrix = glm::mat4(1.0);
			scaleMatrix = glm::mat4(1.0);
			viewMatrix = mainCamera->GetViewMatrix();

			modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(254.070923, 78.639999, 719.125366));
			//modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(435.067078, 60.443657, 484.508911));

			fprintf_s(g_pFile, "In side FireFlies\n");

			// Right Side
			glUseProgram(g_ShaderProgramObject_Particle1);

			glBindVertexArray(g_VertexArrayObject_ParticleInv);

			glUniformMatrix4fv(g_Uniform_ModelMatrix_Particle1, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(g_Uniform_ViewMatrix_Particle1, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(g_Uniform_ProjectionMatrix_Particle1, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));
			glUniform1f(g_Uniform_Time_Particle1, g_pfParticleMoveTime_Particle1);
			glUniform1f(g_Uniform_Anime_Particle1, g_pfParticleAnimeTime_Particle1);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDrawArrays(GL_POINTS, 0, g_arrayWidth_Particle * g_arrayWidth_Particle);
			glDisable(GL_BLEND);
			glBindVertexArray(0);
			glUseProgram(0);

			modelMatrix = glm::mat4(1.0);
			viewMatrix = glm::mat4(1.0);
			rotationMatrix = glm::mat4(1.0);
			scaleMatrix = glm::mat4(1.0);
			viewMatrix = mainCamera->GetViewMatrix();

			modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(369.396118, 65.747421, 613.944824));

			glUseProgram(g_ShaderProgramObject_Particle2);

			glBindVertexArray(g_VertexArrayObject_Particle);

			glUniformMatrix4fv(g_Uniform_ModelMatrix_Particle2, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(g_Uniform_ViewMatrix_Particle2, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(g_Uniform_ProjectionMatrix_Particle2, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));
			glUniform1f(g_Uniform_Time_Particle1, g_pfParticleMoveTime_Particle2);
			glUniform1f(g_Uniform_Anime_Particle1, g_pfParticleAnimeTime_Particle3);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDrawArrays(GL_POINTS, 0, g_arrayWidth_Particle * g_arrayWidth_Particle);
			glDisable(GL_BLEND);
			glBindVertexArray(0);
			glUseProgram(0);
			

			// Left Side
			modelMatrix = glm::mat4(1.0);
			viewMatrix = glm::mat4(1.0);
			rotationMatrix = glm::mat4(1.0);
			scaleMatrix = glm::mat4(1.0);

			viewMatrix = mainCamera->GetViewMatrix();

			modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(457.473816, 55.024982, 370.469208));
			//modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(329.822357, 75.113464, 758.139465));

			glUseProgram(g_ShaderProgramObject_Particle1);

			glBindVertexArray(g_VertexArrayObject_ParticleInv);

			glUniformMatrix4fv(g_Uniform_ModelMatrix_Particle1, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(g_Uniform_ViewMatrix_Particle1, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(g_Uniform_ProjectionMatrix_Particle1, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));
			glUniform1f(g_Uniform_Time_Particle1, g_pfParticleMoveTime_Particle1);
			glUniform1f(g_Uniform_Anime_Particle1, g_pfParticleAnimeTime_Particle2);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDrawArrays(GL_POINTS, 0, g_arrayWidth_Particle * g_arrayWidth_Particle);
			glDisable(GL_BLEND);
			glBindVertexArray(0);
			glUseProgram(0);


			modelMatrix = glm::mat4(1.0);
			viewMatrix = glm::mat4(1.0);
			rotationMatrix = glm::mat4(1.0);
			scaleMatrix = glm::mat4(1.0);

			viewMatrix = mainCamera->GetViewMatrix();

			modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(374.932495, 57.778748, 793.263428));

			glUseProgram(g_ShaderProgramObject_Particle2);

			glBindVertexArray(g_VertexArrayObject_ParticleInv);

			glUniformMatrix4fv(g_Uniform_ModelMatrix_Particle2, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(g_Uniform_ViewMatrix_Particle2, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(g_Uniform_ProjectionMatrix_Particle2, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));
			glUniform1f(g_Uniform_Time_Particle1, g_pfParticleMoveTime_Particle2);
			glUniform1f(g_Uniform_Anime_Particle1, g_pfParticleAnimeTime_Particle4);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDrawArrays(GL_POINTS, 0, g_arrayWidth_Particle * g_arrayWidth_Particle);
			glDisable(GL_BLEND);
			glBindVertexArray(0);
			glUseProgram(0);
		}
		else if (!g_pSkyDome->IsNight())
		{
			fireFly_waitTime = g_universalTimer;
		}
		/********** Particle Code Stop     ************/

		/*** Animated Model Loading Stop  ***/
		modelMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0);
		rotationMatrix = glm::mat4(1.0);
		scaleMatrix = glm::mat4(1.0);
		viewMatrix = mainCamera->GetViewMatrix();
		glUseProgram(g_ShaderProgramObject_Model_Bee);


		glUniform3fv(g_Uniform_Model_LD_Bee, 1, lightDiffuse_animModel);
		glUniform3fv(g_Uniform_Model_LA_Bee, 1, lightAmbient_animModel);
		glUniform3fv(g_Uniform_Model_LS_Bee, 1, lightSpecular_animModel);

		glUniform3fv(g_Uniform_Model_lightPosition_animModel_Bee, 1, lightPosition_animModel);
		glUniform3fv(g_Uniform_Model_ViewPosition_Bee, 1, glm::value_ptr(mainCamera->GetCameraPosition()));

		glUniform1f(g_Uniform_Model_MaterialShininess_Bee, material_shininess_animModel);

		scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0));
		translationMatrix = glm::translate(glm::mat4(1.0f), bee_pos_parabola);
		//translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(447.590820, 28.0, 799.615112));
		rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(300.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(-bee_pos_parabola_angle_degrees), glm::vec3(0.0f, 1.0f, 0.0f));

		modelMatrix = modelMatrix * translationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		modelMatrix = modelMatrix * rotationMatrix;

		glUniformMatrix4fv(g_Uniform_Model_Model_Matrix_Bee, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(g_Uniform_Model_View_Matrix_Bee, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(g_Uniform_Model_Projection_Matrix_Bee, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));

		glUniform1i(g_Uniform_Model_Animation_Bee, true);

		myHoneyBee->draw(g_ShaderProgramObject_Model_Bee, g_bAnimationControl, g_dBeeAnimateTime);

		glUseProgram(0);


		//Lotus start
		//LotusLeaf_0
		modelMatrix = glm::mat4(1.0);
		viewMatrix = glm::mat4(1.0);
		rotationMatrix = glm::mat4(1.0);
		scaleMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0);

		viewMatrix = mainCamera->GetViewMatrix();

		glUseProgram(g_ShaderProgramObject_LotusLeaf);
		//glUniformMatrix4fv(g_Uniform_LotusLeaf_ModelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(g_Uniform_LotusLeaf_ViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(g_Uniform_LotusLeaf_ProjectionMatrix, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_LotusLeaf_DiffuseTexture);
		glUniform1i(g_uniform_LotusLeaf_DiffuseMapSampler, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, g_LotusLeaf_BumpTexture);
		glUniform1i(g_uniform_LotusLeaf_BumpMapSampler, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, g_LotusLeaf_SpecularTexture);
		glUniform1i(g_uniform_LotusLeaf_SpecularMapSampler, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingLotusLeafs0, 0, NUM_INSTANCES_LOTUSLEAFS0 * sizeof(glm::mat4));

		glBindVertexArray(g_VertexArrayObject_LotusLeaf0);
		glDrawArraysInstanced(GL_TRIANGLES, 0, LOTUSLEAF0_SIZE, NUM_INSTANCES_LOTUSLEAFS0);
		glBindVertexArray(0);
		glUseProgram(0);


		//modelMatrix = glm::translate(modelMatrix, glm::vec3(-311.753815f, 13.0f, 344.981079f)); // flower1,y=13.0f
		//modelMatrix = glm::translate(modelMatrix, glm::vec3(-311.753815f, 14.25f, 344.981079f)); // flower0,y=14.25f
		//modelMatrix = glm::translate(modelMatrix, glm::vec3(-311.753815f, 14.2f, 344.981079f));   // flower2, y=14.2f

		//LotusFlower Start
		modelMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();

		glUseProgram(g_ShaderProgramObject_Model_LotusFlower);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(447.590820, 13.0f, 799.615112));
		rotationMatrix = glm::rotate(rotationMatrix, -190.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, 100.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1.5f));

		modelMatrix = modelMatrix * rotationMatrix * scaleMatrix;

		//glUniform3fv(g_Uniform_Model_lightPosition_animModel_LotusFlower, 1, lightPosition_animModel);
		glUniform3fv(g_Uniform_Model_ViewPosition_LotusFlower, 1, glm::value_ptr(mainCamera->GetCameraPosition()));
		//glUniform3fv(g_Uniform_Model_KD_LotusFlower, 1, material_diffuse_animModel);
		//glUniform3fv(g_Uniform_Model_KA_LotusFlower, 1, material_ambient_animModel);
		//glUniform3fv(g_Uniform_Model_KS_LotusFlower, 1, material_specular_animModel);
		//glUniform1f(g_Uniform_Model_MaterialShininess_LotusFlower, material_shininess_animModel);

		glUniformMatrix4fv(g_Uniform_Model_Model_Matrix_LotusFlower, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(g_Uniform_Model_View_Matrix_LotusFlower, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(g_Uniform_Model_Projection_Matrix_LotusFlower, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));
		glUniform1i(g_Uniform_Model_Animation_LotusFlower, false);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_UBO_InstancingNiceLotus1, 0, NUM_INSTANCES_NICELOTUS1 * sizeof(glm::mat4));
		if (g_pSkyDome->IsNight())
		{
			SetNightLight(g_ShaderProgramObject_Model_LotusFlower);
		}
		else
		{
			SetDayLight(g_ShaderProgramObject_Model_LotusFlower);
		}
		myNiceLotus1->drawInstanced(g_ShaderProgramObject_Model_LotusFlower, g_bAnimationControl, 0.0, NUM_INSTANCES_NICELOTUS1);
		glUseProgram(0);

		//LotusFlower Stop 
		//Lotus End
		/*** Animated Model Loading Stop  ***/

		/*** Water Render (Quad) Code Start ***/
		modelMatrix = glm::mat4(1.0);
		translationMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();


		//modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 100.0f, 0.0));
		//scaleMatrix = glm::scale(glm::vec3(10.0f, 1.0f, 10.0f));
		modelMatrix = modelMatrix * scaleMatrix;

		glUseProgram(g_ShaderProgramObject_Water);

		// Set all textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_ColorTexture_Reflection);
		glUniform1i(g_uniform_TextureSampler_FB_Reflection, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, g_ColorTexture_Refraction);
		glUniform1i(g_uniform_TextureSampler_FB_Refraction, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, g_Tex_WaterDuDv);
		glUniform1i(g_uniform_TextureSampler_FB_DuDv, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, g_Tex_WaterNormal);
		glUniform1i(g_uniform_TextureSampler_FB_Normal, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, g_DepthTexture_Refraction);
		glUniform1i(g_uniform_TextureSampler_FB_Depth, 4);

		glUniformMatrix4fv(g_Uniform_Model_Matrix_Water, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(g_Uniform_View_Matrix_Water, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(g_Uniform_Projection_Matrix_Water, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));

		glUniform1f(g_uniform_MoveFactor_Water, g_fWaterMoveFactor);
		glUniform3fv(g_uniform_CamPos_Water, 1, glm::value_ptr(mainCamera->GetCameraPosition()));
		glUniform3fv(g_uniform_LightPosition_Water, 1, glm::value_ptr(g_pSunLight->getDirection())); // Sun Position
		glUniform3fv(g_uniform_LightColor_Water, 1, glm::value_ptr(g_pSunLight->getDiffuse())); // Sun Color

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindVertexArray(g_VertexArrayObject_Water);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glDisable(GL_BLEND);

		glUseProgram(0);
		/*** Water Render (Quad) Code Stop  ***/
		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		translationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		viewMatrix = mainCamera->GetViewMatrix();


		glDisable(GL_CULL_FACE);
		displayGrass(modelMatrix, viewMatrix, g_pTerrain->m4TerrainPerspectiveProjectionMatrix, g_pSunLight);
		glEnable(GL_CULL_FACE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gFBOMSAA);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gFrameBufferObjectHDR);
		glBlitFramebuffer(0, 0, giWinWidth, giWinHeight, 0, 0, giWinWidth, giWinHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBlitFramebuffer(0, 0, giWinWidth, giWinHeight, 0, 0, giWinWidth, giWinHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBlitFramebuffer(0, 0, giWinWidth, giWinHeight, 0, 0, giWinWidth, giWinHeight, GL_STENCIL_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Display End Credits
		if (SceneCounter == 9)
		{
			pCredits->displayEndCredits();
			pCredits->update();
		}

		// Display the HDR
		glUseProgram(gShaderObjectHDR);
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, gColorBufferTexture);
		glUniform1i(glGetUniformLocation(gShaderObjectHDR, "HDRTexture"), 0);
		glUniform1f(glGetUniformLocation(gShaderObjectHDR, "u_AlphaVal"), g_fHDRAlphaValue);
		glUniform1f(glGetUniformLocation(gShaderObjectHDR, "exposure"), fExposure);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		RenderQuadHDR();
		glDisable(GL_BLEND);
		glUseProgram(0);
		/*		MSAA Framebuffer Stop 		*/

		// skydome
		if (g_bSunIsRunning)
		{
			g_pSkyDome->UpdateSky(0.0f);
		}


	}
	else
	{
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.46f, 0.16f, 0.32f, 1.0f)));
		glClearBufferfv(GL_DEPTH, 0, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

		// Splash Screen

		if (pCredits != NULL)
		{
			pCredits->displayStartCredits();
		}
	}


	SwapBuffers(g_hdc);
	// For Camera
	SetCursorPos((int)(g_fCurrrentWidth / 2), (int)(g_fCurrrentHeight / 2));
}

void FullScreen(void)
{
	MONITORINFO mi = { sizeof(mi) };
	dwStyle = GetWindowLong(g_hwnd, GWL_STYLE);
	if (g_bFullScreen == false)
	{
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(g_hwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(g_hwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(g_hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(g_hwnd, HWND_TOP,
					mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		g_bFullScreen = true;
	}
	else
	{
		SetWindowLong(g_hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(g_hwnd, &wpPrev);
		SetWindowPos(g_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		ShowCursor(TRUE);
		g_bFullScreen = false;
	}
}

bool Resize(int iWidth, int iHeight)
{

	glm::mat4 SetInfiniteProjectionMatrix(GLfloat Left, GLfloat Right, GLfloat Bottom, GLfloat Top, GLfloat Near, GLfloat Far);

	// For SkyBox
	GLfloat gfFOVY = 45.0f;
	GLfloat fLeft = 0.0f;
	GLfloat fRight = 0.0f;
	GLfloat fBottom = 0.0f;
	GLfloat fTop = 0.0f;
	GLfloat fNear = FRUSTUM_NEAR;
	GLfloat fFar = FRUSTUM_FAR;
	GLfloat aspectRatio = (GLfloat)iWidth / (GLfloat)iHeight;

	if (iHeight <= 0)
	{
		iHeight = 1;
	}

	glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight);

	g_PersPectiveProjectionMatrix = glm::perspective(45.0f, (float)iWidth / (float)iHeight, 0.1f, 100000.0f);

	g_TrianglePerspectiveProjectionMatrix = glm::perspective(45.0f, (float)iWidth / (float)iHeight, 0.1f, 100000.0f);

	if (g_pTerrain)
	{
		g_pTerrain->SetProjection(g_PersPectiveProjectionMatrix);
	}

	fTop = fNear * (GLfloat)tan(gfFOVY / 360.0 * glm::pi<float>());
	fRight = fTop * ((float)iWidth / (float)iHeight);

	fBottom = -fTop;
	fLeft = -fTop * ((float)iWidth / (float)iHeight);

	g_PersPectiveProjectionMatrix_SkyBox = SetInfiniteProjectionMatrix(fLeft, fRight, fBottom, fTop, fNear, fFar);


	if (pCredits != NULL)
	{
		pCredits->resize(iWidth, iHeight);
	}


	if (gFBOMSAA)
	{
		/*glDeleteTextures(1, &g_ColorTexture_Reflection);
		g_ColorTexture_Reflection = 0;

		glDeleteTextures(1, &g_DepthTexture_Reflection);
		g_DepthTexture_Reflection = 0;

		glDeleteFramebuffers(1, &gFBOMSAA);
		gFBOMSAA = 0;*/



		// resize msaa framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, gFBOMSAA);

		// create a multisampled color attachment
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gTextureMSAA);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, giWinWidth, giWinHeight, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, gTextureMSAA, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, gRBOMSAA);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, giWinWidth, giWinHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gRBOMSAA);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// resize hdr framebuffer
		glBindTexture(GL_TEXTURE_2D, gColorBufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, giWinWidth, giWinHeight, 0, GL_RGBA, GL_FLOAT, NULL);

		// create depth buffer (renderbuffer)
		glBindRenderbuffer(GL_RENDERBUFFER, gRenderBufferObjectDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, giWinWidth, giWinHeight);

		// attach buffers
		glBindFramebuffer(GL_FRAMEBUFFER, gFrameBufferObjectHDR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gColorBufferTexture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gRenderBufferObjectDepth);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	if (g_FrameBuffer_Reflection)
	{
		glDeleteTextures(1, &g_ColorTexture_Reflection);
		g_ColorTexture_Reflection = 0;

		glDeleteTextures(1, &g_DepthTexture_Reflection);
		g_DepthTexture_Reflection = 0;

		glDeleteFramebuffers(1, &g_FrameBuffer_Reflection);
		g_FrameBuffer_Reflection = 0;


		glGenFramebuffers(1, &g_FrameBuffer_Reflection);
		glBindFramebuffer(GL_FRAMEBUFFER, g_FrameBuffer_Reflection);


		glGenTextures(1, &g_ColorTexture_Reflection);
		glBindTexture(GL_TEXTURE_2D, g_ColorTexture_Reflection);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, iWidth, iHeight);
		glBindTexture(GL_TEXTURE_2D, 0);


		glGenTextures(1, &g_DepthTexture_Reflection);
		glBindTexture(GL_TEXTURE_2D, g_DepthTexture_Reflection);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, iWidth, iHeight);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_ColorTexture_Reflection, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, g_DepthTexture_Reflection, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDrawBuffers(2, g_DrawBuffers);
	}

	// Refraction
	if (g_FrameBuffer_Refraction)
	{
		glDeleteTextures(1, &g_ColorTexture_Refraction);
		g_ColorTexture_Refraction = 0;

		glDeleteTextures(1, &g_DepthTexture_Refraction);
		g_DepthTexture_Refraction = 0;

		glDeleteFramebuffers(1, &g_FrameBuffer_Refraction);
		g_FrameBuffer_Refraction = 0;


		glGenFramebuffers(1, &g_FrameBuffer_Refraction);
		glBindFramebuffer(GL_FRAMEBUFFER, g_FrameBuffer_Refraction);


		glGenTextures(1, &g_ColorTexture_Refraction);
		glBindTexture(GL_TEXTURE_2D, g_ColorTexture_Refraction);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, iWidth, iHeight);
		glBindTexture(GL_TEXTURE_2D, 0);


		glGenTextures(1, &g_DepthTexture_Refraction);
		glBindTexture(GL_TEXTURE_2D, g_DepthTexture_Refraction);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, iWidth, iHeight);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_ColorTexture_Refraction, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, g_DepthTexture_Refraction, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDrawBuffers(2, g_DrawBuffers);
	}

	return true;
}


glm::mat4 SetInfiniteProjectionMatrix(GLfloat Left, GLfloat Right, GLfloat Bottom, GLfloat Top, GLfloat Near, GLfloat Far)
{
	glm::mat4 result(glm::mat4(1.0f));

	if ((Right == Left) || (Top == Bottom) || (Near == Far) || (Near < 0.0) || (Far < 0.0))
		return result;

	result[0][0] = (2.0f * Near) / (Right - Left);
	result[1][1] = (2.0f * Near) / (Top - Bottom);

	result[2][0] = (Right + Left) / (Right - Left);
	result[2][1] = (Top + Bottom) / (Top - Bottom);
	//result[2][2] = -(Far + Near) / (Far - Near);
	result[2][2] = -1.0f;
	result[2][3] = -1.0f;

	//result[3][2] = -(2.0f * Far * Near) / (Far - Near);
	result[3][2] = -(2.0f * Near);
	result[3][3] = 0.0f;

	return result;
}



void InitializeQuadHDR(void)
{
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	// setup plane VAO
	glGenVertexArrays(1, &gVAOHDRQuad);
	glBindVertexArray(gVAOHDRQuad);

	glGenBuffers(1, &gVBOHDR);
	glBindBuffer(GL_ARRAY_BUFFER, gVBOHDR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
}

void RenderQuadHDR(void)
{
	glBindVertexArray(gVAOHDRQuad);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}



int UnInitialize(void)
{

	if (g_bFullScreen == true)
	{
		SetWindowLong(g_hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(g_hwnd, &wpPrev);
		SetWindowPos(g_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		ShowCursor(TRUE);
		g_bFullScreen = false;
	}


	if (g_UBO_InstancingPlamST)
	{
		glDeleteBuffers(1, &g_UBO_InstancingPlamST);
		g_UBO_InstancingPlamST = 0;
	}

	if (g_UBO_InstancingPlamTILT)
	{
		glDeleteBuffers(1, &g_UBO_InstancingPlamTILT);
		g_UBO_InstancingPlamTILT = 0;
	}

	if (g_UBO_InstancingPlamBEN)
	{
		glDeleteBuffers(1, &g_UBO_InstancingPlamBEN);
		g_UBO_InstancingPlamBEN = 0;
	}


	if (g_Texture_ShadowDepthTexure)
	{
		glDeleteTextures(1, &g_Texture_ShadowDepthTexure);
		g_Texture_ShadowDepthTexure = 0;
	}

	if (g_ShadowFramebuffer)
	{
		glDeleteFramebuffers(1, &g_ShadowFramebuffer);
		g_ShadowFramebuffer = 0;
	}

	if (gTextureMSAA)
	{
		glDeleteTextures(1, &gTextureMSAA);
		gTextureMSAA = NULL;
	}

	if (gRBOMSAA)
	{
		glDeleteRenderbuffers(1, &gRBOMSAA);
		gRBOMSAA = NULL;
	}

	if (gFBOMSAA)
	{
		glDeleteFramebuffers(1, &gFBOMSAA);
		gFBOMSAA = NULL;
	}

	if (OcclusionFBO)
	{
		glDeleteFramebuffers(1, &OcclusionFBO);
		OcclusionFBO = NULL;
	}

	if (OcclusionMapTexture)
	{
		glDeleteTextures(1, &OcclusionMapTexture);
		OcclusionMapTexture = NULL;
	}

	if (gRenderBufferObjectDepth)
	{
		glDeleteRenderbuffers(1, &gRenderBufferObjectDepth);
		gRenderBufferObjectDepth = NULL;
	}

	if (gColorBufferTexture)
	{
		glDeleteTextures(1, &gColorBufferTexture);
		gColorBufferTexture = NULL;
	}

	if (gFrameBufferObjectHDR)
	{
		glDeleteFramebuffers(1, &gFrameBufferObjectHDR);
		gFrameBufferObjectHDR = NULL;
	}

	// gShaderProgramObject_SkyDome
	glUseProgram(0);
	if (gShaderProgramObject_SkyDome)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;
		glUseProgram(gShaderProgramObject_SkyDome);
		glGetProgramiv(gShaderProgramObject_SkyDome, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint* pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject_SkyDome, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(gShaderProgramObject_SkyDome, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(gShaderProgramObject_SkyDome);
		gShaderProgramObject_SkyDome = NULL;
		glUseProgram(0);
	}

	//gShaderObjectHDR
	if (gShaderObjectHDR)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;
		glUseProgram(gShaderObjectHDR);
		glGetProgramiv(gShaderObjectHDR, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint* pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderObjectHDR, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(gShaderObjectHDR, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(gShaderObjectHDR);
		gShaderObjectHDR = NULL;
		glUseProgram(0);
	}

	CleanUpTriangle();
	CleanUpPoints();
	unInitializeGrass();

	/*** Water Code start ***/
	// Normal Map
	if (g_Tex_WaterNormal)
	{
		glDeleteTextures(1, &g_Tex_WaterNormal);
		g_Tex_WaterNormal = NULL;
	}

	// DuDv Map
	if (g_Tex_WaterDuDv)
	{
		glDeleteTextures(1, &g_Tex_WaterDuDv);
		g_Tex_WaterDuDv = NULL;
	}

	// reflection buffer
	if (g_ColorTexture_Reflection)
	{
		glDeleteTextures(1, &g_ColorTexture_Reflection);
		g_ColorTexture_Reflection = 0;
	}

	if (g_DepthTexture_Reflection)
	{
		glDeleteTextures(1, &g_DepthTexture_Reflection);
		g_DepthTexture_Reflection = 0;
	}

	if (g_FrameBuffer_Reflection)
	{
		glDeleteFramebuffers(1, &g_FrameBuffer_Reflection);
		g_FrameBuffer_Reflection = 0;
	}

	// refraction Buffer
	if (g_ColorTexture_Refraction)
	{
		glDeleteTextures(1, &g_ColorTexture_Refraction);
		g_ColorTexture_Refraction = 0;
	}

	if (g_DepthTexture_Refraction)
	{
		glDeleteTextures(1, &g_DepthTexture_Refraction);
		g_DepthTexture_Refraction = 0;
	}

	if (g_FrameBuffer_Refraction)
	{
		glDeleteFramebuffers(1, &g_FrameBuffer_Refraction);
		g_FrameBuffer_Refraction = 0;
	}

	//g_ShaderProgramObject_Water
	if (g_ShaderProgramObject_Water)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;
		glUseProgram(g_ShaderProgramObject_Water);
		glGetProgramiv(g_ShaderProgramObject_Water, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint* pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(g_ShaderProgramObject_Water, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ShaderProgramObject_Water, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(g_ShaderProgramObject_Water);
		gShaderObjectHDR = NULL;
		glUseProgram(0);
	}
	/*** Water Code stop  ***/

	/*Animated Model Loading Start*/

	if (g_ShaderProgramObject_Model_Sunflower)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;
		glUseProgram(g_ShaderProgramObject_Model_Sunflower);
		glGetProgramiv(g_ShaderProgramObject_Model_Sunflower, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint* pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));

		if (pShaders)
		{
			glGetAttachedShaders(g_ShaderProgramObject_Model_Sunflower, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ShaderProgramObject_Model_Sunflower, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(g_ShaderProgramObject_Model_Sunflower);
		g_ShaderProgramObject_Model_Sunflower = NULL;
		glUseProgram(0);
	}

	if (mySunFlower)
	{
		mySunFlower->ModelCleanUp();

		delete mySunFlower;
		mySunFlower = NULL;
	}

	if (g_ShaderProgramObject_Model_Bee)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;
		glUseProgram(g_ShaderProgramObject_Model_Bee);
		glGetProgramiv(g_ShaderProgramObject_Model_Bee, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint* pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(g_ShaderProgramObject_Model_Bee, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ShaderProgramObject_Model_Bee, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(g_ShaderProgramObject_Model_Bee);
		g_ShaderProgramObject_Model_Bee = NULL;
		glUseProgram(0);
	}

	if (myHoneyBee)
	{
		myHoneyBee->ModelCleanUp();

		delete myHoneyBee;
		myHoneyBee = NULL;
	}

	//myNiceLotus0

	if (myNiceLotus0)
	{
		myNiceLotus0->ModelCleanUp();

		delete myNiceLotus0;
		myNiceLotus0 = NULL;
	}

	if (myNiceLotus1)
	{
		myNiceLotus1->ModelCleanUp();

		delete myNiceLotus1;
		myNiceLotus1 = NULL;
	}


	/*Animated Model Loading Stop*/

	/*		Lotus Start		*/
	//

	if (g_ShaderProgramObject_Model_LotusFlower)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;
		glUseProgram(g_ShaderProgramObject_Model_LotusFlower);
		glGetProgramiv(g_ShaderProgramObject_Model_LotusFlower, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint* pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(g_ShaderProgramObject_Model_LotusFlower, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ShaderProgramObject_Model_LotusFlower, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(g_ShaderProgramObject_Model_LotusFlower);
		g_ShaderProgramObject_Model_LotusFlower = NULL;
		glUseProgram(0);
	}


	if (g_LotusLeaf_BumpTexture)
	{
		glDeleteTextures(1, &g_LotusLeaf_BumpTexture);
		g_LotusLeaf_BumpTexture = NULL;
	}
	if (g_LotusLeaf_DiffuseTexture)
	{
		glDeleteTextures(1, &g_LotusLeaf_DiffuseTexture);
		g_LotusLeaf_DiffuseTexture = NULL;
	}
	if (g_LotusLeaf_SpecularTexture)
	{
		glDeleteTextures(1, &g_LotusLeaf_SpecularTexture);
		g_LotusLeaf_SpecularTexture = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf0_Elements)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf0_Elements);
		g_VertexBufferObject_LotusLeaf0_Elements = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf0_TexCoord)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf0_TexCoord);
		g_VertexBufferObject_LotusLeaf0_TexCoord = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf0_Normal)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf0_Normal);
		g_VertexBufferObject_LotusLeaf0_Normal = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf0_Position)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf0_Position);
		g_VertexBufferObject_LotusLeaf0_Position = NULL;
	}

	if (g_VertexArrayObject_LotusLeaf0)
	{
		glDeleteVertexArrays(1, &g_VertexArrayObject_LotusLeaf0);
		g_VertexArrayObject_LotusLeaf0 = NULL;
	}

	//1
	if (g_VertexBufferObject_LotusLeaf1_Elements)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf1_Elements);
		g_VertexBufferObject_LotusLeaf1_Elements = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf1_TexCoord)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf1_TexCoord);
		g_VertexBufferObject_LotusLeaf1_TexCoord = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf1_Normal)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf1_Normal);
		g_VertexBufferObject_LotusLeaf1_Normal = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf1_Position)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf1_Position);
		g_VertexBufferObject_LotusLeaf1_Position = NULL;
	}

	if (g_VertexArrayObject_LotusLeaf1)
	{
		glDeleteVertexArrays(1, &g_VertexArrayObject_LotusLeaf1);
		g_VertexArrayObject_LotusLeaf1 = NULL;
	}

	//2
	if (g_VertexBufferObject_LotusLeaf2_Elements)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf2_Elements);
		g_VertexBufferObject_LotusLeaf2_Elements = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf2_TexCoord)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf2_TexCoord);
		g_VertexBufferObject_LotusLeaf2_TexCoord = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf2_Normal)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf2_Normal);
		g_VertexBufferObject_LotusLeaf2_Normal = NULL;
	}

	if (g_VertexBufferObject_LotusLeaf2_Position)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_LotusLeaf2_Position);
		g_VertexBufferObject_LotusLeaf2_Position = NULL;
	}

	if (g_VertexArrayObject_LotusLeaf2)
	{
		glDeleteVertexArrays(1, &g_VertexArrayObject_LotusLeaf2);
		g_VertexArrayObject_LotusLeaf2 = NULL;
	}

	glUseProgram(0);

	if (g_ShaderProgramObject_LotusLeaf)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;


		glUseProgram(g_ShaderProgramObject_LotusLeaf);
		glGetProgramiv(g_ShaderProgramObject_LotusLeaf, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint* pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));

		if (pShaders)
		{
			glGetAttachedShaders(g_ShaderProgramObject_LotusLeaf, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ShaderProgramObject_LotusLeaf, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}

		glDeleteProgram(g_ShaderProgramObject_LotusLeaf);
		g_ShaderProgramObject_LotusLeaf = NULL;

		glUseProgram(0);

	}
	/*		Lotus Stop		*/

	/*	CubeMap	Start */
	//g_ShaderProgramObject_CubeMap
	if (g_pCubeMap)
	{
		delete g_pCubeMap;
		g_pCubeMap = NULL;
	}

	/*	CubeMap	Stop  */


	if (g_pFlockingButterfiles)
	{
		delete g_pFlockingButterfiles;
		g_pFlockingButterfiles = NULL;
	}

	if (g_pSkyDome)
	{
		delete g_pSkyDome;
		g_pSkyDome = 0;
	}

	if (mainCamera)
	{
		delete mainCamera;
		mainCamera = NULL;
	}

	if (g_pTerrain)
	{
		delete g_pTerrain;
		g_pTerrain = NULL;
	}

	if (g_pSunLight)
	{
		delete g_pSunLight;
		g_pSunLight = 0;
	}

	/* Particle start */
	if (g_pfVerts_Particle != NULL)
	{
		free(g_pfVerts_Particle);
		g_pfVerts_Particle = NULL;
	}

	if (g_pfColors_Particle != NULL)
	{
		free(g_pfColors_Particle);
		g_pfColors_Particle = NULL;
	}

	if (g_pfVelocities_Particle != NULL)
	{
		free(g_pfVelocities_Particle);
		g_pfVelocities_Particle = NULL;
	}

	if (g_pfStartTimes_Particle != NULL)
	{
		free(g_pfStartTimes_Particle);
		g_pfStartTimes_Particle = NULL;
	}

	if (g_ShaderProgramObject_Particle1)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;
		glUseProgram(g_ShaderProgramObject_Particle1);
		glGetProgramiv(g_ShaderProgramObject_Particle1, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint* pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(g_ShaderProgramObject_Particle1, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ShaderProgramObject_Particle1, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(g_ShaderProgramObject_Particle1);
		g_ShaderProgramObject_Particle1 = NULL;
	}

	if (g_ShaderProgramObject_Particle2)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;
		glUseProgram(g_ShaderProgramObject_Particle2);
		glGetProgramiv(g_ShaderProgramObject_Particle2, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint* pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(g_ShaderProgramObject_Particle2, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ShaderProgramObject_Particle2, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(g_ShaderProgramObject_Particle2);
		g_ShaderProgramObject_Particle2 = NULL;

	}
	/*Particle Stop*/


	/*Audio Part Start*/
	if (g_iAudioBufferId1)
	{
		alDeleteBuffers(1, &g_iAudioBufferId1);
		g_iAudioBufferId1 = 0;
	}

	if (g_pHeavenAndEarthSrc)
	{

		if (g_pHeavenAndEarthSrc->isPlaying())
		{
			g_pHeavenAndEarthSrc->stop();
		}

		delete g_pHeavenAndEarthSrc;
		g_pHeavenAndEarthSrc = NULL;
	}

	if (pMasterAudio)
	{
		delete pMasterAudio;
		pMasterAudio = NULL;
	}
	/*Audio End */

	/*	Font Rendering*/
	if (pCredits != NULL)
	{
		delete pCredits;
		pCredits = NULL;
	}
	/*	Font Rendering	*/

	if (wglGetCurrentContext() == g_hrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (g_hrc)
	{
		wglDeleteContext(g_hrc);
		g_hrc = NULL;
	}

	if (g_hdc)
	{
		ReleaseDC(g_hwnd, g_hdc);
		g_hdc = NULL;
	}


	if (g_pFile)
	{
		fprintf_s(g_pFile, "Closing File \n");
		fclose(g_pFile);
		g_pFile = NULL;
	}
	return 0;
}

GLfloat Get_Parabola_4P(GLfloat X, GLfloat Z)
{
	GLfloat p4 = glm::pow(Z, 2) / X;
	return p4;
}


void SetDayLight(GLuint ShaderObject)
{
	/*glUniform3f(glGetUniformLocation(ShaderObject, "SunLight.ambient"), 1.0, 1.0, 1.0);
	glUniform3f(glGetUniformLocation(ShaderObject, "SunLight.diffuse"),1.0, 1.0, 1.0);
	glUniform3f(glGetUniformLocation(ShaderObject, "SunLight.specular"), 1.0, 1.0, 1.0);
	glUniform3f(glGetUniformLocation(ShaderObject, "SunLight.direction"), 100.0, 900.0, 0.0);*/

	glUniform3fv(glGetUniformLocation(ShaderObject, "SunLight.ambient"), 1, glm::value_ptr(g_pSunLight->getAmbient()));
	glUniform3fv(glGetUniformLocation(ShaderObject, "SunLight.diffuse"), 1, glm::value_ptr(g_pSunLight->getDiffuse()));
	glUniform3fv(glGetUniformLocation(ShaderObject, "SunLight.specular"), 1, glm::value_ptr(g_pSunLight->getSpecular()));
	glUniform3fv(glGetUniformLocation(ShaderObject, "SunLight.direction"), 1, glm::value_ptr(g_pSunLight->getDirection()));
	glUniform3fv(glGetUniformLocation(ShaderObject, "u_view_position"), 1, glm::value_ptr(mainCamera->GetCameraPosition()));
}

void SetNightLight(GLuint ShaderObject)
{
	glUniform3fv(glGetUniformLocation(ShaderObject, "SunLight.ambient"), 1, glm::value_ptr(g_v3NightAmbient));
	glUniform3fv(glGetUniformLocation(ShaderObject, "SunLight.diffuse"), 1, glm::value_ptr(g_v3NightDiffuse));
	glUniform3fv(glGetUniformLocation(ShaderObject, "SunLight.specular"), 1, glm::value_ptr(g_v3NightSpecular));
	glUniform3fv(glGetUniformLocation(ShaderObject, "SunLight.direction"), 1, glm::value_ptr(g_v3NightDirection));
	glUniform3fv(glGetUniformLocation(ShaderObject, "u_view_position"), 1, glm::value_ptr(mainCamera->GetCameraPosition()));
}


vector<glm::vec3> generateBezierCurve(glm::vec3 points[4], int totalPoints)
{
	vector<glm::vec3> retCurve;

	for (float u = 0.0f; u < 1.0f; u = u + 1.0f / (GLfloat)totalPoints)
	{
		glm::vec3 addPoint;

		addPoint.x =
			glm::pow(1.0f - u, 3) * points[0].x +
			3.0f * u * glm::pow(1.0f - u, 2) * points[1].x +
			3.0f * glm::pow(u, 2) * (1.0f - u) * points[2].x +
			glm::pow(u, 3) * points[3].x;
		addPoint.y =
			glm::pow(1.0f - u, 3) * points[0].y +
			3.0f * u * glm::pow(1.0f - u, 2) * points[1].y +
			3.0f * glm::pow(u, 2) * (1.0f - u) * points[2].y +
			glm::pow(u, 3) * points[3].y;
		addPoint.z =
			glm::pow(1.0f - u, 3) * points[0].z +
			3.0f * u * glm::pow(1.0f - u, 2) * points[1].z +
			3.0f * glm::pow(u, 2) * (1.0f - u) * points[2].z +
			glm::pow(u, 3) * points[3].z;

		retCurve.push_back(addPoint);
	}

	return retCurve;
}

// 
void changeCurve(vector<glm::vec3>& curve, glm::vec3 points[4], GLuint vao, GLuint vbo, int numSteps)
{
	curve.clear();

	for (float u = 0.0f; u < 1.0f; u = u + 1.0f / (GLfloat)numSteps)
	{
		glm::vec3 addPoint;

		addPoint.x =
			glm::pow(1.0f - u, 3) * points[0].x +
			3.0f * u * glm::pow(1.0f - u, 2) * points[1].x +
			3.0f * glm::pow(u, 2) * (1.0f - u) * points[2].x +
			glm::pow(u, 3) * points[3].x;
		addPoint.y =
			glm::pow(1.0f - u, 3) * points[0].y +
			3.0f * u * glm::pow(1.0f - u, 2) * points[1].y +
			3.0f * glm::pow(u, 2) * (1.0f - u) * points[2].y +
			glm::pow(u, 3) * points[3].y;
		addPoint.z =
			glm::pow(1.0f - u, 3) * points[0].z +
			3.0f * u * glm::pow(1.0f - u, 2) * points[1].z +
			3.0f * glm::pow(u, 2) * (1.0f - u) * points[2].z +
			glm::pow(u, 3) * points[3].z;

		curve.push_back(addPoint);
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, curve.size() * 3 * sizeof(GLfloat), curve.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//void updateCurve(WPARAM wParam, vector<glm::vec3>& curve)
//{
//	switch (wParam)
//	{
//	case 'u':
//	case 'U':
//		cam_path_control_points[numChangeValue].x += 0.5f;
//		break;
//
//	case 'j':
//	case 'J':
//		cam_path_control_points[numChangeValue].x -= 0.5f;
//		break;
//
//	case 'h':
//	case 'H':
//		cam_path_control_points[numChangeValue].z -= 0.5f;
//		break;
//
//	case 'k':
//	case 'K':
//		cam_path_control_points[numChangeValue].z += 0.5f;
//		break;
//
//	case 't':
//	case 'T':
//		cam_path_control_points[numChangeValue].y += 0.5f;
//		break;
//
//	case 'y':
//	case 'Y':
//		cam_path_control_points[numChangeValue].y -= 0.5f;
//		break;
//	}
//
//	glBindVertexArray(vao_cam_to_bee_control_points);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_cam_to_bee_control_points);
//	GLfloat tpoints[] = {
//		cam_path_control_points[0].x, cam_path_control_points[0].y, cam_path_control_points[0].z,
//		cam_path_control_points[1].x, cam_path_control_points[1].y, cam_path_control_points[1].z,
//		cam_path_control_points[2].x, cam_path_control_points[2].y, cam_path_control_points[2].z,
//		cam_path_control_points[3].x, cam_path_control_points[3].y, cam_path_control_points[3].z,
//		cam_path_control_points[4].x, cam_path_control_points[4].y, cam_path_control_points[4].z
//	};
//	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 15, tpoints, GL_DYNAMIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	changeCurve(curve, cam_path_control_points, vao_cam_to_bee_curve, vbo_cam_to_bee_curve, 100);
//}


void updateCurve(WPARAM wParam, vector<glm::vec3>& curve)
{
	switch (wParam)
	{
	case 'u':
	case 'U':
		bee_to_lake_control_points[numChangeValue].x += 0.5f;
		break;

	case 'j':
	case 'J':
		bee_to_lake_control_points[numChangeValue].x -= 0.5f;
		break;

	case 'h':
	case 'H':
		bee_to_lake_control_points[numChangeValue].z -= 0.5f;
		break;

	case 'k':
	case 'K':
		bee_to_lake_control_points[numChangeValue].z += 0.5f;
		break;

	case 't':
	case 'T':
		bee_to_lake_control_points[numChangeValue].y += 0.5f;
		break;

	case 'y':
	case 'Y':
		bee_to_lake_control_points[numChangeValue].y -= 0.5f;
		break;
	}

	glBindVertexArray(vao_bee_to_lake_control_points);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bee_to_lake_control_points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, bee_to_lake_control_points, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	changeCurve(curve, bee_to_lake_control_points, vao_bee_to_lake_curve, vbo_bee_to_lake_curve, 1000);
}

