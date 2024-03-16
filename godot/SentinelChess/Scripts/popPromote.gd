extends Panel

@onready var game_manager : SentinelChess = get_parent().get_node('SentinelChess')
@onready var Queen : Button = get_node('Queen')
@onready var Rook : Button = get_node('Rook')
@onready var Bishop : Button = get_node('Bishop')
@onready var Knight : Button = get_node('Knight')


# Called when the node enters the scene tree for the first time.
func _ready():
	visibility_changed.connect(_VisibilityChanged)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
var _cancelled : bool
var _color : SentinelChess.ChessColor
var _piece : SentinelChess.ChessPiece

# outbound signal
signal on_closed(_cancelled, _white, _black, _clock)

func _VisibilityChanged():
	if visible:
		var base = 'res://Sprites/RetroWood/'
		_color = game_manager.turn_color()
		if _color == SentinelChess.White:
			base += 'White'
		else:
			base += 'Black'
		Queen.icon = load(base+'Queen.png')
		Rook.icon = load(base+'Rook.png')
		Bishop.icon = load(base+'Bishop.png')
		Knight.icon = load(base + 'Knight.png')
	else:
		on_closed.emit(_cancelled,_color,_piece)
		pass

func _on_queen_pressed():
	_piece = SentinelChess.Queen
	visible = false

func _on_rook_pressed():
	_piece = SentinelChess.Rook
	visible = false

func _on_bishop_pressed():
	_piece = SentinelChess.Bishop
	visible = false

func _on_knight_pressed():
	_piece = SentinelChess.Knight
	visible = false

func _on_cancel_pressed():
	_cancelled = true
	visible = false
