extends SentinelChess

# siblings
@onready var skin : Node = get_node('/root/MainGame/Skin')
@onready var base_title : String = ''
@onready var Background : Sprite2D = get_parent().get_node("Background")
@onready var popNew : Window = get_parent().get_node("popNew")
@onready var popPuzzle : Window = get_parent().get_node("popPuzzle")
@onready var popLoad : FileDialog = get_parent().get_node("popLoad")
@onready var popSave : FileDialog = get_parent().get_node("popSave")
@onready var board : Node2D = get_parent().get_node("Board")
@onready var gameUI : CanvasLayer = get_parent().get_node("GameUI")
@onready var popEnd : Panel = get_parent().get_node("popEnd")
@onready var popPromote : Panel = get_parent().get_node("popPromote")
@onready var pnlCaptured : Panel = get_parent().get_node("GameUI/pnlCaptured")
@onready var popSettings : Window = get_parent().get_node("popSettings")

const GameState = preload("res://Scripts/GameState.gd").GameState_

@export var gamestate : GameState = GameState.INIT
var prepopgamestate : GameState = GameState.INIT
@export var statewait : bool = false
var filename : String
var promotemove : ChessMove

# Called when the node enters the scene tree for the first time.
func _ready():
	# connections
	popNew.on_closed.connect(_on_closed_new)
	popPuzzle.on_closed.connect(_on_closed_puzzle)
	popLoad.on_closed.connect(_on_closed_load)
	popSave.on_closed.connect(_on_closed_save)
	popPromote.on_closed.connect(_on_closed_promote)
	popSettings.on_closed.connect(_on_closed_settings)
	base_title = get_window().title
	applyskin()
	_gamestatereact(GameState.INIT)

func applyskin():
	popNew.applyskin()
	popPuzzle.set_theme(skin.theme)
	popLoad.set_theme(skin.theme)
	popSave.set_theme(skin.theme)
	popPromote.set_theme(skin.theme)
	popEnd.set_theme(skin.theme)
	popEnd.applyskin()
	popSettings.set_theme(skin.theme)
	Background.texture = skin.sprite('Background.jpg')
	board.applyskin()
	gameUI.applyskin()

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
				refresh_board(ce.color(), ce.board())
			ChessEvent.ChessEventType.ceConsider:
				gameUI.thinking(ce.color(), ce.percent())
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
				var wp : int = ce.white_points()
				var bp : int = ce.black_points()
				var cmt : String = ce.cmt()
				if m.is_valid():
					var cm : ChessColor = ChessColor.White
					if c == ChessColor.White:
						cm = ChessColor.Black
					if has_local() and !is_local(cm):
						_computer_moved(n, m, b, cm, cmt)
					else:
						_draw_move(n, m, b, cm, cmt)
				if gamestate != GameState.ANIMATEMOVE:
					_on_turn(n,b,c)
				gameUI.clock_turn(c, wt, bt)
				if g > ChessGameState.Play:
					finish_game(g, wc)
			ChessEvent.ChessEventType.ceState:
				print('ceState')
				_on_state(ce.game_state(), ce.win_color())
			ChessEvent.ChessEventType.cePoints:
				print('cePoints')
				var wp : int = ce.white_points()
				var bp : int = ce.black_points()
				_on_points(wp, bp)
			ChessEvent.ChessEventType.ceChat:
				print('ceChat *** REM *** TODO')

func _gamestatereact(gs):
	gamestate = gs
	match gamestate:
		# clear the pending state flag
		GameState.INIT:
			# Initialization
			print("GS: Initialization")
			initialize('..\\..\\ChessData\\')
			# for now, we want to move to the new game prompt
			# later we will select new or load
			# _newgameprompt()
		GameState.NEW:
			# Select New Game Option
			print("GS: New Prompt")
			_newgameprompt()
		GameState.PUZZLE:
			# Select New Puzzle Option
			print("GS: New Puzzle")
			_newpuzzleprompt();
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
		GameState.PIECESELECT:
			print('GS: Piece Select')
			_pieceselect()
	gameUI.gamestate(gs)

func _newgameprompt():
	popNew.visible = true
	statewait = true
	
func _newpuzzleprompt():
	popPuzzle.visible = true
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

func _pieceselect():
	popPromote.visible = true
		
func user_move(c : ChessColor, m : ChessMove, a : bool):
	var err : int = move_m(c, m)
	if err != 0:
		if errorstr(err).contains('Promote'):
			promotemove = m
			_gamestatereact(GameState.PIECESELECT)
			return true
		if errorstr(err).contains('Incorrect') and puzzle():
			gameUI.incorrectmove()
			return false
		_on_error(err)
		return false
	if a:
		board.animate_move(m)
		gamestate = GameState.ANIMATEMOVE
	else:
		_on_user_moved()
	return true

func user_move_str( c : ChessColor, s : String, a : bool):
	var err : int = move_s(c, s)
	if err != 0:
		if errorstr(err).contains('Incorrect') and puzzle():
			gameUI.incorrectmove()
			return false
		_on_error(err)
		return false
	if a:
		board.animate_move(lastmove())
		gamestate = GameState.ANIMATEMOVE
	else:
		_on_user_moved()
	return true
	
# Callbacks
func _on_user_moved():
	#gameUI.append_move(n, m, get_board(), c)
	_gamestatereact(GameState.PLAY)

func _on_animated():
	var c : ChessColor = turn_color();
	if is_local_active(c):
		var b : ChessBoard = get_board()
		refresh_board(c, b)
		gameUI.refreshPrompt(c)
	_gamestatereact(GameState.PLAY)
	
func save_player(player : ChessPlayer):
	if player.PlayerType == ChessPlayer.Human:
		var err : int = hub_update_player(player)
		if err != 0:
			_on_error(err)

# Dialog Handlers
func _on_closed_new(_cancelled, _title, _white, _black, _clock):
	print("on_closed_new")
	if _cancelled:
		_gamestatereact(prepopgamestate)
		return
	# start new game
	save_player(_white)
	save_player(_black)
	new_game(_title, _white, _black, _clock)
	refresh_board_color(preferred_board_color())
	gameUI.initialize('New Game')
	DisplayServer.window_set_title(base_title + ' - New Game')
	statewait = false
	_gamestatereact(GameState.PLAY)

func _on_closed_puzzle(_cancelled, _player, _keywords, _rating):
	print("on_closed_puzzle")
	if _cancelled:
		_gamestatereact(prepopgamestate)
		return
	# start new game
	save_player(_player)
	var err : int = load_puzzle(_player, _keywords, _rating)
	if err != 0:
		_on_error(err)
		_gamestatereact(prepopgamestate)
		return
	refresh_board_color(preferred_board_color())
	gameUI.initialize('Load Puzzle')
	DisplayServer.window_set_title(base_title + ' - Puzzle')
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
	gameUI.initialize('Load Game - ' + _filename)
	DisplayServer.window_set_title(base_title + ' - ' + _filename)
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
	gameUI.append_history('Save Game - ' + _filename)
	DisplayServer.window_set_title(base_title + ' - ' + _filename)
	statewait = false
	_gamestatereact(GameState.PLAY)

func _on_closed_promote(_cancelled, _color, _piece):
	if !_cancelled:
		promotemove.set_promote(_piece)
		user_move(_color,promotemove,false)

func _on_closed_settings(_cancelled, _skin, _voice, _music, _sfx, _rot):
	if !_cancelled:
		skin.applysettings(_skin,_voice,_music,_sfx,_rot)
		applyskin()
			
func _on_new_game():
	var list = [GameState.INIT,GameState.PLAY,GameState.IDLE,GameState.USERMOVE,GameState.END]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.NEW)
		
func _on_load_game():
	var list = [GameState.INIT,GameState.PLAY,GameState.IDLE,GameState.USERMOVE,GameState.END]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.LOAD)

func _on_load_puzzle():
	var list = [GameState.INIT,GameState.PLAY,GameState.IDLE,GameState.USERMOVE,GameState.END]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.PUZZLE)

func _on_settings():
	popSettings.visible = true
	
func _on_save_game():
	var list = [GameState.INIT,GameState.PLAY,GameState.IDLE,GameState.USERMOVE,GameState.END]
	if list.has(gamestate):
		prepopgamestate = gamestate
		_gamestatereact(GameState.SAVE)
	
func _on_error(_err : int):
	gameUI.show_error('! ' + errorstr(_err))
	print('***** ERROR *****' + errorstr(_err))

func _on_error_msg(_err : String):
	gameUI.show_error(_err)
	print('***** ERROR *****' + _err)
		
# Signal Handlers
func _draw_move(n, m, b, cm, cmt):
	gameUI.append_move(n,m,b,cm,cmt)
	board.move_piece(m.p0, m.p1)
	board.refreshpieces(b)	
	pnlCaptured.refreshpieces(b)
	
func _on_turn(n, b, c):
	refresh_board(c, b)
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
	if popEnd.visible == false:
		gameUI.finish_game(s, w)
		board.finish_game(s, w)
		popEnd.finish_game(s, w)
		
func _on_state(s : ChessGameState, w : ChessColor):
	if s == Idle:
		set_idle(true)
	if s == Play:
		set_idle(false)
	if s > Play:
		finish_game(s, w)
		
func _on_points(wp : int, bp : int):
	gameUI.update_points(wp, bp)
	
func refresh_board(c : ChessColor, b : ChessBoard):
	refresh_board_color(c)
	board.refreshpieces(b)
	pnlCaptured.refreshpieces(b)

func refresh_board_color(c : ChessColor):
	var c1 : ChessColor = c;
	if !skin.rotateBoard or !is_local(c):
		c1 = preferred_board_color()
	board.setup(c1)
	pnlCaptured.setup(c1)
	
func refresh_turn(c : ChessColor, b : ChessBoard):
	board.refreshpieces(b)
	pnlCaptured.refreshpieces(b)
	_gamestatereact(GameState.PLAY)
	
func _user_moved(m):
	board.animate_move(m)
	gamestate = GameState.ANIMATEMOVE
		
func _computer_moved(n, m, b, c, cmt):
	# for now we ONLY will paint the board
	# eventually we will animate the move
	# which will force the board to be redrawn.
	# if the opponent is ALSO computer, do not
	# animate as we will miss the action.
	if board.animate_move(m):
		statewait = false
		gameUI.append_move(n,m,b,c,cmt)
		gamestate = GameState.ANIMATEMOVE
			
