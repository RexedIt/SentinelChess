@uid("uid://b2fwbknmi4rb2") # Generated automatically, do not modify.
extends Window

@export var cancelled : bool = false
@export var white : SentinelChess.ChessPlayer;
@export var black : SentinelChess.ChessPlayer;

@onready var btnCancel : Button = get_node("btnCancel")
@onready var btnOK: Button = get_node("btnOK")

# Called when the node enters the scene tree for the first time.
func _ready():
	btnCancel.pressed.connect(_OnCancel)
	btnOK.pressed.connect(_OnOK)	
	visibility_changed.connect(_VisibilityChanged)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

# outbound signal
signal on_closed(_cancelled, _white, _black)

func _VisibilityChanged():
	if (visible):
		initializePlayer('White','',600,'Human')
		initializePlayer('Black','Computer',600,'Computer')
		cancelled = false
	else:
		readPlayer(white,'White')
		readPlayer(black,'Black')
		on_closed.emit(cancelled, white, black)
	
func _OnCancel():
	cancelled = true
	visible = false

func _OnOK():
	cancelled = false
	visible = false

func initializePlayer(c, n, s, t):
	var base = 'MC/VC/' + c + 'Player/'
	var txtName : TextEdit = get_node(base + 'txtName')
	var	lvlSkill : HSlider = get_node(base + 'lvlSkill')
	var	optHuman : CheckBox = get_node(base + 'optHuman')
	var optComputer : CheckBox = get_node(base + 'optComputer')
	txtName.text = n
	lvlSkill.value = s
	if t == 'Human':
		optHuman.button_pressed = true
	else:
		optComputer.button_pressed = true
		
func readPlayer(p, c):
	var base = 'MC/VC/' + c + 'Player/'
	var txtName : TextEdit = get_node(base + 'txtName')
	var	lvlSkill : HSlider = get_node(base + 'lvlSkill')
	var	optHuman : CheckBox = get_node(base + 'optHuman')
	var optComputer : CheckBox = get_node(base + 'optComputer')
	p.Name = txtName.text
	p.Skill = lvlSkill.value
	if optHuman.button_pressed:
		p.PlayerType = ChessPlayer.ChessPlayerType.Human
	else:
		p.PlayerType = ChessPlayer.ChessPlayerType.Computer
