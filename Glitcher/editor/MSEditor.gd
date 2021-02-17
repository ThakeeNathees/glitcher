extends Control

onready var tree :Tree = $Layout/Body/FileTree

func _ready():
	var root = tree.create_item()
	tree.set_hide_root(true)
	var child1 := tree.create_item(root)
	child1.set_text(0, "scripts")
	var child2 = tree.create_item(root)
	child2.set_text(0, "a_file.ms")
	var subchild1 = tree.create_item(child1)
	subchild1.set_text(0, "main.ms")
	
	pass # Replace with function body.


