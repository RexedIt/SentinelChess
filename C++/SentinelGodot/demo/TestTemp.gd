extends SentinelChess


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	new_game(SentinelChess.ColorWhite, 3)
	var c = ChessCoord.new()
	c.x = 0
	c.y = 0

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_trace(msg):
	pass # Replace with function body.
	print('TRACE: ' + msg);
