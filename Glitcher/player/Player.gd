extends KinematicBody2D

var speed :int = 100
var velocity := Vector2.ZERO

func _ready():
	pass

func _physics_process(delta):
	var input:Vector2 = Vector2.ZERO
	input.x = Game.get_action_strength(Game.keys.RIGHT) - \
			  Game.get_action_strength(Game.keys.LEFT)
	input.y = Game.get_action_strength(Game.keys.DOWN) - \
			  Game.get_action_strength(Game.keys.UP)

	if input != Vector2.ZERO:
		input = input.normalized()
		velocity = input * speed
	else:
		velocity = Vector2.ZERO
	
	velocity = move_and_slide(velocity)

