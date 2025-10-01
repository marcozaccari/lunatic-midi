import rtmidi
from rtmidi import MidiOut
from time import sleep

# Trova le porte di output disponibili
midi_out = MidiOut()

available_ports = midi_out.get_ports()

print(f"{available_ports}")

if available_ports:
    # Se trovi una porta, aprila
    midi_out.open_port(0)  # Sostituisci 0 con l'indice della tua porta MIDI
    print(f"Connesso a: {available_ports[0]}")

    # Invia un messaggio note_on (nota 60, velocità 64)
    msg_on = [0x90, 60, 64]  # 0x90 = 'note_on' per il canale 1, nota 60 (C4), velocità 64
    midi_out.send_message(msg_on)
    print(f"Sent MIDI Note On: {msg_on}")

    # Invia un messaggio note_off dopo 1 secondo
    sleep(1)
    msg_off = [0x80, 60, 64]  # 0x80 = 'note_off' per il canale 1, stessa nota
    midi_out.send_message(msg_off)
    print(f"Sent MIDI Note Off: {msg_off}")

    midi_out.close_port()
else:
    print("Nessuna porta MIDI disponibile")
    
