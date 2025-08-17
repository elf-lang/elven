

typedef struct {
	u16 id;
	u16 flags;

	u16 state;
	u16 sprite;

	u32 extra;

	vec2i pos;
	vec2i bdy;

	u32 state_time;
} Entity;

Entity g_entities[128];

typedef struct Collision_Body {
	Entity *en;
	int tile;
} Collision_Body;


static void QueryCollisionBodies(int x, int y, int w, int h) {

}