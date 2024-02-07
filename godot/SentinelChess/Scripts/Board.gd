@uid("uid://feu87e4pw2w") # Generated automatically, do not modify.
extends Sprite2D

@onready var game : SentinelChess = get_parent().get_node('SentinelChess')
@onready var PieceProto : Area2D = get_node('Piece')

var piece_arr = []
var cell_arr = []

@export var board_x0 : int = -560 # 156
@export var board_y0 : int = 386 # 126
@export var board_dx : int = 160
@export var board_dy : int = 110

# Called when the node enters the scene tree for the first time.
func _ready():
	piece_arr.resize(64)
	


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func setup(_color):
	if (_color == SentinelChess.ChessColor.White):
		rotation_degrees = 0
	else:
		rotation_degrees = 180
	
func refreshpieces():
	for y in 8:
		for x in 8:
			var pc : SentinelChess.ChessColor = game.cell_color(y,x)
			var po = piece_arr[y*8+x]
			if (pc == SentinelChess.ChessColor.cNone):
				if (po!=null):
					remove_child(po)
					piece_arr[y*8+x]=null
			else:
				var pt : SentinelChess.ChessPiece = game.cell_piece(y,x)
				if (po!=null):
					po.refresh(pc,pt)
				else:
					po = PieceProto.duplicate()
					add_child(po)
					po.initialize(pc,pt,y,x,board_y(y),board_x(x),rotation_degrees)
					
func board_y(y):
	return board_y0 - y * board_dy

func board_x(x):
	return board_x0 + x * board_dx
					
# called by piece
func can_drag(y, x) -> bool:
	if game.gamestate != game.GameState.USERMOVE:
		return false
	return game.cell_color(y,x) == game.user_color()

func board_coord(Vector2 piecepos) -> ChessCoord:
	print('todo')
	
func drop_move(c1, c2) -> bool:
	print('todo')	
	
