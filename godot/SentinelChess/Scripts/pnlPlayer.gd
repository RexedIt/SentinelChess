extends Panel

@onready var skin : Node = get_node('/root/MainGame/Skin')
@onready var sprType : Sprite2D = get_node('sprType')
@onready var lblName : Label = get_node('lblName')
@onready var lblSkill : Label = get_node('lblSkill')

# Called when the node enters the scene tree for the first time.
func _ready():
	pass
		
func applyskin():
	# applied heirarchically from game manager
	sprType.texture = skin.sprite('SmallPieces.png')
	lblName.set_theme(skin.theme)
	lblSkill.set_theme(skin.theme)

func refreshplayer(col : SentinelChess.ChessColor, player : ChessPlayer, puzzle : bool):
	if player:
		if col == SentinelChess.ChessColor.White:
			sprType.region_rect = Rect2(200, 0, 40, 40)
		else:
			sprType.region_rect = Rect2(200, 40, 40, 40)
		if player.PlayerType == ChessPlayer.ChessPlayerType.Puzzle:
			sprType.region_rect = Rect2(240, 0, 40, 40)
		var crgb : Color = Color(1,1,1)
		if col == SentinelChess.ChessColor.Black:
			crgb = Color(0,0,0)
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
