//***************************************************************************************
// ParticlesDemo.cpp by Mark M. Miller
//
// Demonstrates the use of GPU based particle systems and stream out.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "FontSheet.h"
#include "SpriteBatch.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Camera.h"
#include "VoxelSystem.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class ParticlesApp : public D3DApp
{
public:
	ParticlesApp(HINSTANCE hInstance);
	~ParticlesApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 
	void CalculateAvgFrameTime();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	bool renderText;

	std::vector<float> frameTimes;
	float averageFrameTime;
private:
	FontSheet mTitleFont;		// Fonts for rendering to the screen
	FontSheet mBodyFont;		
	SpriteBatch mSpriteBatch;

	ID3D11ShaderResourceView* mFlareTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	bool rotateMode;

	VoxelSystem mVoxel;	// The actual voxel system logic processor

	DirectionalLight mDirLights[3];

	Camera mCam;

	bool mWalkCamMode;

	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	ParticlesApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

ParticlesApp::ParticlesApp(HINSTANCE hInstance)
: D3DApp(hInstance), mRandomTexSRV(0), mFlareTexSRV(0), mWalkCamMode(false)
{
	rotateMode = false;

	mMainWndCaption = L"Voxel Rendering Demo";
	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
	//(//-51, 170, -55)
	mCam.SetPosition(-51.0f, 170.0f, -55.0f);
	//mCam.SetPosition(-100.0f, 250.0f, -100.0f);
	
	mCam.RotateY(0.78f);
	mCam.Pitch(0.58f);

	mCam.UpdateViewMatrix();

	mDirLights[0].Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);
}

ParticlesApp::~ParticlesApp()
{
	md3dImmediateContext->ClearState();
	
	ReleaseCOM(mRandomTexSRV);
	ReleaseCOM(mFlareTexSRV);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool ParticlesApp::Init()
{
	if(!D3DApp::Init())
		return false;

	renderText = true;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(md3dDevice);

	HR(mTitleFont.Initialize(md3dDevice, L"COURIER", 24.0f, FontSheet::FontStyleUnderline, true));
	HR(mBodyFont.Initialize(md3dDevice, L"COURIER", 16.0f, FontSheet::FontStyleBoldItalic, true));

	HR(mSpriteBatch.Initialize(md3dDevice));
	
	// Add the texture to a list of textures that are to be used by the shader
	// If you add more textures, make sure you update the value in shader!
	std::vector<std::wstring> smoke;
	smoke.push_back(L"Textures\\grass0.dds");
	smoke.push_back(L"Textures\\Stone0.dds");
	//smoke.push_back(L"Textures\\smoke0.dds");
	//smoke.push_back(L"Textures\\WoodCrate01.dds");
	mFlareTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, smoke);

	// TODO: This file stuff is out of date, remove it?
	/*
	std::string line;
	UINT noofParticles = 0;
	std::string action = "";
	std::ifstream myotherfile ("sysSpecs.txt");
	if (myotherfile.is_open())
	{
		int check = 0; 
		while ( myotherfile.good() )
		{
			getline (myotherfile,line);
			if(check == 0)
			{
				noofParticles = atoi(line.c_str());
				check = check + 1;
			}
			else if (check == 1)
			{
				action.append(line.c_str());
			}
		}
		myotherfile.close();
	}
	else
	{
		std::ofstream myfile ("sysSpecs.txt");
		if (myfile.is_open())
		{
			myfile << "20000\n";
			myfile << "cube\n";
			myfile.close();
		}
		action = "quad";
		
	}*/
	UINT noofParticles = 0;
	noofParticles = 256*256*256;
	//if(action == "quad")
	//{
		//mVoxel.Init(md3dDevice, Effects::SmokeFX, mFlareTexSRV, mRandomTexSRV, noofParticles); 
	//}
	//else
	//{		
	mVoxel.Init(md3dDevice, Effects::RainFX, mFlareTexSRV, mRandomTexSRV, noofParticles, mCam); 		
	//}
	//mVoxel.SetEmitPos(XMFLOAT3(0.0f, 1.0f, 120.0f));
	mVoxel.SetEmitPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
	return true;
}

void ParticlesApp::OnResize()
{
	D3DApp::OnResize();

	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 10000.0f);
}

float timePassed = 0.0f;

float height = 128.0f;

void ParticlesApp::UpdateScene(float dt)
{
	if(timePassed < 8.0f)
	{
		rotateMode = false;
		timePassed+= dt;
	}
	else rotateMode = true;

	// Camera controls
	if( GetAsyncKeyState('W') & 0x8000 )
		mCam.Walk(100.0f*dt);

	if( GetAsyncKeyState('S') & 0x8000 )
		mCam.Walk(-100.0f*dt);

	if( GetAsyncKeyState('A') & 0x8000 )
		mCam.Strafe(-100.0f*dt);

	if( GetAsyncKeyState('D') & 0x8000 )
		mCam.Strafe(100.0f*dt);

	// Reset particle systems if R is pressed	
	if(GetAsyncKeyState('R') & 0x8000)
	{
		mVoxel.Reset();
	}
 
	if(GetAsyncKeyState('1') & 0x8000)
	{
		mVoxel.ToggleValues(1);
	}
	if(GetAsyncKeyState('2') & 0x8000)
	{
		mVoxel.ToggleValues(2);
	}
	if(GetAsyncKeyState('3') & 0x8000)
	{
		mVoxel.ToggleValues(3);
	}

	if(GetAsyncKeyState('R') & 0x8000)
	{
		height = 128.0f;
		mVoxel.ToggleValues(4);
	}
	if(GetAsyncKeyState('T') & 0x8000)
	{
		height = 64.0f;
		mVoxel.ToggleValues(5);
	}
	if(GetAsyncKeyState('Y') & 0x8000)
	{
		height = 128.0f;
		mVoxel.ToggleValues(6);
	}
	if(GetAsyncKeyState('U') & 0x8000)
	{
		height = 128.0f;
		mVoxel.ToggleValues(7);
	}
	if(GetAsyncKeyState('I') & 0x8000)
	{
		height = 128.0f;
		mVoxel.ToggleValues(8);
	}

	if(GetAsyncKeyState('F') & 0x8000)
	{
		height = 128.0f;
		mVoxel.ToggleUpdate();
	}

	if(GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		renderText = !renderText;
	}

	if(GetAsyncKeyState('C') & 0x8000)
	{
		frameTimes.clear();
	}

	// Update the particle system
	mVoxel.Update(dt, mTimer.TotalTime());

	mCam.UpdateViewMatrix();

	rotateMode = false;
	if(rotateMode)
	{
		mCam.Strafe(35.0f*dt);

		mCam.LookAt(mCam.GetPosition(), XMFLOAT3(128.0f,height,128.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	}
}

void ParticlesApp::CalculateAvgFrameTime()
{
	if(frameTimes.size() < 1000)
	{
		frameTimes.push_back(GetFrameTime());	

		float total = 0.0f;

		for(int i = 0; i < frameTimes.size(); i++)
		{
			total += frameTimes[i];
		}

		averageFrameTime = total / frameTimes.size();
	}
}

void ParticlesApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::White));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Voxel);	// TODO: Check whats going on here
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

	//if( GetAsyncKeyState('1') & 0x8000 )
	//	md3dImmediateContext->RSSetState(RenderStates::WireframeRS);


	md3dImmediateContext->RSSetState(0);

	mVoxel.SetEyePos(mCam.GetPosition());
	mVoxel.Draw(md3dDevice, md3dImmediateContext, mCam);	

	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	
	// restore default states.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); 

	// Text start

	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

	std::wstring text = L"System State";
	// TODO: Add in system state variables here, e.g. is frustum culling enabled etc
	// also resize text
	// Calculate the text width.
	int textWidth = 0;
	for(UINT i = 0; i < text.size(); ++i)
	{
		WCHAR character = text[i];
		if(character == ' ') 
		{
			textWidth += mTitleFont.GetSpaceWidth();
		}
		else
		{
			const CD3D11_RECT& r = mTitleFont.GetCharRect(text[i]);
			textWidth += (r.right - r.left + 1);
		}
	}

	CalculateAvgFrameTime();

	if(renderText)
	{
		// Center the text in the screen.
		POINT textPos;
		textPos.x = 0;
		textPos.y = 0;

		mSpriteBatch.DrawString(md3dImmediateContext, mTitleFont, text, textPos, XMCOLOR(0xff000000));

		textPos.y = 40.0f;

		std::string text2 = mVoxel.GetStatus();

	

		text2 += std::string("\nAvg FTime") + std::to_string(averageFrameTime);

		std::wstring ws;
		ws.assign(text2.begin(), text2.end());

		mSpriteBatch.DrawString(md3dImmediateContext, mBodyFont, ws, textPos, XMCOLOR(0xff000000));
	}
	


	// restore default
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	// Text end

	HR(mSwapChain->Present(0, 0));
}

void ParticlesApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void ParticlesApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void ParticlesApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		timePassed = 0.0f;

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
