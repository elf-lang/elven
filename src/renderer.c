#define _CRT_SECURE_NO_WARNINGS
#define              CINTERFACE
#define              COBJMACROS
#define        D3D11_NO_HELPERS
#pragma comment(lib,        "Gdi32")
#pragma comment(lib,       "dxguid")
#pragma comment(lib,        "d3d11")
#pragma comment(lib,  "d3dcompiler")


#include <d3dcompiler.h>
#include   <dxgidebug.h>
#include        <dxgi.h>
#include       <d3d11.h>
#include     <dxgi1_3.h>


#include "elements.h"
#include "draw.h"
#include "platform.h"
#include "renderer.h"

#define DX_RELEASE(I) ((I)->lpVtbl->Release(I))
#define DX_RELEASE_MAYBE(I) ((I) ? DX_RELEASE(I) : 0)


typedef struct R_TEXTURE R_TEXTURE;
struct R_TEXTURE {
	b32                       inuse;
	vec2i                     resolution;
	ID3D11SamplerState       *sampler;
	union {
		ID3D11Texture2D       *texture;
		ID3D11Resource        *texture_resource;
	};
	ID3D11ShaderResourceView *shader_resource_view;
	ID3D11RenderTargetView   *render_target_view;
};


typedef struct R_CONSTANTS R_CONSTANTS;
struct R_CONSTANTS {
	vec4 transform[4];
	vec4 storage[12];
};
STATIC_ASSERT(!(sizeof(R_CONSTANTS) & 15) && sizeof(R_CONSTANTS) >= 64 && sizeof(R_CONSTANTS) <= D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT);


struct R_Renderer {
	HWND             window;

	ID3D11InfoQueue        *info_queue;

	ID3D11Device           *device;
	ID3D11DeviceContext    *context;

	ID3D11Query            *time_frame_start_query;
	ID3D11Query            *time_frame_end_query;

	IDXGISwapChain2          *window_present_mechanism;

	ID3D11RasterizerState    *default_rasterizer;
	ID3D11BlendState         *default_blender;
	ID3D11DepthStencilState  *default_depth_stencil;
	ID3D11Buffer             *constant_buffer;
	ID3D11VertexShader       *vertex_shader;
	ID3D11InputLayout        *input_layout;

	ID3D11Buffer       *vertices_submission_buffer;
	i32                 vertices_submission_buffer_capacity;
	i32                 vertices_submission_buffer_offset;

	R_Draw_State  draw_prev;
	R_Draw_State  draw_prox;
	R_CONSTANTS   draw_constants;
	b32           draw_constants_changed;

	ID3D11SamplerState *samplers[SAMPLER_CAPACITY];
	ID3D11PixelShader   *shaders[SHADER_CAPACITY];
	R_TEXTURE           textures[TEXTURE_CAPACITY];
};


// todo: structure this better!
R_VERTEX_2D  *r_mem_vertices;
i32           r_num_vertices;
i32           r_max_vertices;


static R_TEXTURE *get_texture_by_id(R_Renderer *rend, TextureId id) {
	return & rend->textures[id];
}


R_Renderer *R_InitRenderer(Init_Renderer params) {
	R_Renderer *rend = calloc(1, sizeof(*rend));
	rend->window = (HWND) params.window;

	for (i32 i = 0; i < TEXTURE_FIRST_UNRESERVED_ID; i ++) {
		rend->textures[i].inuse = true;
	}

	r_mem_vertices = VirtualAlloc(0, MEGABYTES(128), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	r_max_vertices = MEGABYTES(128) / sizeof(*r_mem_vertices);
	r_num_vertices = 0;


	b32 success = true;

	i32 device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	device_flags |= D3D11_CREATE_DEVICE_DEBUG|D3D11_CREATE_DEVICE_SINGLETHREADED;

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

		HRESULT error = IDXGIFactory2_CreateSwapChainForHwnd(factory_dxgi,(IUnknown *)rend->device, (HWND) params.window, &config, &fullscreen_config, NULL, (IDXGISwapChain1 **) &rend->window_present_mechanism);
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
		ID3D11Device_CreateBlendState(rend->device,&config,&rend->default_blender);
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

	Color color = {255, 255, 255, 255};
	R_InstallTexture(rend, TEXTURE_DEFAULT, FORMAT_RGBA_U8, (vec2i){1,1}, &color);


	{
		D3D11_BUFFER_DESC config = {
			.ByteWidth      = sizeof(R_CONSTANTS),
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
		"}\n"
		"struct Vertex_In {\n"
		"	float2 position   : POSITION;\n"
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
		"Vertex_Out main_vs(Vertex_In input, uint vertex_id: SV_VertexID) {\n"
		"	Vertex_Out output;\n"
		"	output.position = mul(transform,float4(input.position,0,1));\n"
		"	output.texcoords = input.texcoords;\n"
		"	output.color = input.color;\n"
		"	return output;\n"
		"}\n"
		"float4 main_ps(Vertex_Out input) : SV_TARGET {\n"
		"	float4 color = input.color;\n"
		"	float4 sample = main_t2d.Sample(main_sampler, input.texcoords);\n"
		"	float4 final_color = sample * color;\n"
		"	return final_color;\n"
		"}\n";

		i32 compilation_flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR|D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION|D3DCOMPILE_WARNINGS_ARE_ERRORS;
		ID3DBlob *bytecode_b = NULL;
		ID3DBlob *messages_b = NULL;

		// pixel shader
		if (SUCCEEDED(D3DCompile(shader_source, strlen(shader_source)
		,    shader_name, 0, 0, "main_ps", "ps_5_0", compilation_flags, 0, &bytecode_b, &messages_b)))
		{
			ID3D11Device_CreatePixelShader(rend->device
			, bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
			, bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
			, NULL, &rend->shaders[SHADER_DEFAULT]);
		}
		if (messages_b) {
			TRACELOG("%s",(char*) messages_b->lpVtbl->GetBufferPointer(messages_b));
		}
		if (bytecode_b) bytecode_b->lpVtbl->Release(bytecode_b);
		if (messages_b) messages_b->lpVtbl->Release(messages_b);


   	// vertex shader
		if (SUCCEEDED(D3DCompile(shader_source, strlen(shader_source)
		,    shader_name, 0, 0, "main_vs", "vs_5_0", compilation_flags, 0, &bytecode_b, &messages_b)))
		{
			if (SUCCEEDED(ID3D11Device_CreateVertexShader(rend->device
			,    bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
			,    bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
			,    NULL, &rend->vertex_shader)))
			{
				D3D11_INPUT_ELEMENT_DESC input_layout_config[] = {
					{ "POSITION",  0, DXGI_FORMAT_R32G32_FLOAT  , 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT  , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR"   ,  0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
		if (bytecode_b) bytecode_b->lpVtbl->Release(bytecode_b);
		if (messages_b) messages_b->lpVtbl->Release(messages_b);
	}

	ID3D11VertexShader *vertex_shader = rend->vertex_shader;
	ID3D11InputLayout *input_layout = rend->input_layout;
	ID3D11PixelShader *pixel_shader = rend->shaders[SHADER_DEFAULT];
	ID3D11Buffer *constant_buffer = rend->constant_buffer;
	ID3D11Buffer *vertex_buffer = rend->vertices_submission_buffer;

	ID3D11DeviceContext_OMSetDepthStencilState(rend->context, 0, 0);
	ID3D11DeviceContext_OMSetBlendState(rend->context, rend->default_blender, 0, 0xffffffff);

	ID3D11DeviceContext_RSSetState(rend->context, rend->default_rasterizer);

	ID3D11DeviceContext_VSSetShader(rend->context, vertex_shader, 0, 0);
	ID3D11DeviceContext_PSSetShader(rend->context, pixel_shader, 0, 0);
	ID3D11DeviceContext_VSSetConstantBuffers(rend->context, 0, 1, &constant_buffer);
	ID3D11DeviceContext_PSSetConstantBuffers(rend->context, 0, 1, &constant_buffer);

	u32 stride = sizeof(R_VERTEX_2D);
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
	[FORMAT_R_U8]    = DXGI_FORMAT_R8_UNORM,
	[FORMAT_RGBA_U8] = DXGI_FORMAT_R8G8B8A8_UNORM,
	[FORMAT_RGBA_F32]= DXGI_FORMAT_R32G32B32A32_FLOAT,
};

static const i32 g_format_to_size[] = {
	[FORMAT_NONE]    =  0,
	[FORMAT_R_U8]    =  1,
	[FORMAT_RGBA_U8] =  4,
	[FORMAT_RGBA_F32]= 16,
};

TextureId R_InstallTextureEx(R_Renderer *rend, R_TEXTURE_PARAMS args) {

	TextureId id = args.id;
	if (id == 0) id = R_NewTextureId(rend);

	ASSERT(R_GetSurface(rend) != id);
	ASSERT(R_GetTexture(rend) != id);

	R_TEXTURE *texture = get_texture_by_id(rend, id);

	DX_RELEASE_MAYBE(texture->texture);
	DX_RELEASE_MAYBE(texture->shader_resource_view);
	DX_RELEASE_MAYBE(texture->render_target_view);

	texture->sampler = rend->samplers[SAMPLER_POINT];
	texture->resolution = args.resolution;

	i32 bytes_per_pixel = g_format_to_size[args.format];
	DXGI_FORMAT format = g_format_conversion_table[args.format];

	// .Usage = D3D11_USAGE_DYNAMIC,
	// .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
	{
		D3D11_TEXTURE2D_DESC config = {
			.Width              = args.resolution.x,
			.Height             = args.resolution.y,
			.MipLevels          = 1,
			.ArraySize          = 1,
			.Format             = format,
			.SampleDesc.Count   = 1,
			.SampleDesc.Quality = 0,
			.Usage              = D3D11_USAGE_DEFAULT,
			.BindFlags          = 0,
			.CPUAccessFlags     = 0,
			.MiscFlags          = 0,
		};
		if (args.is_input)  config.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (args.is_output) config.BindFlags |= D3D11_BIND_RENDER_TARGET;

		i32 stride = args.stride;
		if (stride == 0) stride = args.resolution.x * bytes_per_pixel;

		D3D11_SUBRESOURCE_DATA subresource = {
			.pSysMem     = args.colors,
			.SysMemPitch = stride,
		};

		D3D11_SUBRESOURCE_DATA *psubresource = 0;
		if (args.colors) { psubresource = &subresource; }

		ID3D11Device_CreateTexture2D(rend->device, &config, psubresource, &texture->texture);
	}

	if (args.is_input) {
		D3D11_SHADER_RESOURCE_VIEW_DESC config = {
			.Format                    = DXGI_FORMAT_UNKNOWN,
			.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D,
			.Texture2D.MostDetailedMip = 0,
			.Texture2D.MipLevels       = 1,
		};

		ID3D11Device_CreateShaderResourceView(rend->device, texture->texture_resource, &config, &texture->shader_resource_view);
	}

	if (args.is_output) {
		ID3D11Device_CreateRenderTargetView(rend->device, texture->texture_resource, 0, &texture->render_target_view);
	}

	return id;
}

TextureId R_InstallSurface(R_Renderer *rend, TextureId id, R_FORMAT format, vec2i resolution) {
	return R_InstallTextureEx(rend, (R_TEXTURE_PARAMS) {
		.id         = id,
		.resolution = resolution,
		.format     = format,
		.is_input   = true,
		.is_output  = true,
	});
}


TextureId R_InstallTexture(R_Renderer *rend, TextureId id, R_FORMAT format, vec2i resolution, void *contents) {
	return R_InstallTextureEx(rend, (R_TEXTURE_PARAMS) {
		.id         = id,
		.resolution = resolution,
		.format     = format,
		.is_input   = true,
		.colors     = contents,
	});
}

static void rCopyMemory(R_Renderer *rend, ID3D11Resource *resource, void *memory, i32 length) {

	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	ID3D11DeviceContext_Map(rend->context, resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);

	CopyMemory(mapped_subresource.pData, memory, length);

	ID3D11DeviceContext_Unmap(rend->context, resource, 0);
}



static void R_BeginFrame(R_Renderer *rend) {
	{
		RECT window_r;
		GetClientRect(rend->window, &window_r);

		vec2i target_resolution;
		target_resolution.x = window_r.right - window_r.left;
		target_resolution.y = window_r.bottom - window_r.top;

		R_TEXTURE *window = get_texture_by_id(rend, TEXTURE_RT_WINDOW);

		b32 resolution_mismatch =
		window->resolution.x != target_resolution.x ||
		window->resolution.y != target_resolution.y;

		b32 missing_window_render_target = !window->render_target_view;
		if (missing_window_render_target || resolution_mismatch) {
			window->resolution = target_resolution;

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


R_Ticket R_SubmitVertices(R_Renderer *renderer, R_VERTEX_2D *vertices, i32 number) {

	i32 vertices_in_buffer = renderer->vertices_submission_buffer_offset + number;
	if (vertices_in_buffer * sizeof(*vertices) > renderer->vertices_submission_buffer_capacity) {
		renderer->vertices_submission_buffer_offset = 0;
	}

	i32 offset = renderer->vertices_submission_buffer_offset;

	D3D11_MAPPED_SUBRESOURCE subresource;
	ID3D11DeviceContext_Map(renderer->context, (ID3D11Resource *) renderer->vertices_submission_buffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
	R_VERTEX_2D *vertex_memory = subresource.pData;

	R_VERTEX_2D *write_pointer = vertex_memory + renderer->vertices_submission_buffer_offset;
	CopyMemory(write_pointer, vertices, number * sizeof(*vertices));
	renderer->vertices_submission_buffer_offset += number;

	ID3D11DeviceContext_Unmap(renderer->context, (ID3D11Resource *) renderer->vertices_submission_buffer, 0);

	return (R_Ticket){ offset };
}

void rSubmitDrawState(R_Renderer *rend, R_Draw_State prox) {
	R_Draw_State prev = rend->draw_prev;
	rend->draw_prev = prox;

	ASSERT(prox.topology !=    MODE_NONE);
	ASSERT(prox.texture  != TEXTURE_NONE);


	if (prox.output == prox.texture) {
		OS_ShowErrorMessage("Invalid Draw State: Output Is The Same As Input");
	}


	if (rend->draw_constants_changed) {
		rend->draw_constants_changed = false;

		ID3D11Buffer *constant_buffer = rend->constant_buffer;
		rCopyMemory(rend, (ID3D11Resource *) constant_buffer, &rend->draw_constants, sizeof(rend->draw_constants));
	}


	if (prev.output != prox.output) {

		// is the thing that we want to set as output bound as input?
		if (prox.output == prev.texture) {
			ID3D11ShaderResourceView *nullptr_shader_resource_view = NULL;
			ID3D11DeviceContext_PSSetShaderResources(rend->context, 0, 1, &nullptr_shader_resource_view);
		}

		R_TEXTURE *target = get_texture_by_id(rend, prox.output);
		ID3D11DeviceContext_OMSetRenderTargets(rend->context, 1, &target->render_target_view, 0);
	}

	if (prev.sampler != prox.sampler) {
		ID3D11SamplerState *sampler = rend->samplers[prox.sampler];
		ID3D11DeviceContext_PSSetSamplers(rend->context, 0, 1, &sampler);
	}


	if (prev.texture != prox.texture) {
		R_TEXTURE *texture = get_texture_by_id(rend, prox.texture);
		ID3D11DeviceContext_PSSetShaderResources(rend->context, 0, 1, &texture->shader_resource_view);
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
}

void R_DrawVertices(R_Renderer *rend, R_Ticket offset, i32 number) {
	ASSERT(number != 0);

	// check number of vertices matches what we want to draw
	switch (rend->draw_prox.topology) {
	case TOPO_TRIANGLES: { ASSERT(number % 3 == 0); } break;
	case MODE_LINES:     { ASSERT(number % 2 == 0); } break;
	default: { ASSERT(!"SHMUCK"); } break;
	}

	rSubmitDrawState(rend, rend->draw_prox);

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


R_VERTEX_2D *R_QueueVertices(R_Renderer *rend, i32 number) {
	if (r_num_vertices + number > r_max_vertices) {
		R_FlushVertices(rend);
	}
	R_VERTEX_2D *vertices = r_mem_vertices + r_num_vertices;
	r_num_vertices += number;
	return vertices;
}

void R_FlushVertices(R_Renderer *rend) {
	if (r_num_vertices != 0) {
		R_Ticket token = R_SubmitVertices(rend, r_mem_vertices, r_num_vertices);

		R_DrawVertices(rend, token, r_num_vertices);

		r_num_vertices = 0;
	}
}

TextureId R_NewTextureId(R_Renderer *rend) {
	R_TEXTURE *texture = rend->textures;
	R_TEXTURE *end = rend->textures + COUNTOF(rend->textures);
	while (texture->inuse && texture < end) texture ++;

	TextureId id = TEXTURE_NONE;
	if (!texture->inuse) id = texture - rend->textures;
	texture->inuse = true;
	return id;
}

vec2i R_GetTextureInfo(R_Renderer *rend, TextureId id) {
	return get_texture_by_id(rend, id)->resolution;
}

b32 draw_state(R_Renderer *rend, i32 index, i32 value) {
	b32 changed = rend->draw_prox.states[index] != value;

	if (changed) {
		R_FlushVertices(rend);

		rend->draw_prox.states[index] = value;
	}

	return changed;
}

void R_SetShader(R_Renderer *rend, ShaderId value) {
	draw_state(rend, STATE_SHADER, value);
}


void R_SetSampler(R_Renderer *rend, SamplerId value) {
	draw_state(rend, STATE_SAMPLER, value);
}

void R_SetTopology(R_Renderer *rend, Topology value) {
	draw_state(rend, STATE_TOPOLOGY, value);
}

TextureId R_GetTexture(R_Renderer *rend) {
	return rend->draw_prox.texture;
}

TextureId R_GetSurface(R_Renderer *rend) {
	return rend->draw_prox.output;
}

// todo: this should just be in begin frame?
void R_SetSurface(R_Renderer *rend, TextureId value) {

	R_TEXTURE *texture = get_texture_by_id(rend, value);
	ASSERT(texture->render_target_view);

	b32 changed = draw_state(rend, STATE_OUTPUT, value);

	if (changed) {
		// todo:
		vec2i resolution = R_GetTextureInfo(rend, R_GetSurface(rend));
		vec2 scale = { 2.0 / resolution.x, 2.0 / resolution.y };
		vec2 offset = { -1.0, -1.0 };

		rend->draw_constants.transform[0] = (vec4){scale.x, 0, 0, offset.x};
		rend->draw_constants.transform[1] = (vec4){0, scale.y, 0, offset.y};
		rend->draw_constants.transform[2] = (vec4){0, 0, 1, 0};
		rend->draw_constants.transform[3] = (vec4){0, 0, 0, 1};
		rend->draw_constants_changed = true;
	}
}

void R_SetTexture(R_Renderer *rend, TextureId value) {

	R_TEXTURE *texture = get_texture_by_id(rend, value);
	ASSERT(texture->shader_resource_view);

	draw_state(rend, STATE_INPUT, value);
}


void R_SetViewport(R_Renderer *rend, vec2i viewport) {
	if (rend->draw_prox.viewport.x != viewport.x || rend->draw_prox.viewport.y != viewport.y) {

		R_FlushVertices(rend);

		rend->draw_prox.viewport = viewport;
	}
}

void R_SetViewportFullScreen(R_Renderer *rend) {
	R_SetViewport(rend, R_GetTextureInfo(rend, R_GetSurface(rend)));
}


// todo: defer this to when drawing?
// like at the moment of drawing check whether we have
// to clear the render target, and if so clear and unset
// the flag
void R_ClearSurface(R_Renderer *rend, Color color) {
	const f32 inv = 1.0 / 255.0;

	R_TEXTURE target = *get_texture_by_id(rend, rend->draw_prox.output);
	ID3D11RenderTargetView *render_target_view = target.render_target_view;

	if (!render_target_view) {
		OS_ShowErrorMessage("no render target bound");
	}

	f32 fcolor[4] = { color.r * inv, color.g * inv, color.b * inv, color.a * inv };
	ID3D11DeviceContext_ClearRenderTargetView(rend->context, render_target_view, fcolor);
}


static void rDrawQuad(R_Renderer *rend, vec2i xy, vec2i wh) {

	R_SetTopology(rend, TOPO_TRIANGLES);

	vec2 p0 = { xy.x + wh.x * 0, xy.y + wh.y * 0 };
	vec2 p1 = { xy.x + wh.x * 0, xy.y + wh.y * 1 };
	vec2 p2 = { xy.x + wh.x * 1, xy.y + wh.y * 0 };
	vec2 p3 = { xy.x + wh.x * 1, xy.y + wh.y * 1 };
	R_VERTEX_2D *vertices = R_QueueVertices(rend, 6);
	vertices[0] = (R_VERTEX_2D) {p0, {0,1}, WHITE};
	vertices[1] = (R_VERTEX_2D) {p1, {0,0}, WHITE};
	vertices[2] = (R_VERTEX_2D) {p3, {1,0}, WHITE};
	vertices[3] = (R_VERTEX_2D) {p0, {0,1}, WHITE};
	vertices[4] = (R_VERTEX_2D) {p3, {1,0}, WHITE};
	vertices[5] = (R_VERTEX_2D) {p2, {1,1}, WHITE};
}

void R_Blit(R_Renderer *rend, TextureId dest, TextureId src) {
	R_SetSurface(rend, dest);
	R_SetTexture(rend, src);
	R_SetViewportFullScreen(rend);
	R_ClearSurface(rend, BLACK);

	vec2i output_resolution = R_GetTextureInfo(rend, R_GetSurface(rend));
	vec2i input_resolution = R_GetTextureInfo(rend, R_GetTexture(rend));
	f32 max_scale = MIN(output_resolution.x / (f32) input_resolution.x, output_resolution.y / (f32) input_resolution.y);
	vec2i target_resolution = { input_resolution.x * max_scale, input_resolution.y * max_scale };
	vec2i offset = { (output_resolution.x - target_resolution.x) * 0.5, (output_resolution.y - target_resolution.y) * 0.5 };
	rDrawQuad(rend, offset, target_resolution);
}



#if 0
	r_i32 clipping_region = pass->clipping_region;
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