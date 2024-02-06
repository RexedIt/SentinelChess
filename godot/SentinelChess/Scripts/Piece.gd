@uid("uid://jmkj3gw012fe") # Generated automatically, do not modify.
extends Sprite2D

var piececolor : SentinelChess.ChessColor = SentinelChess.ChessColor.cNone
var piecetype : SentinelChess.ChessPiece = SentinelChess.ChessPiece.pNone

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func refresh(pc,pt):
	print('refresh')

	
func initialize(pc, pt, y, x, r):
	piececolor = pc
	piecetype = pt
	var SpriteName : String = 'White'
	if (pc == SentinelChess.ChessColor.Black):
		SpriteName = 'Black'
	match(pt):
		SentinelChess.ChessPiece.Pawn:
			SpriteName += 'Pawn'
		SentinelChess.ChessPiece.Rook:
			SpriteName += 'Rook'
		SentinelChess.ChessPiece.Bishop:
			SpriteName += 'Bishop'
		SentinelChess.ChessPiece.Knight:
			SpriteName += 'Knight'
		SentinelChess.ChessPiece.Queen:
			SpriteName += 'Queen'
		SentinelChess.ChessPiece.King:
			SpriteName += 'King'
	texture = load('res://Sprites/RetroWood/' + SpriteName + '.png')
	position.x = x
	position.y = y
	rotation_degrees = r
	
