extends Node2D

onready var editor :Control = $MSEditor

func _ready():
	editor.get_node("run").connect("button_up", self, "run_miniscript")


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
