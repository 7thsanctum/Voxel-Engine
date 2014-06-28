//=============================================================================
// Fire.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Fire particle system.  Particles are emitted directly in world space.
//=============================================================================


//***********************************************
// GLOBALS                                      *
//***********************************************

cbuffer cbPerFrame
{
	float3 gEyePosW;
	
	// for when the emit position/direction is varying
	float3 gEmitPosW;
	float3 gEmitDirW;
	
	float gGameTime;
	float gTimeStep;
	float4x4 gViewProj; 
};

cbuffer cbFixed
{
	// Net constant acceleration used to accerlate the particles.
	float3 gAccelW = {0.0f, 0.0f, 0.0f};
	
	// Texture coordinates used to stretch texture over quad 
	// when we expand point particle into a quad.
	float2 gQuadTexC[4] = 
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};
 
// Array of textures for texturing the particles.
Texture2DArray gTexArray;

// Random texture used to generate random numbers in shaders.
Texture1D gRandomTex;
 
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = 0x0F;
};

BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	// project onto unit sphere
	return normalize(v);
}
 
//***********************************************
// STREAM-OUT TECH                              *
//***********************************************

#define PT_EMITTER 0
#define PT_FLARE 1
 
struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age          : AGE;
	uint Type          : TYPE;
	int noParticles   : PARTICLES;
};
  
Particle StreamOutVS(Particle vin)
{
	return vin;
}

// The stream-out GS is just responsible for emitting 
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be 
// different.
[maxvertexcount(91)]
void StreamOutGS(point Particle gin[1], 
                 inout PointStream<Particle> ptStream)
{	
	gin[0].Age += gTimeStep;

	if( gin[0].Type == PT_EMITTER )
	{	
		// time to emit a new particle?
		if( gin[0].Age > 0.0f )
		{
			for(int i = 0; i < 90; ++i)
			{
				// Spread rain drops out above the camera.
				float3 vRandom = 1.0f*RandUnitVec3((float)i/45.0f);//*((float)i/45.0f);
				//vRandom.x *= 0.5f*((float)i/45.0f);
				vRandom.y *= 0.5f*((float)i/45.0f);
				//vRandom.z *= 0.5f*((float)i/45.0f);
			
				Particle p;
				p.InitialPosW = gEmitPosW.xyz;//+RandUnitVec3((float)i/100.0f);
				p.InitialVelW = 1000.0f*vRandom *0.1f;//*((float)i/100.0f);
				p.SizeW       = float2(10.0f, 10.0f);
				p.Age         = 0.0f;
				p.Type        = PT_FLARE;
				p.noParticles = gin[0].noParticles + 1;
				
				ptStream.Append(p);
			}
			
			// reset the time to emit
			gin[0].Age = 0.0f;
			gin[0].noParticles += 1;
		}

		// always keep emitters
		ptStream.Append(gin[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		//if( gin[0].Age <= 20.0f )
			ptStream.Append(gin[0]);
	}		
}

GeometryShader gsStreamOut = ConstructGSWithSO( 
	CompileShader( gs_5_0, StreamOutGS() ), 
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x" );
	
technique11 StreamOutTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, StreamOutVS() ) );
        SetGeometryShader( gsStreamOut );
        
        // disable pixel shader for stream-out only
        SetPixelShader(NULL);
        
        // we must also disable the depth buffer for stream-out only
        SetDepthStencilState( DisableDepth, 0 );
    }
}

//***********************************************
// DRAW TECH                                    *
//***********************************************

struct VertexOut
{
	float3 PosW  : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint   Type  : TYPE;
};

VertexOut DrawVS(Particle vin)
{
	VertexOut vout;
	float t = vin.Age;
	float3 accel = {sin(vin.Age)*0.0f,0.0f, cos(vin.Age)*0.0f}; 
	float3 drag = -00.1*t*vin.InitialVelW ; 
	//float3 accel = {sin(vin.Age)*0.0f,vin.Age*-0.2, cos(vin.Age)*0.0f}; 
	
	float3 vAcc = 0.1f*t*t*gAccelW/2  +0.1f*t*t*accel + 0.1*t*vin.InitialVelW  + vin.InitialPosW ;
	// constant acceleration equation
	vout.PosW = vAcc;
	


	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t/8.0f);
	float x = sin(vAcc);
	float y = -sin(vAcc);
	float z = cos(vAcc);
	vout.Color = float4(x, y, z, 1.0f);
	
	vout.SizeW = vin.SizeW+(vin.Age*0.1);
	vout.Type  = vin.Type;
	
	return vout;
}

struct GeoOut
{
	float4 PosH  : SV_Position;
	float4 Color : COLOR;
	float2 Tex   : TEXCOORD;
};

float3 box_vertices[8] =
{
	float3(-1.0f, 1.0f, 1.0f),		// 0
	float3(1.0f, 1.0f, 1.0f),		// 1
	float3(1.0f, -1.0f, 1.0f),		// 2
	float3(-1.0f, -1.0f, 1.0f),		// 3
	float3(1.0f, 1.0f, -1.0f),		// 4
	float3(-1.0f, 1.0f, -1.0f),		// 5
	float3(-1.0f, -1.0f, -1.0f),	// 6
	float3(1.0f, -1.0f, -1.0f)		// 7
};

uint box_indices[24] = 
{
	// Side 1
	6, 5, 7, 4,
	//Side 2
	5, 0, 4, 1,
	// Side 3
	4, 1, 7, 2,
	// Side 4
	1, 0, 2, 3,
	/// Side 5
	2, 3, 7, 6,
	// Side 6
	3, 0, 6, 5

	//// Side 1
	//6, 5, 7, 4,
	////Side 2
	//4, 5, 1, 0,
	//// Side 3
	//0, 3, 1, 2,
	//// Side 4
	//2, 3, 7, 6,
	///// Side 5
	//2, 1, 3, 0,
	//// Side 6
	//2, 3, 6, 7
};

float2 box_texcoords[6] =
{
	float2(0.0f, 1.0f),		// 0
	float2(1.0f, 1.0f),		// 1
	float2(0.0f, 0.0f),		// 3
	float2(1.0f, 1.0f),		// 5
	float2(1.0f, 0.0f),		// 6
	float2(0.0f, 0.0f)		// 7
};


// The draw GS just expands points into camera facing quads.
[maxvertexcount(24)]
void DrawGS(point VertexOut gin[1], 
            inout TriangleStream<GeoOut> triStream)
{	
	// do not draw emitter particles.
	if( gin[0].Type != PT_EMITTER )
	{
		//
		// Compute world matrix so that billboard faces the camera.
		//
		float3 look  = normalize(gEyePosW.xyz - gin[0].PosW);
		float3 right = normalize(cross(float3(0,1,0), look));
		float3 up    = cross(look, right);
		
		//
		// Compute triangle strip vertices (quad) in world space.
		//
		float halfWidth  = 0.5f*gin[0].SizeW.x;
		float halfHeight = 0.5f*gin[0].SizeW.y;

		float4 v[24];
		//v[0] = float4(gin[0].PosW + halfWidth*right - halfHeight*up, 1.0f);
		//v[1] = float4(gin[0].PosW + halfWidth*right + halfHeight*up, 1.0f);
		//v[2] = float4(gin[0].PosW - halfWidth*right - halfHeight*up, 1.0f);
		//v[3] = float4(gin[0].PosW - halfWidth*right + halfHeight*up, 1.0f);
		//v[0] = float4(gin[0].PosW + float3(1.0f, -1.0f, 0.0f) , 1.0f);
		//v[1] = float4(gin[0].PosW + float3(1.0f, 1.0f, 0.0f) , 1.0f);
		//v[2] = float4(gin[0].PosW - float3(1.0f, -1.0f,0.0f) , 1.0f);
		//v[3] = float4(gin[0].PosW - float3(1.0f, 1.0f, 0.0f) , 1.0f);
		
		for (int i = 0; i < 24; ++i)
		{
			v[i] = float4(gin[0].PosW + box_vertices[box_indices[i]] , 1.0f);
		}


		//
		// Transform quad vertices to world space and output 
		// them as a triangle strip.
		//
		GeoOut gout;
		[unroll]
		//for(int i = 0; i < 6; ++i)
		//{
			for(int j = 0; j < 24; ++j)
			{
				gout.PosH  = mul(v[j], gViewProj);
				//gout.Tex   = box_texcoords[j%6];
				gout.Tex   = gQuadTexC[j%4];
				gout.Color = gin[0].Color;
				triStream.Append(gout);
			}
			//triStream.RestartStrip();
	//	}
		
	}
}

float4 DrawPS(GeoOut pin) : SV_TARGET
{
	return gTexArray.Sample(samLinear, float3(pin.Tex, 0))*pin.Color;
}

technique11 DrawTech
{
    pass P0
    {
        SetVertexShader(   CompileShader( vs_5_0, DrawVS() ) );
        SetGeometryShader( CompileShader( gs_5_0, DrawGS() ) );
        SetPixelShader(    CompileShader( ps_5_0, DrawPS() ) );
        
        SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetDepthStencilState( NoDepthWrites, 0 );
    }
}