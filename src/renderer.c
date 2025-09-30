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

#define _WIN32_API
#include "elements.h"
#include "platform.h"
#include "renderer.h"

#define DX_RELEASE(I) ((I)->lpVtbl->Release(I))
#define DX_RELEASE_MAYBE(I) ((I) ? DX_RELEASE(I) : 0)



enum {
	STATE_OUTPUT,
	STATE_INPUT,
	STATE_SAMPLER,
	STATE_TOPOLOGY,
	STATE_SHADER,
	STATE_BLENDER,
	STATE_INT_COUNT,
};

typedef struct R_Draw_State {
	vec2i            viewport;
	union {
		i64           states[STATE_INT_COUNT];
		struct {
			RID  output;
			RID  texture;
			i64  sampler;
			i64  topology;
			i64  shader;
			i64  blender;
		};
	};
} R_Draw_State;


typedef struct R_TEXTURE R_TEXTURE;
struct R_TEXTURE {
	// to defer clearing until the render target is actually set
	b32                       requires_clear;
	Color                     clear_color;
	vec2i                     resolution;
	TextureFormat             format;
	union {
		ID3D11Texture2D       *texture;
		ID3D11Resource        *texture_resource;
	};
	ID3D11ShaderResourceView *shader_resource_view;
	ID3D11RenderTargetView   *render_target_view;
};



typedef struct R_SCB_DEFAULT R_SCB_DEFAULT;
struct R_SCB_DEFAULT {
	vec4 transform[4];
	vec4 texture_color_transform[4];
	vec4 palette[16];
};
STATIC_ASSERT(!(sizeof(R_SCB_DEFAULT) & 15) && sizeof(R_SCB_DEFAULT) >= 64 && sizeof(R_SCB_DEFAULT) <= D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT);


struct R_Renderer {
	WID                     window;
	ID3D11InfoQueue        *info_queue;

	ID3D11Device           *device;
	ID3D11DeviceContext    *context;

	ID3D11Query            *time_frame_start_query;
	ID3D11Query            *time_frame_end_query;
	ID3D11Query            *event_query;
	int                     latency_cycles;
	int                     disable_event_query;

	IDXGISwapChain2          *window_present_mechanism;

	ID3D11RasterizerState    *default_rasterizer;
	ID3D11DepthStencilState  *default_depth_stencil;
	ID3D11Buffer             *constant_buffer;
	ID3D11VertexShader       *vertex_shader;
	ID3D11InputLayout        *input_layout;

	ID3D11Buffer       *vertices_submission_buffer;
	i32                 vertices_submission_buffer_capacity;
	i32                 vertices_submission_buffer_offset;

	R_Draw_State   draw_prev;
	R_Draw_State   draw_prox;
	R_SCB_DEFAULT  draw_constants;
	b32            draw_constants_changed;

	ID3D11BlendState   *blenders[BLENDER_CAPACITY];
	ID3D11SamplerState *samplers[SAMPLER_CAPACITY];
	ID3D11PixelShader   *shaders[SHADER_CAPACITY];
	R_TEXTURE           window_texture;

	// todo: structure this better!
	R_Vertex3  *mem_vertices;
	i32         num_vertices;
	i32         max_vertices;
};




static inline R_TEXTURE *TextureFromRID(R_Renderer *rend, RID id) {
	return (R_TEXTURE *) id;
}


static void R_InitTextureEx(R_Renderer *rend, RID rid, TextureFormat format, vec2i resolution, int flags, void *contents, i32 contentsstride);


R_Renderer *R_InitRenderer(WID window) {

	R_Renderer *rend = calloc(1, sizeof(*rend));
	rend->window = window;

	HWND hwnd = OS_GetWindowHandle(window);
	ASSERT(IsWindow(hwnd));

	rend->mem_vertices = VirtualAlloc(0, MEGABYTES(128), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	rend->max_vertices = MEGABYTES(128) / sizeof(*rend->mem_vertices);
	rend->num_vertices = 0;


	b32 success = true;

	i32 device_flags = 0;
	device_flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	device_flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL feature_menu[2][2] = {
		{D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0},
		{D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0},
	};

	D3D_FEATURE_LEVEL device_features;
	if (SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_HARDWARE,0,device_flags,feature_menu[0],COUNTOF(feature_menu[0]),D3D11_SDK_VERSION,&rend->device,&device_features,&rend->context))
	||  SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_WARP    ,0,device_flags,feature_menu[1],COUNTOF(feature_menu[1]),D3D11_SDK_VERSION,&rend->device,&device_features,&rend->context)))
	{
		if(device_flags & D3D11_CREATE_DEVICE_DEBUG) {
			IProvideClassInfo_QueryInterface(rend->device,&IID_ID3D11InfoQueue,(void**)&rend->info_queue);
			ID3D11InfoQueue_SetBreakOnSeverity(rend->info_queue, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(rend->info_queue, D3D11_MESSAGE_SEVERITY_ERROR,      TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(rend->info_queue, D3D11_MESSAGE_SEVERITY_WARNING,    TRUE);
		}
		D3D11_QUERY_DESC info = {
			.Query = D3D11_QUERY_TIMESTAMP,
		};
		ID3D11Device_CreateQuery(rend->device, &info, &rend->time_frame_start_query);
		ID3D11Device_CreateQuery(rend->device, &info, &rend->time_frame_end_query);

		info.Query = D3D11_QUERY_EVENT;
		ID3D11Device_CreateQuery(rend->device, &info, &rend->event_query);
	} else {
		MessageBoxA(NULL, "failed to acquire graphics device", "Error", MB_OK | MB_ICONERROR);
	}

	{
		IDXGIDevice *device_dxgi = {0};
		ID3D11Device_QueryInterface(rend->device,&IID_IDXGIDevice,(void **)&device_dxgi);

		IDXGIAdapter *adapter_dxgi = {0};
		IDXGIDevice_GetAdapter(device_dxgi,&adapter_dxgi);

		IDXGIFactory2 *factory_dxgi = {0};
		IDXGIAdapter_GetParent(adapter_dxgi,&IID_IDXGIFactory2,(void**)&factory_dxgi);

		DXGI_SWAP_CHAIN_DESC1 config = {
			.Width              = 0,
			.Height             = 0,
			.Format             = DXGI_FORMAT_R8G8B8A8_UNORM,
			.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount        = 2,
			.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
			.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
			.SampleDesc.Count   = 1,
			.SampleDesc.Quality = 0,
		};

      // the refresh rate is only applied when in full screen
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen_config = {
			.RefreshRate.Numerator = 60,
			.RefreshRate.Denominator = 1,
			.Windowed = TRUE,
		};

		HRESULT error = IDXGIFactory2_CreateSwapChainForHwnd(factory_dxgi, (IUnknown *) rend->device, hwnd, &config, &fullscreen_config, NULL, (IDXGISwapChain1 **) &rend->window_present_mechanism);
		if(FAILED(error)){
			// os_display_error_dialog(0);
		}

		IDXGIFactory_Release(factory_dxgi);
		IDXGIAdapter_Release(adapter_dxgi);
		IDXGIDevice_Release(device_dxgi);
	}

	{
		D3D11_DEPTH_STENCIL_DESC config = {
			.DepthEnable = FALSE,
			.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
			.DepthFunc = D3D11_COMPARISON_GREATER,
			.StencilEnable = FALSE,
			.StencilReadMask  = D3D11_DEFAULT_STENCIL_READ_MASK,
			.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
			.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR,
			.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS,
			.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR,
			.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS,
		};

		ID3D11Device_CreateDepthStencilState(rend->device,&config,&rend->default_depth_stencil);
	}

	{
		D3D11_RASTERIZER_DESC config = {
			.FillMode = D3D11_FILL_SOLID,
			.CullMode = D3D11_CULL_NONE,
			.FrontCounterClockwise = FALSE,
			.DepthBias = D3D11_DEFAULT_DEPTH_BIAS,
			.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
			.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			.DepthClipEnable = FALSE,
			.ScissorEnable = FALSE,
			.MultisampleEnable = FALSE,
			.AntialiasedLineEnable = FALSE,
		};
		ID3D11Device_CreateRasterizerState(rend->device,&config,&rend->default_rasterizer);
	}

	{
		D3D11_BLEND_DESC config = {
			.RenderTarget[0] = {
				.BlendEnable           = TRUE,
				.SrcBlend              = D3D11_BLEND_SRC_ALPHA,
				.DestBlend             = D3D11_BLEND_INV_SRC_ALPHA,
				.BlendOp               = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha         = D3D11_BLEND_SRC_ALPHA,
				.DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA,
				.BlendOpAlpha          = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
			}
		};
		ID3D11Device_CreateBlendState(rend->device,&config,&rend->blenders[BLENDER_ALPHA_BLEND]);
	}

	{
		D3D11_SAMPLER_DESC config = {
			.ComparisonFunc = D3D11_COMPARISON_NEVER,
			.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP,
			.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP,
			.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP,
			.MinLOD         = 0,
			.MaxLOD         = D3D11_FLOAT32_MAX,
		};

		config.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		ID3D11Device_CreateSamplerState(rend->device, &config, &rend->samplers[SAMPLER_POINT]);

		config.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		ID3D11Device_CreateSamplerState(rend->device, &config, &rend->samplers[SAMPLER_LINEAR]);
	}

	{
		rend->vertices_submission_buffer_capacity = MEGABYTES(256);

		D3D11_BUFFER_DESC config = {
			.Usage = D3D11_USAGE_DYNAMIC,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0,
			.StructureByteStride = 0,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.ByteWidth = rend->vertices_submission_buffer_capacity,
		};
		ID3D11Device_CreateBuffer(rend->device, &config, NULL, &rend->vertices_submission_buffer);
	}


	{
		D3D11_BUFFER_DESC config = {
			.ByteWidth      = sizeof(R_SCB_DEFAULT),
			.Usage          = D3D11_USAGE_DYNAMIC,
			.BindFlags      = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		};
		ID3D11Device_CreateBuffer(rend->device, &config, 0, &rend->constant_buffer);
	}

	{
		char shader_name[] = "simple_shader";
		char shader_source[] =
		"cbuffer Constants : register(b0) {\n"
		"	float4x4	transform;\n"
		"	float4x4	texture_color_transform;\n"
		"	float4   g_palette[16];\n"
		"}\n"
		"struct Vertex_In {\n"
		"	float3 position   : POSITION;\n"
		"	float2 texcoords  : TEXCOORD;\n"
		"	float4 color      : COLOR;\n"
		"};\n"
		"struct Vertex_Out {\n"
		"	float4 position   : SV_Position;\n"
		"	float2 texcoords  : TEXCOORD;\n"
		"	float4 color      : COLOR;\n"
		"};\n"
		"Texture2D    main_t2d     : register(t0);\n"
		"SamplerState main_sampler : register(s0);\n"
		"Vertex_Out MainVS(Vertex_In input, uint vertex_id: SV_VertexID) {\n"
		"	Vertex_Out output;\n"
		"	output.position = mul(transform, float4(input.position, 1));\n"
		"	output.texcoords = input.texcoords;\n"
		"	output.color = input.color;\n"
		"	return output;\n"
		"}\n"
		"float4 PS_Main(Vertex_Out input) : SV_TARGET {\n"
		"	float4 color = input.color;\n"
		"	float4 sample = main_t2d.Sample(main_sampler, input.texcoords);\n"
		"	float4 final_color = mul(texture_color_transform, sample) * color;\n"
		"	return final_color;\n"
		"}\n"
		"float4 PS_Grayscale(Vertex_Out input) : SV_TARGET {\n"
		"	float4 sample = main_t2d.Sample(main_sampler, input.texcoords);\n"
		"	sample = mul(texture_color_transform, sample);\n"
		"	float lumen = dot(sample.rgb, float3(0.2126, 0.7152, 0.0722));\n"
		"	float4 final_color = float4(lumen,lumen,lumen,sample.a);\n"
		"	return final_color;\n"
		"}\n"
		"static const float3 g_palette2[5] = {\n"
		"		float3(15 ,  56, 15) / 255.0,\n"
		"		float3(15 ,  56, 15) / 255.0,\n"
		"		float3(48 ,  98, 48) / 255.0,\n"
		"		float3(139, 172, 15) / 255.0,\n"
		"		float3(155, 188, 15) / 255.0,\n"
		"};\n"
		"float4 PS_Palette(Vertex_Out input) : SV_TARGET {\n"
		"	float4 sample = main_t2d.Sample(main_sampler, input.texcoords);\n"
		"	sample = mul(texture_color_transform, sample);\n"
		"	float lumen = dot(sample.rgb, float3(0.2126, 0.7152, 0.0722));\n"
		"	float4 final_color = float4(g_palette2[(int)(lumen*5)],sample.a);\n"
		"	return final_color;\n"
		"}\n"
		;






		i32 compilation_flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR|D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION|D3DCOMPILE_WARNINGS_ARE_ERRORS;
		ID3DBlob *bytecode_b = NULL;
		ID3DBlob *messages_b = NULL;

		const char *ps_entries[] = {
			[SHADER_NONE]      = 0,
			[SHADER_DEFAULT]   = "PS_Main",
			[SHADER_GRAYSCALE] = "PS_Grayscale",
			[SHADER_PALETTE]   = "PS_Palette",
		};

		int i;
		for (i=SHADER_NONE+1; i < COUNTOF(ps_entries); ++i) {
			// pixel shader
			if (SUCCEEDED(D3DCompile(shader_source, strlen(shader_source)
			,    shader_name, 0, 0, ps_entries[i], "ps_5_0", compilation_flags, 0, &bytecode_b, &messages_b)))
			{
				ID3D11Device_CreatePixelShader(rend->device
				, bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
				, bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
				, NULL, &rend->shaders[i]);
			}
			if (messages_b) {
				TRACELOG("%s",(char*) messages_b->lpVtbl->GetBufferPointer(messages_b));
			}
			DX_RELEASE_MAYBE(bytecode_b);
			DX_RELEASE_MAYBE(messages_b);
		}



   	// vertex shader
		if (SUCCEEDED(D3DCompile(shader_source, strlen(shader_source)
		,    shader_name, 0, 0, "MainVS", "vs_5_0", compilation_flags, 0, &bytecode_b, &messages_b)))
		{
			if (SUCCEEDED(ID3D11Device_CreateVertexShader(rend->device
			,    bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
			,    bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
			,    NULL, &rend->vertex_shader)))
			{
				D3D11_INPUT_ELEMENT_DESC input_layout_config[] = {
					{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT  , 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT     , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR"   ,  0, DXGI_FORMAT_R8G8B8A8_UNORM   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				ID3D11Device_CreateInputLayout(rend->device
				,  input_layout_config
				, COUNTOF(input_layout_config)
				,  bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
				,  bytecode_b->lpVtbl->GetBufferSize(bytecode_b), &rend->input_layout);
			}
		}
		if (messages_b) {
			TRACELOG("%s",(char*) messages_b->lpVtbl->GetBufferPointer(messages_b));
		}
		DX_RELEASE_MAYBE(bytecode_b);
		DX_RELEASE_MAYBE(messages_b);
	}

	ID3D11VertexShader *vertex_shader = rend->vertex_shader;
	ID3D11InputLayout *input_layout = rend->input_layout;
	ID3D11PixelShader *pixel_shader = rend->shaders[SHADER_DEFAULT];
	ID3D11Buffer *constant_buffer = rend->constant_buffer;
	ID3D11Buffer *vertex_buffer = rend->vertices_submission_buffer;

	ID3D11DeviceContext_OMSetDepthStencilState(rend->context, 0, 0);

	ID3D11DeviceContext_RSSetState(rend->context, rend->default_rasterizer);

	ID3D11DeviceContext_VSSetShader(rend->context, vertex_shader, 0, 0);
	ID3D11DeviceContext_PSSetShader(rend->context, pixel_shader, 0, 0);
	ID3D11DeviceContext_VSSetConstantBuffers(rend->context, 0, 1, &constant_buffer);
	ID3D11DeviceContext_PSSetConstantBuffers(rend->context, 0, 1, &constant_buffer);

	u32 stride = sizeof(R_Vertex3);
	u32 offset = 0;
	ID3D11DeviceContext_IASetInputLayout(rend->context, input_layout);
	ID3D11DeviceContext_IASetVertexBuffers(rend->context, 0, 1, &vertex_buffer, &stride, &offset);
	return rend;
}


static const i32 g_topology_conversion_table[] = {
	[MODE_NONE]      = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
	[TOPO_TRIANGLES] = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	[MODE_LINES]     = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
};

static const DXGI_FORMAT g_format_conversion_table[] = {
	[FORMAT_NONE]    = DXGI_FORMAT_UNKNOWN,
	[FORMAT_R8_UNORM]    = DXGI_FORMAT_R8_UNORM,
	[FORMAT_R8G8B8_UNORM] = DXGI_FORMAT_R8G8B8A8_UNORM,
	[FORMAT_R32G32B32_FLOAT]= DXGI_FORMAT_R32G32B32A32_FLOAT,
};

static const i32 g_format_to_size[] = {
	[FORMAT_NONE]    =  0,
	[FORMAT_R8_UNORM]    =  1,
	[FORMAT_R8G8B8_UNORM] =  4,
	[FORMAT_R32G32B32_FLOAT]= 16,
};



void R_UpdateTexture(R_Renderer *rend, RID rid, iRect region, Color *contents, int stride)
{
	R_TEXTURE *texture = TextureFromRID(rend, rid);

	D3D11_BOX box = {
		.left = region.x,
		.right = region.x+region.w,
		.top = region.y,
		.bottom = region.y+region.h,
		.front = 0,
		.back = 1
	};
	ID3D11DeviceContext_UpdateSubresource(rend->context
	, texture->texture_resource, 0, & box, contents, stride, 0);
}



static void R_InitTextureEx(R_Renderer *rend, RID rid, TextureFormat format, vec2i resolution, int flags, void *contents, i32 contentsstride)
{
	ASSERT(resolution.x != 0);
	ASSERT(resolution.y != 0);
	ASSERT(R_GetSurface(rend) != rid);
	ASSERT(R_GetTexture(rend) != rid);

	R_TEXTURE *texture = TextureFromRID(rend, rid);

	// DX_RELEASE_MAYBE(texture->texture);
	// DX_RELEASE_MAYBE(texture->shader_resource_view);
	// DX_RELEASE_MAYBE(texture->render_target_view);

	texture->resolution = resolution;
	texture->format = format;
	texture->texture = 0;
	texture->shader_resource_view = 0;
	texture->render_target_view = 0;

	i32 bytes_per_pixel = g_format_to_size[format];
	DXGI_FORMAT dxgi_format = g_format_conversion_table[format];

	// .Usage = D3D11_USAGE_DYNAMIC,
	// .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
	{
		D3D11_TEXTURE2D_DESC config = {
			.Width              = resolution.x,
			.Height             = resolution.y,
			.MipLevels          = 1,
			.ArraySize          = 1,
			.Format             = dxgi_format,
			.SampleDesc.Count   = 1,
			.SampleDesc.Quality = 0,
			.Usage              = D3D11_USAGE_DEFAULT,
			.BindFlags          = 0,
			.CPUAccessFlags     = 0,
			.MiscFlags          = 0,
		};

		if (flags & R_BIND_INPUT) config.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (flags & R_BIND_OUTPUT) config.BindFlags |= D3D11_BIND_RENDER_TARGET;

		if (contentsstride <= 0) {
			contentsstride = resolution.x * bytes_per_pixel;
		}

		D3D11_SUBRESOURCE_DATA subresource = {
			.pSysMem     = contents,
			.SysMemPitch = contentsstride,
		};

		D3D11_SUBRESOURCE_DATA *psubresource = 0;
		if (contents) { psubresource = &subresource; }

		HRESULT hr = ID3D11Device_CreateTexture2D(rend->device, &config, psubresource, &texture->texture);
		ASSERT(SUCCEEDED(hr));
	}

	if (flags & R_BIND_INPUT) {
		D3D11_SHADER_RESOURCE_VIEW_DESC config = {
			.Format                    = DXGI_FORMAT_UNKNOWN,
			.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D,
			.Texture2D.MostDetailedMip = 0,
			.Texture2D.MipLevels       = 1,
		};

		HRESULT hr = ID3D11Device_CreateShaderResourceView(rend->device, texture->texture_resource, &config, &texture->shader_resource_view);
		ASSERT(SUCCEEDED(hr));
	}

	if (flags & R_BIND_OUTPUT) {
		HRESULT hr = ID3D11Device_CreateRenderTargetView(rend->device, texture->texture_resource, 0, &texture->render_target_view);
		ASSERT(SUCCEEDED(hr));
	}
}


RID R_InstallTextureEx(R_Renderer *rend, TextureFormat format, vec2i resolution, int flags, void *contents, i32 contentsstride) {
	R_TEXTURE *texture = malloc(sizeof(*texture));
	R_InitTextureEx(rend, (RID) texture, format, resolution, flags, contents, contentsstride);
	return (RID) texture;
}


RID R_InstallSurface(R_Renderer *rend, TextureFormat format, vec2i resolution) {
	return R_InstallTextureEx(rend, format, resolution, R_BIND_INPUT|R_BIND_OUTPUT, 0, 0);
}


RID R_InstallTexture(R_Renderer *rend, TextureFormat format, vec2i resolution, void *contents) {
	return R_InstallTextureEx(rend, format, resolution, R_BIND_INPUT, contents, 0);
}


static void rCopyMemory(R_Renderer *rend, ID3D11Resource *resource, void *memory, i32 length) {

	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	ID3D11DeviceContext_Map(rend->context, resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);

	CopyMemory(mapped_subresource.pData, memory, length);

	ID3D11DeviceContext_Unmap(rend->context, resource, 0);
}



static void R_BeginFrame(R_Renderer *rend) {
	{
		vec2i target_resolution = OS_GetWindowResolution(rend->window);

		R_TEXTURE *window = & rend->window_texture;

		b32 resolution_mismatch =
		window->resolution.x != target_resolution.x ||
		window->resolution.y != target_resolution.y;

		b32 missing_window_render_target = !window->render_target_view;
		if (missing_window_render_target || resolution_mismatch) {
			window->resolution = target_resolution;

			window->requires_clear = true;

			DX_RELEASE_MAYBE(window->render_target_view);
			DX_RELEASE_MAYBE(window->texture);

			IDXGISwapChain_ResizeBuffers(rend->window_present_mechanism, 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			IDXGISwapChain_GetBuffer(rend->window_present_mechanism, 0, &IID_ID3D11Texture2D, (void **)(&window->texture));
			ID3D11Device_CreateRenderTargetView(rend->device, window->texture_resource, 0, &window->render_target_view);
		}
	}
}



static void R_EndFrame(R_Renderer *rend) {

	// issue draw call for any remaining vertices
	R_FlushVertices(rend);

	IDXGISwapChain_Present(rend->window_present_mechanism,1,0);

	// d3d11 will unbind the render target, so this reflects that
	rend->draw_prev.output = 0;
}



void R_Synchronize(R_Renderer *rend) {
	ID3D11DeviceContext_Flush(rend->context);
}



R_SubmissionTicket R_SubmitVertices(R_Renderer *rend, R_Vertex3 *vertices, i32 number) {

	i32 vertices_in_buffer = rend->vertices_submission_buffer_offset + number;
	if (vertices_in_buffer * sizeof(*vertices) > rend->vertices_submission_buffer_capacity) {
		rend->vertices_submission_buffer_offset = 0;
	}

	i32 offset = rend->vertices_submission_buffer_offset;

	D3D11_MAPPED_SUBRESOURCE subresource;
	ID3D11DeviceContext_Map(rend->context, (ID3D11Resource *) rend->vertices_submission_buffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
	R_Vertex3 *vertex_memory = subresource.pData;

	R_Vertex3 *write_pointer = vertex_memory + rend->vertices_submission_buffer_offset;
	CopyMemory(write_pointer, vertices, number * sizeof(*vertices));
	rend->vertices_submission_buffer_offset += number;

	ID3D11DeviceContext_Unmap(rend->context, (ID3D11Resource *) rend->vertices_submission_buffer, 0);

	return (R_SubmissionTicket){ offset };
}



void R_DrawVertices(R_Renderer *rend, R_SubmissionTicket offset, i32 number) {
	ASSERT(number != 0);

	R_Draw_State prox = rend->draw_prox;
	R_Draw_State prev = rend->draw_prev;
	rend->draw_prev = prox;

	ASSERT(prox.topology != MODE_NONE);
	ASSERT(prox.texture  != RID_NONE);

	// check number of vertices matches what we want to draw
	switch (prox.topology) {
		case TOPO_TRIANGLES: { ASSERT(number % 3 == 0); } break;
		case MODE_LINES:     { ASSERT(number % 2 == 0); } break;
		default: { ASSERT(!"SHMUCK"); } break;
	}

	if (prox.output == prox.texture) {
		OS_ShowErrorMessage("Invalid Draw State: Output Is The Same As Input");
	}

	b32 draw_constants_changed = rend->draw_constants_changed;
	rend->draw_constants_changed = false;

	if (prev.blender != prox.blender) {
		ID3D11BlendState *blender = rend->blenders[prox.blender];
		ID3D11DeviceContext_OMSetBlendState(rend->context, blender, 0, 0xffffffff);
	}

	{
		R_TEXTURE *output_texture = TextureFromRID(rend, prox.output);
		ID3D11RenderTargetView *render_target_view = output_texture->render_target_view;

		if (prev.output != prox.output) {
			// is the thing that we want to set as output bound as input?
			if (prox.output == prev.texture) {
				ID3D11ShaderResourceView *nullptr_shader_resource_view = NULL;
				ID3D11DeviceContext_PSSetShaderResources(rend->context, 0, 1, &nullptr_shader_resource_view);
			}

			ID3D11DeviceContext_OMSetRenderTargets(rend->context, 1, &render_target_view, 0);
		}

		if (output_texture->requires_clear) {
			output_texture->requires_clear = false;

			Color color = output_texture->clear_color;
			const f32 inv = 1.0 / 255.0;
			f32 fcolor[4] = { color.r * inv, color.g * inv, color.b * inv, color.a * inv };
			ID3D11DeviceContext_ClearRenderTargetView(rend->context, render_target_view, fcolor);
		}
	}

	if (prev.sampler != prox.sampler) {
		ID3D11SamplerState *sampler = rend->samplers[prox.sampler];
		ID3D11DeviceContext_PSSetSamplers(rend->context, 0, 1, &sampler);
	}


	if (prev.viewport.x != prox.viewport.x || prev.viewport.y != prox.viewport.y) {
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, prox.viewport.x, prox.viewport.y, 0.0f, 1.0f };
		ID3D11DeviceContext_RSSetViewports(rend->context, 1, &viewport);
	}

	if (prev.topology != prox.topology) {
		i32 topo = g_topology_conversion_table[prox.topology];
		ID3D11DeviceContext_IASetPrimitiveTopology(rend->context, topo);
	}

	if (prev.shader != prox.shader) {
		ID3D11PixelShader *shader = rend->shaders[prox.shader];
		ID3D11DeviceContext_PSSetShader(rend->context, shader, 0, 0);
	}


	if (prev.texture != prox.texture)
	{
		R_TEXTURE *texture = TextureFromRID(rend, prox.texture);
		R_TEXTURE *texture_prev = TextureFromRID(rend, prev.texture);
		if (!texture_prev || texture->format != texture_prev->format) {
			if (texture->format == FORMAT_R8_UNORM) {
				rend->draw_constants.texture_color_transform[0] = (vec4){ 1, 0, 0, 0 };
				rend->draw_constants.texture_color_transform[1] = (vec4){ 1, 0, 0, 0 };
				rend->draw_constants.texture_color_transform[2] = (vec4){ 1, 0, 0, 0 };
				rend->draw_constants.texture_color_transform[3] = (vec4){ 1, 0, 0, 0 };
			} else {
				rend->draw_constants.texture_color_transform[0] = (vec4){ 1, 0, 0, 0 };
				rend->draw_constants.texture_color_transform[1] = (vec4){ 0, 1, 0, 0 };
				rend->draw_constants.texture_color_transform[2] = (vec4){ 0, 0, 1, 0 };
				rend->draw_constants.texture_color_transform[3] = (vec4){ 0, 0, 0, 1 };
			}
			draw_constants_changed = true;
		}
		ID3D11DeviceContext_PSSetShaderResources(rend->context, 0, 1, &texture->shader_resource_view);

#if defined(FLUSHHACK)
		ID3D11DeviceContext_Flush(rend->context);
#endif
	}

	if (draw_constants_changed) {
		ID3D11Buffer *constant_buffer = rend->constant_buffer;
		rCopyMemory(rend, (ID3D11Resource *) constant_buffer, &rend->draw_constants, sizeof(rend->draw_constants));
	}

	ID3D11DeviceContext_Draw(rend->context, number, offset.offset);
}



void rInstallShader(R_Renderer *rend, ShaderId id, char *entry, char *contents) {

	enum {
		SHADER_COMPILE_FLAGS = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR|D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION|D3DCOMPILE_WARNINGS_ARE_ERRORS,
	};

	ID3DBlob *bytecode_b = NULL;
	ID3DBlob *messages_b = NULL;

	ID3D11PixelShader *shader = NULL;

	if (SUCCEEDED(D3DCompile(contents, strlen(contents)
	,		entry, 0, 0, entry, "ps_5_0"
	,		SHADER_COMPILE_FLAGS, 0, &bytecode_b, &messages_b)))
	{
		ID3D11Device_CreatePixelShader(rend->device
		, bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
		, bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
		, NULL, &shader);
	}
	if (messages_b) {
		TRACELOG("%s",(char*) messages_b->lpVtbl->GetBufferPointer(messages_b));
	}
	if (bytecode_b) bytecode_b->lpVtbl->Release(bytecode_b);
	if (messages_b) messages_b->lpVtbl->Release(messages_b);

	rend->shaders[id] = shader;
}


// todo: sometimes you may want to reserve X vertices, but you
// only end up using Y, so maybe have another function like
// EndVertices that takes how many vertices you actually used...
// This will also make the API easier to debug because we can
// check there that the number of vertices is the valid for the
// current topology...
R_Vertex3 *R_QueueVertices(R_Renderer *rend, i32 number) {
	if (rend->num_vertices + number > rend->max_vertices) {
		R_FlushVertices(rend);
	}
	R_Vertex3 *vertices = rend->mem_vertices + rend->num_vertices;
	rend->num_vertices += number;
	return vertices;
}



void R_FlushVertices(R_Renderer *rend) {
	if (rend->num_vertices != 0) {
		R_SubmissionTicket token = R_SubmitVertices(rend, rend->mem_vertices, rend->num_vertices);

		R_DrawVertices(rend, token, rend->num_vertices);

		rend->num_vertices = 0;
	}
}

vec2i R_GetTextureInfo(R_Renderer *rend, RID id) {
	return TextureFromRID(rend, id)->resolution;
}

static b32 rSetDrawStateVar(R_Renderer *rend, i32 index, i64 value) {
	b32 changed = rend->draw_prox.states[index] != value;

	if (changed) {
		R_FlushVertices(rend);

		rend->draw_prox.states[index] = value;
	}

	return changed;
}



RID R_GetWindowOutput(R_Renderer *rend) {
	return (RID) & rend->window_texture;
}



void R_SetShader(R_Renderer *rend, ShaderId value) {
	rSetDrawStateVar(rend, STATE_SHADER, value);
}


void R_SetSampler(R_Renderer *rend, SamplerId value) {
	rSetDrawStateVar(rend, STATE_SAMPLER, value);
}

void R_SetBlender(R_Renderer *rend, BlenderId value) {
	rSetDrawStateVar(rend, STATE_BLENDER, value);
}

void R_SetTopology(R_Renderer *rend, Topology value) {
	rSetDrawStateVar(rend, STATE_TOPOLOGY, value);
}

RID R_GetTexture(R_Renderer *rend) {
	return rend->draw_prox.texture;
}

RID R_GetSurface(R_Renderer *rend) {
	return rend->draw_prox.output;
}

// todo: this should just be in begin frame?
void R_SetOutput(R_Renderer *rend, RID value) {

	R_TEXTURE *texture = TextureFromRID(rend, value);
	ASSERT(texture->render_target_view);

	b32 changed = rSetDrawStateVar(rend, STATE_OUTPUT, value);
	if (changed) {
		R_SetViewport(rend, texture->resolution);
		R_SetVirtualReso(rend, texture->resolution);
	}
}



void R_SetTexture(R_Renderer *rend, RID value) {

	R_TEXTURE *texture = TextureFromRID(rend, value);
	ASSERT(texture);
	ASSERT(texture->shader_resource_view);

	rSetDrawStateVar(rend, STATE_INPUT, value);
}



void R_SetViewport(R_Renderer *rend, vec2i viewport) {
	if (rend->draw_prox.viewport.x != viewport.x || rend->draw_prox.viewport.y != viewport.y) {

		R_FlushVertices(rend);

		rend->draw_prox.viewport = viewport;
	}
}



void R_SetVirtualReso(R_Renderer *rend, vec2i reso) {
	vec2 scale = { 2.0 / reso.x, 2.0 / reso.y };
	vec2 offset = { -1.0, -1.0 };

	rend->draw_constants.transform[0] = (vec4){scale.x, 0, 0, offset.x};
	rend->draw_constants.transform[1] = (vec4){0, scale.y, 0, offset.y};
	rend->draw_constants.transform[2] = (vec4){0, 0, 1, 0};
	rend->draw_constants.transform[3] = (vec4){0, 0, 0, 1};
	rend->draw_constants_changed = true;
}



void R_ClearSurface(R_Renderer *rend, Color color) {

	R_TEXTURE *output_texture = TextureFromRID(rend, rend->draw_prox.output);
	ID3D11RenderTargetView *render_target_view = output_texture->render_target_view;

	if (!render_target_view) {
		OS_ShowErrorMessage("no render target bound");
	}

	//
	// todo: remove this
	//
	output_texture->requires_clear = true;
	output_texture->clear_color = color;
}



#if 0
	iRect clipping_region = pass->clipping_region;
	if (clipping_region.width != 0 && clipping_region.height != 0){
		D3D11_RECT rect = {0};
		rect.left = clipping_region.x;
		rect.right = clipping_region.x + clipping_region.width;
		rect.top = clipping_region.y;
		rect.bottom = clipping_region.y + clipping_region.height;
		ID3D11DeviceContext_RSSetScissorRects(rend->context, 1, &rect);
	} else {
		D3D11_RECT rect = {};
		rect.left = 0;
		rect.right = (LONG) resolution.x;
		rect.top = 0;
		rect.bottom = (LONG) resolution.y;
		ID3D11DeviceContext_RSSetScissorRects(rend->context, 1, &rect);
	}
#endif