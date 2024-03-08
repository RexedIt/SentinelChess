extends HBoxContainer

@export var milliseconds : int
@export var def : int
@export var mult : float

@onready var slider : HSlider = get_node("HSlider")
@onready var label : Label = get_node("lblTime")

# Called when the node enters the scene tree for the first time.
func _ready():
	slider.value = def
	slider.value_changed.connect(_slid)

func set_slider(v):
	slider.value = v
		
func _slid(v):
	label.text = str(v)
	milliseconds = int(v * mult)

