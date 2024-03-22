extends Window

@export var cancelled : bool = false
var _skin : String
var _voice : int
var _music : int
var _sfx : int

@onready var skin : Node = get_node('/root/MainGame/Skin')
@onready var btnOK : Button = get_node("btnOK")
@onready var btnCancel : Button = get_node("btnCancel")
@onready var optRetro : CheckBox = get_node("MC/VC/HC/optRetro")
@onready var optPhoto : CheckBox = get_node("MC/VC/HC/optPhoto")
@onready var optChessCom : CheckBox = get_node("MC/VC/HC/optChessCom")
@onready var sldVoice : HSlider = get_node("MC/VC/sldVoice")
@onready var sldMusic : HSlider = get_node("MC/VC/sldMusic")
@onready var sldSfx : HSlider = get_node("MC/VC/sldSfx")

# outbound signal
signal on_closed(_cancelled, _skin, _voice, _music, _sfx)

# Called when the node enters the scene tree for the first time.
func _ready():
	btnCancel.pressed.connect(_OnCancel)
	btnOK.pressed.connect(_OnOK)
	visibility_changed.connect(_VisibilityChanged)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _VisibilityChanged():
	if (visible):
		initialize(skin.skinname, skin.voicelvl, skin.musiclvl, skin.sfxlvl)
		cancelled = false
	else:
		if cancelled == false:
			readsettings()
		on_closed.emit(cancelled, _skin, _voice, _music, _sfx)

func readsettings():
	if optRetro.button_pressed:
		_skin = "RetroWood"
	if optPhoto.button_pressed:
		_skin = "PhotoBoard"
	#if optChessCom.pressed:
	#	_skin = "ChessCom"
	_voice = sldVoice.value
	_music = sldMusic.value
	_sfx = sldSfx.value
	
func _OnCancel():
	cancelled = true
	visible = false

func _OnOK():
	cancelled = false
	visible = false

func initialize(s, v, m, f):
	_skin = s
	_voice = v
	_music = m
	if s=="RetroWood":
		optRetro.button_pressed = true
	if s == "PhotoBoard":
		optPhoto.button_pressed = true
	if s == "ChessCom":
		optChessCom.button_pressed = true
	sldVoice.value = v
	sldMusic.value = m
	sldSfx.value = f
	
