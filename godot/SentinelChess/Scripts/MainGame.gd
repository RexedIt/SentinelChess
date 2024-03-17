extends Node2D

@onready var skin : Node = get_node('/root/MainGame/Skin')
@onready var Background : Sprite2D = get_node('Background')

# Called when the node enters the scene tree for the first time.
func _ready():
	get_window().borderless = false
	Background.texture = skin.sprite('Background.jpg')	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

