// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
//#include "ColourShader.h"
#include "DepthShader.h"
#include "ShadowShader.h"
#include "LightShader.h"
#include "TessellationShader.h"
#include "VerticalBlurShader.h"
#include "HorizontalBlurShader.h"
#include "TextureShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in);

	bool frame();

protected:
	bool render();
	void gui();

	//Shadows
	void depthRender();
	void shadowRender();

	//Tessellation
	void tessellationRender();


	//Gaussian Blur
	bool RenderToOrtho();
	void downScale();
	void upScale();
	void horizontalBlur();
	void verticalBlur();
	void GausianRender();
	bool renderMain();


private:

	// Models and Meshes
	PlaneMesh* mesh;
	PlaneMesh* mesh3;
	CubeMesh* mesh2;
	Model* model;
	PlaneMesh* displacementMesh;

	//lights
	Light* m_Light[3];
	Light* shadow_Light;

	//timers for waves
	Timer m_Time;
	float totalTime;

	//render to textures
	RenderTexture* downScaleTexture;
	RenderTexture* upScaleTexture;
	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;
	RenderTexture* blurTexture;
	RenderTexture* renderTexture;

	// Shaders
	LightShader* lightShader;
	DepthShader* depthShader;
	ShadowShader* shadowShader;
	TessellationShader* tessellationShader;
	TextureShader* textureShader;
	VerticalBlurShader* verticalBlurShader;
	HorizontalBlurShader* horizontalBlurShader;

	// ortho meshes
	OrthoMesh* downScaleMesh;
	OrthoMesh* upScaleMesh;
	OrthoMesh* orthoMesh;

	//variables for imGui
	bool isBlur = false;
	bool wireframe = false;
	float tessEdges = 2, tessInside = 2;
};

#endif