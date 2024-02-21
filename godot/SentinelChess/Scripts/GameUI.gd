extends CanvasLayer

@onready var game_manager : SentinelChess = get_parent().get_node('SentinelChess')
@onready var lblHistory : RichTextLabel = get_node('lblHistory')
@onready var txtCmd : LineEdit = get_node('txtCmd')
@onready var lblError : Label = get_node('lblError')
@onready var lblCmd : Label = get_node('lblCmd')
@export var step : int

# Called when the node enters the scene tree for the first time.
func _ready():
	txtCmd.grab_focus()

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
	if m.promote() != 0:
		str += ' PROM'
	return str
	
func append_history(msg : String, color : String = 'blue'):
	lblHistory.push_color(color)
	lblHistory.add_text(msg.to_upper())
	lblHistory.newline()
	lblHistory.pop()

func refreshPrompt(col : SentinelChess.ChessColor):
	var crgb : Color = Color(1,1,1)
	if col == SentinelChess.ChessColor.Black:
		crgb = Color(0,0,0)
	lblCmd.set('theme_override_colors/font_color', crgb)	
	
func append_load(msg: String):
	append_history('Load Game - ' + msg)
	# for last move info
	if game_manager.turnno()>1:
		append_move(game_manager.lastturnno(),game_manager.lastmove(), game_manager.lastcolor())
		
func append_move(n : int, m : ChessMove, col : SentinelChess.ChessColor):
	var color : String = 'white'
	if col == SentinelChess.ChessColor.Black:
		color = 'black'
	append_history(str(n) + ' ' + color + ' ' + movestr(m), color)
	if game_manager.has_local():
		if (game_manager.check_state(SentinelChess.Black)):
			append_history('Black in Check.', 'black')	
		if (game_manager.check_state(SentinelChess.White)):
			append_history('White in Check.', 'white')
		
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

func _on_btn_pause_pressed():
	pass # Replace with function body.

func _on_btn_rewind_pressed():
	var move_no : int = game_manager.turnno()-2
	if move_no<0:
		show_error('At Beginning.')
		return
	handle_rewind(move_no)

func handle_rewind(move_no: int) -> bool:
	var err : int = game_manager.rewind_game(move_no)
	if err != 0:
		show_error('!' + game_manager.errorstr(err))
		return false
	append_history('Rewind - ' + str(move_no))
	game_manager.refresh_turn(game_manager.get_board())
	return true
	
func _on_txt_cmd_text_submitted(new_text):
	if new_text == '':
		return
	var ucmd : String = new_text.to_upper()	
	var ucmd1 : String = ucmd.left(1)
	var m : ChessMove
	var handled : bool = false
	match ucmd1:
		'N':
			_on_btn_new_pressed()
			handled = true
		'M':
			handled = handle_move(ucmd.get_slice(' ',1))
		'L':
			handled = handle_load(new_text.get_slice(' ',1))
		'S':
			handled = handle_save(new_text.get_slice(' ',1))
		'<':
			var s : String = new_text.get_slice(' ',1)
			var move_no : int = int(s)
			if s == '':
				move_no = game_manager.turnno()-2
			handled=handle_rewind(move_no)
		# a move?
		_:
			handled = handle_move(ucmd)
	if handled:
		txtCmd.text = ''		
	else:
		show_error('Invalid Move or Command')
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
	clear_history()
	append_load(toload)
	game_manager.refresh_board()
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
	if cmd.get_slice_count('-')==2:
		var p0str: String = cmd.get_slice('-',0)
		var p1str: String = cmd.get_slice('-',1)
		if p0str.length()==2 and p1str.length()==2:
			var p0: ChessCoord = strcoord(p0str)
			var p1: ChessCoord = strcoord(p1str)
			if p0 != null and p1 != null:
				# time to check
				if possible_move(p0,p1):
					var m : ChessMove = ChessMove.new()
					var c : SentinelChess.ChessColor = game_manager.turn_color()
					m.p0=p0
					m.p1=p1
					var err : int = game_manager.move_m(c, m)
					if err != 0:
						show_error('!' + game_manager.errorstr(err))
						return false
					if game_manager.check_state(c):
						if game_manager.state() == SentinelChess.ChessGameState.Play:
							show_error("You are in Check.")
							return false
					append_move(game_manager.turnno(), m, c)
					game_manager._user_moved(m)
					return true				
	return false
