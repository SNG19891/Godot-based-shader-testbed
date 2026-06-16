@tool
extends Node

@export_tool_button("Ejecutar Test", "Callable") var run_action := select_test


var scene_root: Node3D
var test_scene_root: Node3D
var model: MeshInstance3D

func select_test():
	var dialog := FileDialog.new()
	dialog.access = FileDialog.ACCESS_FILESYSTEM
	dialog.file_mode = FileDialog.FILE_MODE_OPEN_FILE
	dialog.filters = PackedStringArray(["*.test"])
	dialog.title = "Selecciona test"

	dialog.file_selected.connect(func(path):
		run(path)
		dialog.queue_free()
	)

	get_tree().root.add_child(dialog)
	dialog.popup_centered()

func _get_scene_root():
	if Engine.is_editor_hint():
		return get_tree().edited_scene_root
	return get_tree().current_scene
	
func create_test_scene():
	# Elimina la anterior si existe
	if is_instance_valid(test_scene_root):
		test_scene_root.queue_free()

	test_scene_root = Node3D.new()
	test_scene_root.name = "TestScene"

	get_tree().edited_scene_root.add_child(test_scene_root)

	scene_root = test_scene_root
	model = null

	print("TestScene creada")

func run(path:String):
	create_test_scene()
	if !FileAccess.file_exists(path):
		push_error("No existe: " + path)
		return

	var file = FileAccess.open(path, FileAccess.READ)

	while !file.eof_reached():
		var line = file.get_line().strip_edges()
		if line == "":
			continue

		var tokens = line.split(" ", false)
		execute_command(tokens)
	file.close()
	print("Test finalizado")
	
func execute_command(tokens: Array):
	if tokens.is_empty():
		return

	var cmd = tokens[0]
	match cmd:
		# Crea una escena vacía
		"clearScene":
			clear_scene()

		# Ignorar
		"resize":
			pass

		# Carga un objeto obj a la escena
		"loadObject":
			load_object(tokens[1])

		#Añade un obj a la escena
		"addObject":
			add_object(tokens[1])

		# Ignora
		"wire":
			pass

		#Ignora
		"solid":
			pass

		# Ignora
		"axes":
			pass

		#Ignora
		"noaxes":
			pass

		# Ignora
		"clearColor":
			pass

		# Cámara
		"resetCamera":
			reset_camera()

		"dist":
			set_camera_distance(float(tokens[1]))

		"pitch":
			set_camera_pitch(float(tokens[1]))

		"yaw":
			set_camera_yaw(float(tokens[1]))

		"roll":
			set_camera_roll(float(tokens[1]))

		"preset1":
			camera_preset(1)

		"preset2":
			camera_preset(2)

		"preset3":
			camera_preset(3)

		"preset4":
			camera_preset(4)

		"preset5":
			camera_preset(5)

		# Texturas ( Carga textura en TEXTUREi )
		"unit0":
			load_texture(0, tokens[1])

		"unit1":
			load_texture(1, tokens[1])

		"unit2":
			load_texture(2, tokens[1])

		"unit3":
			load_texture(3, tokens[1])

		# Luces / materiales
		"lightAmbient":
			set_light_ambient(
				Vector3(
					float(tokens[1]),
					float(tokens[2]),
					float(tokens[3])
				)
			)

		"lightDiffuse":
			set_light_diffuse(
				Vector3(
					float(tokens[1]),
					float(tokens[2]),
					float(tokens[3])
				)
			)

		"lightSpecular":
			set_light_specular(
				Vector3(
					float(tokens[1]),
					float(tokens[2]),
					float(tokens[3])
				)
			)

		"lightPositionEye":
			set_light_position_eye(
				Vector4(
					float(tokens[1]),
					float(tokens[2]),
					float(tokens[3]),
					float(tokens[4])
				)
			)

		"lightPositionWorld":
			set_light_position_world(
				Vector4(
					float(tokens[1]),
					float(tokens[2]),
					float(tokens[3]),
					float(tokens[4])
				)
			)

		"resetLightPosition":
			reset_light_position()

		# Material
		"matAmbient":
			set_material_ambient(
				Vector3(
					float(tokens[1]),
					float(tokens[2]),
					float(tokens[3])
				)
			)

		"matDiffuse":
			set_material_diffuse(
				Vector3(
					float(tokens[1]),
					float(tokens[2]),
					float(tokens[3])
				)
			)

		"matSpecular":
			set_material_specular(
				Vector3(
					float(tokens[1]),
					float(tokens[2]),
					float(tokens[3])
				)
			)

		"matShininess":
			set_material_shininess(
				float(tokens[1])
			)

		# Captura ( Marca el fin/inicio del siguiente test ) 
		"grab":
			grab(tokens[1])

		# Uniforms ( Definir el valor de uniform ) 
		"int":
			set_uniform_int(
				tokens[1],
				int(tokens[2])
			)

		"float":
			set_uniform_float(
				tokens[1],
				float(tokens[2])
			)

		"bool":
			set_uniform_bool(
				tokens[1],
				tokens[2] == "true" or tokens[2] == "1"
			)

		# Definir el valor de time
		"time":
			set_uniform_float(
				"time",
				float(tokens[1])
			)

		_:
			print("Comando desconocido: ", cmd)

func assign_default_material(mi: MeshInstance3D):

	var shader = load(
		"res://godot/GDshaders/DefaultShader.gdshader"
	)

	if shader == null:
		return

	var mat = ShaderMaterial.new()
	mat.shader = shader

	mi.material_override = mat

func clear_scene():
	if scene_root == null:
		return

	for child in scene_root.get_children():
		child.queue_free()

	model = null

func load_object(fName:String):

	var path = "res://models/" + fName.get_file()

	var mesh = load(path)

	if mesh == null:
		push_error("No se pudo cargar: " + path)
		return

	clear_scene()

	model = MeshInstance3D.new()
	model.name = "Model"
	model.mesh = mesh

	assign_default_material(model)

	scene_root.add_child(model)
	model.owner = scene_root

	print("Objeto cargado:", path)


func add_object(fName:String):

	var path = "res://models/" + fName.get_file()

	var mesh = load(path)

	if mesh == null:
		push_error("No se pudo cargar: " + path)
		return

	var mi = MeshInstance3D.new()

	mi.mesh = mesh

	assign_default_material(mi)

	scene_root.add_child(mi)
	mi.owner = scene_root

	print("Objeto añadido:", path)

func reset_camera():
	pass

func set_camera_distance(value:float):
	pass

func set_camera_pitch(value:float):
	pass

func set_camera_yaw(value:float):
	pass

func set_camera_roll(value:float):
	pass

func camera_preset(id:int):
	pass


var texture_uniforms = {
	0: "colorMap",
	1: "colorMap1",
	2: "colorMap2",
	3: "colorMap3"
}
func load_texture(unit:int, path:String):

	if model == null:
		return

	var tex = load(path)

	if tex == null:
		push_error("No se pudo cargar textura: " + path)
		return

	var mat = model.material_override

	if !(mat is ShaderMaterial):
		return

	if !texture_uniforms.has(unit):
		return

	mat.set_shader_parameter(
		texture_uniforms[unit],
		tex
	)


func set_light_ambient(value:Vector3):
	pass

func set_light_diffuse(value:Vector3):
	pass

func set_light_specular(value:Vector3):
	pass

func set_light_position_eye(value:Vector4):
	pass

func set_light_position_world(value:Vector4):
	pass

func reset_light_position():
	pass


func set_material_ambient(value:Vector3):
	pass

func set_material_diffuse(value:Vector3):
	pass

func set_material_specular(value:Vector3):
	pass

func set_material_shininess(value:float):
	pass


func grab(name:String):
	pass


func set_uniform_int(namei:String, value:int):
	if model == null:
		return

	var mat = model.material_override

	if mat is ShaderMaterial:
		mat.set_shader_parameter(namei, value)

func set_uniform_float(namef:String, value:float):
	if model == null:
		return

	var mat = model.material_override

	if mat is ShaderMaterial:
		mat.set_shader_parameter(namef, value)

func set_uniform_bool(nameb:String, value:bool):

	if model == null:
		return

	var mat = model.material_override

	if mat is ShaderMaterial:
		mat.set_shader_parameter(nameb, value)
