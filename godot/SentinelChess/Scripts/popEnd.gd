extends Panel

@onready var skin : Node = get_node('/root/MainGame/Skin')
@onready var game_manager : SentinelChess = get_parent().get_node('SentinelChess')
@onready var background : Sprite2D = get_node('background')
@onready var black : Sprite2D = get_node('black')
@onready var white : Sprite2D = get_node('white')
@onready var status : Label = get_node('status')
@onready var winner : Label = get_node('winner')
@onready var music : AudioStreamPlayer = get_node('music')

# Called when the node enters the scene tree for the first time.
var elapsed : float = 0;
var frame : int = 0;
var loaded : bool = false
var skinned : bool = false

func _ready():
	music.volume_db = -15
	music.finished.connect(suppress)
	loaded = true
	if not skinned:
		applyskin()
	
func applyskin():
	if loaded:
		black.texture = skin.sprite('BlackKnight.png')
		white.texture = skin.sprite('WhiteKnight.png')
		music.volume_db = skin.music_db()
		skinned = true
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if (Input.is_anything_pressed()): 
		suppress()
	
func suppress():
	visible = false
	frame = 0
	elapsed = 0
	music.stop()
			
func finish_game(g : SentinelChess.ChessGameState, w : SentinelChess.ChessColor):
	var musictrack = null
	var localw : bool = game_manager.is_local(w)
	var puzzle : bool = game_manager.puzzle()
	match w:
		SentinelChess.ChessColor.Black:
			background.texture = skin.sprite('BlackWin.jpg')
			white.visible = false
			black.visible = true
			winner.text = 'Black Wins!'
		SentinelChess.ChessColor.White:
			background.texture = skin.sprite('WhiteWin.jpg')
			white.visible = true
			black.visible = false
			winner.text = 'White Wins!'
		SentinelChess.ChessColor.cNone:
			background.texture = skin.sprite('Draw.jpg')
			white.visible = false
			black.visible = false
			winner.text = ''
	if localw:
		if puzzle:
			musictrack = skin.music('PuzzleWin.mp3')
		else:
			musictrack = skin.music('Win.mp3')
	else:
		musictrack = skin.music('Loss.mp3')
	if musictrack != null:
		music.stream = musictrack
		music.play()
	status.text = game_manager.gamestatestr(g)
	frame = 0
	elapsed = 0
	visible = true
	
	
	
