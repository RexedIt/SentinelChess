extends Panel

@onready var skin : Node = get_node('/root/MainGame/Skin')
@onready var sprType : Sprite2D = get_node('sprType')
@onready var lblName : Label = get_node('lblName')
@onready var lblSkill : Label = get_node('lblSkill')
@onready var prgStatus : ProgressBar = get_node('prgStatus')

var mycolor : SentinelChess.ChessColor

# Called when the node enters the scene tree for the first time.
func _ready():
	pass
		
func applyskin():
	# applied heirarchically from game manager
	sprType.texture = skin.sprite('SmallPieces.png')
	lblName.set_theme(skin.theme)
	lblSkill.set_theme(skin.theme)

func thinking(col : SentinelChess.ChessColor, pct : int):
	if pct < 0 or pct > 100 or col != mycolor:
		if prgStatus.visible:
			prgStatus.visible = false
	else:
		if prgStatus.visible == false:
			prgStatus.visible = true
		prgStatus.value = pct
		
func refreshplayer(col : SentinelChess.ChessColor, player : ChessPlayer, puzzle : bool):
	prgStatus.visible = false
	mycolor = col
	if player:
		if col == SentinelChess.ChessColor.White:
			sprType.region_rect = Rect2(200, 0, 40, 40)
		else:
			sprType.region_rect = Rect2(200, 40, 40, 40)
		if player.PlayerType == ChessPlayer.ChessPlayerType.Puzzle:
			sprType.region_rect = Rect2(240, 0, 40, 40)
		var crgb : Color = Color(1,1,1)
		var prgm : Color = Color(0,0,0,.75)
		if col == SentinelChess.ChessColor.Black:
			crgb = Color(0,0,0)
			prgm = Color(1,1,1,.75)
		prgStatus.modulate = prgm
		lblName.set('theme_override_colors/font_color', crgb)	
		lblSkill.set('theme_override_colors/font_color', crgb)	
		lblName.text = player.Name
		if puzzle:
			lblSkill.text = str(player.puzzlepoints())
		else:
			lblSkill.text = str(player.Skill)
	else:
		sprType.region_rect = Rect2(240, 40, 40, 40)
		lblName.text = ''
		lblSkill.text = ''
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
