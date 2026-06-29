import mido


def midi_note_to_pitch_octave(note: int) -> tuple[int, int]:
    """
    Converts a MIDI note number to a pitch and octave.
    """
    return note % 12, note // 12 - 1


def serialize_event(
    pitch: int, octave: int, velocity: int, duration: int
) -> tuple[int, int, int, int, int]:
    """
    Serializes a MIDI event into a format that is sent over USB to the Syntharoni.
    """
    return (
        pitch & 0xFF,
        octave & 0xFF,
        velocity & 0xFF,
        (duration >> 8) & 0xFF,
        duration & 0xFF,
    )


def serialize_midi(file: str) -> list[int]:
    """
    Serializes a MIDI file into a format that is sent over USB to the Syntharoni.
    """
    mid = mido.MidiFile(file)

    tempo = 1e6 * 60 / 120  # 120 BPM in microseconds per beat

    events: list[tuple[int, int, int, int]] = (
        []
    )  # list of (pitch, octave, velocity, duration) tuples
    active = None
    active_ms = 0
    rest_ms = 0

    def add_event(note: int, velocity: int, duration_ms: int, rest: bool):
        if duration_ms > 0:
            if rest:
                events.append((12, 0, velocity, duration_ms))
            else:
                events.append(
                    midi_note_to_pitch_octave(note)
                    + (
                        velocity,
                        duration_ms,
                    )
                )

    for msg in mido.merge_tracks(mid.tracks):
        dt_ms = round(mido.tick2second(msg.time, mid.ticks_per_beat, tempo) * 1000)

        if active is None:
            rest_ms += dt_ms
        else:
            active_ms += dt_ms

        if msg.type == "note_on" and msg.velocity > 0:
            # End active note
            if active is not None:
                add_event(active.note, active.velocity, active_ms, rest=False)
                active = None
                active_ms = 0

            # Add accumulated rest
            if rest_ms > 0:
                add_event(12, 0, rest_ms, rest=True)
                rest_ms = 0

            # Start new active note
            active = msg

        elif msg.type == "note_off" or (msg.type == "note_on" and msg.velocity == 0):
            # End active note
            if active is not None and active.note == msg.note:
                add_event(active.note, active.velocity, active_ms, rest=False)
                active = None
                active_ms = 0

            add_event(12, 0, 10, rest=True)  # Add short rest after note off

    # Add active note
    if active is not None:
        add_event(active.note, active.velocity, active_ms, rest=False)

    return [
        len(events),
        *(value for event in events for value in serialize_event(*event)),
    ]
