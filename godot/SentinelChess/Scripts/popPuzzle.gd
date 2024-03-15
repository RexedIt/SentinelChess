extends Window

@export var cancelled : bool = false
var _title : String
var _me : ChessPlayer
var _rating : int

@onready var btnCancel : Button = get_node("btnCancel")
@onready var btnOK : Button = get_node("btnOK")
@onready var txtName : TextEdit = get_node('MC/Player/txtName')
@onready var lvlSkill : HSlider = get_node('MC/Player/lvlSkill')
@onready var lvlRating : HSlider = get_node('MC/Player/lvlRating')

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
signal on_closed(_cancelled, _text, _white, _black, _clock)

func _VisibilityChanged():
	if (visible):
		txtName.text = 'Me'
		lvlSkill.value = 600
		lvlRating.value = 600
		cancelled = false
	else:
		_me.Name = txtName.text
		_me.Skill = lvlSkill.value
		_rating = lvlRating.value
		on_closed.emit(cancelled, _me, _rating)
		
func _OnCancel():
	cancelled = true
	visible = false

func _OnOK():
	cancelled = false
	visible = false


