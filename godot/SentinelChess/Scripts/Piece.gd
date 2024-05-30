extends Area2D

@export var step : int
var sprite : Sprite2D
var board : Sprite2D

@onready var skin : Node = get_node('/root/MainGame/Skin')

var piececolor : SentinelChess.ChessColor = SentinelChess.ChessColor.cNone
var piecetype : SentinelChess.ChessPiece = SentinelChess.ChessPiece.pNone
var y_ind : int = -1
var x_ind : int = -1

enum DragStatus { NONE, DRAGGING, CLICKED, RELEASED }
var dragstatus : DragStatus = DragStatus.NONE
var dragstart : Vector2

func BezierBlend(t):
	return t * t * (3.0 - 2.0 * t)

var animstart : Vector2
var animend : Vector2
var animtime : float = 0
var animdur : float = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	pass
	
# Called on mouse input
func _input_event(viewport: Object, event: InputEvent, shape_idx: int):
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if event.pressed:
			if board == null:
				return
			# can/should we drag?  Is it our color and are there possible moves?
			if (board.drag_start(y_ind,x_ind)):
				dragstart = position
				dragstatus = DragStatus.DRAGGING
				self.z_index = 1

func _input(event: InputEvent):
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if dragstatus == DragStatus.DRAGGING and not event.pressed:
			var dest : ChessCoord = board.board_c(center())
			var src : ChessCoord = ChessCoord.new()
			src.y = y_ind
			src.x = x_ind
			if not board.drop_move(src,dest):
				position = dragstart
				if not src.matches(dest):
					board.handle_error_msg("Invalid Move.")
			else:
				position = board.screen_v(dest)
				y_ind = dest.y
				x_ind = dest.x
				board.move_piece(src,dest)
			self.z_index = 0
			modulate.a = 1
			dragstatus = DragStatus.NONE
					
func _physics_process(delta):
	if dragstatus == DragStatus.DRAGGING:
		global_position = get_global_mouse_position()
		if step>0:
			position.x = int(position.x) / step * step
			position.y = int(position.y) / step * step
		var dest : ChessCoord = board.board_c(center())
		var src : ChessCoord = coord(y_ind, x_ind)
		if board.can_drop(src, dest):
			modulate.a = 1
		else:
			modulate.a = 0.5
	if animdur>0:
		animtime += delta
		var frac = BezierBlend(animtime/animdur)
		var smoothvec = animstart + (animend - animstart) * frac
		if (step):
			smoothvec.x = int(smoothvec.x) / step * step
			smoothvec.y = int(smoothvec.y) / step * step
		position = smoothvec
		if animtime>animdur:
			position = animend
			animdur = 0
			var dest : ChessCoord = board.board_c(center())
			var src : ChessCoord = coord(y_ind, x_ind)
			y_ind = dest.y
			x_ind = dest.x
			board._on_animated(src, dest)
			self.z_index = 0

func applyskin():
	var SpriteName : String = 'White'
	if (piececolor == SentinelChess.ChessColor.Black):
		SpriteName = 'Black'
	match(piecetype):
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
	sprite.texture = skin.sprite(SpriteName + '.png')
				
func refresh(pc,pt,y,r):
	rotation_degrees = r
	position.y = y
	if pc != piececolor or pt != piecetype:
		piececolor = pc
		piecetype = pt
	applyskin()
		
func center() -> Vector2:
	var v : Vector2
	v = position
	v.x += sprite.texture.get_width()/2
	v.y -= sprite.texture.get_height()/2
	return v

func coord(y, x) -> ChessCoord:
	var c : ChessCoord = ChessCoord.new()
	c.y = y
	c.x = x
	return c
	
func initialize(pc, pt, yi, xi, y, x, r):
	y_ind = yi
	x_ind = xi
	board = get_parent()
	sprite = get_node('Sprite')
	position.x = x
	refresh(pc, pt, y, r)

func animate_move(p1 : ChessCoord):
	animstart = position
	animend = board.screen_v(p1)
	animdur = 0.25
	animtime = 0
	self.z_index = 1
	

