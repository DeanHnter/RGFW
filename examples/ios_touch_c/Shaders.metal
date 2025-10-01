#include <metal_stdlib>
using namespace metal;

struct VSOut { float4 pos [[position]]; float4 col; };

vertex VSOut v_main(uint vid [[vertex_id]],
                    constant float2* verts [[buffer(0)]],
                    constant float4* col   [[buffer(1)]]) {
    float2 p = verts[vid];
    VSOut o; o.pos = float4(p, 0, 1); o.col = *col; return o;
}

fragment float4 f_main(VSOut in [[stage_in]]) { return in.col; }
