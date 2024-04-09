extends Node

var config = ConfigFile.new()
var skinname : String = 'ChessCom'
var voicelvl : int = 50
var musiclvl : int = 50
var rotateBoard : bool = false
var sfxlvl : int = 50
var themepath : String = 'res://Skins/ChessCom/theme.tres'
var theme : Theme

# Called when the node enters the scene tree for the first time.
func _init():
	config.set_value('Skin', 'name', 'ChessCom')
	config.set_value('Audio', 'voice', 50)	
	config.set_value('Audio', 'music', 50)
	config.set_value('Audio', 'sfx', 50)
	config.set_value('Board', 'rotate', false)
	config.load('settings.cfg')
	skinname = config.get_value('Skin', 'name')
	voicelvl = config.get_value('Audio', 'voice')
	musiclvl = config.get_value('Audio', 'music')
	sfxlvl = config.get_value('Audio', 'sfx')
	rotateBoard = config.get_value('Board', 'rotate')
	themepath = 'res://Skins/' + skinname + '/theme.tres'
	theme = load(themepath)
	ProjectSettings.set_setting('theme/custom', themepath)
	
func _notification(what: int) -> void:
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		config.set_value('Skin', 'name', skinname)
		config.set_value('Audio', 'voice', voicelvl)	
		config.set_value('Audio', 'music', musiclvl)
		config.set_value('Audio', 'sfx', sfxlvl)
		config.set_value('Board', 'rotate', rotateBoard)
		config.save('settings.cfg')      

func applysettings(_skinname, _voicelvl, _musiclvl, _sfxlvl, _rotateBoard):
	skinname = _skinname
	voicelvl = _voicelvl
	musiclvl = _musiclvl
	sfxlvl = _sfxlvl
	rotateBoard = _rotateBoard
	themepath = 'res://Skins/' + skinname + '/theme.tres'
	theme = load(themepath)
	ProjectSettings.set_setting('theme/custom', themepath)
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

# when initialized will look in user settings
# to obtain this value
# also will return themes

func path() -> String:
	return "res://Skins/" + skinname + "/"
	
func commonpath() -> String:
	return "res://Skins/Common/"
	
func step() -> int:
	if skinname == 'RetroWood':
		return 16
	return 1
	
func load(filename):
	return load(path() + filename)
	
func sprite(filename):
	return load(path() + 'Sprites/' + filename)

func music(filename):
	return load(path() + 'Music/' + filename)
	
func sound(filename):
	return load(path() + 'SFX/' + filename)
	
func voice(filename):
	return load(path() + 'Voice/' + filename)
	
func commonsound(filename):
	return load(commonpath() + 'SFX/' + filename)
	
	


