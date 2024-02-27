extends Window

@export var cancelled : bool = false
var _white : ChessPlayer
var _black : ChessPlayer
var _clock : ChessClock

@onready var btnCancel : Button = get_node("btnCancel")
@onready var btnOK : Button = get_node("btnOK")
@onready var optNone : CheckBox = get_node("MC/VC/Clock/HC/VC/optNone")
@onready var optSuddenDeath : CheckBox = get_node("MC/VC/Clock/HC/VC/optNone")
@onready var optIncrement : CheckBox = get_node("MC/VC/Clock/HC/VC/optIncrement")
@onready var optBronstein : CheckBox = get_node("MC/VC/Clock/HC/VC/optBronstein")
@onready var optSimple : CheckBox = get_node("MC/VC/Clock/HC/VC/optSimple")
@onready var chkSame : CheckBox = get_node("MC/VC/Clock/HC/TVC/chkSame")
@onready var AllowWhite : HBoxContainer = get_node("MC/VC/Clock/HC/TVC/AllowWhite")
@onready var AllowBlack : HBoxContainer = get_node("MC/VC/Clock/HC/TVC/AllowBlack")
@onready var AddWhite : HBoxContainer = get_node("MC/VC/Clock/HC/TVC/AddWhite")
@onready var AddBlack : HBoxContainer = get_node("MC/VC/Clock/HC/TVC/AddBlack")
@onready var TVC : VBoxContainer = get_node("MC/VC/Clock/HC/TVC")

# Called when the node enters the scene tree for the first time.
func _ready():
	btnCancel.pressed.connect(_OnCancel)
	btnOK.pressed.connect(_OnOK)
	chkSame.toggled.connect(_OnSame)
	optNone.toggled.connect(_OnType)
	optSuddenDeath.toggled.connect(_OnType)
	optIncrement.toggled.connect(_OnType)
	optBronstein.toggled.connect(_OnType)
	optSimple.toggled.connect(_OnType)
			
	visibility_changed.connect(_VisibilityChanged)
	_white = ChessPlayer.new()
	_black = ChessPlayer.new()
	_clock = ChessClock.new()
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

# outbound signal
signal on_closed(_cancelled, _white, _black, _clock)

func _VisibilityChanged():
	if (visible):
		initializePlayer('White','',600,'Human')
		initializePlayer('Black','Computer',600,'Computer')
		initializeClock(ChessClock.ccNone, 60, 10)
		cancelled = false
	else:
		readPlayer(_white,'White')
		readPlayer(_black,'Black')
		readClock(_clock)
		on_closed.emit(cancelled, _white, _black, _clock)
	
func _OnCancel():
	cancelled = true
	visible = false

func _OnOK():
	cancelled = false
	visible = false

func _OnSame(ch):
	if ch == false:
		AllowBlack.visible = false
		AddBlack.visible = false
	else:
		AllowBlack.visible = true
		AddBlack.visible = optIncrement.button_pressed or optBronstein.button_pressed or optSimple.button_pressed	

func _OnType(ch):
	TVC.visible = !optNone.button_pressed
	AllowWhite.visible = !optNone.button_pressed
	AllowBlack.visible = AllowWhite.visible and chkSame.button_pressed
	AddWhite.visible = optIncrement.button_pressed or optBronstein.button_pressed or optSimple.button_pressed
	AddBlack.visible = AddWhite.visible and chkSame.button_pressed
	
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

func initializeClock(cc, al, ad):
	chkSame.button_pressed = true
	match cc:
		ChessClock.ccNone:
			optNone.button_pressed = true
		ChessClock.ccSuddenDeath:
			optSuddenDeath.button_pressed = true
		ChessClock.ccIncrement:
			optIncrement.button_pressed = true
		ChessClock.ccBronsteinDelay:
			optBronstein.button_pressed = true
		ChessClock.ccSimpleDelay:
			optSimple.button_pressed = true
	AllowWhite.set_slider(al)
	AllowBlack.set_slider(al)
	AddWhite.set_slider(ad)
	AddBlack.set_slider(ad)
	
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

func readClock(c):
	if optNone.button_pressed:
		_clock.ClockType = ChessClock.ccNone
	if optSuddenDeath.button_pressed:
		_clock.ClockType = ChessClock.ccSuddenDeath
	if optIncrement.button_pressed:
		_clock.ClockType = ChessClock.ccIncrement
	if optBronstein.button_pressed:
		_clock.ClockType = ChessClock.ccBronsteinDelay
	if optSimple.button_pressed:
		_clock.ClockType = ChessClock.ccSimpleDelay
	_clock.set_allowed_white(AllowWhite.milliseconds)
	_clock.set_allowed_black(AllowBlack.milliseconds)
	_clock.AddWhite = AddWhite.milliseconds
	_clock.AddBlack = AddBlack.milliseconds
	if chkSame.button_pressed:
		_clock.AllowedBlack = _clock.AllowedWhite
		_clock.AddBlack = _clock.AddWhite
