#include "getPlatformInfo.h"
#include "glwidget.h"
#include <iostream>

const char* decodeCompressedTextureFormat(int i) {
    switch (i) {
    case GL_COMPRESSED_RED:
        return "GL_COMPRESSED_RED";
        break;
    case GL_COMPRESSED_RG:
        return "GL_COMPRESSED_RG";
        break;
    case GL_COMPRESSED_RGB:
        return "GL_COMPRESSED_RGB";
        break;
    case GL_COMPRESSED_RGBA:
        return "GL_COMPRESSED_RGBA";
        break;
    case GL_COMPRESSED_SRGB:
        return "GL_COMPRESSED_SRGB";
        break;
    case GL_COMPRESSED_SRGB_ALPHA:
        return "GL_COMPRESSED_SRGB_ALPHA";
        break;
    case GL_COMPRESSED_ALPHA:
        return "GL_COMPRESSED_ALPHA";
        break;    
    case GL_COMPRESSED_LUMINANCE:
        return "GL_COMPRESSED_LUMINANCE";
        break;    
    case GL_COMPRESSED_LUMINANCE_ALPHA:
        return "GL_COMPRESSED_LUMINANCE_ALPHA";
        break;    
    case GL_COMPRESSED_INTENSITY:
        return "GL_COMPRESSED_INTENSITY";
        break;    
    case GL_COMPRESSED_SLUMINANCE:
        return "GL_COMPRESSED_SLUMINANCE";
        break;    
    case GL_COMPRESSED_SLUMINANCE_ALPHA:
        return "GL_COMPRESSED_SLUMINANCE_ALPHA";
        break;    
    case GL_COMPRESSED_RED_RGTC1:
        return "GL_COMPRESSED_RED_RGTC1";
        break;    
    case GL_COMPRESSED_SIGNED_RED_RGTC1:
        return "GL_COMPRESSED_SIGNED_RED_RGTC1";
        break;    
    case GL_COMPRESSED_RG_RGTC2:
        return "GL_COMPRESSED_RG_RGTC2";
        break;    
    case GL_COMPRESSED_SIGNED_RG_RGTC2:
        return "GL_COMPRESSED_SIGNED_RG_RGTC2";
        break;    
    case GL_COMPRESSED_RGBA_BPTC_UNORM:
        return "GL_COMPRESSED_RGBA_BPTC_UNORM";
        break;    
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
        return "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
        break;    
    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
        return "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
        break;    
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
        return "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";
        break;    
    case GL_COMPRESSED_RGB8_ETC2:
        return "GL_COMPRESSED_RGB8_ETC2";
        break;    
    case GL_COMPRESSED_SRGB8_ETC2:
        return "GL_COMPRESSED_SRGB8_ETC2";
        break;    
    case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
        return "GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2";
        break;    
    case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
        return "GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2";
        break;    
    case GL_COMPRESSED_RGBA8_ETC2_EAC:
        return "GL_COMPRESSED_RGBA8_ETC2_EAC";
        break;    
    case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
        return "GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC";
        break;    
    case GL_COMPRESSED_R11_EAC:
        return "GL_COMPRESSED_R11_EAC";
        break;    
    case GL_COMPRESSED_SIGNED_R11_EAC:
        return "GL_COMPRESSED_SIGNED_R11_EAC";
        break;    
    case GL_COMPRESSED_RG11_EAC:
        return "GL_COMPRESSED_RG11_EAC";
        break;    
    case GL_COMPRESSED_SIGNED_RG11_EAC:
        return "GL_COMPRESSED_SIGNED_RG11_EAC";
        break;    
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        return "GL_COMPRESSED_RGB_S3TC_DXT1_EXT";
        break;    
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        return "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT";
        break;    
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        return "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT";
        break;    
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        return "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT";
        break;    
    case GL_COMPRESSED_RGB_FXT1_3DFX:
        return "GL_COMPRESSED_RGB_FXT1_3DFX";
        break;    
    case GL_COMPRESSED_RGBA_FXT1_3DFX:
        return "GL_COMPRESSED_RGBA_FXT1_3DFX";
        break;    
    default:
        return "--- UNKNOWN  ---";
    }
}

const char* decodeHint(int ans) {
    switch (ans) {
    case GL_FASTEST:
        return "GL_FASTEST";
        break;
    case GL_NICEST:
        return "GL_NICEST";
        break;    
    case GL_DONT_CARE:
        return "GL_DONT_CARE";
        break;
    default:
        return "--- UNKNOWN  ---";
    }
}


void GetPlatformInfo::onPluginLoad()
{
	GLWidget &g = *glwidget();
	g.makeCurrent();
	
	GLfloat fbuff[128];
	GLint   ibuff[128];

	g.glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, fbuff);
	cout << endl << "GL_ALIASED_LINE_WIDTH_RANGE:        " << fbuff[0] << ", " << fbuff[1] << endl;
	
	g.glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, fbuff);
	cout << "GL_SMOOTH_LINE_WIDTH_RANGE:         " << fbuff[0] << ", " << fbuff[1] << endl;
	
	g.glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, fbuff);
	cout << "GL_SMOOTH_LINE_WIDTH_GRANULARITY:   " << fbuff[0] << endl << endl;
	
	g.glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, ibuff);
	cout << "GL_NUM_COMPRESSED_TEXTURE_FORMATS,  " << ibuff[0] <<endl;
	int numformats = ibuff[0];
	g.glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, ibuff);
	cout << "GL_COMPRESSED_TEXTURE_FORMATS:" << endl;
	for (unsigned int i=0; int(i) < numformats; ++i) {
	    cout << "                                    " << "0x" << hex << ibuff[i] << ": " 
	         << decodeCompressedTextureFormat(ibuff[i]) << endl;
	}
    cout << dec << endl;
    
    g.glGetIntegerv(GL_DOUBLEBUFFER, ibuff);
    cout << "GL_DOUBLEBUFFER:                    " << (ibuff[0]?"true":"false") << endl;
    g.glGetIntegerv(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, ibuff);
    cout << "GL_FRAGMENT_SHADER_DERIVATIVE_HINT: " << decodeHint(ibuff[0]) << endl;
    g.glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, ibuff);
    cout << "GL_MAX_3D_TEXTURE_SIZE:             " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_CLIP_DISTANCES, ibuff);
    cout << "GL_MAX_CLIP_DISTANCES:              " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, ibuff);
    cout << "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS: " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, ibuff);
    cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, ibuff);
    cout << "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS: " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, ibuff);
    cout << "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS: " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_VARYING_COMPONENTS, ibuff);
    cout << "GL_MAX_VARYING_COMPONENTS:          " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, ibuff);
    cout << "GL_MAX_COMBINED_UNIFORM_BLOCKS:     " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, ibuff);
    cout << "GL_MAX_CUBE_MAP_TEXTURE_SIZE:       " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_DRAW_BUFFERS, ibuff);
    cout << "GL_MAX_DRAW_BUFFERS:                " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, ibuff);
    cout << "GL_MAX_DUAL_SOURCE_DRAW_BUFFERS:    " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_ELEMENTS_INDICES, ibuff);
    cout << "GL_MAX_ELEMENTS_INDICES:            " << ibuff[0] << endl;
    g.glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, ibuff);
    cout << "GL_MAX_ELEMENTS_VERTICES:           " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, ibuff);
    cout << "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS: " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS , ibuff);
    cout << "GL_MAX_FRAGMENT_UNIFORM_BLOCKS:     " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, ibuff);
    cout << "GL_MAX_FRAGMENT_INPUT_COMPONENTS:   " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, ibuff);
    cout << "GL_MIN_PROGRAM_TEXEL_OFFSET:        " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, ibuff);
    cout << "GL_MAX_PROGRAM_TEXEL_OFFSET:        " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, ibuff);
    cout << "GL_MAX_RECTANGLE_TEXTURE_SIZE:      " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, ibuff);
    cout << "GL_MAX_TEXTURE_IMAGE_UNITS:         " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_TEXTURE_LOD_BIAS, ibuff);
    cout << "GL_MAX_TEXTURE_LOD_BIAS:            " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_TEXTURE_SIZE, ibuff);
    cout << "GL_MAX_TEXTURE_SIZE:                " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, ibuff);
    cout << "GL_MAX_RENDERBUFFER_SIZE:           " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, ibuff);
    cout << "GL_MAX_ARRAY_TEXTURE_LAYERS:        " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, ibuff);
    cout << "GL_MAX_TEXTURE_BUFFER_SIZE:         " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, ibuff);
    cout << "GL_MAX_UNIFORM_BLOCK_SIZE:          " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_VARYING_FLOATS, ibuff);
    cout << "GL_MAX_VARYING_FLOATS:              " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, ibuff);
    cout << "GL_MAX_VERTEX_ATTRIBS:              " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, ibuff);
    cout << "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:  " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, ibuff);
    cout << "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS: " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, ibuff);
    cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS:   " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, ibuff);
    cout << "GL_MAX_VERTEX_OUTPUT_COMPONENTS:    " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, ibuff);
    cout << "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS: " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, ibuff);
    cout << "GL_MAX_SAMPLE_MASK_WORDS:           " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, ibuff);
    cout << "GL_MAX_COLOR_TEXTURE_SAMPLES:       " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, ibuff);
    cout << "GL_MAX_DEPTH_TEXTURE_SAMPLES:       " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_INTEGER_SAMPLES, ibuff);
    cout << "GL_MAX_INTEGER_SAMPLES:             " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT, ibuff);
    cout << "GL_MAX_SERVER_WAIT_TIMEOUT:         " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, ibuff);
    cout << "GL_MAX_UNIFORM_BUFFER_BINDINGS:     " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, ibuff);
    cout << "GL_MAX_UNIFORM_BLOCK_SIZE:          " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, ibuff);
    cout << "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT: " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, ibuff);
    cout << "GL_MAX_VERTEX_UNIFORM_BLOCKS:       " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, ibuff);
    cout << "GL_MAX_GEOMETRY_UNIFORM_BLOCKS:     " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, ibuff);
    cout << "GL_MAX_GEOMETRY_INPUT_COMPONENTS:   " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, ibuff);
    cout << "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS:  " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAX_VIEWPORT_DIMS, ibuff);
    cout << "GL_MAX_VIEWPORT_DIMS:               " << ibuff[0] << ", " << ibuff[1] << endl;        
    g.glGetFloatv(GL_POINT_SIZE_GRANULARITY, fbuff);
    cout << "GL_POINT_SIZE_GRANULARITY:          " << fbuff[0] << endl;        
    g.glGetFloatv(GL_POINT_SIZE_RANGE, fbuff);
    cout << "GL_POINT_SIZE_RANGE:                " << fbuff[0] << ", " << fbuff[1] << endl;
    g.glGetFloatv(GL_POINT_FADE_THRESHOLD_SIZE, fbuff);
    cout << "GL_POINT_FADE_THRESHOLD_SIZE:       " << fbuff[0] << endl;        
    g.glGetIntegerv(GL_SAMPLE_BUFFERS, ibuff);
    cout << "GL_SAMPLE_BUFFERS:                  " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_SAMPLES, ibuff);
    cout << "GL_SAMPLES:                         " << ibuff[0] << endl;
    g.glGetIntegerv(GL_STEREO, ibuff);
    cout << "GL_STEREO:                          " << (ibuff[0]?"true":"false") << endl;        
    g.glGetIntegerv(GL_SUBPIXEL_BITS, ibuff);
    cout << "GL_SUBPIXEL_BITS:                   " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_NUM_EXTENSIONS, ibuff);
    cout << "GL_NUM_EXTENSIONS:                  " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MAJOR_VERSION, ibuff);
    cout << "GL_MAJOR_VERSION                    " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_MINOR_VERSION, ibuff);
    cout << "GL_MINOR_VERSION:                   " << ibuff[0] << endl;        
    g.glGetIntegerv(GL_CONTEXT_FLAGS, ibuff);
    cout << "GL_CONTEXT_FLAGS:                   " << "0x" << hex << ibuff[0] 
         << dec << endl;
}

