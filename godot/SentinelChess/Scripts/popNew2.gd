extends Window

@export var cancelled : bool = false
var _white : ChessPlayer
var _black : ChessPlayer

@onready var btnCancel : Button = get_node("btnCancel")
@onready var btnOK: Button = get_node("btnOK")

# Called when the node enters the scene tree for the first time.
func _ready():
	btnCancel.pressed.connect(_OnCancel)
	btnOK.pressed.connect(_OnOK)	
	visibility_changed.connect(_VisibilityChanged)
	_white = ChessPlayer.new()
	_black = ChessPlayer.new()
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
		readPlayer(_white,'White')
		readPlayer(_black,'Black')
		on_closed.emit(cancelled, _white, _black)
	
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
	var	optHuman : CheckBox = get_node(base + 'HC/optHuman')
	var optComputer : CheckBox = get_node(base + 'HC/optComputer')
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
	var	optHuman : CheckBox = get_node(base + 'HC/optHuman')
	var optComputer : CheckBox = get_node(base + 'HC/optComputer')
	p.Name = txtName.text
	p.Skill = lvlSkill.value
	if optHuman.button_pressed:
		p.PlayerType = ChessPlayer.ChessPlayerType.Human
	else:
		p.PlayerType = ChessPlayer.ChessPlayerType.Computer
