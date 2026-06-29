from serialize_midi import serialize_midi
from usb_setup import ep_out

songs = {
    "1": "happy_birthday",
    "2": "hot_cross_buns",
    "3": "mission_impossible",
    "4": "never_gonna_give_you_up",
    "5": "once_upon_a_time",
    "6": "tetris",
}

while True:
    choice = input(
        f"Choose a song:\n  {'\n  '.join(f'{k}. {v}' for k, v in songs.items())}\n> "
    ).strip()

    if choice not in songs:
        print("Invalid choice, try again.")
        continue

    ep_out.write(serialize_midi(f"songs/{songs[choice]}.mid"))
