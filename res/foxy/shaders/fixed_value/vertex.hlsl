// simple vertex
// http://www.rastertek.com/dx11tut04.html

// cbuffer Uniforms: register(b0) {
//   float4x4 viewProj;
// }


struct VertexInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct FragInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

FragInput main(uint vertex_index : SV_VertexID)
{
    FragInput output;

    // Vulkan is (-,-) top-left and (+,+) bottom-right
    float4 positions[3] = {
        float4(-0.5,  0.5, 0.0, 1.0),
        float4( 0.5,  0.5, 0.0, 1.0),
        float4( 0.0, -0.5, 0.0, 1.0),
    };

    float4 colors[3] = {
        float4(1.0, 0.25, 0.25, 1.0),
        float4(0.25, 1.0, 0.25, 1.0),
        float4(0.25, 0.25, 1.0, 1.0),
    };

    // input.position.w = 1.0f;

    // output.position = mul(viewProj, input.position);
    output.position = positions[vertex_index];
    output.color = colors[vertex_index];

    return output;
}
