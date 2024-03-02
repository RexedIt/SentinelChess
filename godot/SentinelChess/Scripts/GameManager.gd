extends SentinelChess

# siblings
@onready var popNew : Window = get_parent().get_node("popNew")
@onready var popLoad : FileDialog = get_parent().get_node("popLoad")
@onready var popSave : FileDialog = get_parent().get_node("popSave")
@onready var board : Node2D = get_parent().get_node("Board")
@onready var gameUI : CanvasLayer = get_parent().get_node("GameUI")

const GameState = preload("res://Scripts/GameState.gd").GameState_

@export var gamestate : GameState = GameState.INIT
var prepopgamestate : GameState = GameState.INIT
@export var statewait : bool = false
var filename : String

# Called when the node enters the scene tree for the first time.
func _ready():
	# connections
	popNew.on_closed.connect(_on_closed_new)
	popLoad.on_closed.connect(_on_closed_load)
	popSave.on_closed.connect(_on_closed_save)
	_gamestatereact(GameState.INIT)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
func _physics_process(delta):
	# polling, unfortunately.
	if hasevent():
		var ce : ChessEvent = popevent()
		match ce.event_type():
			ChessEvent.ChessEventType.ceRefreshBoard:
				print('ceRefreshBoard')
				refresh_board(ce.board())
			ChessEvent.ChessEventType.ceConsider:
				board.thinking(ce.move().p1)
			ChessEvent.ChessEventType.ceTurn:
				var n : int = ce.turn_no()
				var m : ChessMove = ce.move()
				var ch : bool = ce.check()
				var b : ChessBoard = ce.board()
				var c : ChessColor = ce.color()
				var g : ChessGameState = ce.game_state()
				var wc : ChessColor = ce.win_color();
				var wt : int = ce.white_time()
				var bt : int = ce.black_time()
				if m.is_valid():
					var cm : ChessColor = ChessColor.White
					if c == ChessColor.White:
						cm = ChessColor.Black
					if has_local() and !is_local(cm):
						_computer_moved(n, m, b, cm)
					else:
						_draw_move(n, m, b, cm)
				if gamestate != GameState.ANIMATEMOVE:
					_on_turn(n,b,c)
				gameUI.clock_turn(c, wt, bt)
				if g > ChessGameState.Play:
					gameUI.finish_game(g, wc)
			ChessEvent.ChessEventType.ceState:
				print('ceState')
				_on_state(ce.game_state(), ce.win_color())
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
			# _newgameprompt()
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
		GameState.IDLE:
			print("GS: Idle")
		GameState.USERMOVE:
			print("GS: User Move")
			_usermove()
		GameState.COMPUTERMOVE:
			print("GS: Computer Move")
			_computermove()
	gameUI.gamestate(gs)

func _newgameprompt():
	popNew.visible = true
	statewait = true
	
func _loadgameprompt():
	popLoad.visible = true
	statewait = true

func _savegameprompt():
	popSave.visible = true
	statewait = true
	
func _gameplay():
	if state() == SentinelChess.ChessGameState.Play:
		if is_local(turn_color()):
			_gamestatereact(GameState.USERMOVE)
		else:
			_gamestatereact(GameState.COMPUTERMOVE)
	elif state() == SentinelChess.ChessGameState.Idle:
		_gamestatereact(GameState.IDLE)
	else:
		_gamestatereact(GameState.END)
	
func _usermove():
	print("User Move")	

func _computermove():
	statewait = true
	
# Callbacks
func _on_user_moved():
	#gameUI.append_move(n, m, get_board(), c)
	_gamestatereact(GameState.PLAY)

func _on_animated():
	var c : ChessColor = turn_color();
	if is_local_active(c):
		var b : ChessBoard = get_board()
		refresh_board(b)
		gameUI.refreshPrompt(c)
	_gamestatereact(GameState.PLAY)
	
# Dialog Handlers
func _on_closed_new(_cancelled, _white, _black, _clock):
	print("on_closed_new")
	if _cancelled:
		_gamestatereact(prepopgamestate)
		return
	# start new game
	new_game(_white, _black, _clock)
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
	var list = [GameState.INIT,GameState.PLAY,GameState.IDLE,GameState.USERMOVE]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.NEW)
		
func _on_load_game():
	var list = [GameState.INIT,GameState.PLAY,GameState.IDLE,GameState.USERMOVE]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.LOAD)

func _on_save_game():
	var list = [GameState.INIT,GameState.PLAY,GameState.IDLE,GameState.USERMOVE]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.SAVE)
	
func _on_error(_err : int):
	gameUI.show_error('! ' + errorstr(_err))
	print('***** ERROR *****' + errorstr(_err))
	
func _on_error_msg(msg : String):
	gameUI.show_error(msg)
	
# Signal Handlers
func _draw_move(n, m, b, c):
	gameUI.append_move(n,m,b,c)
	board.move_piece(m.p0, m.p1)
	board.refreshpieces(b)	
	
func _on_turn(n, b, c):
	if is_local_active(c):
		refresh_board(b)
	gameUI.refreshPrompt(c)

func set_idle(b : bool):
	if b:
		_gamestatereact(GameState.IDLE)
	else:
		_gamestatereact(GameState.PLAY)
	gameUI.set_idle(b)
	board.set_idle(b)

func finish_game(s : ChessGameState, w : ChessColor):
	_gamestatereact(GameState.END)
	board.finish_game(s, w)
	
func _on_state(s : ChessGameState, w : ChessColor):
	if s == Idle:
		set_idle(true)
	if s == Play:
		set_idle(false)
	if s > Play:
		finish_game(s, w)
					
func refresh_board(b : ChessBoard):
	board.setup(turn_color())
	board.refreshpieces(b)

func refresh_turn(b : ChessBoard):
	board.refreshpieces(b)
	_gamestatereact(GameState.PLAY)
	
func _user_moved(m):
	board.animate_move(m)
	print('uman')
	gamestate = GameState.ANIMATEMOVE
		
func _computer_moved(n, m, b, c):
	# for now we ONLY will paint the board
	# eventually we will animate the move
	# which will force the board to be redrawn.
	# if the opponent is ALSO computer, do not
	# animate as we will miss the action.
	if board.animate_move(m):
		statewait = false
		gameUI.append_move(n,m,b,c)
		gamestate = GameState.ANIMATEMOVE
			
