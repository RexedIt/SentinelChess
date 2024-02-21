@uid("uid://b1pg13u8nfix6") # Generated automatically, do not modify.

extends SentinelChess

# siblings
@onready var popNew2 : Window = get_parent().get_node("popNew2")
@onready var popLoad : FileDialog = get_parent().get_node("popLoad")
@onready var popSave : FileDialog = get_parent().get_node("popSave")
@onready var board : Node2D = get_parent().get_node("Board")
@onready var gameUI : CanvasLayer = get_parent().get_node("GameUI")

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
var prepopgamestate : GameState = GameState.INIT
@export var statewait : bool = false
var filename : String

# Called when the node enters the scene tree for the first time.
func _ready():
	# connections
	popNew2.on_closed.connect(_on_closed_new)
	popLoad.on_closed.connect(_on_closed_load)
	popSave.on_closed.connect(_on_closed_save)
	_gamestatereact(GameState.INIT)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
var lasttime : float = 0.0
var thistime : float = 0.0

func _physics_process(delta):
	thistime += delta
	# polling, unfortunately.
	if hasevent():
		var ce : ChessEvent = popevent()
		match ce.event_type():
			ChessEvent.ChessEventType.ceRefreshBoard:
				print('ceRefreshBoard')
				refresh_board(ce.board())
			ChessEvent.ChessEventType.ceConsider:
				pass
			ChessEvent.ChessEventType.ceMove:
				print('ceMove')
				if has_local() and !is_local(ce.color()):
					_computer_moved(ce.move_no(), ce.move(), ce.color())
				else:
					_draw_move(ce.move_no(), ce.move(), ce.color())
			ChessEvent.ChessEventType.ceTurn:
				print('ceTurn')
				if gamestate != GameState.ANIMATEMOVE:
					_on_turn(ce.move_no(),ce.board(),ce.color())
			ChessEvent.ChessEventType.ceEnd:
				print('ceEnd *** REM *** TODO')
			ChessEvent.ChessEventType.ceChat:
				print('ceChat *** REM *** TODO')

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
		GameState.LOAD:
			# Select Load Option
			print("GS: Load Prompt")
			_loadgameprompt()
		GameState.SAVE:
			# Select Save Option
			print("GS: Save Prompt")
			_savegameprompt()
		GameState.PLAY:
			print("GS: Game Play")
			_gameplay()
		GameState.USERMOVE:
			print("GS: User Move")
			_usermove()
		GameState.COMPUTERMOVE:
			print("GS: Computer Move")
			_computermove()

func _newgameprompt():
	popNew2.visible = true
	statewait = true
	
func _loadgameprompt():
	popLoad.visible = true
	statewait = true

func _savegameprompt():
	popSave.visible = true
	statewait = true
	
func _gameplay():
	if (state() == SentinelChess.ChessGameState.Play):
		if is_local(turn_color()):
			_gamestatereact(GameState.USERMOVE)
		else:
			_gamestatereact(GameState.COMPUTERMOVE)
	else:
		_gamestatereact(GameState.END)
	
func _usermove():
	print("User Move")	

func _computermove():
	statewait = true
	
# Callbacks
func _on_user_moved(n, m, c):
	#gameUI.append_move(n, m, c)
	_gamestatereact(GameState.PLAY)

func _on_animated():
	var c : ChessColor = turn_color();
	if is_local_active(c):
		var b : ChessBoard = get_board()
		refresh_board(b)
		gameUI.refreshPrompt(c)
	_gamestatereact(GameState.PLAY)
	
# Dialog Handlers
func _on_closed_new(_cancelled, _white, _black):
	print("on_closed_new")
	if _cancelled:
		_gamestatereact(prepopgamestate)
		return
	# start new game
	new_game(_white, _black)
	board.setup(preferred_board_color())
	gameUI.clear_history()
	gameUI.append_history('New Game')
	statewait = false
	_gamestatereact(GameState.PLAY)

func _on_closed_load(_cancelled, _filename):
	print("on_closed_load")
	if _cancelled:
		_gamestatereact(prepopgamestate)
		return
	# load a game
	filename = _filename
	var err : int = load_game(_filename)
	if err != 0:
		_on_error(err)
		_gamestatereact(prepopgamestate)
		return
	gameUI.clear_history()
	gameUI.append_load(_filename)
	statewait = false
	_gamestatereact(GameState.PLAY)

func _on_closed_save(_cancelled, _filename):
	print("on_closed_save")
	if _cancelled:
		_gamestatereact(prepopgamestate)
		return
	# load a game
	filename = _filename
	var err : int = save_game(_filename)
	if err != 0:
		_on_error(err)
		_gamestatereact(prepopgamestate)
		return
	gameUI.clear_history()
	gameUI.append_history('Save Game - ' + _filename)
	statewait = false
	_gamestatereact(GameState.PLAY)

func _on_new_game():
	var list = [GameState.INIT,GameState.PLAY,GameState.USERMOVE]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.NEW)
		
func _on_load_game():
	var list = [GameState.INIT,GameState.PLAY,GameState.USERMOVE]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.LOAD)

func _on_save_game():
	var list = [GameState.INIT,GameState.PLAY,GameState.USERMOVE]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.SAVE)
	
func _on_error(_err : int):
	gameUI.show_error('! ' + errorstr(_err))
	print('***** ERROR *****' + errorstr(_err))
	
func _on_error_msg(msg : String):
	gameUI.show_error(msg)
	
# Signal Handlers
func _draw_move(n, m, c):
	gameUI.append_move(n,m,c)
	board.move_piece(m.p0, m.p1)
	board.refreshpieces(self.get_board())	
	
func _on_turn(n, b, c):
	if is_local_active(c):
		refresh_board(b)
	gameUI.refreshPrompt(c)
	
func refresh_board(b : ChessBoard):
	board.setup(turn_color())
	board.refreshpieces(b)

func refresh_turn(b : ChessBoard):
	board.refreshpieces(b)
	_gamestatereact(GameState.PLAY)
	
func _user_moved(m):
	board.animate_move(m)
	gamestate = GameState.ANIMATEMOVE
		
func _computer_moved(n, m, c):
	# for now we ONLY will paint the board
	# eventually we will animate the move
	# which will force the board to be redrawn.
	statewait = false
	# if the opponent is ALSO computer, do not
	# animate as we will miss the action.
	gameUI.append_move(n,m,c)
	board.animate_move(m)
	gamestate = GameState.ANIMATEMOVE
			
