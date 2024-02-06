@uid("uid://kldbmn05hgxk") # Generated automatically, do not modify.
extends Window

@export var cancelled : bool = false
@export var level : int = 3
@export var color : SentinelChess.ChessColor = SentinelChess.ChessColor.White

@onready var btnCancel : Button = get_node("btnCancel")
@onready var btnOK: Button = get_node("btnOK")
@onready var sldLevel: HSlider = get_node("MC/GC/Level")
@onready var btnWhite: CheckBox = get_node("MC/GC/HC/White")
@onready var btnBlack: CheckBox = get_node("MC/GC/HC/Black")

# Called when the node enters the scene tree for the first time.
func _ready():
	btnCancel.pressed.connect(_OnCancel)
	btnOK.pressed.connect(_OnOK)	
	visibility_changed.connect(_VisibilityChanged)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

# outbound signal
signal on_closed(_cancelled, _level, _color)

func _VisibilityChanged():
	if (visible):
		HSlider.value = 3
		btnWhite.button_pressed = true
		cancelled = false
	else:
		level = sldLevel.value
		if (btnWhite.button_pressed):
			color = SentinelChess.ChessColor.White
		else:
			color = SentinelChess.ChessColor.Black
		on_closed.emit(cancelled, level, color)
	
func _OnCancel():
	cancelled = true
	visible = false

func _OnOK():
	cancelled = false
	visible = false
