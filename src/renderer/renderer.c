
#include "base.h"
#include "elements.h"
#include "platform_api.h"
#include "renderer_api.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BASE FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


RID R_CreateTextureEx(R_Renderer *rend, R_CreationFlags flags, R_Format format, vec2i reso, u32 stride, void *data) {
	RID texture = R_CreateTexturePro(rend, (R_CreateTextureParams) {
		.flags    = flags,
		.format   = format,
		.reso     = reso,
		.stride   = stride,
		.data     = data,
	});
	return texture;
}

RID R_CreateInputOutputTexture(R_Renderer *rend, R_Format format, vec2i reso) {
	return R_CreateTextureEx(rend, R_BIND_INPUT | R_BIND_OUTPUT, format, reso, 0, 0);
}

RID R_CreateTexture(R_Renderer *rend, R_Format format, vec2i reso, u32 stride, void *data) {
	return R_CreateTextureEx(rend, R_BIND_INPUT, format, reso, stride, data);
}


// PRIVATE
typedef struct {
	R_Format   format;
	vec2i        reso;
	const char *label;
} R_InitBaseTextureParams;

static void R_InitBaseTexture(R_Texture *texture, R_InitBaseTextureParams params) {
	texture->format = params.format;
	texture->reso   = params.reso;
	// Todo, don't do this...
	if (params.format == R_FORMAT_R_U8) {
		texture->mixer.rows[0] = (vec4){ 1, 0, 0, 0 };
		texture->mixer.rows[1] = (vec4){ 1, 0, 0, 0 };
		texture->mixer.rows[2] = (vec4){ 1, 0, 0, 0 };
		texture->mixer.rows[3] = (vec4){ 1, 0, 0, 0 };
	} else {
		texture->mixer = IDENTITY_MATRIX();
	}
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// D3D BACKEND /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define _CRT_SECURE_NO_WARNINGS
#define              CINTERFACE
#define              COBJMACROS
#define        D3D11_NO_HELPERS
#pragma comment(lib,        "Gdi32")
#pragma comment(lib,       "dxguid")
#pragma comment(lib,        "d3d11")
#pragma comment(lib,  "d3dcompiler")


// todo:
#define FLUSHHACK

#include <d3dcompiler.h>
#include   <dxgidebug.h>
#include        <dxgi.h>
#include       <d3d11.h>
#include     <dxgi1_3.h>


OS_GET_WINDOW_HANDLE_API


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HELPER MACROS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#define DX_RELEASE(I) ((I) ? ((I)->lpVtbl->Release(I)) : 0)




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERNAL STRUCTURES /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct D3D_Texture D3D_Texture;
struct D3D_Texture {
	R_Texture                 base;

	union {
		ID3D11Texture2D       *texture;
		ID3D11Resource        *texture_resource;
	};
	ID3D11ShaderResourceView *shader_resource_view;
	ID3D11RenderTargetView   *render_target_view;
};

typedef struct D3D_Window D3D_Window;
struct D3D_Window {
	R_Window base;
	IDXGISwapChain2 *swapchain;
	ID3D11Texture2D *staging_texture;
	D3D_Texture      offscreen_render_target;
	D3D_Texture      swapchain_buffer;
};

typedef struct D3D_CBUFFER D3D_CBUFFER;
struct D3D_CBUFFER {
	Matrix transform;
	Matrix mixer;
	vec2   texture0_reso;
	vec2   padd;
};

STATIC_ASSERT(!(sizeof(D3D_CBUFFER) & 15) && sizeof(D3D_CBUFFER) >= 64 && sizeof(D3D_CBUFFER) <= D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT);

enum {
	VBUFFER_CAPACITY = MEGABYTES(1),
};

typedef struct {
	RID                  output;
	rect_i32           viewport;
	RID                 texture;
	rect_i32            scissor;
	R_Sampler           sampler;
	R_Blender           blender;
	R_Topology         topology;
	ID3D11PixelShader  *pshader;
	ID3D11VertexShader *vshader;
	ID3D11Buffer       *vbuffer;
	ID3D11InputLayout  *ilayout;
	u32                 vstride;
} D3D_PipelineState;

struct R_Renderer {
	ID3D11InfoQueue         *info;
	ID3D11Device            *device;

	ID3D11DeviceContext     *context;
	D3D_PipelineState        state;

	ID3D11Buffer            *vbuffer;
	ID3D11RasterizerState   *default_rasterizer;
	ID3D11DepthStencilState *default_depth_stencil;
	ID3D11Buffer            *cbuffer;

	ID3D11PixelShader       *pshaders[R_SHADER_COUNT_];
	ID3D11VertexShader      *vshaders[R_SHADER_COUNT_];
	ID3D11InputLayout       *ilayouts[R_SHADER_COUNT_];
	u32                      vstrides[R_SHADER_COUNT_];
	ID3D11BlendState        *blenders[R_BLENDER_COUNT_];
	ID3D11SamplerState      *samplers[R_SAMPLER_COUNT_];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BASE -> D3D /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static D3D11_PRIMITIVE_TOPOLOGY D3D11PrimitiveTopologyFromBase(R_Topology topo)  {
	switch (topo) {
		case R_TOPO_TRIANGLES: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case R_TOPO_LINES:     return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		default: ;
	}
	return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

static DXGI_FORMAT DXGIFormatFromBase(R_Format format) {
	switch (format) {
		case R_FORMAT_R_U8:     return DXGI_FORMAT_R8_UNORM;
		case R_FORMAT_RGBA_U8:  return DXGI_FORMAT_R8G8B8A8_UNORM;
		case R_FORMAT_RGBA_F32: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default: ;
	}
	return DXGI_FORMAT_UNKNOWN;
}

static inline D3D_Texture *D3DTextureFromBase(R_Texture *texture) {
	return (D3D_Texture *) texture;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMBDEDDED ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Todo, support SDF rectangles, instanced rendering ...

global const char *d3d_g_embedded_text__default_shader_source =
"		cbuffer Constants : register(b0) {                                                                         \n"
"			float4x4	transform;                                                                                     \n"
"			float4x4	color_transform;                                                                               \n"
"			float2	texture0_reso;                                                                                 \n"
"		}                                                                                                          \n"
"		struct Vertex_In {                                                                                         \n"
"			float3 position   : POSITION;                                                                           \n"
"			float2 texcoords  : TEXCOORD;                                                                           \n"
"			float4 color      : COLOR;                                                                              \n"
"			float2 tblend     : TBLND;                                                                              \n"
"		};                                                                                                         \n"
"		struct Vertex_Out {                                                                                        \n"
"			float4 position  : SV_Position;                                                                         \n"
"			float2 texcoords : TEXCOORD;                                                                            \n"
"			float4 color     : COLOR;                                                                               \n"
"			float2 tblend    : TBLND;                                                                               \n"
"		};                                                                                                         \n"
"		Texture2D    main_t2d     : register(t0);                                                                  \n"
"		SamplerState main_sampler : register(s0);                                                                  \n"
"                                                                                                                \n"
"		Vertex_Out VS_FSBlit(uint vertex_id: SV_VertexID) {                                                        \n"
"			float2 pos_array[6] = {                                                                                 \n"
"				{0, 0},{0, 1},{1, 1},                                                                                \n"
"				{0, 0},{1, 1},{1, 0},                                                                                \n"
"			};                                                                                                      \n"
"			float2 tex_array[6] = {                                                                                 \n"
"				{0, 1},{0, 0},{1, 0},                                                                                \n"
"				{0, 1},{1, 0},{1, 1},                                                                                \n"
"			};                                                                                                      \n"
"			Vertex_Out output;                                                                                      \n"
"			output.position = float4(pos_array[vertex_id] * 2.f - 1.f, 0, 1);                                       \n"
"			output.texcoords = tex_array[vertex_id];                                                                \n"
"			output.color = float4(1.f, 1.f, 1.f, 1.f);                                                              \n"
"			output.tblend = 1.f;                                                                                    \n"
"        return output;                                                                                          \n"
"     }                                                                                                          \n"
"                                                                                                                \n"
"		Vertex_Out VS_Main(Vertex_In input, uint vertex_id: SV_VertexID) {                                         \n"
"			Vertex_Out output;                                                                                      \n"
"			output.position = mul(transform, float4(input.position, 1));                                            \n"
"			output.texcoords = input.texcoords;                                                                     \n"
"			output.color = input.color;                                                                             \n"
"			output.tblend = input.tblend;                                                                           \n"
"			return output;                                                                                          \n"
"		}                                                                                                          \n"
"                                                                                                                \n"
"                                                                                                                \n"
"		float4 PS_Main(Vertex_Out input) : SV_TARGET {                                                             \n"
"			float4 color = input.color;                                                                             \n"
"			float4 sample = main_t2d.Sample(main_sampler, input.texcoords);                                         \n"
"		   float4 sample_color = mul(color_transform, sample) * color;                                             \n"
"			float4 final_color = lerp(color, sample_color, input.tblend.x);                                         \n"
"			return final_color;                                                                                     \n"
"		}                                                                                                          \n"
"		                                                                                                           \n";



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


R_Renderer *R_InitRenderer() {

	// Todo, allocation!
	R_Renderer *rend = calloc(1, sizeof(*rend));


	ID3D11InfoQueue *info = NULL;
	ID3D11Device *device = NULL;
	ID3D11DeviceContext *context = NULL;


	i32 device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
   // these crash intel's graphic analyzer on my machine for some reason
	device_flags |= D3D11_CREATE_DEVICE_DEBUG|D3D11_CREATE_DEVICE_SINGLETHREADED;

	HRESULT hr;
	{
		D3D_FEATURE_LEVEL requires[] = {D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0};
		hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, device_flags
		,                      requires, _countof(requires), D3D11_SDK_VERSION, &device, NULL, &context);
	}

	if (FAILED(hr))
	{
		D3D_FEATURE_LEVEL requires[] = {D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0};
		hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, 0, device_flags
		,                      requires, _countof(requires), D3D11_SDK_VERSION, &device, NULL, &context);
		ASSERT(SUCCEEDED(hr));
	}


	if (device_flags & D3D11_CREATE_DEVICE_DEBUG)
	{
		hr = ID3D11Device_QueryInterface(device, &IID_ID3D11InfoQueue, (void **) & info);
		if (SUCCEEDED(hr)) {

			{
				D3D11_MESSAGE_ID id = D3D11_MESSAGE_ID_OMSETRENDERTARGETS_UNBINDDELETINGOBJECT;
				D3D11_INFO_QUEUE_FILTER filter = {
					.DenyList.NumIDs = 1,
					.DenyList.pIDList = &id,
				};
				hr = ID3D11InfoQueue_AddStorageFilterEntries(info, &filter);
				ASSERT(SUCCEEDED(hr));
			}

			ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_WARNING,    TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_ERROR,      TRUE);
		}
	}

	rend->device = device;
	rend->context = context;
	rend->info = info;

	////////////////////////////////////////////////
	// DEFAULT RESOURCES
	////////////////////////////////////////////////

	{
		D3D11_DEPTH_STENCIL_DESC desc = {
			.DepthEnable                  = FALSE,
			.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL,
			.DepthFunc                    = D3D11_COMPARISON_GREATER,
			.StencilEnable                = FALSE,
			.StencilReadMask              = D3D11_DEFAULT_STENCIL_READ_MASK,
			.StencilWriteMask             = D3D11_DEFAULT_STENCIL_WRITE_MASK,
			.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP,
			.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR,
			.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP,
			.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS,
			.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP,
			.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR,
			.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP,
			.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS,
		};

		hr = ID3D11Device_CreateDepthStencilState(device, &desc, &rend->default_depth_stencil);
		ASSERT(SUCCEEDED(hr));
	}


	{
		D3D11_RASTERIZER_DESC desc = {
			.FillMode               = D3D11_FILL_SOLID,
			.CullMode               = D3D11_CULL_NONE,
			.FrontCounterClockwise  = FALSE,
			.DepthBias              = D3D11_DEFAULT_DEPTH_BIAS,
			.DepthBiasClamp         = D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
			.SlopeScaledDepthBias   = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			.DepthClipEnable        = FALSE,
			.ScissorEnable          = TRUE,
			.MultisampleEnable      = FALSE,
			.AntialiasedLineEnable  = FALSE,
		};
		hr = ID3D11Device_CreateRasterizerState(device, &desc, &rend->default_rasterizer);
		ASSERT(SUCCEEDED(hr));
	}

	// BLENDERS
	{
		D3D11_BLEND_DESC desc = {
			.RenderTarget[0].BlendEnable = FALSE,
			.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
		};
		hr = ID3D11Device_CreateBlendState(device, &desc, &rend->blenders[R_BLENDER_NONE]);
		ASSERT(SUCCEEDED(hr));
	}

	{
		D3D11_BLEND_DESC desc = {
			.RenderTarget[0].BlendEnable           = TRUE,
			.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA,
			.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA,
			.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD,
			.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO,
			.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO,
			.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD,
			.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
		};
		hr = ID3D11Device_CreateBlendState(device, &desc, &rend->blenders[R_BLENDER_ALPHA_BLEND]);
		ASSERT(SUCCEEDED(hr));
	}

	// SAMPLERS
	{
		D3D11_SAMPLER_DESC desc = {
			.ComparisonFunc  = D3D11_COMPARISON_NEVER,
			.AddressU        = D3D11_TEXTURE_ADDRESS_CLAMP,
			.AddressV        = D3D11_TEXTURE_ADDRESS_CLAMP,
			.AddressW        = D3D11_TEXTURE_ADDRESS_CLAMP,
			.MinLOD          = 0,
			.MaxLOD          = D3D11_FLOAT32_MAX,
		};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		hr = ID3D11Device_CreateSamplerState(device, &desc, &rend->samplers[R_SAMPLER_POINT]);
		ASSERT(SUCCEEDED(hr));

		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = ID3D11Device_CreateSamplerState(device, &desc, &rend->samplers[R_SAMPLER_LINEAR]);
		ASSERT(SUCCEEDED(hr));
	}

	// VERTEX BUFFER
	{
		D3D11_BUFFER_DESC desc = {
			.Usage               = D3D11_USAGE_DYNAMIC,
			.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags           = 0,
			.StructureByteStride = 0,
			.BindFlags           = D3D11_BIND_VERTEX_BUFFER,
			.ByteWidth           = VBUFFER_CAPACITY,
		};
		hr = ID3D11Device_CreateBuffer(device, &desc, NULL, &rend->vbuffer);
		ASSERT(SUCCEEDED(hr));
	}

	// CONSTANT BUFFER
	{
		// must be 16 byte aligned
		i32 size = sizeof(D3D_CBUFFER) + 15 & ~15;
		D3D11_BUFFER_DESC desc = {
			.ByteWidth      = size,
			.Usage          = D3D11_USAGE_DYNAMIC,
			.BindFlags      = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		};
		hr = ID3D11Device_CreateBuffer(device, &desc, 0, &rend->cbuffer);
		ASSERT(SUCCEEDED(hr));
	}

	// DEFAULT SHADERS
	{
		const char *shader_name = "SimpleShader";

		const char *shader_source = d3d_g_embedded_text__default_shader_source;
		u32         shader_length = strlen(shader_source);

		u32 compilation_flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR|D3DCOMPILE_WARNINGS_ARE_ERRORS;

		if (device_flags & D3D11_CREATE_DEVICE_DEBUG) {
			compilation_flags |= D3DCOMPILE_SKIP_OPTIMIZATION|D3DCOMPILE_DEBUG;
		}

		enum {
			VS_MAIN,
			VS_FS_BLIT,
			VS_COUNT_,
		};
		const char *vs_entries[] = {
			[VS_MAIN]    = "VS_Main",
			[VS_FS_BLIT] = "VS_FSBlit",
		};

		typedef struct {
			ID3DBlob           *bytecode_b;
			ID3D11VertexShader *vshader;
		} VShader;
		VShader vshaders[VS_COUNT_];

		for (u32 i = 0; i < VS_COUNT_; ++i)
		{
			ID3D11VertexShader *vshader = NULL;
			ID3DBlob *bytecode_b = NULL;
			ID3DBlob *messages_b = NULL;
			{
				hr = D3DCompile(shader_source, shader_length, vs_entries[i], 0, 0, vs_entries[i], "vs_5_0", compilation_flags, 0, &bytecode_b, &messages_b);
				if (SUCCEEDED(hr)) {

					hr = ID3D11Device_CreateVertexShader(rend->device
					,                                    bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
					,                                    bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
					,                                    NULL, &vshader);
					ASSERT(SUCCEEDED(hr));

				}
				if (messages_b) {
					TRACELOG("%s",(char*) messages_b->lpVtbl->GetBufferPointer(messages_b));
				}
			}
			vshaders[i].vshader = vshader;
			vshaders[i].bytecode_b = bytecode_b;

			DX_RELEASE(messages_b);
		}

		enum {
			PS_MAIN,
			PS_COUNT_,
		};
		const char *ps_entries[] = {
			[PS_MAIN] = "PS_Main",
		};

		ID3D11PixelShader *pshaders[PS_COUNT_];

		for (u32 i = 0; i < PS_COUNT_; ++i)
		{
			ID3D11PixelShader *pshader = NULL;

			{
				ID3DBlob *bytecode_b = NULL;
				ID3DBlob *messages_b = NULL;

				hr = D3DCompile(shader_source, shader_length, ps_entries[i], 0, 0, ps_entries[i], "ps_5_0", compilation_flags, 0, &bytecode_b, &messages_b);

				if (SUCCEEDED(hr)) {
					hr = ID3D11Device_CreatePixelShader(rend->device
					,                                   bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
					,                                   bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
					,                                   NULL, &pshader);
					ASSERT(SUCCEEDED(hr));
				}

				if (messages_b) {
					TRACELOG("%s",(char*) messages_b->lpVtbl->GetBufferPointer(messages_b));
				}

				DX_RELEASE(bytecode_b);
				DX_RELEASE(messages_b);
			}

			pshaders[i] = pshader;
		}

		{
			ID3D11InputLayout  *ilayout = NULL;
			u32                 vstride = sizeof(R_Vertex);

			ID3DBlob *bytecode_b = vshaders[VS_MAIN].bytecode_b;

			{
				D3D11_INPUT_ELEMENT_DESC desc[] = {
					{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT  , 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT     , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR"   ,  0, DXGI_FORMAT_R8G8B8A8_UNORM   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TBLND"   ,  0, DXGI_FORMAT_R32G32_FLOAT     , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};


				hr = ID3D11Device_CreateInputLayout(rend->device
				,                                   desc, COUNTOF(desc)
				,                                   bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
				,                                   bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
				,                                   &ilayout);

				ASSERT(SUCCEEDED(hr));
			}

			DX_RELEASE(bytecode_b);

			rend->vshaders[R_SHADER_SIMPLE] = vshaders[VS_MAIN].vshader;
			rend->vstrides[R_SHADER_SIMPLE] = vstride;
			rend->ilayouts[R_SHADER_SIMPLE] = ilayout;
			rend->pshaders[R_SHADER_SIMPLE] = pshaders[PS_MAIN];

			rend->vshaders[R_SHADER_FS_BLIT] = vshaders[VS_FS_BLIT].vshader;
			rend->vstrides[R_SHADER_FS_BLIT] = 0;
			rend->ilayouts[R_SHADER_FS_BLIT] = 0;
			rend->pshaders[R_SHADER_FS_BLIT] = pshaders[PS_MAIN];
		}
	}


	// DEFAULT PIPELINE STATE
	ID3D11DeviceContext_OMSetDepthStencilState(rend->context, 0, 0);
	ID3D11DeviceContext_RSSetState(rend->context, rend->default_rasterizer);
	ID3D11DeviceContext_VSSetConstantBuffers(rend->context, 0, 1, &rend->cbuffer);
	ID3D11DeviceContext_PSSetConstantBuffers(rend->context, 0, 1, &rend->cbuffer);

	return rend;
}

static void D3DUpdatePipelineState(R_Renderer *rend, D3D_PipelineState state) {
	ASSERT(state.sampler != R_SAMPLER_NONE);
	ASSERT(state.pshader != 0);
	ASSERT(state.vshader != 0);
	ASSERT(state.ilayout != 0);
	ASSERT(state.texture != RID_NONE);
	ASSERT(state.vstride != 0);


	if (rend->state.output != state.output) {

		if (rend->state.texture == state.output) {
			ID3D11ShaderResourceView *nullptr_shader_resource_view = NULL;
			ID3D11DeviceContext_PSSetShaderResources(rend->context, 0, 1, &nullptr_shader_resource_view);
		}

		D3D_Texture *texture = (D3D_Texture *) state.output;
		ID3D11RenderTargetView *render_target_view = texture->render_target_view;
		ID3D11DeviceContext_OMSetRenderTargets(rend->context, 1, &render_target_view, 0);
	}

	if (rend->state.blender != state.blender) {
		ID3D11BlendState *blender = rend->blenders[state.blender];
		ID3D11DeviceContext_OMSetBlendState(rend->context, blender, 0, 0xffffffff);
	}

	if (!RECT_MATCH(rend->state.viewport, state.viewport)) {
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, state.viewport.w, state.viewport.h, 0.0f, 1.0f };
		ID3D11DeviceContext_RSSetViewports(rend->context, 1, &viewport);
	}

	if (!RECT_MATCH(rend->state.scissor, state.scissor)) {
		D3D11_RECT rect = {
			.left   = state.scissor.x,
			.right  = state.scissor.x + state.scissor.w,
			.top    = state.scissor.y,
			.bottom = state.scissor.y + state.scissor.h,
		};
		ID3D11DeviceContext_RSSetScissorRects(rend->context, 1, &rect);
	}

	if (rend->state.vshader != state.vshader) {
		ID3D11VertexShader *vshader = state.vshader;
		ID3D11DeviceContext_VSSetShader(rend->context, vshader, 0, 0);
	}

	if (rend->state.pshader != state.pshader) {
		ID3D11PixelShader *pshader = state.pshader;
		ID3D11DeviceContext_PSSetShader(rend->context, pshader, 0, 0);
	}

	if (rend->state.texture != state.texture) {
		D3D_Texture *texture = (D3D_Texture *) state.texture;
		ID3D11ShaderResourceView *shader_resource_view = texture->shader_resource_view;

		ID3D11DeviceContext_PSSetShaderResources(rend->context, 0, 1, &shader_resource_view);
	}

	if (rend->state.sampler != state.sampler) {
		ID3D11SamplerState *sampler = rend->samplers[state.sampler];
		ID3D11DeviceContext_PSSetSamplers(rend->context, 0, 1, &sampler);
	}

	if (rend->state.ilayout != state.ilayout) {
		ID3D11DeviceContext_IASetInputLayout(rend->context, state.ilayout);
	}

	if (rend->state.topology != state.topology) {
		D3D11_PRIMITIVE_TOPOLOGY topo = D3D11PrimitiveTopologyFromBase(state.topology);
		ID3D11DeviceContext_IASetPrimitiveTopology(rend->context, topo);
	}

	// Todo, no ... remove this ...
	if (rend->state.vstride != state.vstride || rend->state.vbuffer != state.vbuffer) {
		u32 stride = state.vstride;
		u32 offset = 0;
		ID3D11DeviceContext_IASetVertexBuffers(rend->context, 0, 1, &rend->vbuffer, &stride, &offset);
	}

	rend->state = state;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// D3D-WINDOW //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

R_Texture *R_GetWindowOutput(R_Window *window) {
	D3D_Window *d_window = (D3D_Window *) window;
	return (R_Texture *) & d_window->swapchain_buffer;
}

// Todo:!
void R_PresentWindow(R_Renderer *rend, R_Window *window) {
	D3D_Window *d_window = (D3D_Window *) window;
	IDXGISwapChain_Present(d_window->swapchain, 1, 0);

	// Todo:!
	rend->state.output = 0;
}

void R_UpdateWindowTargets(R_Renderer *rend, R_Window *window, vec2i reso) {
	D3D_Window *d_window = (D3D_Window *) window;

	// Do we have to resize always, or only when the resolution is larger ???
	if (window->reso.x != reso.x || window->reso.y != reso.y) {
		window->reso = reso;

		HRESULT hr;
		{
			DX_RELEASE(d_window->swapchain_buffer.render_target_view);
			DX_RELEASE(d_window->swapchain_buffer.texture);

			hr = IDXGISwapChain_ResizeBuffers(d_window->swapchain, 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			ASSERT(SUCCEEDED(hr));

			hr = IDXGISwapChain_GetBuffer(d_window->swapchain, 0, &IID_ID3D11Texture2D, (void **)(&d_window->swapchain_buffer.texture));
			ASSERT(SUCCEEDED(hr));

			hr = ID3D11Device_CreateRenderTargetView(rend->device, d_window->swapchain_buffer.texture_resource, 0, &d_window->swapchain_buffer.render_target_view);
			ASSERT(SUCCEEDED(hr));

			R_InitBaseTextureParams init_params = {
				.format = R_FORMAT_RGBA_U8,
				.reso   = reso,
			};
			R_InitBaseTexture(&d_window->swapchain_buffer.base, init_params);
		}

		// Do we have an offscreen target?
		if (window->offscreen_target) {
			R_ReleaseTexture(rend, window->offscreen_target);

			window->offscreen_target = R_CreateInputOutputTexture(rend, R_FORMAT_RGBA_F32, reso);
		}

		// Todo, do this if recording ...
		// DX_RELEASE(rend->window_staging_texture);
		// D3D11_TEXTURE2D_DESC config = {
		// 	.Width              = target_resolution.x,
		// 	.Height             = target_resolution.y,
		// 	.MipLevels          = 1,
		// 	.ArraySize          = 1,
		// 	.Format             = g_format_conversion_table[window->pub.format],
		// 	.SampleDesc.Count   = 1,
		// 	.SampleDesc.Quality = 0,
		// 	.Usage              = D3D11_USAGE_STAGING,
		// 	.BindFlags          = 0,
		// 	.CPUAccessFlags     = D3D11_CPU_ACCESS_READ,
		// 	.MiscFlags          = 0,
		// };
		// ID3D11Texture2D *texture = NULL;
		// HRESULT hr = ID3D11Device_CreateTexture2D(rend->device, &config, NULL, &texture);
		// ASSERT(SUCCEEDED(hr));

		// rend->window_staging_texture = texture;
	}
}

R_Window *R_CreateWindow(R_Renderer *rend, WID os_window) {
	D3D_Window *window = calloc(1, sizeof(*window));

	// OS_Window -> Win32 Window
	HWND win32_window = OS_GetWindowHandle(os_window);

	IDXGISwapChain1 *sc = NULL;

	HRESULT hr;

	// SWAPCHAIN
	{
		IDXGIDevice *device_dxgi = {0};
		hr = ID3D11Device_QueryInterface(rend->device, &IID_IDXGIDevice, (void **) &device_dxgi);
		ASSERT(SUCCEEDED(hr));

		IDXGIAdapter *adapter_dxgi = {0};
		hr = IDXGIDevice_GetAdapter(device_dxgi, &adapter_dxgi);
		ASSERT(SUCCEEDED(hr));

		IDXGIFactory2 *factory_dxgi = {0};
		hr = IDXGIAdapter_GetParent(adapter_dxgi, &IID_IDXGIFactory2, (void**) &factory_dxgi);
		ASSERT(SUCCEEDED(hr));


		DXGI_SWAP_CHAIN_DESC1 sc_desc = {
			.Width               = 0,
			.Height              = 0,
			.Format              = DXGI_FORMAT_R8G8B8A8_UNORM,
			.BufferUsage         = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount         = 2,
			.SwapEffect          = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
			.Flags               = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
			.SampleDesc.Count    = 1,
			.SampleDesc.Quality  = 0,
		};

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fs_desc = {
			.RefreshRate.Numerator   = 60,
			.RefreshRate.Denominator = 1,
			.Windowed                = TRUE,
		};


		hr = IDXGIFactory2_CreateSwapChainForHwnd(factory_dxgi, (IUnknown *) rend->device, win32_window, &sc_desc, &fs_desc, NULL, &sc);
		ASSERT(SUCCEEDED(hr));


		DX_RELEASE(factory_dxgi);
		DX_RELEASE(adapter_dxgi);
		DX_RELEASE(device_dxgi);
	}

	window->swapchain = (IDXGISwapChain2 *) sc;

	return (R_Window *) window;
}


static void D3DMappedWriteDiscard(R_Renderer *rend, ID3D11Resource *resource, void *data, u32 size) {
	D3D11_MAPPED_SUBRESOURCE mapped;
	ID3D11DeviceContext_Map(rend->context, resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	CopyMemory(mapped.pData, data, size);
	ID3D11DeviceContext_Unmap(rend->context, resource, 0);
}

void R_Draw(R_Renderer *rend, R_DrawParams draw) {

	if (draw.verts_size_in_bytes == 0) {
		return;
	}

	// -- Update dirty textures
	for (u32 i = 0; i < draw.num_passes; ++ i)
	{
		R_Texture *texture = draw.passes[i].params.texture;
		if (texture->dirty)
		{
			texture->dirty = 0;
			rect_i32	region = { .size = texture->image.reso };
			R_UpdateTexture(rend, texture, region, texture->image.elem_stride, texture->image.data);
		}
	}

	// Update Vertex Buffer
	{
		ASSERT(draw.verts_size_in_bytes < VBUFFER_CAPACITY);
		D3DMappedWriteDiscard(rend, (ID3D11Resource *) rend->vbuffer, draw.verts, draw.verts_size_in_bytes);
	}

	for (u32 i = 0; i < draw.num_passes; ++ i) {
		R_PassParams params = draw.passes[i].params;
		u32 offset = draw.passes[i].offset;
		u32 length = draw.passes[i].length;

		{
			// Todo, instead of encoding the entire pipeline state, we could encode a set of instructions,
			// the instruction directly maps to a command setting a specific pipeline state ...
			// Then, for each pass we'd run the instructions, and directly call the appropriate pipeline
			// set function, without having to check and without storing so much data ...
			D3D_PipelineState pipeline_state = {
				.output   = params.output,
				.texture  = params.texture,
				.viewport = params.viewport,
				.scissor  = params.scissor,
				.sampler  = params.sampler,
				.blender  = params.blender,
				.topology = params.topology,
				.pshader  = rend->pshaders[params.shader],
				.vshader  = rend->vshaders[params.shader],
				.ilayout  = rend->ilayouts[params.shader],
				.vstride  = rend->vstrides[params.shader],
				.vbuffer  = rend->vbuffer,
			};
			D3DUpdatePipelineState(rend, pipeline_state);
		}

		{
			vec2i viewport_size = params.viewport.size;
			Matrix transform;
			// transform.rows[0] = (vec4) { 2.f / viewport_size.x, 0, 0, - 1.f };
			// transform.rows[1] = (vec4) { 0, 2.f / viewport_size.y, 0, - 1.f };
			// transform.rows[2] = (vec4) { 0, 0, 1, 0 };
			// transform.rows[3] = (vec4) { 0, 0, 0, 1 };
			transform.rows[0] = (vec4) { 1, 0, 0, 0 };
			transform.rows[1] = (vec4) { 0, 1, 0, 0 };
			transform.rows[2] = (vec4) { 0, 0, 1, 0 };
			transform.rows[3] = (vec4) { 0, 0, 0, 1 };

			D3D_CBUFFER cdata = {
				.transform       = transform,
				.mixer           = params.mixer,
				.texture0_reso.x = (f32) params.texture->reso.x,
				.texture0_reso.y = (f32) params.texture->reso.y,
			};
			D3DMappedWriteDiscard(rend, (ID3D11Resource *) rend->cbuffer, &cdata, sizeof(cdata));
		}

		ID3D11DeviceContext_Draw(rend->context, length, offset);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURES ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void R_UpdateTexture(R_Renderer *rend, RID texture, rect_i32 region, u32 stride, void *data)
{
	D3D_Texture *d_texture = D3DTextureFromBase(texture);

	D3D11_BOX box = {
		.left   = region.x,
		.right  = region.x + region.w,
		.top    = region.y,
		.bottom = region.y + region.h,
		.front  = 0,
		.back   = 1
	};
	ID3D11DeviceContext_UpdateSubresource(rend->context, d_texture->texture_resource, 0, & box, data, stride, 0);
}

void R_ReleaseTexture(R_Renderer *rend, R_Texture *texture) {
	D3D_Texture *d_texture = (D3D_Texture *) texture;
	DX_RELEASE(d_texture->texture);
	DX_RELEASE(d_texture->shader_resource_view);
	DX_RELEASE(d_texture->render_target_view);
	free(d_texture);
	ZeroMemory(d_texture, sizeof(*d_texture));
}

RID R_CreateTexturePro(R_Renderer *rend, R_CreateTextureParams params)
{
	TRACELOG("new texture");
	R_Format format = params.format;
	vec2i    reso   = params.reso;
	int      flags  = params.flags;
	void    *data   = params.data;
	u32      stride = params.stride;

	if (stride == 0) {
		stride = reso.x * R_GetFormatSize(format);
	}

	// 1x1 textures are problematic for some drivers ???
	ASSERT(reso.x >= 2);
	ASSERT(reso.y >= 2);
	ASSERT(stride >= reso.x * R_GetFormatSize(format));

	// todo: arena allocator!
	D3D_Texture *texture = calloc(sizeof(*texture), 1);
	R_InitBaseTextureParams init_params = {
		.format = format,
		.reso   = reso,
	};
	R_InitBaseTexture(&texture->base, init_params);

	texture->texture                = 0;
	texture->shader_resource_view   = 0;
	texture->render_target_view     = 0;


	// .Usage          = D3D11_USAGE_DYNAMIC
	// .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	{
		D3D11_TEXTURE2D_DESC desc = {
			.Width              = reso.x,
			.Height             = reso.y,
			.MipLevels          = 1,
			.ArraySize          = 1,
			.Format             = DXGIFormatFromBase(format),
			.SampleDesc.Count   = 1,
			.SampleDesc.Quality = 0,
			.Usage              = D3D11_USAGE_DEFAULT,
			.BindFlags          = 0,
			.CPUAccessFlags     = 0,
			.MiscFlags          = 0,
		};

		if (flags & R_BIND_INPUT)  desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (flags & R_BIND_OUTPUT) desc.BindFlags |= D3D11_BIND_RENDER_TARGET;



		D3D11_SUBRESOURCE_DATA data_desc = {
			.pSysMem     = data,
			.SysMemPitch = stride,
		};

		D3D11_SUBRESOURCE_DATA *pdata_desc = 0;
		if (data) { pdata_desc = &data_desc; }

		HRESULT hr = ID3D11Device_CreateTexture2D(rend->device, &desc, pdata_desc, &texture->texture);
		ASSERT(SUCCEEDED(hr));
	}

	if (flags & R_BIND_INPUT) {
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {
			.Format                    = DXGI_FORMAT_UNKNOWN,
			.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D,
			.Texture2D.MostDetailedMip = 0,
			.Texture2D.MipLevels       = 1,
		};

		HRESULT hr = ID3D11Device_CreateShaderResourceView(rend->device, texture->texture_resource, &desc, &texture->shader_resource_view);
		ASSERT(SUCCEEDED(hr));
	}

	if (flags & R_BIND_OUTPUT) {
		HRESULT hr = ID3D11Device_CreateRenderTargetView(rend->device, texture->texture_resource, 0, &texture->render_target_view);
		ASSERT(SUCCEEDED(hr));
	}

	return (RID) texture;
}

void R_ClearOutput(R_Renderer *rend, RID texture, Color color) {
	D3D_Texture *d_texture = D3DTextureFromBase(texture);
	ID3D11RenderTargetView *render_target_view = d_texture->render_target_view;
	if (!render_target_view) {
		OS_ShowErrorMessage("no render target bound");
	}
	// todo: why do I want to defer this until we actually draw something?
	const f32 inv = 1.0 / 255.0;
	f32 fcolor[4] = { color.r * inv, color.g * inv, color.b * inv, color.a * inv };
	ID3D11DeviceContext_ClearRenderTargetView(rend->context, render_target_view, fcolor);
}

