extends Node

## Action keys. Used as is_action_pressed(Game.keys.UP.name).
var keys:Dictionary

## An internal struct to action keys.
class Action:
	var name :String = ""
	var scanecode :int = 0
	func _init(name:String, scanecode:int):
		self.name = name
		self.scanecode = scanecode

## Returns if action pressed. Set parameter [just] to get if the key is just
## pressed.
func is_pressed(action:Action, just:bool) -> bool:
	if just:
		return Input.is_action_just_pressed(action.name)
	return Input.is_action_pressed(action.name)

## Returns true if the action is just released.
func is_released(action:Action) -> bool:
	return Input.is_action_just_released(action.name)
	

## Returns the actions strength between 0 - 1.
func get_action_strength(action:Action) -> float:
	return Input.get_action_strength(action.name)

func _ready():
	
	## Initialize keys.
	keys.LEFT     = Action.new("k_left",     KEY_A)
	keys.RIGHT    = Action.new("k_right",    KEY_D)
	keys.UP       = Action.new("k_up",       KEY_W)
	keys.DOWN     = Action.new("k_down",     KEY_S)
	keys.INTERACT = Action.new("k_interact", KEY_E)
	
	for key in keys:
		var action = keys[key]
		assert(not InputMap.has_action(action.name))
		InputMap.add_action(action.name)
		var event = InputEventKey.new()
		event.scancode = action.scanecode
		InputMap.action_add_event(action.name, event)


