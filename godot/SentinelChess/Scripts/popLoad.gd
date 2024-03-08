extends FileDialog

@export var cancelled : bool = false
@export var loadfile : String

# Called when the node enters the scene tree for the first time.
func _ready():
	visibility_changed.connect(_VisibilityChanged)
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

# outbound signal
signal on_closed(_cancelled, _loadfile)

func _VisibilityChanged():
	if (visible):
		cancelled = false
	else:
		loadfile = current_path
		on_closed.emit(cancelled, loadfile)
	
func _OnCancel():
	cancelled = true
	visible = false

func _OnOK():
	cancelled = false
	visible = false


func _on_canceled():
	cancelled = true


func _on_confirmed():
	cancelled = false
