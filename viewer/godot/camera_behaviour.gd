extends Node3D

@export var rotation_speed = 0.005

var _dragging = false
var _last_mouse

func _input(event):

	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT:
			_dragging = event.pressed
			if _dragging:
				_last_mouse = event.position

	if event is InputEventMouseMotion and _dragging:
		var delta = event.position - _last_mouse
		_last_mouse = event.position
		_orbit(delta)

func _orbit(delta):

	rotate_y(-delta.x * rotation_speed)

	var new_pitch = clamp(
		rotation.x - delta.y * rotation_speed,
		-PI/2 + 0.05,
		PI/2 - 0.05
	)

	rotation.x = new_pitch
