extends Window

@export var cancelled : bool = false
var _title : String
var _me : ChessPlayer
var _keywords : String
var _rating : int

@onready var Player : HBoxContainer = get_node('MC/HC/Player')
@onready var btnCancel : Button = get_node("btnCancel")
@onready var btnOK : Button = get_node("btnOK")
@onready var lvlRating : HSlider = get_node('MC/HC/GC/lvlRating')
@onready var txtKeywords : TextEdit = get_node('MC/HC/GC/txtKeywords')

# Called when the node enters the scene tree for the first time.
func _ready():
	btnCancel.pressed.connect(_OnCancel)
	btnOK.pressed.connect(_OnOK)
	visibility_changed.connect(_VisibilityChanged)
	_me = ChessPlayer.new()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

# outbound signal
signal on_closed(_cancelled, _me, _keywords, _rating)

func _VisibilityChanged():
	if (visible):
		Player.initialize(SentinelChess.ChessColor.cNone,'Human',600,ChessPlayer.ChessPlayerType.Human)
		lvlRating.value = 600
		txtKeywords.text = ''
		cancelled = false
	else:
		_me = Player.read()
		_keywords = txtKeywords.text
		_rating = lvlRating.value
		on_closed.emit(cancelled, _me, _keywords, _rating)
		
func _OnCancel():
	cancelled = true
	visible = false

func _OnOK():
	cancelled = false
	visible = false


