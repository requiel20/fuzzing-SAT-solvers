import os


def save_text(text_to_save, path):
    with open(path, 'w') as f:
        os.makedirs(path)
        f.write(text_to_save)
        f.flush()
