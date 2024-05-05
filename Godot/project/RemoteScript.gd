extends Control

@onready var label = $Label
@onready var label2 = $Name

var value = 0

func updateValue(newValue, senderID = 1):
	if !(is_multiplayer_authority() or senderID == 0):
		updateValueRPC.rpc_id(1, newValue, multiplayer.get_unique_id())
		return
	value = newValue
	label.text = "Value: " + String.num(value)
	
	if is_multiplayer_authority() and senderID != 0:
		print(senderID, " has set the value to: ", value)
		updateValueRPC.rpc(newValue, 0)

@rpc("any_peer")
func updateValueRPC(newValue, senderID):
	if is_multiplayer_authority() or senderID == 0:
		updateValue(newValue, senderID)

func _on_inc_pressed():
	updateValue(value + 1)
func _on_dec_pressed():
	updateValue(value - 1)
	
func _ready():
	label2.text = "Player " + String.num(multiplayer.get_unique_id())
