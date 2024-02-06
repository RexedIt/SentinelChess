@uid("uid://c2fxtcukqail3") # Generated automatically, do not modify.

extends SentinelChess

# siblings
@onready var popNew : Window = get_parent().get_node("popNew")
@onready var board : Node2D = get_parent().get_node("Board")

enum GameState {
		INIT,
		NEWORLOAD,
		NEW,
		LOAD,
		SAVE,
		PLAY,
		USERMOVE,
		COMPUTERMOVE,
		PIECESELECT,
		END		
	 }

var gamestate : GameState = GameState.INIT
var statewait : bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	# connections
	popNew.on_closed.connect(_on_closed_new)
	trace.connect(_trace)
	draw_board.connect(_draw_board)
	_gamestatereact(GameState.INIT)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _gamestatereact(gs):
	gamestate = gs
	match gamestate:
		# clear the pending state flag
		GameState.INIT:
			# Initialization
			print("GS: Initialization")
			# for now, we want to move to the new game prompt
			# later we will select new or load
			_newgameprompt()
		GameState.NEWORLOAD:
			# Select new or Load Option
			print("GS: New or Load Prompt")
			# for now, we want to move to the new game prompt
			# later we will select new or load
			_newgameprompt()
		GameState.NEW:
			# Select New Game Option
			print("GS: New Prompt")
			_newgameprompt()
		GameState.PLAY:
			print("GS: Game Play")
			_gameplay()
		GameState.LOAD:
			print("GS: Load Prompt")
		GameState.SAVE:
			print("GS: Save Prompt")

func _newgameprompt():
	popNew.visible = true
	statewait = true;
	
func _gameplay():
	if (state() == SentinelChess.ChessGameState.Play):
		if (turn_color() == user_color()):
			_gamestatereact(GameState.USERMOVE)
		else:
			_gamestatereact(GameState.COMPUTERMOVE)
	else:
		_gamestatereact(GameState.END)
		
# Dialog Handlers
func _on_closed_new(_cancelled, _level, _color):
	print("on_closed_new")
	# ignore cancelled for now
	board.setup(_color)
	new_game(_color, _level)
	statewait = false
	_gamestatereact(GameState.PLAY)
		
# Signal Handlers
func _draw_board(node, n):
	# we restrict when this gets
	# executed to load and new game
	if (gamestate < GameState.PLAY):
		board.refreshpieces()

func _trace(node, msg):
	print(":" + msg)
