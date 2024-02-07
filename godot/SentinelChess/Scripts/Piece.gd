@uid("uid://jmkj3gw012fe") # Generated automatically, do not modify.
extends Area2D

var sprite : Sprite2D
var board : Sprite2D

var piececolor : SentinelChess.ChessColor = SentinelChess.ChessColor.cNone
var piecetype : SentinelChess.ChessPiece = SentinelChess.ChessPiece.pNone
var y_ind : int = -1
var x_ind : int = -1

enum DragStatus { NONE, DRAGGING, CLICKED, RELEASED }
var dragstatus : DragStatus = DragStatus.NONE
var dragstart : Vector2

# Called when the node enters the scene tree for the first time.
func _ready():
	pass
	
# Called on mouse input
func _input_event(viewport: Object, event: InputEvent, shape_idx: int):
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if event.pressed:
			# can/should we drag?  Is it our color and are there possible moves?
			if (board.can_drag(y_ind,x_ind)):
				dragstart = position
				dragstatus = DragStatus.DRAGGING

func _input(event: InputEvent):
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if dragstatus == DragStatus.DRAGGING and not event.pressed:
			var dest : ChessCoord = board.coord(position)
			var src : ChessCoord = ChessCoord.new()
			src.y = y_ind
			src.x = x_ind
			if not board.drop_move(src,dest):
				position = dragstart
			dragstatus = DragStatus.NONE
					
func _physics_process(delta):
	if dragstatus == DragStatus.DRAGGING:
		global_position = get_global_mouse_position()
			
func refresh(pc,pt):
	print('refresh')

	
func initialize(pc, pt, yi, xi, y, x, r):
	piececolor = pc
	piecetype = pt
	y_ind = yi
	x_ind = xi
	board = get_parent()
	sprite = get_node('Sprite')
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
	sprite.texture = load('res://Sprites/RetroWood/' + SpriteName + '.png')
	position.x = x
	position.y = y
	rotation_degrees = r
	
