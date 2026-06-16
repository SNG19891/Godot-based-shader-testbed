@tool
extends Node3D

# Guarda mesh actual por nodo para detectar cambios
var _tracked: Dictionary = {}  
var cameras: Array = []
var current_index: int = 0

func _ready():
	# Nodos ya presentes al cargar la escena
	for node in find_children("*", "MeshInstance3D"):
		_register(node)
	
	# Nodos añadidos después
	get_tree().node_added.connect(_on_node_added)
	
	#Cambios de camara
	for node in find_children("*", "Camera3D"):
		cameras.append(node)
	print("Camaras disponibles: ", cameras.size()) #Deberia salir el numero 4 (ya que hay 4 camaras)
	if cameras.size() > 0:
		_activate(0)

func _on_node_added(node: Node):
	if node is MeshInstance3D:
		# Espera un frame para que el mesh esté asignado
		await get_tree().process_frame
		_register(node)

func _register(mi: MeshInstance3D):
	_tracked[mi] = mi.mesh
	_process_mesh(mi)
	
	# Detecta cambios de propiedad en el nodo
	if not mi.property_list_changed.is_connected(_on_property_changed.bind(mi)):
		mi.property_list_changed.connect(_on_property_changed.bind(mi))

func _on_property_changed(mi: MeshInstance3D):
	if not is_instance_valid(mi):
		return
	# Solo actúa si el mesh cambió
	if _tracked.get(mi) != mi.mesh:
		_tracked[mi] = mi.mesh
		_process_mesh(mi)

func _process_mesh(mi: MeshInstance3D):
	if mi.mesh == null:
		return
	
	# Convierte a ArrayMesh si no lo es
	var array_mesh := _to_array_mesh(mi.mesh)
	if array_mesh == null:
		push_error("No se pudo convertir el mesh de: " + mi.name)
		return
	
	var result := _generate_uvs_if_missing(array_mesh, mi.name)
	if result != null:
		mi.mesh = result
		_tracked[mi] = result  # actualiza referencia

func _to_array_mesh(mesh: Mesh) -> ArrayMesh:
	if mesh is ArrayMesh:
		return mesh
	# Para ImporterMesh, PrimitiveMesh, etc.
	var array_mesh := ArrayMesh.new()
	for i in mesh.get_surface_count():
		var arrays = mesh.surface_get_arrays(i)
		if arrays.is_empty():
			continue
		array_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	return array_mesh

func _generate_uvs_if_missing(old_mesh: ArrayMesh, node_name: String) -> ArrayMesh:
	var new_mesh := ArrayMesh.new()
	var modified := false

	var aabb: AABB = old_mesh.get_aabb()
	var radius: float = max(aabb.size.length() * 0.5, 0.0001)

	for surface in range(old_mesh.get_surface_count()):
		var arrays = old_mesh.surface_get_arrays(surface)
		var vertices: PackedVector3Array = arrays[Mesh.ARRAY_VERTEX]
		var uvs: PackedVector2Array
		if arrays[Mesh.ARRAY_TEX_UV] != null:
			uvs = arrays[Mesh.ARRAY_TEX_UV]
		else:
			uvs = PackedVector2Array()

		if uvs.size() > 0:
			new_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
			continue

		modified = true
		print("Generando UVs para: ", node_name, " superficie: ", surface)

		var generated_uvs := PackedVector2Array()
		generated_uvs.resize(vertices.size())
		for i in range(vertices.size()):
			var p := vertices[i]
			generated_uvs[i] = Vector2(
				(p.x + p.z) / radius,
				p.y / radius
			)

		arrays[Mesh.ARRAY_TEX_UV] = generated_uvs
		new_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)

	return new_mesh if modified else null


#SWITCHING CAMERAS
func _input(event: InputEvent):
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_1: _activate(0)
			KEY_2: _activate(1)
			KEY_3: _activate(2)
			KEY_4: _activate(3)

func _activate(index: int):
	if index >= cameras.size():
		return
	current_index = index
	for cam in cameras:
		cam.current = false
	cameras[index].current = true
	print("Cámara activa: ", cameras[index].name)
