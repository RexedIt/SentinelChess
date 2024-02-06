@uid("uid://c2cp8yin4xi3e") # Generated automatically, do not modify.
extends SentinelChess


# Called when the node enters the scene tree for the first time.
func _ready():
	trace.connect(_on_trace)
	draw_board.connect(_on_draw_board)
	#new_game(SentinelChess.White, 3)
	var c = ChessCoord.new()
	c.x = 0
	c.y = 0

# Called every frame. 'delta' is the elapsed time since the previous frame.
var first_time : bool = true
func _process(delta):
	if (first_time):
		first_time = false
		new_game(SentinelChess.White, 3)
	pass


func _on_trace(node, msg):
	print('TRACE: ' + msg);


func _on_draw_board(node, n):
	print('DRAW');
