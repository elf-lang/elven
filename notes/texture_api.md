//
//	TEXTURE API:
//
// NewTexture(w, h, config ?= ...)
//	LoadTexture(file_name)
//	LoadTextureFromImage(image)
//
//	NewSamplingRegionMapper(w, h, size)
//	SetMapperRegion(region_id, x, y, w, h)
//	SetMapperRegionUV(region_id, x, y, w, h)
//
//
//
// get number of texture registers
//	GetNumTextureRegisters()
//
// get a texture from a texture register
//	GetTexture(register ?= 0)
//
// register a texture
//	SetTexture(texture_handle, register ?= 0)
//
//	register a region mapper, region_mapper_registers == texture_registers
//	SetTextureRegionMapper(mapper, register ?= 0)
// 	or SetRegionMapper(mapper, register ?= 0)
//
