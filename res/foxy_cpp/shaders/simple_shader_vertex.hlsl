// http://www.rastertek.com/dx11tut04.html

cbuffer Uniforms: register(b0) {
  float4x4 viewProj;
}

struct VertexInput {
  float4 position: POSITION;
  float4 color: COLOR;
};

struct FragInput {
  float4 position: SV_POSITION;
  float4 color: COLOR;
};

FragInput main(VertexInput input) {
  FragInput output;

  input.position.w = 1.0f;

//   output.position = mul(viewProj, input.position);
  output.position = input.position;
  output.color = input.color;

  return output;
}