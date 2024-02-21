extends Sprite2D

@export var endalpha : float
@export var start : float
@export var end : float
@export var step : int

var startalpha : float
var elapsed : float

# Called when the node enters the scene tree for the first time.
func _ready():
	startalpha = modulate.a
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	elapsed += delta
	if (end-start>0) and (elapsed >= start) && (elapsed <= end):
		var frac = (elapsed - start) / (end-start)
		var smooth = startalpha + (endalpha - startalpha) * frac + .02
		if (step):
			var pct = int(smooth * 100) / step * step
			smooth = float(pct)/100.0
		modulate.a = smooth
