extends Panel

@onready var game_manager : SentinelChess = get_parent().get_node('SentinelChess')
@onready var sequence : Sprite2D = get_node('sequence')
@onready var black : Sprite2D = get_node('black')
@onready var white : Sprite2D = get_node('white')
@onready var status : Label = get_node('status')
@onready var winner : Label = get_node('winner')
@onready var music : AudioStreamPlayer = get_node('music')

# Called when the node enters the scene tree for the first time.
var elapsed : float = 0;
var frame : int = 0;

func _ready():
	music.finished.connect(suppress)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if visible:
		elapsed += delta;
		frame = int(elapsed/0.4)
		sequence.frame = frame % 16
		if Input.is_anything_pressed():
			suppress()

func suppress():
	visible = false
	frame = 0
	elapsed = 0
	music.stop()
			
func finish_game(g : SentinelChess.ChessGameState, w : SentinelChess.ChessColor):
	var musictrack = null
	match w:
		SentinelChess.ChessColor.Black:
			sequence.texture = load('res://Sprites/RetroWood/BlackWin/SpriteSheet.jpg')
			white.visible = false
			black.visible = true
			winner.text = 'Black Wins!'
			musictrack = preload('res://Sound/Music/USSR_Short.mp3')
		SentinelChess.ChessColor.White:
			sequence.texture = load('res://Sprites/RetroWood/WhiteWin/SpriteSheet.jpg')
			white.visible = true
			black.visible = false
			winner.text = 'White Wins!'
			musictrack = preload('res://Sound/Music/USA_Short.mp3')
		SentinelChess.ChessColor.cNone:
			sequence.texture = load('res://Sprites/RetroWood/WhiteWin/SpriteSheet.jpg')
			white.visible = false
			black.visible = false
			winner.text = ''
	if musictrack != null:
		music.stream = musictrack
		music.play()
	status.text = game_manager.gamestatestr(g)
	frame = 0
	elapsed = 0
	visible = true
	
	
	
