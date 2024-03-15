extends Panel

@onready var lblPoints : Label = get_node('lblPoints')
@onready var lblHints : Label = get_node('lblHints')

var _maxhints : int = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func setvalues(points : int, hints : int, first : bool):
	if first:
		_maxhints = hints
	lblPoints.text = str(points)
	lblHints.visible = _maxhints > 0
	lblHints.text = '0 / ' + str(_maxhints)
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
