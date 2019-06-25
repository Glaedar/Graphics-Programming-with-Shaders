// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	//BaseApplication::BaseApplication();
	mesh = nullptr;
	depthShader = nullptr;
	lightShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	// Create Mesh object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	
	displacementMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	mesh2 = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	mesh3 = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	//mesh2 = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	model = new Model(renderer->getDevice(), renderer->getDeviceContext(), "../res/teapot.obj");

	
	textureMgr->loadTexture("brick", L"../res/brick1.dds");
	textureMgr->loadTexture("bunny", L"../res/bunny.png");
	
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	tessellationShader = new TessellationShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	// Create light object
	shadow_Light = new Light;

	shadow_Light->setLookAt(2.f, 0.0f, 0.f);
	shadow_Light->generateViewMatrix();
	shadow_Light->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	shadow_Light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	shadow_Light->setPosition(0.0f, 10.f, -15.f);
	shadow_Light->generateProjectionMatrix(SCREEN_NEAR, SCREEN_DEPTH);


	//Multiple Lights
	m_Light[0] = new Light;
	m_Light[0]->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	m_Light[0]->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light[0]->setPosition(30.0f, 10.0f, 20.0f);

	m_Light[1] = new Light;
	m_Light[1]->setAmbientColour(0.0f, 1.0f, 0.0f, 1.0f);
	m_Light[1]->setDiffuseColour(0.0f, 1.0f, 0.0f, 1.0f);
	m_Light[1]->setPosition(20.0f, 10.0f, 20.0f);

	m_Light[2] = new Light;
	m_Light[2]->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	m_Light[2]->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light[2]->setPosition(10.0f, 10.0f, 20.0f);



	// RenderTexture, OrthoMesh and shader set for different renderTarget
	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	downScaleTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	upScaleTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	blurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	downScaleMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2, 0, 0);
	upScaleMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);

	camera->setPosition(0.0f, 5.0f, 0.0f);

	totalTime = 0;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}
	if (displacementMesh)
	{
		delete displacementMesh;
		displacementMesh = 0;
	}

	if (lightShader)
	{
		delete lightShader;
		lightShader = 0;
	}
	if (shadowShader)
	{
		delete shadowShader;
		shadowShader = 0;
	}

}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	totalTime += timer->getTime();

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}


void App1::depthRender()
{
	XMMATRIX worldMatrix, translation, lightViewMatrix, lightProjectionMatrix, scaleMatrix;

	// Set the render target to be the render to texture.
	renderTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	shadow_Light->generateViewMatrix();
	translation = XMMatrixTranslation(10.f, 2.f, 10.f);
	lightViewMatrix = shadow_Light->getViewMatrix();
	lightProjectionMatrix = shadow_Light->getProjectionMatrix();
	worldMatrix = renderer->getWorldMatrix();


	//// Send geometry data (from mesh)
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(60.f, 8.0f, 50.f);
	scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::shadowRender()
{
	XMMATRIX worldMatrix, translation, viewMatrix, projectionMatrix, scaleMatrix;

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	translation = XMMatrixTranslation(10.f, 1.f, 10.f);
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();


	//// Send geometry data (from mesh)
	mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), renderTexture->getShaderResourceView(), shadow_Light);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(60.f, 8.f, 50.f);
	scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	//// Send geometry data (from mesh)
	model->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), renderTexture->getShaderResourceView(), shadow_Light);
	shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());

}

void App1::tessellationRender()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, scaleMatrix, translation;
	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	translation = XMMatrixTranslation(10.f, 4.f, 60.f);
	scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(translation, scaleMatrix);

	//// Send geometry data (from mesh)
	displacementMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)

	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("bunny"), shadow_Light, totalTime, tessInside, tessEdges);
	tessellationShader->render(renderer->getDeviceContext(), displacementMesh->getIndexCount());


}

void App1::GausianRender()
{
	downScale();

	horizontalBlur();

	verticalBlur();

	upScale();

	RenderToOrtho();
}

void App1::downScale()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// Set the render target to be the render to texture.
	downScaleTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	downScaleTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = downScaleTexture->getOrthoMatrix();

	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	downScaleMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, blurTexture->getShaderResourceView());
	// Render object (combination of mesh geometry and shader process
	textureShader->render(renderer->getDeviceContext(), downScaleMesh->getIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::horizontalBlur()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeX;

	// Store the screen width in a float that will be used in the horizontal blur shader.
	screenSizeX = (float)horizontalBlurTexture->getTextureWidth();

	// Set the render target to be the render to texture.
	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	downScaleMesh->sendData(renderer->getDeviceContext());

	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, downScaleTexture->getShaderResourceView(), screenSizeX);
	// Render object (combination of mesh geometry and shader process
	horizontalBlurShader->render(renderer->getDeviceContext(), downScaleMesh->getIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::verticalBlur()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeY;

	// Store the screen width in a float that will be used in the horizontal blur shader.
	screenSizeY = (float)verticalBlurTexture->getTextureHeight();

	// Set the render target to be the render to texture.
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	downScaleMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, horizontalBlurTexture->getShaderResourceView(), screenSizeY);

	// Render object (combination of mesh geometry and shader process
	verticalBlurShader->render(renderer->getDeviceContext(), downScaleMesh->getIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::upScale()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// Set the render target to be the render to texture.
	upScaleTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	upScaleTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getOrthoViewMatrix();

	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = upScaleTexture->getOrthoMatrix();

	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	upScaleMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, verticalBlurTexture->getShaderResourceView());
	// Render object (combination of mesh geometry and shader process
	textureShader->render(renderer->getDeviceContext(), upScaleMesh->getIndexCount());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

bool App1::RenderToOrtho()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = renderer->getOrthoMatrix();


	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// ortho matrix for 2D rendering
	upScaleMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, upScaleTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), upScaleMesh->getIndexCount());

	renderer->setZBuffer(true);
	gui();
	// Present the rendered scene to the screen.
	renderer->endScene();

	return true;

}

bool App1::renderMain()
{
	if (isBlur == true)
	{
		blurTexture->setRenderTarget(renderer->getDeviceContext());
		blurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	}

	else if (isBlur == false) 
	{
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	}

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translation, scaleMatrix;
	//// Generate the view matrix based on the camera's position.
	camera->update();

	

	tessellationRender();
	depthRender();
	shadowRender();

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	translation = XMMatrixTranslation(10.f, 2.f, 30.f);
	scaleMatrix = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	worldMatrix = XMMatrixMultiply(translation, scaleMatrix);

	mesh2->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), m_Light);
	lightShader->render(renderer->getDeviceContext(), mesh2->getIndexCount());



	if (wireframe == true)
	{
		renderer->setWireframeMode(true);
	}

	if (wireframe == false)
	{
		renderer->setWireframeMode(false);
	}
	


	if (isBlur == true)
	{
	renderer->setBackBufferRenderTarget();
	GausianRender();
	
	}
	else if (isBlur == false)
	{
	gui();
	// Present the rendered scene to the screen.
	renderer->endScene();
	}

	return true;
}

bool App1::render()
{

	renderMain();

	return true;
}


void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Gaussian Blurring", &isBlur);
	ImGui::Checkbox("Wireframe", &wireframe);

	ImGui::SliderFloat("Edge tessellation factor", &tessEdges, 3.0f, 64.0f, "%0.1");
	ImGui::SliderFloat("Inside tessellation factor", &tessInside, 3.0f, 64.0f, "%0.1");

	// Render UI
	ImGui::Render();
}

