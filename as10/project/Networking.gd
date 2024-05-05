extends Control

@onready var address_entry = $PanelContainer/VBoxContainer/IPAddress

const PORT = 9999
var peer = ENetMultiplayerPeer.new()
	
func _on_host_button_pressed():
	peer.create_server(PORT)
	multiplayer.peer_connected.connect(addPlayer)
	multiplayer.peer_disconnected.connect(removePlayer)
	multiplayer.multiplayer_peer = peer
	get_tree().change_scene_to_file("res://Player.tscn")
	
func _on_join_button_pressed():
	peer.create_client(address_entry.text, PORT)
	multiplayer.multiplayer_peer = peer
	get_tree().change_scene_to_file("res://Player.tscn")

func addPlayer(peer_id):
	pass

func removePlayer(peer_id):
	pass
