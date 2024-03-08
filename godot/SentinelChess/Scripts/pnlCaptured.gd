extends Panel

@onready var PieceProto : Sprite2D = get_node('Piece')

const piece_arr_size = 27
const piece_delta = 36
var piece_arr = []

# Called when the node enters the scene tree for the first time.
func _ready():
	piece_arr.resize(piece_arr_size)
	PieceProto.visible = false
	for y in piece_arr_size:
		var po = PieceProto.duplicate()
		po.position.y = piece_delta*y
		add_child(po)
		piece_arr[y]=po

func refreshpieces(turn_color : SentinelChess.ChessColor, board : ChessBoard):
		if turn_color == SentinelChess.White:
			refreshCaptured(
				board.captured_pieces_abbr(SentinelChess.White),
				board.captured_pieces_abbr(SentinelChess.Black))
		else:
			refreshCaptured(
				board.captured_pieces_abbr(SentinelChess.Black),
				board.captured_pieces_abbr(SentinelChess.White))
		
func refreshCaptured(toppieces: String, bottompieces: String):
	var y : int = 0;
	for p in toppieces:
		piece_arr[y].setpiece(p)
		y = y + 1
	var n : int = piece_arr_size - bottompieces.length() - 1
	while y <= n:
		piece_arr[y].setpiece('')
		y = y + 1
	y = piece_arr_size - 1
	for p in bottompieces:
		piece_arr[y].setpiece(p)
		y = y - 1
		
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
