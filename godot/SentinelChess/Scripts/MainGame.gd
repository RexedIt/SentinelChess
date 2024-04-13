extends Node2D

# Called when the node enters the scene tree for the first time.
func _ready():
	var winsize = get_window().size;
	get_window().borderless = false
	get_window().size = winsize
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

