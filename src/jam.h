#if !defined(JAPI)
	#define JAPI static
#endif


// typedef struct {
// 	vec2 basis_x,basis_y;
// 	vec2 center;
// 	vec2 translation;
// } trans2d;

// static inline trans2d trans2d_rotation(f32 rotation, vec2 center) {
// 	vec2 basis_x = (vec2){cos(rotation),sin(rotation)};
// 	vec2 basis_y = (vec2){-basis_x.y,basis_x.x};
// 	return (trans2d){basis_x,basis_y,center,vec2(0,0)};
// }
// static inline trans2d trans2d_translation(vec2 translation) {
// 	return (trans2d){vec2(1,0),vec2(0,1),vec2(0,0),translation};
// }
//
// todo: simplify this...
//
// T + C + ((X - C) * BX + (Y - C) * BY)
//
// T + C +
//  X * BX - C * BX +
//  Y * BY - C * BY
//
// static inline vec2 apply_trans2d(trans2d trans, vec2 v) {
// 	vec2 o =
// 	vec2_add(vec2_add(trans.translation,trans.center)
// 	, vec2_add(
// 	vec2_mul(trans.basis_x,vec2_sub(vec2(v.x,v.x),trans.center)),
// 	vec2_mul(trans.basis_y,vec2_sub(vec2(v.y,v.y),trans.center))));
// 	return o;
// }

typedef struct Controller_State Controller_State;
struct Controller_State {
	b32 shoulders[2];
	b32 start;
	b32 back;
	b32 DPAD[4];
	b32 ABXY[4];
	b32 bthumbs[2];
	f32 triggers[2];
	vec2 thumbs[2];
};



// static char *j_obj2str[]={
// 	"none",
// 	"image",
// 	"sound",
// 	"texture",
// };


// ----------------
// DRAW
// ----------------





typedef struct jam_State jam_State;
struct jam_State {
	// MUST BE FIRST FIELD
	elf_State  R;

	R_Renderer *rend;
	OS_Window  *window;

	i64 begin_cycle_clock;
	f64 clocks_to_seconds;
	f64 target_seconds_to_sleep;
	f64 pending_seconds_to_sleep;
};