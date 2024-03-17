extends Node2D

@export var duration : float
@export var loadscene : String

@onready var skin : Node = get_node('Skin')
@onready var BannerBack : Sprite2D = get_node('BannerBack')
@onready var BannerLeft : Sprite2D = get_node('BannerLeft')
@onready var BannerRight : Sprite2D = get_node('BannerRight')
@onready var BannerText : Sprite2D = get_node('BannerText')

var elapsed : float

# Called when the node enters the scene tree for the first time.
func _ready():
	BannerBack.texture = skin.sprite('BannerBack.jpg')
	BannerRight.texture = skin.sprite('BannerRight.png')
	BannerRight.step = skin.step()
	BannerLeft.texture = skin.sprite('BannerLeft.png')
	BannerLeft.step = skin.step()
	BannerText.texture = skin.sprite('BannerText.png')
	BannerText.step = skin.step()
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
		
		
