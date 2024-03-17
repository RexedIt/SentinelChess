extends CanvasLayer

@onready var game_manager : SentinelChess = get_parent().get_node('SentinelChess')

@onready var pnlScore : Panel = get_node('pnlScore')
@onready var lblTitle : Label = get_node('lblTitle')
@onready var pnlPlayerTop : Panel = get_node('pnlPlayerTop')
@onready var pnlPlayerBottom : Panel = get_node('pnlPlayerBottom')
@onready var lblHistory : RichTextLabel = get_node('lblHistory')
@onready var txtCmd : LineEdit = get_node('txtCmd')
@onready var lblError : Label = get_node('lblError')
@onready var lblCmd : Label = get_node('lblCmd')
@onready var btnSave : Button = get_node('btnSave')
@onready var btnRewind : Button = get_node('btnRewind')
@onready var btnAdvance : Button = get_node('btnAdvance')
@onready var btnHint : Button = get_node('btnHint')
@onready var btnPause : Button = get_node('btnPause')
@onready var lblWhiteClock : Label = get_node('lblWhiteClock')
@onready var lblBlackClock : Label = get_node('lblBlackClock')
@onready var voice : AudioStreamPlayer = get_node('voice')
@onready var MoveSound = preload('res://Sound/SFX/Open_01.mp3')
@onready var PromoteSound = preload('res://Sound/SFX/Collect_Point_01.mp3')
@onready var PlayTexture : Texture2D = preload('res://Sprites/RetroWood/Play.png')
@export var step : int

const GameState = preload("res://Scripts/GameState.gd").GameState_

var is_idle : bool = false
var PauseTexture : Texture2D
var meta : ChessMeta = null
var puzzle : bool = false
var hints : int = 0
var points : int = 0
var white_points : int = 0
var black_points : int = 0
var voice_queue = []
var do_voice : bool = false
var do_sfx : bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	txtCmd.grab_focus()
	PauseTexture = btnPause.icon

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

var errortime : float = 0.0

func _physics_process(delta):
	if errortime > 0.0:
		errortime -= delta
		if errortime < 0.0:
			errortime = 0.0
		if errortime<1.0:
			var smooth = errortime
			if (step):
				var pct = int(smooth * 100) / step * step
				smooth = float(pct)/100.0
			lblError.modulate.a = smooth
	clock_update(delta)
	play_voice()

func update_players(turnc : SentinelChess.ChessColor):
	# who's on top?
	var botc : SentinelChess.ChessColor
	if game_manager.is_local_active(turnc):
		botc = turnc
	else:
		botc = game_manager.preferred_board_color()
	if botc == SentinelChess.White:
		pnlPlayerTop.refreshplayer(SentinelChess.Black, meta.black())
		pnlPlayerBottom.refreshplayer(botc, meta.white())
	else:
		pnlPlayerTop.refreshplayer(SentinelChess.White, meta.white())
		pnlPlayerBottom.refreshplayer(botc, meta.black())
	
func initialize(msg : String):
	meta = game_manager.get_meta()
	var tc = game_manager.turn_color()
	lblTitle.text = meta.title()
	puzzle = meta.puzzle()
	hints = meta.hints()
	points = meta.points()
	white_points = meta.white_points()
	black_points = meta.black_points()
	pnlScore.visible = true
	if puzzle:
		pnlScore.setPuzzleValues(points,hints,true)
	else:
		pnlScore.setScoreValues(white_points,black_points);
	btnHint.disabled = (not puzzle) or hints <= 0
	btnRewind.disabled = puzzle
	btnPause.disabled = puzzle
	btnAdvance.disabled = puzzle
	clear_history()
	append_history(msg)
	#update_players(tc)
	announceTurn(tc)
	
func clear_history():
	lblHistory.clear()
	
func coordstr(c: ChessCoord) -> String:
	var str : String
	if c == null:
		return "??"
	str += char(97 + c.x)
	str += char(49 + c.y)
	return str
	
func strcoord(str: String) -> ChessCoord:
	if str.length()!=2:
		return null
	var astr : PackedByteArray = str.to_upper().to_ascii_buffer()
	var x : int = astr[0] - 65
	var y : int = astr[1] - 49
	if y >= 0 and y <= 7:
		if x >=0 and x <= 7:
				var cc : ChessCoord = ChessCoord.new()
				cc.y = y
				cc.x = x
				return cc
	return null
	
func movestr(m : ChessMove) -> String:
	var str : String = coordstr(m.p0) + '-' + coordstr(m.p1)
	if m.en_passant():
		str += ' EP'
	if m.c!=-1:
		str += ' CAST'
	if m.get_promote() != 0:
		str += ' PROM'
	return str
	
func append_history(msg : String, color : String = 'blue'):
	lblHistory.push_color(color)
	lblHistory.add_text(msg.to_upper())
	lblHistory.newline()
	lblHistory.pop()

func announceTurn(col : SentinelChess.ChessColor):
	var vprompt = 'WhiteTurn'
	if col == SentinelChess.ChessColor.Black:
		vprompt = 'BlackTurn'
	if game_manager.is_local(col):
		add_voice(vprompt)
		
func refreshPrompt(col : SentinelChess.ChessColor):
	var crgb : Color = Color(1,1,1)
	if col == SentinelChess.ChessColor.Black:
		crgb = Color(0,0,0)
	lblCmd.set('theme_override_colors/font_color', crgb)	
	update_players(col)
	
func append_move(n : int, m : ChessMove, b : ChessBoard, col : SentinelChess.ChessColor):
	var color : String = 'white'
	if col == SentinelChess.ChessColor.Black:
		color = 'black'
	#print(color + ' ' + movestr(m))
	play_move_sfx()
	append_history(str(n) + ' ' + color + ' ' + movestr(m), color)
	if (b.check_state(SentinelChess.Black)):
		append_history('Black in Check.', 'black')	
		add_voice('Check')
	if (b.check_state(SentinelChess.White)):
		append_history('White in Check.', 'white')
		add_voice('Check')
		
# UI Handlers
func show_error(msg : String):
	errortime = 3
	lblError.text = msg
	lblError.modulate.a = 1.0
	
func _on_btn_new_pressed():
	game_manager._on_new_game()

func _on_btn_load_pressed():
	game_manager._on_load_game()

func _on_btn_save_pressed():
	game_manager._on_save_game()

func _on_btn_rewind_pressed():
	handle_rewind()

func _on_btn_pause_pressed():
	if is_idle:
		handle_play()
	else:
		handle_pause()

func _on_btn_advance_pressed():
	handle_advance()

func _on_btn_puzzle_pressed():
	game_manager._on_load_puzzle()

func _on_btn_hint_pressed():
	handle_hint()

func handle_goto(turn_no: int) -> bool:
	if puzzle:
		show_error('In Puzzle')
		true
	var err : int = game_manager.goto_turn(turn_no)
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	append_history('Goto Turn - ' + str(turn_no))
	#game_manager.refresh_turn(game_manager.get_board())
	return true
	
func handle_rewind() -> bool:
	if puzzle:
		show_error('In Puzzle')
		true
	var err : int = game_manager.rewind_game()
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	append_history('Rewind')
	#game_manager.refresh_turn(game_manager.get_board())
	return true

func handle_advance() -> bool:
	if puzzle:
		show_error('In Puzzle')
		true
	var err : int = game_manager.advance_game()
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	append_history('Advance')
	#game_manager.refresh_turn(game_manager.get_board())
	return true

func handle_hint() -> bool:
	return true
	
func handle_pause():
	if puzzle:
		show_error('In Puzzle')
		true
	var err : int = game_manager.pause_game()
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	append_history('Pause')

func handle_play():
	var err : int = game_manager.play_game()
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	append_history('Play')
	
func _on_txt_cmd_text_submitted(new_text):
	if new_text == '':
		return
	var cmd : String = new_text;
	var ucmd1 : String = cmd.left(1).to_upper();
	var m : ChessMove
	var handled : bool = false
	match ucmd1:
		'N':
			_on_btn_new_pressed()
			handled = true
		'Z':
			_on_btn_puzzle_pressed()
			handled = true
		'M':
			handled = handle_move(cmd.get_slice(' ',1))
		'L':
			handled = handle_load(cmd.get_slice(' ',1))
		'S':
			handled = handle_save(cmd.get_slice(' ',1))
		'T':
			var s : String = cmd.get_slice(' ',1)
			var turn_no : int = int(s)
			if s == '':
				show_error('Need Turn Number')
			handled=handle_goto(turn_no)
		'<':
			handled = handle_rewind()
		'>':
			handled = handle_advance()
		'P':
			handled = handle_play()
		'I':
			handled = handle_pause()
		# a move?
		_:
			handled = handle_move(cmd)
	if handled:
		txtCmd.text = ''		
	else:
		show_error('Invalid Move or Command')
		add_voice('InvalidMove')
	return

func possible_move(p0 : ChessCoord, p1 : ChessCoord) -> bool:
	var possible_moves : Array = game_manager.possible_moves(game_manager.turn_color())
	for possible_move in possible_moves:
		if possible_move.matches_p0p1(p0,p1):
			return true
	return false

func handle_load(filename: String) -> bool:
	var toload : String = filename + '.chs'
	var err : int = game_manager.load_game(toload)
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	initialize('Load Game - ' + toload)
	#game_manager.refresh_board()
	game_manager.set_idle(true)
	return true

func handle_save(filename: String) -> bool:
	var tosave : String = filename + '.chs'
	var err : int = game_manager.save_game(tosave)
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	append_history('Save Game - ' + tosave)
	return true
	
func handle_move(cmd: String) -> bool:
	var c : SentinelChess.ChessColor = game_manager.turn_color()
	return game_manager.user_move_str(c, cmd, true)

var countdown : float = 0
var countcol : SentinelChess.ChessColor

func time_str(t : int) -> String:
	if t > 0:
		var tenths = int((t % 1000) / 100)
		var seconds = int(t / 1000)
		var minutes = int(seconds/60)
		var hours = int(minutes/60)
		#returns a string with the format "HH:MM:SS"
		return "%01d:%02d:%02d.%1d" % [hours, minutes%60, seconds%60, tenths%10]
	else:
		return ""
	
func clock_turn(col : SentinelChess.ChessColor, wt : int, bt : int):
	lblWhiteClock.text = time_str(wt)
	lblBlackClock.text = time_str(wt)
	countcol = col
	if col == SentinelChess.White:
		countdown = float(wt) / 1000.0
	if col == SentinelChess.Black:
		countdown = float(bt) / 1000.0
		
func clock_update(delta : float):
	if countdown>=0.0:
		countdown -= delta
		var countms : int = int(countdown*1000)
		if countcol == SentinelChess.White:
			lblWhiteClock.text = time_str(countms)
		if countcol == SentinelChess.Black:
			lblBlackClock.text = time_str(countms)
	
func set_idle(b : bool):
	is_idle = b
	if is_idle:
		btnPause.icon = PlayTexture
	else:
		btnPause.icon = PauseTexture

func gamestate(gs):
	if btnRewind:
		var no_game = gs < GameState.PLAY
		var puzzle = game_manager.puzzle()
		btnRewind.disabled = no_game or puzzle
		btnPause.disabled = no_game or puzzle
		btnAdvance.disabled = no_game or puzzle
		btnHint.disabled = no_game or (not puzzle) or hints <= 0
		btnSave.disabled = no_game
		do_voice = game_manager.has_local()
		do_sfx = do_voice
		
func finish_game(s : SentinelChess.ChessGameState, w : SentinelChess.ChessColor):
	append_history(game_manager.gamestatestr(s))
	match s:
		SentinelChess.CheckMate:
			add_voice('CheckMate')
		SentinelChess.StaleMate:
			add_voice('StaleMate')
		SentinelChess.TimeUp:
			add_voice('TimesUp')
		SentinelChess.Forfeit:
			add_voice('Forfeit')
	if s > SentinelChess.StaleMate:
		add_voice('Draw')
	if w != SentinelChess.ChessColor.cNone:
		var color : String = 'White'
		var winstr = 'White Wins!'
		var voicestr = 'WhiteWins'
		if w == SentinelChess.ChessColor.Black:
			color = 'Black'
			winstr = 'Black Wins!'
			voicestr = 'BlackWins'
		append_history(winstr, color)
		add_voice(voicestr)
	print('UI: Finish Game')
	
func add_voice(s : String):
	if do_voice:
		if !s in voice_queue:
			voice_queue.push_back(s)

func has_voice() -> bool:
	return !voice_queue.is_empty()
	
func pop_voice() -> String:
	if voice_queue.is_empty():
		return ''
	return voice_queue.pop_front()

func play_voice():
	if do_voice:
		if !voice.playing:
			if has_voice():
				var s = pop_voice()
				var r = 'res://Sound/Voice/' + s + '.wav'
				voice.stream = load(r)
				voice.play()
				
func play_move_sfx():
	if do_sfx:
		voice.stream = MoveSound
		voice.play()
	
func play_promote_sfx():
	if do_sfx:
		voice.stream = PromoteSound
		voice.play()

