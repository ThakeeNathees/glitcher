extends Node2D

const EDITOR_SCN:PackedScene = preload("res://editor/MSEditor.tscn")
var editor :Control= EDITOR_SCN.instance()

func _ready():
	editor.get_node("run").connect("button_up", self, "run_miniscript")
	#$device/Area2D.connect("body_entered", self, "add_editor")
	#$device/Area2D.connect("body_exited", self, "remove_editor")
	
	## To update it's rect_size.
	add_child(editor)
	remove_child(editor)
	editor.rect_position = (get_viewport_rect().size - editor.rect_size)/2

func add_editor(body_p):
	add_child(editor)
	
func remove_editor(body_p):
	remove_child(editor)

func run_miniscript():
	
	var console :RichTextLabel = editor.get_node("Layout/Body/TextArea/Console")
	console.clear()
	MiniScript.set_source(
		editor.get_node("Layout/Body/TextArea/TextEdit").text)
	var result:int = MiniScript.interpret()
	if result == 0:
		console.text = MiniScript.get_stdout()
	elif result == 1: ## compile time
		var errors = MiniScript.get_errors()
		for e in errors:
			console.text += e.message + '\n    at ' + e.file + ':' + str(e.line) + '\n'
	else: ## runtime error
		pass
