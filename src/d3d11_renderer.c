#include "renderer.h"

typedef struct {
	jam_State *jam;
	HWND window;
	int res_x;
	int res_y;
	int resizable;
} Init_Renderer;

b32 rInitRenderer(Init_Renderer params) {
	jam_State *jam = params.jam;
	jam->resizable = params.resizable;
	jam->base_resolution.x = params.res_x;
	jam->base_resolution.y = params.res_y;

	b32 success = true;

	i32 device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	device_flags |= D3D11_CREATE_DEVICE_DEBUG|D3D11_CREATE_DEVICE_SINGLETHREADED;

	D3D_FEATURE_LEVEL feature_menu[2][2] = {
		{D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0},
		{D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0},
	};

	D3D_FEATURE_LEVEL device_features;
	if (SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_HARDWARE,0,device_flags,feature_menu[0],_countof(feature_menu[0]),D3D11_SDK_VERSION,&jam->device,&device_features,&jam->context))
	||  SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_WARP    ,0,device_flags,feature_menu[1],_countof(feature_menu[1]),D3D11_SDK_VERSION,&jam->device,&device_features,&jam->context)))
	{
		if(device_flags & D3D11_CREATE_DEVICE_DEBUG) {
			IProvideClassInfo_QueryInterface(jam->device,&IID_ID3D11InfoQueue,(void**)&jam->info_queue);
			ID3D11InfoQueue_SetBreakOnSeverity(jam->info_queue, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(jam->info_queue, D3D11_MESSAGE_SEVERITY_ERROR,      TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(jam->info_queue, D3D11_MESSAGE_SEVERITY_WARNING,    TRUE);
		}
		D3D11_QUERY_DESC info = {};
		info.Query = D3D11_QUERY_TIMESTAMP;
		ID3D11Device_CreateQuery(jam->device, &info, &jam->time_frame_start_query);
		ID3D11Device_CreateQuery(jam->device, &info, &jam->time_frame_end_query);
	} else {
		MessageBoxA(NULL, "failed to acquire graphics device", "Error", MB_OK | MB_ICONERROR);
	}

	{
		IDXGIDevice *device_dxgi = {0};
		ID3D11Device_QueryInterface(jam->device,&IID_IDXGIDevice,(void **)&device_dxgi);

		IDXGIAdapter *adapter_dxgi = {0};
		IDXGIDevice_GetAdapter(device_dxgi,&adapter_dxgi);

		IDXGIFactory2 *factory_dxgi = {0};
		IDXGIAdapter_GetParent(adapter_dxgi,&IID_IDXGIFactory2,(void**)&factory_dxgi);

		DXGI_SWAP_CHAIN_DESC1 config = {};
		config.Width  = params.res_x;
		config.Height = params.res_y;
		config.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		config.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		config.BufferCount = 2;
		config.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		config.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		config.SampleDesc.Count = 1;
		config.SampleDesc.Quality = 0;

      // the refresh rate is only applied when in full screen
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen_config = {};
		fullscreen_config.RefreshRate.Numerator = 60;
		fullscreen_config.RefreshRate.Denominator = 1;
		fullscreen_config.Windowed = TRUE;

		HRESULT errorcode = IDXGIFactory2_CreateSwapChainForHwnd(factory_dxgi,(IUnknown *)jam->device,(HWND)params.window,&config,&fullscreen_config,NULL,(IDXGISwapChain1 **)&jam->window_present_mechanism);
		if(FAILED(errorcode)){
			// os_display_error_dialog(0);
		}

		IDXGIFactory_Release(factory_dxgi);
		IDXGIAdapter_Release(adapter_dxgi);
		IDXGIDevice_Release(device_dxgi);
	}

	// create base render target
	if(params.res_x != 0 && params.res_y != 0) {
		D3D11_TEXTURE2D_DESC config = {};
		config.Width = params.res_x;
		config.Height = params.res_y;
		config.MipLevels = 1;
		config.ArraySize = 1;
		config.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		config.SampleDesc.Count = 1;
		config.Usage = D3D11_USAGE_DEFAULT;
		config.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		ID3D11Device_CreateTexture2D(jam->device, &config, 0, &jam->base_render_target_texture);
		ID3D11Device_CreateRenderTargetView(jam->device, (ID3D11Resource *) jam->base_render_target_texture, 0, &jam->base_render_target_view);
		ID3D11Device_CreateShaderResourceView(jam->device, (ID3D11Resource *) jam->base_render_target_texture, 0, &jam->base_render_target_shader_view);
	}

	{
		D3D11_DEPTH_STENCIL_DESC config = {};
		config.DepthEnable = FALSE;
		config.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		config.DepthFunc = D3D11_COMPARISON_GREATER;
		config.StencilEnable = FALSE;
		config.StencilReadMask  = D3D11_DEFAULT_STENCIL_READ_MASK;
		config.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		config.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		config.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		config.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		config.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		config.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		config.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		config.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		config.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		ID3D11Device_CreateDepthStencilState(jam->device,&config,&jam->default_depth_stencil);
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
		ID3D11Device_CreateRasterizerState(jam->device,&config,&jam->default_rasterizer);
	}

	{
		D3D11_BLEND_DESC config = {};
		config.RenderTarget[0].BlendEnable = TRUE;
		config.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		config.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		config.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		config.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		config.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		config.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		config.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		ID3D11Device_CreateBlendState(jam->device,&config,&jam->default_blender);
	}

	{
      /* create some default samplers */
		D3D11_SAMPLER_DESC config = {};
		config.ComparisonFunc = D3D11_COMPARISON_NEVER;
		config.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		config.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		config.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		config.MinLOD = 0.0f;
		config.MaxLOD = D3D11_FLOAT32_MAX;

		config.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		ID3D11Device_CreateSamplerState(jam->device,&config,&jam->samplers[SAMPLER_POINT]);

		config.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		ID3D11Device_CreateSamplerState(jam->device,&config,&jam->samplers[SAMPLER_LINEAR]);
	}

	{
		// the reasoning behind creating this single large
		// buffer is A) use this buffer for all dynamic
		// geometry and avoid creating other buffers
		// which complicates my life, B) if the buffer
		// is large enough we can guarantee the CPU
		// and GPU won't overlap and so we can do much
		// faster maps. I don't actually know anything,
		// so perhaps there's another way of doing this.
		jam->vertices_submission_buffer_capacity = MEGABYTES(256);

		D3D11_BUFFER_DESC config = {
			.Usage = D3D11_USAGE_DYNAMIC,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0,
			.StructureByteStride = 0,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.ByteWidth = jam->vertices_submission_buffer_capacity,
		};
		ID3D11Device_CreateBuffer(jam->device, &config, NULL, &jam->vertices_submission_buffer);
	}

	u8x4 color = {255, 255, 255, 255};
	rInstallTexture(jam, TEXTURE_DEFAULT, FORMAT_RGBA_U8, (vec2i){1,1}, &color);

	{
		D3D11_BUFFER_DESC config = {};
		config.ByteWidth = sizeof(Per_Pass_Constants);
		config.Usage = D3D11_USAGE_DYNAMIC;
		config.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		config.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ID3D11Device_CreateBuffer(jam->device, &config, 0, &jam->constant_buffer);
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
			ID3D11Device_CreatePixelShader(jam->device
			, bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
			, bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
			, NULL, &jam->shaders[SHADER_DEFAULT]);
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
			if (SUCCEEDED(ID3D11Device_CreateVertexShader(jam->device
			,    bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
			,    bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
			,    NULL, &jam->vertex_shader)))
			{
				D3D11_INPUT_ELEMENT_DESC input_layout_config[] = {
					{ "POSITION",  0, DXGI_FORMAT_R32G32_FLOAT  , 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT  , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR"   ,  0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				ID3D11Device_CreateInputLayout(jam->device
				,  input_layout_config
				, _countof(input_layout_config)
				,  bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
				,  bytecode_b->lpVtbl->GetBufferSize(bytecode_b), &jam->input_layout);
			}
		}
		if (messages_b) {
			TRACELOG("%s",(char*) messages_b->lpVtbl->GetBufferPointer(messages_b));
		}
		if (bytecode_b) bytecode_b->lpVtbl->Release(bytecode_b);
		if (messages_b) messages_b->lpVtbl->Release(messages_b);
	}

	ID3D11VertexShader *vertex_shader = jam->vertex_shader;
	ID3D11InputLayout *input_layout = jam->input_layout;
	ID3D11PixelShader *pixel_shader = jam->shaders[SHADER_DEFAULT];
	ID3D11Buffer *constant_buffer = jam->constant_buffer;
	ID3D11Buffer *vertex_buffer = jam->vertices_submission_buffer;

	ID3D11DeviceContext_OMSetDepthStencilState(jam->context, 0, 0);
	ID3D11DeviceContext_OMSetBlendState(jam->context, jam->default_blender, 0, 0xffffffff);

	ID3D11DeviceContext_RSSetState(jam->context, jam->default_rasterizer);

	ID3D11DeviceContext_VSSetShader(jam->context, vertex_shader, 0, 0);
	ID3D11DeviceContext_PSSetShader(jam->context, pixel_shader, 0, 0);
	ID3D11DeviceContext_VSSetConstantBuffers(jam->context, 0, 1, &constant_buffer);
	ID3D11DeviceContext_PSSetConstantBuffers(jam->context, 0, 1, &constant_buffer);

	u32 stride = sizeof(Vertex2D);
	u32 offset = 0;
	ID3D11DeviceContext_IASetInputLayout(jam->context, input_layout);
	ID3D11DeviceContext_IASetVertexBuffers(jam->context, 0, 1, &vertex_buffer, &stride, &offset);
	return success;
}

#define DX_RELEASE(I) ((I)->lpVtbl->Release(I))

// todo: ensure sizes are proper
static void rInstallTexture(jam_State *J, TextureId id, TextureType type, vec2i resolution, void *contents) {

	rTextureStruct *texture = & J->textures[id];

	if (texture->texture) DX_RELEASE(texture->texture);
	if (texture->shader_resource_view) DX_RELEASE(texture->shader_resource_view);
	if (texture->render_target_view) DX_RELEASE(texture->render_target_view);

	i32 bytes_per_pixel = 0;
	switch (type) {
		case DXGI_FORMAT_R8_UNORM: {
			bytes_per_pixel = 1;
		} break;
		case DXGI_FORMAT_R8G8B8A8_UNORM: {
			bytes_per_pixel = 4;
		} break;
		default: { ASSERT(!"ERROR"); } break;
	}

	{
		D3D11_TEXTURE2D_DESC config = {
			.Width = resolution.x,
			.Height = resolution.y,
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = (DXGI_FORMAT) type,
			.SampleDesc.Count = 1,
			.SampleDesc.Quality = 0,
			// .Usage = D3D11_USAGE_DYNAMIC,
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_SHADER_RESOURCE,
			// .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.CPUAccessFlags = 0,
			.MiscFlags = 0,
		};

		D3D11_SUBRESOURCE_DATA sub_config = {
			.pSysMem = contents,
			.SysMemPitch = resolution.x * bytes_per_pixel,
		};

		D3D11_SUBRESOURCE_DATA *psub_config = 0;
		if(contents){ psub_config = &sub_config; }

		ID3D11Device_CreateTexture2D(J->device, &config, psub_config, &texture->texture);
	}

	if (texture->texture) {
		D3D11_SHADER_RESOURCE_VIEW_DESC config = {};
		config.Format = DXGI_FORMAT_UNKNOWN;
		config.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		config.Texture2D.MostDetailedMip = 0;
		config.Texture2D.MipLevels = 1;

		ID3D11Device_CreateShaderResourceView(J->device,(ID3D11Resource*)texture->texture,&config,&texture->shader_resource_view);

		texture->sampler = J->samplers[SAMPLER_POINT];
		texture->resolution = resolution;
	}
}



static SubmissionQueueToken
rSubmitVertices(jam_State *renderer, Vertex2D *vertices, i32 number) {

	i32 vertices_in_buffer = renderer->vertices_submission_buffer_offset + number;
	if (vertices_in_buffer * sizeof(*vertices) > renderer->vertices_submission_buffer_capacity) {
		renderer->vertices_submission_buffer_offset = 0;
	}

	i32 offset = renderer->vertices_submission_buffer_offset;

	D3D11_MAPPED_SUBRESOURCE subresource;
	ID3D11DeviceContext_Map(renderer->context, (ID3D11Resource *) renderer->vertices_submission_buffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
	Vertex2D *vertex_memory = subresource.pData;

	Vertex2D *write_pointer = vertex_memory + renderer->vertices_submission_buffer_offset;
	CopyMemory(write_pointer, vertices, number * sizeof(*vertices));
	renderer->vertices_submission_buffer_offset += number;

	ID3D11DeviceContext_Unmap(renderer->context, (ID3D11Resource *) renderer->vertices_submission_buffer, 0);

	return (SubmissionQueueToken){ offset };
}

static void EndFrame(jam_State *jam){
	vec2i window_dimensions = jam->window_dimensions;

	b32 resolution_mismatch =
	jam->window_render_target_resolution.x != window_dimensions.x ||
	jam->window_render_target_resolution.y != window_dimensions.y;
	b32 missing_window_render_target = !jam->window_render_target_view;
	if (missing_window_render_target || resolution_mismatch) {
		jam->window_render_target_resolution = window_dimensions;

		if(jam->window_render_target_view) {
			ID3D11RenderTargetView_Release(jam->window_render_target_view);
			ID3D11Texture2D_Release(jam->window_render_target);
		}
		IDXGISwapChain_ResizeBuffers(jam->window_present_mechanism, 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		IDXGISwapChain_GetBuffer(jam->window_present_mechanism, 0, &IID_ID3D11Texture2D, (void **)(&jam->window_render_target));
		ID3D11Device_CreateRenderTargetView(jam->device, (ID3D11Resource *) jam->window_render_target, 0, &jam->window_render_target_view);
	}

	#if 1
	{
		ID3D11Buffer *constant_buffer = jam->constant_buffer;
		Per_Pass_Constants globals = {};
		globals.transform[0] = (vec4){1, 0, 0, 0};
		globals.transform[1] = (vec4){0, 1, 0, 0};
		globals.transform[2] = (vec4){0, 0, 1, 0};
		globals.transform[3] = (vec4){0, 0, 0, 1};

		D3D11_MAPPED_SUBRESOURCE map;
		ID3D11DeviceContext_Map(jam->context, (ID3D11Resource *)constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		CopyMemory(map.pData, &globals, sizeof(globals));
		ID3D11DeviceContext_Unmap(jam->context, (ID3D11Resource *)constant_buffer, 0);
	}
	{
		vec2i window_resolution = jam->window_render_target_resolution;
		// f32 aspect_ratio = window_resolution.x / (f32) window_resolution.y;
		Vertex2D vertices[] = {
			{{-1,-1},{0,1},{255,255,255,255}},
			{{-1,1},{0,0},{255,255,255,255}},
			{{1,1},{1,0},{255,255,255,255}},
			{{-1,-1},{0,1},{255,255,255,255}},
			{{1,1},{1,0},{255,255,255,255}},
			{{1,-1},{1,1},{255,255,255,255}},
		};
		i32 vertex_offset = rSubmitVertices(jam,vertices,_countof(vertices)).offset;
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, window_resolution.x, window_resolution.y, 0.0f, 1.0f };
		ID3D11DeviceContext_RSSetViewports(jam->context, 1, &viewport);

		D3D11_PRIMITIVE_TOPOLOGY topology;
		ID3D11DeviceContext_IAGetPrimitiveTopology(jam->context, &topology);

		ID3D11DeviceContext_IASetPrimitiveTopology(jam->context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ID3D11DeviceContext_OMSetRenderTargets(jam->context, 1, &jam->window_render_target_view, 0);
		f32 fcolor[4] = { };
		ID3D11DeviceContext_ClearRenderTargetView(jam->context, jam->window_render_target_view, fcolor);

		ID3D11DeviceContext_PSSetShaderResources(jam->context, 0, 1, &jam->base_render_target_shader_view);
		ID3D11DeviceContext_PSSetSamplers(jam->context, 0, 1, &jam->samplers[SAMPLER_POINT]);
		ID3D11DeviceContext_Draw(jam->context, 6, vertex_offset);

		ID3D11ShaderResourceView *nullptr_shader_resource_view = {0};
		ID3D11DeviceContext_PSSetShaderResources(jam->context, 0, 1, &nullptr_shader_resource_view);

		ID3D11DeviceContext_IASetPrimitiveTopology(jam->context, topology);
	}
	#endif

	IDXGISwapChain_Present(jam->window_present_mechanism,1,0);

	ID3D11DeviceContext_OMSetRenderTargets(jam->context, 1, &jam->base_render_target_view, 0);

	vec2i resolution = jam->base_resolution;
	{

		vec2 scale = { 2.0 / resolution.x, 2.0 / resolution.y };
		vec2 offset = { -1.0, -1.0 };

		Per_Pass_Constants globals = {};
		globals.transform[0] = (vec4){scale.x, 0, 0, offset.x};
		globals.transform[1] = (vec4){0, scale.y, 0, offset.y};
		globals.transform[2] = (vec4){0, 0, 1, 0};
		globals.transform[3] = (vec4){0, 0, 0, 1};

		ID3D11Buffer *constant_buffer = jam->constant_buffer;
		D3D11_MAPPED_SUBRESOURCE map;
		ID3D11DeviceContext_Map(jam->context, (ID3D11Resource *)constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		CopyMemory(map.pData, &globals, sizeof(globals));
		ID3D11DeviceContext_Unmap(jam->context, (ID3D11Resource *)constant_buffer, 0);
	}

	{
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, resolution.x, resolution.y, 0.0f, 1.0f };
		ID3D11DeviceContext_RSSetViewports(jam->context, 1, &viewport);
	}
}

//	static int r_texture_pass(jam_State *jam, rTextureStruct *texture, Vertex2D *vertices, i32 num_vertices) {
//		int result = rSubmitVertices(jam,vertices,num_vertices);
//		if(!texture) texture = &jam->fallback_texture;
//		ID3D11ShaderResourceView *input = texture->view;
//		ID3D11SamplerState *sampler = texture->sampler;
//		ID3D11DeviceContext_PSSetShaderResources(jam->context, 0, 1, &input);
//		ID3D11DeviceContext_PSSetSamplers(jam->context, 0, 1, &sampler);
//		ID3D11DeviceContext_Draw(jam->context, num_vertices, result);
//		return result;
//	}



#if 0
	r_i32 clipping_region = pass->clipping_region;
	if (clipping_region.width != 0 && clipping_region.height != 0){
		D3D11_RECT rect = {0};
		rect.left = clipping_region.x;
		rect.right = clipping_region.x + clipping_region.width;
		rect.top = clipping_region.y;
		rect.bottom = clipping_region.y + clipping_region.height;
		ID3D11DeviceContext_RSSetScissorRects(jam->context, 1, &rect);
	} else {
		D3D11_RECT rect = {};
		rect.left = 0;
		rect.right = (LONG) resolution.x;
		rect.top = 0;
		rect.bottom = (LONG) resolution.y;
		ID3D11DeviceContext_RSSetScissorRects(jam->context, 1, &rect);
	}
#endif