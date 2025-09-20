#include <metal_stdlib>
using namespace metal;

vertex float4 v_main(uint vid [[vertex_id]]) {
    float2 p[3] = { float2(0.0, 0.8), float2(-0.8, -0.8), float2(0.8, -0.8) };
    return float4(p[vid], 0, 1);
}

fragment float4 f_main() { return float4(1,1,1,1); }
