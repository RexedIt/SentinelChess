extends Window

@export var cancelled : bool = false
var _title : String
var _white : ChessPlayer
var _black : ChessPlayer
var _clock : ChessClock

@onready var skin : Node = get_node('/root/MainGame/Skin')
@onready var txtTitle : TextEdit = get_node("MC/VC/HC/txtTitle")
@onready var WhitePlayer : HBoxContainer = get_node('MC/VC/WhitePlayer')
@onready var BlackPlayer : HBoxContainer = get_node('MC/VC/BlackPlayer')
@onready var btnCancel : Button = get_node("btnCancel")
@onready var btnOK : Button = get_node("btnOK")
@onready var optNone : CheckBox = get_node("MC/VC/Clock/HC/VC/optNone")
@onready var optSuddenDeath : CheckBox = get_node("MC/VC/Clock/HC/VC/optSuddenDeath")
@onready var optIncrement : CheckBox = get_node("MC/VC/Clock/HC/VC/optIncrement")
@onready var optBronstein : CheckBox = get_node("MC/VC/Clock/HC/VC/optBronstein")
@onready var optSimple : CheckBox = get_node("MC/VC/Clock/HC/VC/optSimple")
@onready var chkSame : CheckBox = get_node("MC/VC/Clock/HC/TVC/chkSame")
@onready var AllowWhite : HBoxContainer = get_node("MC/VC/Clock/HC/TVC/AllowWhite")
@onready var AllowBlack : HBoxContainer = get_node("MC/VC/Clock/HC/TVC/AllowBlack")
@onready var AddWhite : HBoxContainer = get_node("MC/VC/Clock/HC/TVC/AddWhite")
@onready var AddBlack : HBoxContainer = get_node("MC/VC/Clock/HC/TVC/AddBlack")
@onready var TVC : VBoxContainer = get_node("MC/VC/Clock/HC/TVC")

var loaded : bool = false
var skinned : bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	btnCancel.pressed.connect(_OnCancel)
	btnOK.pressed.connect(_OnOK)
	chkSame.toggled.connect(_OnSame)
	optNone.toggled.connect(_OnType)
	optSuddenDeath.toggled.connect(_OnType1)
	optIncrement.toggled.connect(_OnType2)
	optBronstein.toggled.connect(_OnType3)
	optSimple.toggled.connect(_OnType4)
			
	visibility_changed.connect(_VisibilityChanged)
	_white = ChessPlayer.new()
	_black = ChessPlayer.new()
	_clock = ChessClock.new()
	loaded = true
	if not skinned:
		applyskin()

func applyskin():
	if loaded:
		set_theme(skin.theme)
		WhitePlayer.applyskin()
		BlackPlayer.applyskin()
		skinned = true

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

# outbound signal
signal on_closed(_cancelled, _text, _white, _black, _clock)

func _VisibilityChanged():
	if (visible):
		txtTitle.text = "New Game"
		WhitePlayer.initialize(SentinelChess.ChessColor.White,'Human',600,ChessPlayer.ChessPlayerType.Human)
		BlackPlayer.initialize(SentinelChess.ChessColor.Black,'Computer',600,ChessPlayer.ChessPlayerType.Computer)
		initializeClock(ChessClock.ccNone, 60, 10)
		cancelled = false
	else:
		_title = txtTitle.text
		_white = WhitePlayer.read()
		_black = BlackPlayer.read()
		readClock(_clock)
		on_closed.emit(cancelled, _title, _white, _black, _clock)
	
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

func _OnType1(ch):
	_OnType(ch)
func _OnType2(ch):
	_OnType(ch)
func _OnType3(ch):
	_OnType(ch)
func _OnType4(ch):
	_OnType(ch)
	
func _OnType(ch):
	TVC.visible = !optNone.button_pressed
	AllowWhite.visible = !optNone.button_pressed
	AllowBlack.visible = AllowWhite.visible and chkSame.button_pressed
	AddWhite.visible = optIncrement.button_pressed or optBronstein.button_pressed or optSimple.button_pressed
	AddBlack.visible = AddWhite.visible and chkSame.button_pressed
	
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
