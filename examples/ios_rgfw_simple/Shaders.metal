#include <metal_stdlib>
using namespace metal;

struct VSOut { float4 pos [[position]]; float4 col; };

vertex VSOut v_main(uint vid [[vertex_id]]) {
    float2 positions[3] = { float2(0.0,  0.8), float2(-0.8, -0.8), float2(0.8, -0.8) };
    float4 colors[3] = { float4(1,0,0,1), float4(0,1,0,1), float4(0,0,1,1) };
    VSOut o; o.pos = float4(positions[vid], 0.0, 1.0); o.col = colors[vid]; return o;
}

fragment float4 f_main(VSOut in [[stage_in]]) {
    return in.col;
}

