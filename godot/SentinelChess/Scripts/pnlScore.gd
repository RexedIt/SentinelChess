extends Panel

@onready var skin : Node = get_node('/root/MainGame/Skin')
@onready var lblPoints1 : Label = get_node('lblPoints1')
@onready var lblPoints2 : Label = get_node('lblPoints2')
@onready var sprPuzzle : Sprite2D = get_node('sprPuzzle')

var _maxhints : int = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	pass
		
func applyskin():
	# applied heirarchically from game manager
	sprPuzzle.texture = skin.sprite('PuzzlePiece.png')
	lblPoints1.set_theme(skin.theme)
	lblPoints2.set_theme(skin.theme)
	
func setPuzzleValues(points : String, hints : int, first : bool):
	if first:
		_maxhints = hints
	sprPuzzle.visible = true
	lblPoints1.text = points
	lblPoints2.visible = _maxhints > 0
	lblPoints2.text = str(hints) + ' Hints Remain' # str(_maxhints-hints) + '/' + str(_maxhints) + ' Hints'

func setScoreValues(white : String, black : String):
	sprPuzzle.visible = false
	lblPoints1.text = white
	lblPoints2.text = black
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
