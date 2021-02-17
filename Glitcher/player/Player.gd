extends KinematicBody2D

#const TARGET_FPS = 60
const ACCELERATION = 800
const MAX_SPEED = 200
const FRICTION = .25
const AIR_RESISTANCE = .02
const GRAVITY = 600
const JUMP_FORCE = 300

var motion = Vector2.ZERO
onready var sprite = $Sprite

## TODO: testing (call run_miniscript() if KEY_I pressed)
func _input(event):
	if event is InputEventKey:
		if event.pressed and event.scancode == KEY_I:
			pass

func _ready():
	pass

func _physics_process(delta):
	
	var x_input = int(Input.is_key_pressed(KEY_D)) - \
				  int(Input.is_key_pressed(KEY_A))
	
	if x_input != 0:
		#anim.play("Run")
		motion.x += x_input * ACCELERATION * delta #* TARGET_FPS
		motion.x = clamp(motion.x, -MAX_SPEED, MAX_SPEED)
		sprite.flip_h = x_input < 0
	else:
		#anim.play("Stand")
		pass
	
	if is_on_floor():
		if x_input == 0:
			motion.x = lerp(motion.x, 0, FRICTION)
			
		if Input.is_action_just_pressed("ui_accept"):
			motion.y = -JUMP_FORCE
	else:
		##anim.play("Jump")
		if Input.is_action_just_released("ui_accept") and motion.y < -JUMP_FORCE/2:
			motion.y = -JUMP_FORCE/2
			
		if x_input == 0:
			motion.x = lerp(motion.x, 0, AIR_RESISTANCE)
			
	
	motion.y += GRAVITY * delta
	motion = move_and_slide(motion, Vector2.UP)
