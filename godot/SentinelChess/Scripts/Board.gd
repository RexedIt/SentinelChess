@uid("uid://b5nfmf1511hyn") # Generated automatically, do not modify.
extends Sprite2D

var piece_dict = {}

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func setup(_color):
	if (_color == SentinelChess.ChessColor.White):
		rotation_degrees = 0
	else:
		rotation_degrees = 180
	
func refreshpieces():
	
