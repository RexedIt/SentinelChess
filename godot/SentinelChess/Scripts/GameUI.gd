extends CanvasLayer

@onready var skin : Node = get_node('/root/MainGame/Skin')
@onready var game_manager : SentinelChess = get_parent().get_node('SentinelChess')

@onready var pnlScore : Panel = get_node('pnlScore')
@onready var lblTitle : Label = get_node('lblTitle')
@onready var lblEco : Label = get_node('lblEco')
@onready var pnlPlayerTop : Panel = get_node('pnlPlayerTop')
@onready var pnlPlayerBottom : Panel = get_node('pnlPlayerBottom')
@onready var pnlCaptured : Panel = get_node('pnlCaptured')
@onready var lblHistory : RichTextLabel = get_node('lblHistory')
@onready var txtCmd : LineEdit = get_node('txtCmd')
@onready var lblError : Label = get_node('lblError')
@onready var lblCmd : Label = get_node('lblCmd')

@onready var btnNew : Button = get_node('btnNew')
@onready var btnNewPuzzle : Button = get_node('btnNewPuzzle')
@onready var btnOpen : Button = get_node('btnOpen')
@onready var btnSave : Button = get_node('btnSave')
@onready var btnStart : Button = get_node('btnStart')
@onready var btnRewind : Button = get_node('btnRewind')
@onready var btnPause : Button = get_node('btnPause')
@onready var btnAdvance : Button = get_node('btnAdvance')
@onready var btnEnd : Button = get_node('btnEnd')
@onready var btnForfeit : Button = get_node('btnForfeit')
@onready var btnHint : Button = get_node('btnHint')
@onready var btnChat : Button = get_node('btnChat')
@onready var btnSettings : Button = get_node('btnSettings')
@onready var lblWhiteClock : Label = get_node('lblWhiteClock')
@onready var lblBlackClock : Label = get_node('lblBlackClock')
@onready var voice : AudioStreamPlayer = get_node('voice')
@onready var sfx : AudioStreamPlayer = get_node('sfx')
@onready var MoveSound
@onready var PromoteSound
@export var step : int

const GameState = preload("res://Scripts/GameState.gd").GameState_

var PlayTexture : Texture2D;
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
var loaded: bool = false
var skinned: bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	# do some skinning
	loaded = true	
	if not skinned:
		applyskin()
	txtCmd.grab_focus()
		
func applyskin():
	# applied heirarchically from game manager
	if not loaded:
		return
	btnNew.icon = skin.sprite('New.png')
	btnNewPuzzle.icon = skin.sprite('NewPuzzle.png')
	btnOpen.icon = skin.sprite('Open.png')
	btnSave.icon = skin.sprite('Save.png')
	btnStart.icon = skin.sprite('Start.png')
	btnRewind.icon = skin.sprite('Rewind.png')
	btnPause.icon = skin.sprite('Pause.png')
	btnAdvance.icon = skin.sprite('Advance.png')
	btnEnd.icon = skin.sprite('End.png')
	btnForfeit.icon = skin.sprite('Forfeit.png')
	btnHint.icon = skin.sprite('Help.png')
	btnChat.icon = skin.sprite('Chat.png')
	btnSettings.icon = skin.sprite('Gear.png')
	PauseTexture = btnPause.icon
	PlayTexture = skin.sprite('Play.png')
	MoveSound = skin.commonsound('Slide.mp3')
	PromoteSound = skin.sound('Promote.mp3')
	lblWhiteClock.set_theme(skin.theme)
	lblBlackClock.set_theme(skin.theme)
	lblTitle.set_theme(skin.theme)
	lblEco.set_theme(skin.theme)
	lblHistory.set_theme(skin.theme)
	lblCmd.set_theme(skin.theme)
	txtCmd.set_theme(skin.theme)
	pnlScore.applyskin()
	pnlPlayerTop.applyskin()
	pnlPlayerBottom.applyskin()
	pnlCaptured.applyskin()
	voice.volume_db = skin.voice_db()
	sfx.volume_db = skin.sfx_db()	
	skinned = true
	
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
	if is_idle == false:
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
	setEcoValues(meta.eco(), meta.open_title())
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
	enableplaybuttons()
	lblWhiteClock.visible = false
	lblBlackClock.visible = false
	clear_history()
	append_history(msg)
	#update_players(tc)
	announceTurn(tc)
	
func setEcoValues(eco : String, open_title : String):
	var s : String = eco
	if open_title != '':
		if s != '':
			s = s + ' - '
		s = s + open_title
	lblEco.text = s
	
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
	
func movestr(n : int, m : ChessMove) -> String:
	return game_manager.movestr(n, m);
	
func append_history(msg : String, color : String = 'blue'):
	lblHistory.push_color(color)
	lblHistory.add_text(msg)
	lblHistory.newline()
	lblHistory.pop()

func stripcurlies(cmt : String):
	if cmt.begins_with('{') and cmt.ends_with('}'):
		return cmt.substr(1, cmt.length()-2)
	return cmt
	
func append_history2(msg : String, color : String, cmt : String, cmtcol : String = 'dark_slate_gray'):
	lblHistory.push_color(color)
	lblHistory.add_text(msg)
	if cmt != '':
		lblHistory.push_color(cmtcol)
		lblHistory.add_text(' ' + stripcurlies(cmt))
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
	
func append_move(n : int, m : ChessMove, b : ChessBoard, col : SentinelChess.ChessColor, cmt : String):
	var color : String = 'white'
	if col == SentinelChess.ChessColor.Black:
		color = 'black'
	play_move_sfx()
	append_history2(str(n) + ' ' + movestr(n, m), color, cmt)
	if (b.check_state(SentinelChess.Black)):
		append_history('Black in Check.', 'black')	
		add_voice('Check')
	if (b.check_state(SentinelChess.White)):
		append_history('White in Check.', 'white')
		add_voice('Check')
	setEcoValues(game_manager.eco(), game_manager.open_title())
	
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

func _on_btn_start_pressed():
	handle_start()

func _on_btn_end_pressed():
	handle_end()

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

func _on_btn_settings_pressed():
	game_manager._on_settings()

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
	enableplaybuttons()
	return true

func handle_start() -> bool:
	if puzzle:
		show_error('In Puzzle')
		true
	var err : int = game_manager.goto_turn(0)
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	enableplaybuttons()
	return true

func handle_end() -> bool:
	if puzzle:
		show_error('In Puzzle')
		true
	var err : int = game_manager.goto_turn(game_manager.playmax())
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	append_history('End')
	enableplaybuttons()
	return true
		
func enableplaybuttons():
	if btnRewind:
		var no_game = game_manager == null
		var atbeg = false
		var atend = false
		var is_play = true
		if !no_game:
			no_game = game_manager.gamestate < GameState.PLAY
			is_play = game_manager.state() == SentinelChess.Play
			atbeg = game_manager.playno()==0
			atend = game_manager.playno() == game_manager.playmax()
		var disp : bool = no_game or puzzle
		btnStart.disabled = is_play or disp or atbeg
		btnRewind.disabled = is_play or disp or atbeg
		btnPause.disabled = disp
		btnAdvance.disabled = is_play or disp or atend
		btnEnd.disabled = is_play or disp or atend
	
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
	enableplaybuttons()
	#game_manager.refresh_turn(game_manager.get_board())
	return true

func refreshhints():
	hints = game_manager.hints()
	points = game_manager.win_points(SentinelChess.cNone)
	btnHint.disabled =(not puzzle) or hints <= 0
	pnlScore.setPuzzleValues(points,hints,false)

func handle_hint() -> bool:
	var h : String = game_manager.hintstr()
	append_history('*** HINT ***', 'darkgreen')
	append_history(h, 'darkgreen')
	refreshhints()
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
		'C':
			var s = ''
			var l : int = cmd.find(' ')
			if l>=0:
				s = cmd.substr(l+1).strip_edges(true,true)
			game_manager.comment(game_manager.playno()+1, s)
			handled=true
		'[':
			handled = handle_start()
		'<':
			handled = handle_rewind()
		'>':
			handled = handle_advance()
		']':
			handled = handle_end()
		'P':
			handled = handle_play()
		'I':
			handled = handle_pause()
		'H':
			if hints <=0:
				show_error('No Hint Available')
			else:
				handled = handle_hint()
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
	
func incorrectmove():
	refreshhints()
	if hints > 0:
		add_voice('IncorrectMoveHintUsed')
	else:
		add_voice('IncorrectMove')
	
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
	lblWhiteClock.visible = wt > 0
	lblBlackClock.visible = bt > 0
	lblWhiteClock.text = time_str(wt)
	lblBlackClock.text = time_str(bt)
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
		btnHint.disabled = no_game or (not puzzle) or hints <= 0
		btnSave.disabled = no_game
		btnForfeit.disabled = no_game
		btnChat.disabled = no_game
		enableplaybuttons()
		do_voice = game_manager.has_local()
		do_sfx = do_voice
		
func finish_game(s : SentinelChess.ChessGameState, w : SentinelChess.ChessColor):
	append_history(game_manager.gamestatestr(s))
	countdown = -1.0
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
				voice.stream = skin.voice(s + '.mp3')
				if game_manager.gamestate != GameState.IDLE:
					voice.play()
				
func play_move_sfx():
	if do_sfx and game_manager.gamestate != GameState.IDLE:
		sfx.stream = MoveSound
		sfx.play()
	
func play_promote_sfx():
	if do_sfx and game_manager.gamestate != GameState.IDLE:
		sfx.stream = PromoteSound
		sfx.play()

