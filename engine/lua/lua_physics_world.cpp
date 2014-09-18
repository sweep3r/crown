/*
Copyright (c) 2013 Daniele Bartolini, Michele Rossi
Copyright (c) 2012 Daniele Bartolini, Simone Boscaratto

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#include "lua_environment.h"
#include "lua_stack.h"
#include "physics_world.h"
#include "quaternion.h"
#include "memory.h"

namespace crown
{

//-----------------------------------------------------------------------------
static int physics_world_gravity(lua_State* L)
{
	LuaStack stack(L);

	PhysicsWorld* world = stack.get_physics_world(1);

	stack.push_vector3(world->gravity());
	return 1;
}

//-----------------------------------------------------------------------------
static int physics_world_set_gravity(lua_State* L)
{
	LuaStack stack(L);

	PhysicsWorld* world = stack.get_physics_world(1);
	const Vector3& gravity = stack.get_vector3(2);

	world->set_gravity(gravity);
	return 0;
}

//-----------------------------------------------------------------------------
static int physics_world_make_raycast(lua_State* L)
{
	LuaStack stack(L);

	PhysicsWorld* world = stack.get_physics_world(1);
	/* const char* callback = */ stack.get_string(2);
	int mode = stack.get_int(3);
	int filter = stack.get_int(4);

	RaycastId raycast = world->create_raycast((CollisionMode::Enum) mode, (CollisionType::Enum) filter);

	stack.push_raycast(world->get_raycast(raycast));
	return 1;
}

//-----------------------------------------------------------------------------
static int physics_world_overlap_test(lua_State* L)
{
	LuaStack stack(L);

	PhysicsWorld* world = stack.get_physics_world(1);
	CollisionType::Enum filter = (CollisionType::Enum) stack.get_int(2);
	ShapeType::Enum shape_type = (ShapeType::Enum) stack.get_int(3);
	Vector3 pos = stack.get_vector3(4);
	Quaternion rot = stack.get_quaternion(5);
	Vector3 size = stack.get_vector3(6);


	Array<Actor*> actors(default_allocator());

	world->overlap_test(filter, shape_type, pos, rot, size, actors);

	stack.push_table();
	for (uint32_t i = 0; i < array::size(actors); i++)
	{
		stack.push_key_begin(i+1);
		stack.push_actor(actors[i]);
		stack.push_key_end();
	}

	return 1;
}

//-----------------------------------------------------------------------------
static int physics_world_tostring(lua_State* L)
{
	LuaStack stack(L);
	PhysicsWorld* pw = stack.get_physics_world(1);
	stack.push_fstring("PhysicsWorld (%p)", pw);
	return 1;
}

//-----------------------------------------------------------------------------
void load_physics_world(LuaEnvironment& env)
{
	env.load_module_function("PhysicsWorld", "gravity",				physics_world_gravity);
	env.load_module_function("PhysicsWorld", "set_gravity",			physics_world_set_gravity);
	env.load_module_function("PhysicsWorld", "make_raycast",		physics_world_make_raycast);
	env.load_module_function("PhysicsWorld", "overlap_test",		physics_world_overlap_test);
	env.load_module_function("PhysicsWorld", "__index",				"PhysicsWorld");
	env.load_module_function("PhysicsWorld", "__tostring",			physics_world_tostring);

	// Actor types
	env.load_module_enum("ActorType", "STATIC",						ActorType::STATIC);
	env.load_module_enum("ActorType", "DYNAMIC_PHYSICAL",			ActorType::DYNAMIC_PHYSICAL);
	env.load_module_enum("ActorType", "DYNAMIC_KINEMATIC",			ActorType::DYNAMIC_KINEMATIC);

	// Shape types
	env.load_module_enum("ShapeType", "SPHERE",						ShapeType::SPHERE);
	env.load_module_enum("ShapeType", "CAPSULE",					ShapeType::CAPSULE);
	env.load_module_enum("ShapeType", "BOX",						ShapeType::BOX);
	env.load_module_enum("ShapeType", "PLANE",						ShapeType::PLANE);
	env.load_module_enum("ShapeType", "CONVEX_MESH",				ShapeType::CONVEX_MESH);

	// SceneQuery modes
	env.load_module_enum("CollisionMode", "CLOSEST",				CollisionMode::CLOSEST);
	env.load_module_enum("CollisionMode", "ANY",					CollisionMode::ANY);
	env.load_module_enum("CollisionMode", "ALL",					CollisionMode::ALL);

	// SceneQuery filters
	env.load_module_enum("CollisionType", "STATIC",					CollisionType::STATIC);
	env.load_module_enum("CollisionType", "DYNAMIC",				CollisionType::DYNAMIC);
	env.load_module_enum("CollisionType", "BOTH",					CollisionType::BOTH);
}

} // namespace crown