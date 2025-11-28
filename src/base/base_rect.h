typedef union {
	struct { vec2 pos, size;  };
	struct { f32 x, y, w, h;   };
	struct { f32 xy[2], wh[2]; };
} rect_f32;

typedef union {
	struct { vec2i pos, size;  };
	struct { i32 x, y, w, h;   };
	struct { i32 xy[2], wh[2]; };
} rect_i32;

#define RECT_MATCH(a, b) ((a).x == (b).x && (a).y == (b).y && (a).w == (b).w && (a).h == (b).h)

PORTABLEFUNC
rect_i32 rect_i32_clip(rect_i32 r, rect_i32 c)
{
	r.x = Max(r.x, c.x);
	r.y = Max(r.y, c.y);
	r.w = Max(0, Min(r.x + r.w, c.x + c.w) - r.x);
	r.h = Max(0, Min(r.y + r.h, c.y + c.h) - r.y);
	return r;
}

PORTABLEFUNC
rect_f32 rect_f32_clip(rect_f32 r, rect_f32 c)
{
	r.x = Max(r.x, c.x);
	r.y = Max(r.y, c.y);
	r.w = Max(0, Min(r.x + r.w, c.x + c.w) - r.x);
	r.h = Max(0, Min(r.y + r.h, c.y + c.h) - r.y);
	return r;
}

PORTABLEFUNC
rect_i32 rect_i32_from_pos_size(vec2i pos, vec2i size)
{
	rect_i32 result = { pos, size };
	return result;
}

PORTABLEFUNC
rect_i32 rect_i32_from_size(vec2i size)
{
	rect_i32 r = {};
	r.size = size;
	return r;
}

PORTABLEFUNC
rect_f32 rect_f32_rounded_outwards(rect_f32 r)
{
	r.x = floorf(r.x);
	r.y = floorf(r.y);
	r.w =  ceilf(r.w);
	r.h =  ceilf(r.h);
	return r;
}

PORTABLEFUNC
rect_i32 rect_i32_from_f32(rect_f32 r)
{
	rect_i32 i;
	i.x = (i32) floorf(r.x);
	i.y = (i32) floorf(r.y);
	i.w = (i32)  ceilf(r.w);
	i.h = (i32)  ceilf(r.h);
	return i;
}

PORTABLEFUNC
rect_i32 rect_i32_from_extremes(vec2i p0, vec2i p1)
{
	rect_i32 i;
	i.x = Min(p0.x, p1.x);
	i.y = Min(p0.y, p1.y);
	i.w = ABS(p1.x - p0.x);
	i.h = ABS(p1.y - p0.y);
	return i;
}

PORTABLEFUNC
rect_f32 rect_f32_from_pos_size(vec2 pos, vec2 size)
{
	rect_f32 result = { pos, size };
	return result;
}

PORTABLEFUNC
rect_f32 rect_f32_from_axis(i32 axis, f32 x, f32 y, f32 w, f32 h)
{
	rect_f32 result;
	result.xy[ axis] = x;
	result.xy[!axis] = y;
	result.wh[ axis] = w;
	result.wh[!axis] = h;
	return result;
}

PORTABLEFUNC
rect_f32 rect_f32_from_size(vec2 size)
{
	rect_f32 result = { 0, 0, size };
	return result;
}

PORTABLEFUNC
rect_f32 rect_f32_mapped_from(rect_f32 r, vec2 c, vec2 t)
{
	rect_f32 o = { .size = c };
	o.x = r.x + (r.size.x - c.x) * t.x;
	o.y = r.y + (r.size.y - c.y) * t.y;
	return o;
}

PORTABLEFUNC
rect_f32 trim_rect(rect_f32 rec, f32 size)
{
	rec.x += size;
	rec.y += size;
	rec.size.x -= size*2;
	rec.size.y -= size*2;
	return rec;
}

PORTABLEFUNC
rect_i32 rect_i32_trim(rect_i32 rec, i32 size)
{
	rec.x += size;
	rec.y += size;
	rec.size.x -= size*2;
	rec.size.y -= size*2;
	return rec;
}

PORTABLEFUNC
rect_i32 rect_i32_from_slice(rect_i32 r, i32 axis, i32 size)
{
	rect_i32 o;
	o.xy[ axis] = r.xy[ axis] + (size < 0) * (r.wh[axis] + size);
	o.xy[!axis] = r.xy[!axis];
	o.wh[ axis] = ABS(size);
	o.wh[!axis] = r.wh[!axis];
	return o;
}

PORTABLEFUNC
rect_i32 rect_i32_slice(rect_i32 *r, i32 axis, i32 size)
{
	rect_i32 o = rect_i32_from_slice(*r, axis, size);
	r->xy[axis] += MAX(0, size);
	r->wh[axis] -= ABS(size);
	return o;
}

PORTABLEFUNC
rect_f32 rect_f32_from_slice(rect_f32 r, i32 axis, f32 size)
{
	rect_f32 o;
	o.xy[ axis] = r.xy[ axis] + (size < 0) * (r.wh[axis] + size);
	o.xy[!axis] = r.xy[!axis];
	o.wh[ axis] = ABS(size);
	o.wh[!axis] = r.wh[!axis];
	return o;
}

PORTABLEFUNC
rect_f32 rect_f32_slice(rect_f32 *r, i32 axis, f32 size)
{
	rect_f32 o = rect_f32_from_slice(*r, axis, size);
	r->xy[axis] += MAX(0, size);
	r->wh[axis] -= ABS(size);
	return o;
}

PORTABLEFUNC
rect_f32 rect_split(rect_f32 *r, i32 axis, f32 ratio)
{
	f32 size = r->size.xy[axis] * ratio;
	rect_f32 o = { r->pos };
	o.size.xy[axis] = size;
	o.size.xy[!axis] = r->size.xy[!axis];

	r->pos.xy[axis] += size;
	r->size.xy[axis] -= size;
	return o;
}

PORTABLEFUNC
b32 rect_f32_contains_point(rect_f32 r, vec2 p)
{
	return p.x > r.x && p.y > r.y && r.x + r.w > p.x && r.y + r.h > p.y;
}

PORTABLEFUNC
rect_f32 rect_f32_offset_from(rect_f32 r, vec2 trans)
{
	r.x += trans.x;
	r.y += trans.y;
	return r;
}

PORTABLEFUNC
rect_f32 rect_f32_offset_by_axis_from(rect_f32 r, i32 axis, f32 trans)
{
	r.xy[axis] += trans;
	return r;
}

PORTABLEFUNC
rect_i32 rect_abs(rect_i32 r)
{
	if(r.size.x < 0) (r.x += r.size.x, r.size.x = -r.size.x);
	if(r.size.y < 0) (r.y += r.size.y, r.size.y = -r.size.y);
	return r;
}

PORTABLEFUNC
b32 r_touches(rect_i32 a, rect_i32 b)
{
	return a.x <= b.x + b.size.x && a.x + a.size.x > b.x
	&&     a.y <= b.y + b.size.y && a.y + a.size.y > b.y;
}

PORTABLEFUNC
UV_Coords uv_coords_from_rect_i32(rect_i32 rect, vec2 inv_scale) {
	UV_Coords coords;
	coords.x0 = (rect.x + rect.w * 0) * inv_scale.x;
	coords.y0 = (rect.y + rect.h * 0) * inv_scale.y;
	coords.x1 = (rect.x + rect.w * 1) * inv_scale.x;
	coords.y1 = (rect.y + rect.h * 1) * inv_scale.y;
	return coords;
}
