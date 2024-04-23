extends HBoxContainer

var player : ChessPlayer
var color : SentinelChess.ChessColor
var _initializing : bool = false
var players : Array
var HumanName : String = 'Human2'
var HumanSkill : int = 600

@onready var game : SentinelChess = get_parent().get_node('/root/MainGame/SentinelChess')
@onready var Avatar : TextureRect = get_node("Avatar")
@onready var optName : OptionButton = get_node("GC/optName")
@onready var txtName : TextEdit = get_node("GC/optName/txtName")
@onready var lvlSkill : HSlider = get_node("GC/lvlSkill")
@onready var optHuman : CheckBox = get_node("GC/HC/optHuman")
@onready var optComputer : CheckBox = get_node("GC/HC/optComputer")
@onready var chkSave : Button = get_node("GC/HC/chkSave")

func initialize(c, n, s, t):
	_initializing = true
	player = ChessPlayer.new()
	player.PlayerType = t
	player.PlayerColor = c
	color = c
	player.Name = n
	player.Skill = s
	_initializeSelections()
	lvlSkill.value = player.Skill
	if player.PlayerType == ChessPlayer.ChessPlayerType.Computer:
		optComputer.button_pressed = true
		chkSave.visible = false
	else:
		txtName.text = player.Name
		HumanName = player.Name
		HumanSkill = player.Skill
		optHuman.button_pressed = true
		chkSave.visible = true
		chkSave.button_pressed = player.Persistent
	_initializing = false

func read():
	player.Name = txtName.text
	player.Skill = lvlSkill.value
	if optHuman.button_pressed:
		player.PlayerType = ChessPlayer.ChessPlayerType.Human
		player.Persistent = chkSave.button_pressed
	else:
		player.PlayerType = ChessPlayer.ChessPlayerType.Computer
	return player

func _initializeSelections():
	optName.clear()
	players = game.hub_players(player.PlayerType, true, 0, true)
	var selItem : int = 0
	var index : int = 0
	for pl: ChessPlayer in players:
		if pl.Name.to_upper() == player.Name.to_upper():
			selItem = index
		optName.add_item(pl.Name + ' (' + str(pl.Skill) + ')')
		index = index + 1
	if selItem >= 0 and players.size()>0:
		optName.selected = selItem
		txtName.text = players[selItem].Name
		lvlSkill.value = players[selItem].Skill
		_setAvatar(players[selItem].Avatar)
	else:
		_setAvatar('')
	#var b : String = 'C:\\Projects\\SentinelChess\\Assets\\Characters\\'
	#_convertAvatar(b+'Veronica.png')
	
func save_to_file(path, content):
	var file = FileAccess.open(path, FileAccess.WRITE)
	file.store_string(content)
	file.close()
		
func _convertAvatar(fn: String):
	var i = Image.load_from_file(fn)
	var bytes = i.save_jpg_to_buffer(0.75)
	var str = Marshalls.raw_to_base64(bytes)
	save_to_file(fn + '.txt', str)
	
func _setAvatar(avatar: String):
	if avatar == '':
		Avatar.texture = load("res://Skins/Common/Sprites/Empty.png")
		return
	var bytes = Marshalls.base64_to_raw(avatar)
	var i = Image.new()
	i.load_jpg_from_buffer(bytes)
	var t = ImageTexture.new()
	t.set_image(i)
	Avatar.texture = t

# Called when the node enters the scene tree for the first time.
func _ready():
	optHuman.pressed.connect(_human_pressed)
	optComputer.pressed.connect(_computer_pressed)
	optName.item_selected.connect(_name_selected)
	txtName.text_changed.connect(_name_changed)
	lvlSkill.value_changed.connect(_value_changed)
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _human_pressed():
	if _initializing:
		return
	player.PlayerType = ChessPlayer.ChessPlayerType.Human
	txtName.text = HumanName
	lvlSkill.value = HumanSkill
	_initializeSelections()
	chkSave.visible = true
	chkSave.button_pressed = player.Persistent
	
func _computer_pressed():
	if _initializing:
		return
	player.PlayerType = ChessPlayer.ChessPlayerType.Computer
	_initializeSelections()
	chkSave.visible = false

func _name_selected(index):
	if _initializing:
		return
	player = players[index]
	_initializing = true
	lvlSkill.value = player.Skill
	txtName.text = player.Name
	_setAvatar(players[index].Avatar)
	_initializing = false
	
func _name_changed():
	if _initializing:
		return
	var itemMatch: int = -1
	var index: int = 0;
	for pl: ChessPlayer in players:
		if pl.Name.to_upper() == txtName.text.to_upper():
			itemMatch = index
		index = index + 1
	if itemMatch >= 0:
		player = players[itemMatch]
		optName.selected = itemMatch
		lvlSkill.value = player.Skill
		lvlSkill.editable = false
	else:
		player = ChessPlayer.new()
		player.Skill = lvlSkill.value
		lvlSkill.editable = true

func _value_changed(v: float):
	if _initializing:
		return
	if optHuman.button_pressed:
		HumanSkill = v

