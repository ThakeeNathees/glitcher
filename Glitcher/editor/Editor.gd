extends Node2D

func _ready():
	return
	for i in range(20000):
		print(i)

func _input(event):
	if event is InputEventKey:
		if event.is_pressed():
			if false and event.scancode == KEY_CAPSLOCK:
				MiniScript.set_source($TextArea.get_text())
				$TextArea.clear_errors()
				var result = MiniScript.interpret()
				if result == 0:
					print(MiniScript.get_stdout())
				else:
					for err in MiniScript.get_errors():
						$TextArea.set_line_error(err.line)
						print(err.message)
				
				pass
	


func _on_ColorPicker_color_changed(color):
	$TextArea.set_test_color(color)
