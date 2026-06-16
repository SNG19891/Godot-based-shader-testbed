@tool
extends SceneTree

const SCENE_PATH = "res://godot/DefaultScene.tscn"
const OBJ_PATH = "res://models/default.obj"
const SHADER_DIR = "res://godot/GDshaders/"

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

	if nodeBehaviour != null:
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

	var latest_shader_path = get_latest_shader()

	if latest_shader_path == "":
		push_error("No se encontró ningún shader")
		return ShaderMaterial.new()

	var shader : Shader = load(latest_shader_path)

	if shader == null:
		push_error("No se pudo cargar el shader")
		return ShaderMaterial.new()
	else: 
		print("Cargando " + latest_shader_path)

	var material = ShaderMaterial.new()
	material.shader = shader

	return material


func get_latest_shader() -> String:

	var dir := DirAccess.open(SHADER_DIR)

	if dir == null:
		return ""

	dir.list_dir_begin()

	var latest_time := 0
	var latest_file := ""

	while true:

		var file := dir.get_next()

		if file == "":
			break

		if file.ends_with(".gdshader"):

			var full_path = SHADER_DIR + file
			var mod_time = FileAccess.get_modified_time(full_path)

			if mod_time > latest_time:
				latest_time = mod_time
				latest_file = full_path

	dir.list_dir_end()

	return latest_file


func create_front_camera(center, dist, rootNode:Node3D):

	var cam = Camera3D.new()
	cam.name = "FrontCamera"

	var pos = center + Vector3(0,0,dist)
	cam.position = pos
	cam.look_at_from_position(pos, center, Vector3.UP)

	rootNode.add_child(cam)
	cam.owner = rootNode
	cam.add_to_group("cameras")
	cam.make_current()


func create_side_camera(center, dist, rootNode:Node3D):

	var cam = Camera3D.new()
	cam.name = "SideCamera"

	var pos = center + Vector3(dist,0,0)
	cam.position = pos
	cam.look_at_from_position(pos, center, Vector3.UP)

	rootNode.add_child(cam)
	cam.owner = rootNode
	cam.add_to_group("cameras")


func create_top_camera(center, dist, rootNode:Node3D):

	var cam = Camera3D.new()
	cam.name = "TopCamera"

	var pos = center + Vector3(0,dist,0)
	cam.position = pos
	cam.look_at_from_position(pos, center, Vector3(0,0,1))

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
	if script != null:
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
	if light_script != null:
		pivot.set_script(light_script)

	var light = DirectionalLight3D.new()
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
	create_orbit_camera(center, cam_dist, rootNode)
	create_light(center, rootNode)


func save_scene(rootNode:Node3D):

	var packed = PackedScene.new()
	packed.pack(rootNode)

	var err = ResourceSaver.save(packed, SCENE_PATH)

	if err == OK:
		print("Escena creada correctamente")
	else:
		push_error("Error guardando escena")
