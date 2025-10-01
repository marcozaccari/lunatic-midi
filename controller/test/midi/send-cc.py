import mido
from mido import Message

print("Porte MIDI disponibili:")
print(mido.get_output_names())

#midi_port_name = "VirMIDI 1-0"
#midi_port_name = "Virtual Raw MIDI 1-0:VirMIDI 1-0 20:0"
#midi_port_name = "Midi Through:Midi Through Port-0 14:0"
midi_port_name = "Midi Through Port-0"

midi_out = mido.open_output(midi_port_name)

msg = Message('control_change', control=1, value=60)
midi_out.send(msg)

print(f"Sent MIDI CC: {msg}")

msg2 = Message('note_on', note=60, velocity=64)
midi_out.send(msg2)
print(f"Sent MIDI note: {msg2}")
