@uid("uid://dpqyd1rq8kg5c") # Generated automatically, do not modify.
extends Node

@onready var ChessObj : SentinelChess = get_parent().get_node("SentinelChess")

# Called when the node enters the scene tree for the first time.
func _ready():
	ChessObj.draw_board.connect(draw_board)
	ChessObj.trace.connect(trace)
	print("ready")
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func draw_board(node, n):
	print("GOT IT!")
	
func trace(node, msg):
	print("TRACE!")
