struct VertexInput
{
  [[vk::location(0)]] float3 position : POSITION;
  [[vk::location(1)]] float4 color : COLOR;
};

struct UBO
{
  float4x4 projection_matrix;
  float4x4 model_matrix;
  float4x4 view_matrix;
};
cbuffer ubo : register(b0) { UBO ubo; }

struct FragInput
{
  float4 position : SV_POSITION;
  [[vk::location(0)]] float4 color : COLOR;
};

FragInput vertex_main(uint vertex_index : SV_VertexID)
{
  FragInput output;

  // Vulkan is (-,-) top-left and (+,+) bottom-right
  float4 positions[3] = {
    float4(-0.5,  0.5, 0.0, 1.0),
    float4( 0.5,  0.5, 0.0, 1.0),
    float4( 0.0, -0.5, 0.0, 1.0),
  };

  float4 colors[3] = {
    float4(1.00, 0.25, 0.25, 1.00),
    float4(0.25, 1.00, 0.25, 1.00),
    float4(0.25, 0.25, 1.00, 1.00),
  };

  // input.position.w = 1.0f;

  // output.position = mul(viewProj, input.position);
  output.position = positions[vertex_index];
  output.color = colors[vertex_index];

  return output;
}

float4 fragment_main(FragInput input) : SV_TARGET
{
  return input.color;
}