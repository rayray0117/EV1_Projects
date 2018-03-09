
Texture2D<float4> tImage : register(t0);

SamplerState sSampler : register(s0);


cbuffer CLEAR_CONSTANTS : register(b0)
{
    float width;		   //4
    float height;		   //4
    float POWER;
    uint MAX_ITERATIONS;
    //float padding2;		   //4
    float ZOOM;
    float2 PAN;
	float use_lerp;

    float4 min_color;	 //16
    float4 max_color;	 //16
};

RWTexture2D<unorm float4> tOutput : register(u0);

struct vs_input_t
{
    float3 position : POSITION;
    float4 tint : TINT;
    float2 uv : UV;

	//float3 normal : NORMAL;
};

struct v2f_t
{

    float4 position : SV_Position;
    float4 tint : TINT;
    float2 uv : UV;
	//float3 normal: NORMAL;
};

struct compute_input_t
{
    uint3 group_id : SV_GroupID; // What is my index within the context of a the dispatch call
    uint group_idx : SV_GroupIndex; // Flattend version of the top 
                                             // (z * (width * height) + y * width + x)
    uint3 group_thread_id : SV_GroupThreadID; // My thread within the context of the group
    uint3 dispatch_id : SV_DispatchThreadID; // My thread within context of the entire dispatch 
                                             // [5x5x1, with threads 5,5,1]  If I was in 2, 2, 0 group 
                                             // and 3, 3, 0 thread, I would be in 13 13 0 thread id
};


v2f_t VertexFunction(vs_input_t input)
{
    v2f_t v2f = (v2f_t) 0;

    float4 local = float4(input.position, 1.0f);

    
    v2f.position = local;
    v2f.tint = input.tint;
    v2f.uv = input.uv;

    return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{

    float4 albedo = tImage.Sample(sSampler, input.uv);

    return albedo * input.tint;
}
//////////////////////////////////
static uint CheckMandlebrotSet(float x0, float y0)
{

    float x = 0.0f;
    float y = 0.0f;

    uint iteration = 0;
	
    while ((((x * x) + (y * y)) < 4.0f) && (iteration < MAX_ITERATIONS))
    {
		/*/
        float xtemp = x * x - y * y + x0;
        y = 2.0f * x * y + y0;
        x = xtemp;
		//*/
		//*/
        float mag_z = abs(sqrt(x * x + y * y));	  
        float theta = 0.f;
        if(x != 0.0f)
        {
            theta = atan2(y, x);
        }
		


        float powered_z = 1.f;
        powered_z = pow(mag_z, POWER);

        x = powered_z * cos(POWER * theta) + x0;
        y = powered_z * sin(POWER * theta) + y0;
		//*/
        iteration++;
    }
				   
    return iteration;
}




[numthreads(16, 16, 1)]
void ComputeFunction(compute_input_t input)
{

    float4 color;

    float x_base = ((float) input.dispatch_id.x / (float) width) * 3.5f - 2.5f;
    float y_base = ((float) input.dispatch_id.y / (float) height) * 2.0f - 1.0f;

    float x0 = x_base * ZOOM + PAN.x;
    float y0 = y_base * ZOOM + PAN.y;
    uint iterations = CheckMandlebrotSet(x0, y0);

	if(use_lerp == 1.0f)
    {
        color = lerp(min_color, max_color, sqrt((float)iterations / (float)MAX_ITERATIONS));
    }
	else
	{
        if (iterations == MAX_ITERATIONS)
        {
            color = max_color;
        }
        else
        {
            color = min_color;
        }
    }
    


    tOutput[input.dispatch_id.xy] = color;
}
