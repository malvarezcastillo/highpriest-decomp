#!/usr/bin/env python3
"""Build data_en.grf, the English game data.

    data.py DATA_HP.GRF DATA_EN.GRF

The translated client loads data_en.grf, when it is in the game folder, on top
of data_hp.grf, so a file in it replaces the one of the same name in the game's
own archive. Nothing here is stored in the repository: every file is made from
the original in DATA_HP.GRF and the translations in this folder."""
import sys
import grf, images, tables

def main(src, dst):
    g = grf.Grf(src)
    files = {}
    files.update(images.build(g))
    files.update(tables.build(g))
    grf.pack(dst, files)
    print('%d files in %s' % (len(files), dst))

if __name__ == '__main__':
    if len(sys.argv) != 3: sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
