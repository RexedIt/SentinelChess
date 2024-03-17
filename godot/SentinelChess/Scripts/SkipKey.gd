extends Node2D

@export var duration : float
@export var loadscene : String


var elapsed : float

# Called when the node enters the scene tree for the first time.
func _ready():
	set_process_input(true) 
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	elapsed += delta
	if (!loadscene.is_empty()):
		var skip : bool = elapsed >= duration
		if (Input.is_anything_pressed()): 
			skip = true
		if (skip):
			get_tree().change_scene_to_file(loadscene)
		
		
