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
	float depth : V_Depth;
	float3 Normal : V_Normal;
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

float3 box_normals[6] =
{
	float3(0.0f, 1.0f, 0.0f),		// 0
	float3(0.0f, -1.0f, 0.0f),		// 1
	float3(1.0f, 0.0f, 0.0f),		// 2
	float3(-1.0f, 0.0f, 0.0f),		// 3
	float3(0.0f, 0.0f, 1.0f),		// 4
	float3(0.0f, 0.0f, -1.0f)		// 5
};

float3 box_normals2[8] =
{
	normalize(float3(-1.0f, 1.0f, 1.0f)),
	normalize(float3(1.0f, 1.0f, 1.0f)),
	normalize(float3(1.0f, -1.0f, 1.0f)),
	normalize(float3(-1.0f, -1.0f, 1.0f)),
	normalize(float3(1.0f, 1.0f, -1.0f)),
	normalize(float3(-1.0f, 1.0f, -1.0f)),
	normalize(float3(-1.0f, -1.0f, -1.0f)),
	normalize(float3(1.0f, -1.0f, -1.0f))
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

float3 groove[9] = 
{
    float3( 0.3783264,  0.4260425,  0.4504587),
    float3( 0.2887813,  0.3586803,  0.4147053),
    float3( 0.0379030,  0.0295216,  0.0098567),
    float3(-0.1033028, -0.1031690, -0.0884924),
    float3(-0.0621750, -0.0554432, -0.0396779),
    float3( 0.0077820, -0.0148312, -0.0471301),
    float3(-0.0935561, -0.1254260, -0.1525629),
    float3(-0.0572703, -0.0502192, -0.0363410),
    float3( 0.0203348, -0.0044201, -0.0452180)
};

float3 beach[9] = 
{
    float3( 0.6841148,  0.6929004,  0.7069543),
    float3( 0.3173355,  0.3694407,  0.4406839),
    float3(-0.1747193, -0.1737154, -0.1657420),
    float3(-0.4496467, -0.4155184, -0.3416573),
    float3(-0.1690202, -0.1703022, -0.1525870),
    float3(-0.0837808, -0.0940454, -0.1027518),
    float3(-0.0319670, -0.0214051, -0.0147691),
    float3( 0.1641816,  0.1377558,  0.1010403),
    float3( 0.3697189,  0.3097930,  0.2029923)
};

float3 tomb[9] = 
{
    float3( 1.0351604,  0.7603549,  0.7074635),
    float3( 0.4442150,  0.3430402,  0.3403777),
    float3(-0.2247797, -0.1828517, -0.1705181),
    float3( 0.7110400,  0.5423169,  0.5587956),
    float3( 0.6430452,  0.4971454,  0.5156357),
    float3(-0.1150112, -0.0936603, -0.0839287),
    float3(-0.3742487, -0.2755962, -0.2875017),
    float3(-0.1694954, -0.1343096, -0.1335315),
    float3( 0.5515260,  0.4222179,  0.4162488)
};

float3 sh_light(float3 normal, float3 l[9]){
    float x = normal.x;
    float y = normal.y; 
    float z = normal.z; 

    const float C1 = 0.429043;
    const float C2 = 0.511664;
    const float C3 = 0.743125;
    const float C4 = 0.886227;
    const float C5 = 0.247708;
    
    return (
        C1 * l[8] * (x * x - y * y) +
        C3 * l[6] * z * z +
        C4 * l[0] -
        C5 * l[6] +
        2.0 * C1 * l[4] * x * y +
        2.0 * C1 * l[7]  * x * z +
        2.0 * C1 * l[5] * y * z +
        2.0 * C2 * l[3]  * x +
        2.0 * C2 * l[1] * y +
        2.0 * C2 * l[2]  * z
    );
}

// The draw GS just expands points into cubes.
[maxvertexcount(12)]
void DrawGS(point VertexOut gin[1], 
            inout TriangleStream<GeoOut> triStream)
{
	GeoOut gout;
	int indexes[12] = { 0,0,0,0,0,0, 0,0,0,0,0,0 }; 
	int norms[3] = {0, 0, 0};
	//2,1,6 // Starting
	//2,6,4
	//2,4,3
	//2,3,1 // Far Right

	//6,5,4 // Lower Left
	//6,4,2
	//6,2,1
	//6,1,5

	if( step(gEyePosW.y, gin[0].PosW.y))
	{
		if(gEyePosW.x > gin[0].PosW.x && gEyePosW.z > gin[0].PosW.z)
		{
			int setIndies[12] = {	// Side 3
									4, 1, 7, 2,
									// Side 5
									2, 3, 7, 6,
									// Side 4
									1, 0, 2, 3}; 			
			indexes = setIndies;

			int setNorms[3] = { 5, 1, 3};
			norms = setNorms;
		}
		if(gEyePosW.x > gin[0].PosW.x && gEyePosW.z < gin[0].PosW.z)
		{
			int setIndies[12] = {	// Side 1
									6, 5, 7, 4,
									// Side 5
									2, 3, 7, 6,
									// Side 3
									4, 1, 7, 2 }; 
			indexes = setIndies;
			int setNorms[3] = { 2, 1, 5};
			norms = setNorms;
		}
	
		if(gEyePosW.x < gin[0].PosW.x && gEyePosW.z > gin[0].PosW.z)
		{
			// lower left
			int setIndies[12] = {	// Side 4
									1, 0, 2, 3,
									/// Side 5
									2, 3, 7, 6,
									// Side 6
									3, 0, 6, 5 }; 
			indexes = setIndies;
			int setNorms[3] = { 3, 1, 4};
			norms = setNorms;
		}
		if(gEyePosW.x < gin[0].PosW.x && gEyePosW.z < gin[0].PosW.z)
		{
			int setIndies[12] = {	// Side 6
									3, 0, 6, 5,
									// Side 5
									2, 3, 7, 6,																		
									// Side 1
									6, 5, 7, 4 }; 
			indexes = setIndies;
			int setNorms[3] = { 4, 1, 2};
			norms = setNorms;
		}
	}
	else
	{
		if(gEyePosW.x > gin[0].PosW.x && gEyePosW.z < gin[0].PosW.z)
		{
			// faces 0, 1, 3
			// Right upper Quad
			int setIndies[12] = {	// Side 1
									6, 5, 7, 4,
									//Side 2
									5, 0, 4, 1,
									// Side 3
									4, 1, 7, 2 }; 
			indexes = setIndies;
			int setNorms[3] = { 2, 0, 5};
			norms = setNorms;
		}
		else if(gEyePosW.x < gin[0].PosW.x && gEyePosW.z < gin[0].PosW.z)
		{
			// Starting
			int setIndies[12] = {	//Side 2
									5, 0, 4, 1,
									// Side 1
									6, 5, 7, 4, 
									// Side 6
									3, 0, 6, 5 }; 
			indexes = setIndies;
			int setNorms[3] = { 0, 2, 4};
			norms = setNorms;

		}
		else if(gEyePosW.x < gin[0].PosW.x && gEyePosW.z > gin[0].PosW.z)
		{
			// Top Quad
			int setIndies[12] = {	//Side 2
									5, 0, 4, 1,
									// Side 6
									3, 0, 6, 5, 
									// Side 4
									1, 0, 2, 3 }; 
			indexes = setIndies;
			int setNorms[3] = { 0, 4, 3};
			norms = setNorms;
		}
		else if(gEyePosW.x > gin[0].PosW.x && gEyePosW.z > gin[0].PosW.z)
		{
			// Other Top Quad
			int setIndies[12] = {	//Side 2
									5, 0, 4, 1,
									// Side 4
									1, 0, 2, 3, 
									// Side 3
									4, 1, 7, 2 }; 
			indexes = setIndies;
			int setNorms[3] = { 0, 3, 5};
			norms = setNorms;
		}
	}

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
	/*
	[unroll]
	//for(int j = 0; j < 12; ++j)
	for(int j = 0; j < 4; ++j)
	{
		// 1, 3, 5
		gout.PosH  = mul(float4(gin[0].PosW + mul(box_vertices[box_indices[j]], 1.0f / sizeMod) , 1.0f), gViewProj);
		//gout.Tex   = box_texcoords[j%6];
		gout.Type = gin[0].Type;
		gout.Tex   = gQuadTexC[j%4];
		gout.Color = gin[0].Color;
		triStream.Append(gout);	
	}	*/

	// Backface culled loop
	int temp = 0;
	[unroll]
	for(int j = 0; j < 12; ++j)
	{
		// 2, 1, 6
		gout.PosH  = mul(float4(gin[0].PosW + mul(box_vertices[indexes[j]], 1.0f / sizeMod) , 1.0f), gViewProj);	
		gout.depth = length(gout.PosH);
		gout.Normal = box_normals2[indexes[j]];
		//gout.Normal = box_normals[norms[temp]];
		gout.Type = gin[0].Type;
		gout.Tex   = gQuadTexC[j%4];
		gout.Color = gin[0].Color;
		triStream.Append(gout);	
		if(j % 4 == 0 && j > 0)
			temp++;
	}	
}

// Helper functions (include elsewhere)
// ####################################
float3 get_eye_normal(float2 viewport, float4x4 inverse_projection, GeoOut pin)
{
    float4 device_normal = float4(((pin.PosH.xy/viewport)-0.5)*2.0, 0.0, 1.0);
    return normalize((mul(inverse_projection, device_normal)).xyz);
}

float3 gamma(float3 color)
{
    return pow(color, float3(0.5f, 0.5f, 0.5f));
}

float3 fog(float3 color, float3 fcolor, float depth, float density)
{
    const float e = 2.71828182845904523536028747135266249;
    float f = pow(e, -pow(depth*density, 2));
    return lerp(fcolor, color, f);
}
// ####################################

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
	
	float4 outside = gTexArray.Sample(samLinear, float3(pin.Tex, pin.Type % 2))*pin.Color*float4(sh_light(pin.Normal, tomb) * 0.5, 1.0f);
	// pin.Type % numOfTextures : this is for selecting texture based on type 
	float4 at_observer = float4((fog(outside, float3(1.0, 1.0, 1.0), pin.depth, 0.0015)), 1.0f);

	//at_observer = gTexArray.Sample(samLinear, float3(pin.Tex, pin.Type % 2))*pin.Color;
	//(//-51, 170, -55)
	return at_observer;
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