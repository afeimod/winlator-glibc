package com.winlator.renderer.material;

public class WindowMaterial extends ShaderMaterial {
    public WindowMaterial() {
        setUniformNames("xform", "viewSize", "texture", "fx_brightness", "fx_contrast", "fx_fxaa", "fx_gamma", "fx_reflection", "fx_saturation");
    }

    @Override
    protected String getVertexShader() {
        return
            "uniform float xform[6];\n" +
            "uniform vec2 viewSize;\n" +
            "attribute vec2 position;\n" +
            "varying vec2 vUV;\n" +

            "void main() {\n" +
                "vUV = position;\n" +
                "vec2 transformedPos = applyXForm(position, xform);\n" +
                "gl_Position = vec4(2.0 * transformedPos.x / viewSize.x - 1.0, 1.0 - 2.0 * transformedPos.y / viewSize.y, 0.0, 1.0);\n" +
            "}"
        ;
    }

    @Override
    protected String getFragmentShader() {
        return
            "precision mediump float;\n" +

            "uniform sampler2D texture;\n" +
            "uniform vec2 viewSize;\n" +
            "uniform float fx_brightness;\n" +
            "uniform float fx_contrast;\n" +
            "uniform float fx_fxaa;\n" +
            "uniform float fx_gamma;\n" +
            "uniform float fx_reflection;\n" +
            "uniform float fx_saturation;\n" +
            "varying vec2 vUV;\n" +

            "void main() {\n" +
                "vec4 color = vec4(texture2D(texture, vUV).rgb, 1.0);\n" +
                getFXAA() + getFastReflections() + getColorCorrection() +
                "gl_FragColor = color;\n" +
            "}"
        ;
    }

    private String getColorCorrection() {
        return
            "if (abs(fx_saturation - 1.0) > 0.0)\n" +
                "color.rgb = vec3(mix(vec3(dot(color.rgb, vec3(0.299, 0.587, 0.114))), color.rgb, fx_saturation));\n" +
            "if ((abs(fx_brightness - 1.0) > 0.0) || (abs(fx_contrast - 1.0) > 0.0))\n" +
                "color.rgb = (color.rgb-0.5)*fx_contrast+0.5+fx_brightness-1.0;\n" +
            "if (abs(fx_gamma - 1.0) > 0.0)\n" +
               "color.rgb = pow(color.rgb, vec3(1.0/fx_gamma));\n";
    }

    private String getFastReflections() {
        return
            "if (fx_reflection > 0.0) {\n" +
                "//get the pixel color\n" +
                "float gray = (color.r + color.g + color.b) / 3.0;\n" +
                "float saturation = (abs(color.r - gray) + abs(color.g - gray) + abs(color.b - gray)) / 3.0;\n" +
                "\n" +
                "//persistent random offset to hide that the reflection is wrong\n" +
                "float rndx = mod(vUV.x + gray, 0.03) + mod(vUV.y + saturation, 0.05);\n" +
                "float rndy = mod(vUV.y + saturation, 0.03) + mod(vUV.x + gray, 0.05);\n" +
                "\n" +
                "//show the effect mainly on the bottom part of the screen\n" +
                "float step = (max(gray, saturation) + 0.1) * vUV.y;\n" +
                "\n" +
                "//the fake reflection is just a watered copy of the frame moved slightly lower\n" +
                "vec3 reflection = texture2D(texture, vUV + vec2(rndx, rndy - min(vUV.y, 0.25)) * step).rgb;\n" +
                "\n" +
                "//apply parameters and mix the colors\n" +
                "reflection *= 4.0 * (1.0 - gray) * fx_reflection;\n" +
                "reflection *= reflection * step * fx_reflection;\n" +
                "color.rgb += reflection;\n" +
            "}\n";
    }

    private String getFXAA() {
        return
            "if (fx_fxaa > 0.5) {\n" +
                "// The parameters are hardcoded for now, but could be\n" +
                "// made into uniforms to control fromt he program.\n" +
                "float FXAA_SPAN_MAX = 8.0;\n" +
                "float FXAA_REDUCE_MUL = 1.0/8.0;\n" +
                "float FXAA_REDUCE_MIN = (1.0/128.0);\n" +
                "\n" +
                "vec3 rgbNW = texture2D(texture, vUV.xy + (vec2(-1.0, -1.0) / viewSize)).xyz;\n" +
                "vec3 rgbNE = texture2D(texture, vUV.xy + (vec2(+1.0, -1.0) / viewSize)).xyz;\n" +
                "vec3 rgbSW = texture2D(texture, vUV.xy + (vec2(-1.0, +1.0) / viewSize)).xyz;\n" +
                "vec3 rgbSE = texture2D(texture, vUV.xy + (vec2(+1.0, +1.0) / viewSize)).xyz;\n" +
                "vec3 rgbM  = color.rgb;\n" +
                "\n" +
                "vec3 luma = vec3(0.299, 0.587, 0.114);\n" +
                "float lumaNW = dot(rgbNW, luma);\n" +
                "float lumaNE = dot(rgbNE, luma);\n" +
                "float lumaSW = dot(rgbSW, luma);\n" +
                "float lumaSE = dot(rgbSE, luma);\n" +
                "float lumaM  = dot( rgbM, luma);\n" +
                "\n" +
                "float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));\n" +
                "float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));\n" +
                "\n" +
                "vec2 dir;\n" +
                "dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));\n" +
                "dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));\n" +
                "\n" +
                "float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);\n" +
                "\n" +
                "float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);\n" +
                "\n" +
                "dir = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX), \n" +
                "      max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) / viewSize;\n" +
                "\n" +
                "vec3 rgbA = (1.0/2.0) * (\n" +
                "            texture2D(texture, vUV.xy + dir * (1.0/3.0 - 0.5)).xyz +\n" +
                "            texture2D(texture, vUV.xy + dir * (2.0/3.0 - 0.5)).xyz);\n" +
                "vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (\n" +
                "            texture2D(texture, vUV.xy + dir * (0.0/3.0 - 0.5)).xyz +\n" +
                "            texture2D(texture, vUV.xy + dir * (3.0/3.0 - 0.5)).xyz);\n" +
                "float lumaB = dot(rgbB, luma);\n" +
                "\n" +
                "if((lumaB < lumaMin) || (lumaB > lumaMax)){\n" +
                    "color.xyz=rgbA;\n" +
                "} else {\n" +
                    "color.xyz=rgbB;\n" +
                "}\n" +
            "}\n";
    }
}
