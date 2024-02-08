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
		ANIMATEMOVE,
		PIECESELECT,
		END		
	 }

@export var gamestate : GameState = GameState.INIT
@export var statewait : bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	# connections
	popNew.on_closed.connect(_on_closed_new)
	trace.connect(_trace)
	draw_move.connect(_draw_move)
	draw_board.connect(_draw_board)
	#signals cannot be fired outside of ours so we poll.
	#computer_moved.connect(_computer_moved)
	thinking.connect(_thinking)
	_gamestatereact(GameState.INIT)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
var lasttime : float = 0.0
var thistime : float = 0.0

func _physics_process(delta):
	thistime += delta
	# polling, unfortunately.
	if gamestate == GameState.COMPUTERMOVE and statewait:
		if thistime - lasttime > .1:
			lasttime = thistime
			if computer_moving() == false:
				_computer_moved(null, turnno(), lastmove(), computer_color())
			else:
				print('wait')
			

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
		GameState.USERMOVE:
			print("GS: User Move")
			_usermove()
		GameState.COMPUTERMOVE:
			print("GS: Computer Move")
			_computermove()
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
	
func _usermove():
	print("User Move")	

func _computermove():
	statewait = true
	var err = computer_move(computer_color())
	if err > 0:
		_on_error(err)
	
# Callbacks
func _on_user_moved():
	_gamestatereact(GameState.PLAY)

func _on_animated():
	_gamestatereact(GameState.PLAY)
	
# Dialog Handlers
func _on_closed_new(_cancelled, _level, _color):
	print("on_closed_new")
	# ignore cancelled for now
	board.setup(_color)
	new_game(_color, _level)
	statewait = false
	_gamestatereact(GameState.PLAY)

func _on_error(_err):
	print('***** ERROR *****' + errorstr(_err))
	
# Signal Handlers
func _draw_move(node, n, m, c):
	print("move!")
	
func _draw_board(node, n):
	# we restrict when this gets
	# executed to load and new game
	if (gamestate < GameState.PLAY):
		board.refreshpieces()

func _computer_moved(node, n, m, c):
	# for now we ONLY will paint the board
	# eventually we will animate the move
	# which will force the board to be redrawn.
	statewait = false
	board.animate_move(m)
	gamestate = GameState.ANIMATEMOVE
	
func _thinking(node, m, p):
	print("thinking!")
	
func _trace(node, msg):
	print(":" + msg)
