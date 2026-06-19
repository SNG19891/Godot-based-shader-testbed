extends Node3D

@export var rotation_speed: float = 0.005
@export var light_distance: float = 5.0

var _dragging: bool = false
var _last_mouse: Vector2


func _input(event: InputEvent):
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_RIGHT:
			_dragging = event.pressed
			if _dragging:
				_last_mouse = event.position

	if event is InputEventMouseMotion and _dragging:
		var delta = event.position - _last_mouse
		_last_mouse = event.position
		_orbit(delta)

func _orbit(delta: Vector2):
	rotate_y(-delta.x * rotation_speed)
	var new_pitch = clamp(
		rotation.x - delta.y * rotation_speed,
		-PI / 2 + 0.05,
		PI / 2 - 0.05
	)
	rotation.x = new_pitch
