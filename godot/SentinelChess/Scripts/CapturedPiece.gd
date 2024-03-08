extends Sprite2D

var abbr : String = ''


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
func setpiece(p : String):
	abbr = p
	if abbr == '':
		visible = false
		return
	else:
		visible = true
		match p:
			'P':
				region_rect = Rect2(0, 0, 40, 40)
			'N':
				region_rect = Rect2(40, 0, 40, 40)
			'B':
				region_rect = Rect2(80, 0, 40, 40)
			'R':
				region_rect = Rect2(120, 0, 40, 40)
			'Q':
				region_rect = Rect2(160, 0, 40, 40)
			'p':
				region_rect = Rect2(0, 40, 40, 40)
			'n':
				region_rect = Rect2(40, 40, 40, 40)
			'b':
				region_rect = Rect2(80, 40, 40, 40)
			'r':
				region_rect = Rect2(120, 40, 40, 40)
			'q':
				region_rect = Rect2(160, 40, 40, 40)
				
				
