//=============================================================================
// Cube particle system by Mark M. Miller - 10006179@live.napier.ac.uk
// This shader takes in points and generates cubes based on that point
// also deals with basic movement of these particles and stream-out
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
    BlendEnable[0] = FALSE;
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
float3 RandUnitfloat3(float offset)
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
 
struct Voxel
{
	float3 InitialPosW : POSITION;
	//float3 InitialVelW : VELOCITY;
	//float2 SizeW       : SIZE;
	float Age          : AGE;
	uint Type          : TYPE;
};

Voxel StreamOutVS(Voxel vin)
{
	return vin;
}

// The stream-out GS is just responsible for emitting 
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be 
// different.
[maxvertexcount(1)]
void StreamOutGS(point Voxel gin[1], 
                 inout PointStream<Voxel> ptStream)
{	
	gin[0].Age += gTimeStep;
	ptStream.Append(gin[0]);
}

GeometryShader gsStreamOut = ConstructGSWithSO( 
	CompileShader( gs_5_0, StreamOutGS() ), 
	"POSITION.xyz; AGE.x; TYPE.x" );
	
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
	//float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint   Type  : TYPE;
};

float rand_1_05(in float2 uv)
{
    float2 noise = (frac(sin(dot(uv ,float2(12.9898,78.233)*2.0)) * 43758.5453));
    return abs(noise.x + noise.y) * 0.5;
}

VertexOut DrawVS(Voxel vin)
{
	VertexOut vout;
	
	float t = vin.Age;
	
	float3 vAcc = 0.1f*t*t*gAccelW/2 + vin.InitialPosW ;
	
	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t/8.0f);
	float x = sin(vAcc);
	float y = -sin(vAcc);
	float z = cos(vAcc);

	x = y = z = 0.4f;	// Approx normal colour
	//x = y = z = vin.Type /2;	// Approx normal colour
	//x = y = z = (vAcc.y * 1.0f)/255;	// This line makes it black and white

	vout.Color = float4(x * 2.0f, y * 2.0f, z * 2.0f, opacity);
	// constant acceleration equation

	vout.PosW = vin.InitialPosW;
	vout.Type  = vin.Type;
	
	return vout;
}

struct GeoOut
{
	float4 PosH  : SV_Position;
	float4 Color : COLOR;
	uint Type   : TYPE;
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
	// Side 2
	5, 0, 4, 1,
	// Side 3
	4, 1, 7, 2,
	// Side 4
	1, 0, 2, 3,
	// Side 5
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


// The draw GS just expands points into cubes.
[maxvertexcount(24)]
void DrawGS(point VertexOut gin[1], 
            inout TriangleStream<GeoOut> triStream)
{
	GeoOut gout;

	//
	// Compute world matrix so that billboard faces the camera.
	//
	//float3 look  = normalize(gEyePosW.xyz - gin[0].PosW);
	//float3 right = normalize(cross(float3(0,1,0), look));
	//float3 up    = cross(look, right);
		
	//
	// Compute triangle strip vertices (quad) in world space.
	//
	//float halfWidth  = 0.5f;//*gin[0].SizeW.x;
	//float halfHeight = 0.5f;//*gin[0].SizeW.y;
	//float4 v[24];

	float sizeMod = 2.0f;

	if(gin[0].Type == 0)	// if its a 
		sizeMod = 8.0f;

	//
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	// TODO: Add old loop in for testing purposes
	
	[unroll]
	//for(int j = 0; j < 12; ++j)
	for(int j = 0; j < 24; ++j)
	{
		// 1, 3, 5
		gout.PosH  = mul(float4(gin[0].PosW + mul(box_vertices[box_indices[j]], 1.0f / sizeMod) , 1.0f), gViewProj);
		//gout.Tex   = box_texcoords[j%6];
		gout.Type = gin[0].Type;
		gout.Tex   = gQuadTexC[j%4];
		gout.Color = gin[0].Color;
		triStream.Append(gout);	
	}	
}

float4 DrawPS(GeoOut pin) : SV_TARGET
{/*
	float4 ambientColour = gTexArray.Sample(samLinear, float3(pin.Tex, pin.Type % 2))*pin.Color;

	float s = max(dot(float3(0.3f, 0.4f, 0.5f), pin.Normal), 0.0);
	float4 diffuseColour = float4(s * gTexArray.Sample(samLinear, float3(pin.Tex, pin.Type % 2))*pin.Color);

	float4 light = gTexArray.Sample(samLinear, float3(pin.Tex, pin.Type % 2))*pin.Color / 4  + ambientColour/4 - diffuseColour/4;
	light.a = 0.4;
	return light;*/

	//return float4(sh_light(pin.Normal, beach) * 0.5, 1.0f);
	//GOOD STUFF
	
	//float4 outside = gTexArray.Sample(samLinear, float3(pin.Tex, pin.Type % 2))*pin.Color*float4(sh_light(pin.Normal, tomb) * 0.5, 1.0f);
	// pin.Type % numOfTextures : this is for selecting texture based on type 
	//float4 at_observer = float4((fog(outside, float3(1.0, 1.0, 1.0), pin.depth, 0.0015)), 1.0f);

	//at_observer = gTexArray.Sample(samLinear, float3(pin.Tex, pin.Type % 2))*pin.Color;
	//(//-51, 170, -55)
	return gTexArray.Sample(samLinear, float3(pin.Tex, pin.Type % 2))*pin.Color;
	//return gTexArray.Sample(samLinear, float3(pin.Tex, pin.Type % 2))*pin.Color + outside;
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