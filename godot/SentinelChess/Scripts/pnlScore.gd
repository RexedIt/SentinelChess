extends Panel

@onready var lblPoints1 : Label = get_node('lblPoints1')
@onready var lblPoints2 : Label = get_node('lblPoints2')
@onready var sprPuzzle : Sprite2D = get_node('sprPuzzle')

var _maxhints : int = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func setPuzzleValues(points : int, hints : int, first : bool):
	if first:
		_maxhints = hints
	sprPuzzle.visible = true
	lblPoints1.text = str(points)
	lblPoints2.visible = _maxhints > 0
	lblPoints2.text = '0 / ' + str(_maxhints)

func setScoreValues(white : int, black : int):
	sprPuzzle.visible = false
	lblPoints1.text = str(white)
	lblPoints2.text = str(black)
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
