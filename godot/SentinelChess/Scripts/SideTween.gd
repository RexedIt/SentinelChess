extends Sprite2D

@export var startpos : Vector2
@export var start : float
@export var end : float
@export var step : int

var initpos : Vector2
var elapsed : float

func BezierBlend(t):
	return t * t * (3.0 - 2.0 * t)

# Called when the node enters the scene tree for the first time.
func _ready():
	initpos = position
	if startpos != position:
		position = startpos

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	elapsed += delta
	if (end-start>0) and (elapsed >= start) && (elapsed <= end):
		var frac = BezierBlend((elapsed - start) / (end-start))
		var smoothvec = startpos + (initpos - startpos) * frac
		if (step):
			smoothvec.x = int(smoothvec.x) / step * step
			smoothvec.y = int(smoothvec.y) / step * step
		position = smoothvec
