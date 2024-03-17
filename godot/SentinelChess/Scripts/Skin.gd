extends Node


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

# when initialized will look in user settings
# to obtain this value
# also will return themes

func path() -> String:
	return "res://Skins/RetroWood/"
	
func step() -> int:
	return 16
	
func load(filename):
	return load(path() + filename)
	
func sprite(filename):
	return load(path() + 'Sprites/' + filename)

func music(filename):
	return load(path() + 'Music/' + filename)
	
