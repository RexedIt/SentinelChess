extends Sprite2D

@onready var game : SentinelChess = get_parent().get_node('SentinelChess')
@onready var PieceProto : Area2D = get_node('Piece')

var piece_arr = []
var cell_arr = []
var last_drag_y : int
var last_drag_x : int
var possible_moves : Array

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
	
func refreshpieces(b : ChessBoard):
	for y in 8:
		for x in 8:
			var pc : SentinelChess.ChessColor = b.color_(y,x)
			var po = piece_arr[y*8+x]
			if (pc == SentinelChess.ChessColor.cNone):
				if (po!=null):
					remove_child(po)
					po.queue_free()
					po=null
			else:
				var pt : SentinelChess.ChessPiece = b.piece(y,x)
				if (po!=null):
					po.refresh(pc,pt,rotation_degrees)
				else:
					po = PieceProto.duplicate()
					add_child(po)
					po.initialize(pc,pt,y,x,screen_y(y),screen_x(x),rotation_degrees)
			piece_arr[y*8+x]=po

func screen_y(by : int) -> int:
	var sy = board_y0 - by * board_dy
	if sy < board_y0 - 8 * board_dy or sy > board_y0:
		sy = board_y0
	return sy

func screen_x(bx : int) -> int:
	var sx = board_x0 + bx * board_dx
	if sx < board_x0 or sx > board_x0 + 8 * board_dx:
		sx = board_x0
	return sx

func screen_v(b: ChessCoord) -> Vector2:
	var v : Vector2
	if b == null:
		v.y = board_y0
		v.x = board_x0
		return v
	v.y = screen_y(b.y)
	v.x = screen_x(b.x)
	return v
	
func board_y(sy : int) -> int:
	var by = -1 * (sy - board_y0) / board_dy
	if by < 0 or by > 7:
		by = -1
	return by
	
func board_x(sx : int) -> int:
	var bx = (sx - board_x0) / board_dx
	if bx < 0 or bx > 7:
		bx = -1
	return bx

func board_c(s: Vector2) -> ChessCoord:
	var c = ChessCoord.new()
	c.y = board_y(s.y)
	c.x = board_x(s.x)
	return c
	
# called by piece
func drag_start(y : int, x : int) -> bool:
	if game.gamestate != game.GameState.USERMOVE:
		return false
	if game.cell_interactive(y,x):
		last_drag_y = y
		last_drag_x = x
		possible_moves = game.possible_moves(game.turn_color())
		return true
	return false
	
func can_drop(c1 : ChessCoord, c2 : ChessCoord) -> bool:
	# if same square, let it be.
	if c1.matches(c2):
		return true
	for possible_move in possible_moves:
		if possible_move.matches_p0p1(c1,c2):
			#print('match')
			return true
	#print('no match')
	return false
	
func drop_move(p0 : ChessCoord, p1 : ChessCoord) -> bool:
	if p0.matches(p1):
		return false
	if not can_drop(p0, p1):
		return false
	var c : SentinelChess.ChessColor = game.turn_color();
	var err : int = game.move_c(c, p0, p1, SentinelChess.ChessPiece.pNone)
	var n : int = game.lastturnno();
	var m: ChessMove = game.lastmove();
	if err != 0:
		handle_error(err)
		return false
	if game.check_state(c):
		if game.state() == SentinelChess.ChessGameState.Play:
			handle_error_msg("You are in Check.")
			return false
		# *** REM *** TODO Extra Checks?
	game._on_user_moved(n,m,c)
	return true

func move_piece(p0 : ChessCoord, p1 : ChessCoord):
	var pc : Area2D = piece_arr[p0.y*8+p0.x]
	if pc == null:
		return
	var pd : Area2D = piece_arr[p1.y*8+p1.x]
	if pd != null:
		pd.queue_free()
	piece_arr[p0.y*8+p0.x] = null
	piece_arr[p1.y*8+p1.x] = pc
	pc.position.y = screen_y(p1.y)
	pc.position.x = screen_x(p1.x)
	
func handle_error(err : int):
	game._on_error(err)

func handle_error_msg(err : String):
	game._on_error_msg(err)
	
func animate_move(m : ChessMove):
	if m != null:
		var p0 : ChessCoord = m.p0
		var p1 : ChessCoord = m.p1
		if p0 != null and p1 != null:
			var po = piece_arr[p0.y*8+p0.x]
			if po != null:
				po.animate_move(p1)

func coordstr(p0 : ChessCoord) -> String:
	var s = "%d.%d"
	return s % [p0.y, p0.x]
	
func _on_animated(p0 : ChessCoord, p1 : ChessCoord):
	move_piece(p0,p1)
	# we do this just in case of moves like promotion or castling, en passant
	refreshpieces(game.get_board())
	# print('_on_animated ' + coordstr(p0) + ' to ' + coordstr(p1))
	game._on_animated()			
		
