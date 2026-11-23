#!/usr/bin/env python3
"""Build data_en.grf, the English game data.

    data.py DATA_HP.GRF DATA_EN.GRF [SKINDIR]

The translated client loads data_en.grf, when it is in the game folder, on top
of data_hp.grf, so a file in it replaces the one of the same name in the game's
own archive. Nothing here is stored in the repository: every file is made from
the original in DATA_HP.GRF and the translations in this folder.

The installer also puts copies of many interface pictures in skin/default,
which the option window offers as a skin. Given that folder as SKINDIR, the
copies of redrawn pictures are replaced there too."""
import os, sys
import grf, images, tables

def skin(g, pictures, folder):
    have = {}
    for d, _, fs in os.walk(folder):
        for f in fs:
            have[os.path.relpath(os.path.join(d, f), folder).replace(os.sep, '\\').lower()] = os.path.join(d, f)
    root = (images.SPEC['buttons'][0]['dir'] + '/').replace('/', '\\').encode('cp949')
    n = 0
    for name, data in pictures.items():
        path = have.get(name[len(root):].decode('cp949').lower())
        if path and open(path, 'rb').read() == g.read(g.find(name.decode('cp949'))):
            open(path, 'wb').write(data); n += 1
    print('%d pictures replaced in %s' % (n, folder))

def main(src, dst, skindir=None):
    g = grf.Grf(src)
    pictures = images.build(g)
    files = dict(pictures)
    files.update(tables.build(g))
    grf.pack(dst, files)
    print('%d files in %s' % (len(files), dst))
    if skindir: skin(g, pictures, skindir)

if __name__ == '__main__':
    if len(sys.argv) not in (3, 4): sys.exit(__doc__)
    main(*sys.argv[1:])
