//	== LONG TERM DATA STORAGE ==
//
//
// A handle may come in different forms, but it typically boils
// down to a pointer sized integer, its interpretation is unknown
// and irrelevant you.
//
//
//	You get a handle from some other system, and only the system from
// which you got the handle knows what you're referring to.
//
// So, a handle's primary purpose it to allow for your program to
// communicate with some foreign system about a thing in particular.
//
//	A handle is typically a proxy for some memory.
//
// Could be memory on the CPU, or on the GPU.
//
// Might even be special memory, "read-only", "write-only", it depends.
//
// The point is, it's probably just memory that is being used in very
// particular ways.
//
//
//
//	Whenever you call LoadTexture(), you receive a new handle, this handle is
// only valid during the duration of your program.
//
//	So if you were to save this handle to a file and attempt to reuse it
// after your program has terminated, it will be invalid.
//
//
//	Once your program ends, all those resources are reclaimed, and your handles
// to it become invalid.
//
//	This is a problem if you ever need a way to remember what it is that you're
// talking about 3 or 4 weeks from now.
//
// A practical way to do this is to use stable id's, id's that never change and
// always refer to the same concept or idea you have in mind.
//
//
//
//	TEXTURES :: enum {
//		RID_NONE = 0,
//		TEXTURE_TILESET,
//		TEXTURE_MAIN_CHARACTER,
//		TEXTURE_BAD_GUY_0,
//		TEXTURE_BAD_GUY_1,
//	}
//
//
//
//
//	live_handles := {}
//
//	LoadTextureId :: function(id, name) ? {
//		live_handles[id] = LoadTexture(name)
//	}
//
//	SetTextureId :: function(id) ? {
//		SetTexture(live_handles[id])
//	}
//
//
//	; load the texture for this id
//	LoadTexture(TEXTURE_MAIN_CHARACTER, "data/main_char.png")
//
//	; bind the texture for this id
//	SetTextureId(TEXTURE_MAIN_CHARACTER)
//
