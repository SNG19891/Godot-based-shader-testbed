@tool
extends SceneTree

const SCENE_PATH = "res://godot/DefaultScene.tscn"
const SHADER_PATH = "res://godot/GDshaders/DefaultShader.gdshader"
const OBJ_PATH = "res://models/default.obj"

func _initialize():

	print("Creando escena por CLI")

	var rootNode = create_root()
	var mesh_instance = create_mesh_instance()

	if mesh_instance == null:
		quit(1)
		return

	rootNode.add_child(mesh_instance)
	mesh_instance.owner = rootNode
	var script = load("res://godot/TestRunner.gd")
	mesh_instance.set_script(script)

	setup_environment(rootNode, mesh_instance)

	save_scene(rootNode)

	quit()


func create_root() -> Node3D:

	var rootNode = Node3D.new()
	rootNode.name = "Root"

	var nodeBehaviour = load("res://godot/nodeBehaviour.gd")

	if nodeBehaviour == null:
		push_error("No se pudo cargar nodeBehaviour.bd")
		return rootNode

	rootNode.set_script(nodeBehaviour)

	return rootNode


func create_mesh_instance() -> MeshInstance3D:

	var mesh = load(OBJ_PATH)

	if mesh == null:
		push_error("No se pudo cargar el OBJ")
		return null

	var mi = MeshInstance3D.new()
	mi.name = "Model"
	mi.mesh = mesh

	mi.material_override = create_material()

	return mi


func create_material() -> ShaderMaterial:

	var shader = create_default_shader()

	var err = ResourceSaver.save(shader, SHADER_PATH)

	if err == OK:
		print("Shader creado correctamente")

	var material = ShaderMaterial.new()
	material.shader = shader

	return material


func create_default_shader() -> Shader:

	var shader = Shader.new()

	shader.code = """
shader_type spatial;

render_mode unshaded; //elimina la iluminación
render_mode vertex_lighting;
render_mode skip_vertex_transform;
render_mode diffuse_lambert;
//render_mode cull_disabled; //desactiva el backface culling (esta activado por defecto, descomentar esta linea para desactivarlo)


void vertex(){

	POSITION = PROJECTION_MATRIX * MODELVIEW_MATRIX * vec4(VERTEX,1.0);
	COLOR = vec4(COLOR.rgb,1.0);
}

void fragment(){

	ALBEDO = COLOR.rgb;
}
"""

	return shader

func create_front_camera(center, dist, rootNode:Node3D):

	var cam = Camera3D.new()
	cam.name = "FrontCamera"
	var pos = center + Vector3(0,0,dist)
	cam.position = pos
	cam.look_at_from_position(pos,center,Vector3.UP)

	rootNode.add_child(cam)
	cam.owner = rootNode
	cam.add_to_group("cameras")
	cam.make_current()

func create_side_camera(center, dist, rootNode:Node3D):

	var cam = Camera3D.new()
	cam.name = "SideCamera"
	var pos = center + Vector3(dist,0,0)
	cam.position = pos
	cam.look_at_from_position(pos,center,Vector3.UP)
	
	rootNode.add_child(cam)
	cam.owner = rootNode
	cam.add_to_group("cameras")


func create_top_camera(center, dist, rootNode:Node3D):
	var cam = Camera3D.new()
	cam.name = "TopCamera"
	var pos = center + Vector3(0,dist, 0)
	cam.position = pos
	cam.look_at_from_position(pos,center,Vector3(0,0,1))
	rootNode.add_child(cam)
	cam.owner = rootNode
	cam.add_to_group("cameras")

func create_orbit_camera(center, cam_dist, rootNode:Node3D):
	var pivot = Node3D.new()
	pivot.name = "OrbitCameraPivot"
	pivot.position = center

	rootNode.add_child(pivot)
	pivot.owner = rootNode

	var script = load("res://godot/camera_behaviour.gd")
	pivot.set_script(script)

	var camera = Camera3D.new()
	camera.name = "OrbitCamera"
	camera.position = Vector3(0,0,cam_dist)
	camera.add_to_group("cameras")

	pivot.add_child(camera)
	camera.owner = rootNode


func create_light(center, rootNode):
	var pivot = Node3D.new()
	pivot.name = "OrbitLightPivot"
	pivot.position = center
	rootNode.add_child(pivot)
	pivot.owner = rootNode
	var light_script = load("res://godot/light_behaviour.gd")
	pivot.set_script(light_script)
	
	var light =  DirectionalLight3D.new()
	light.name = "OrbitLight"
	light.position = Vector3(0,0,5)

	pivot.add_child(light)
	light.owner = rootNode
	
func setup_environment(rootNode:Node3D, model:Node3D):

	var aabb = model.mesh.get_aabb()
	var center = aabb.position + aabb.size * 0.5
	var radius = max(aabb.size.length() * 0.5, 1.0)

	var cam_dist = radius * 3.0
	
	create_front_camera(center, cam_dist, rootNode)

	create_side_camera(center, cam_dist, rootNode)

	create_top_camera(center, cam_dist, rootNode)

	create_orbit_camera(center, cam_dist,rootNode)

	create_light(center, rootNode)



func save_scene(rootNode:Node3D):

	var packed = PackedScene.new()

	packed.pack(rootNode)

	var err = ResourceSaver.save(packed, SCENE_PATH)

	if err == OK:
		print("Escena creada correctamente")
	else:
		push_error("Error guardando escena")
