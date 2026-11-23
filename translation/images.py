#!/usr/bin/env python3
"""English versions of the interface pictures that carry Korean text.

    images.py GRF OUTDIR     redraw the pictures listed in images.json and
                             write them under OUTDIR as loose files

Every picture is rebuilt from the original inside the game archive: the
Korean label is wiped and the English one drawn in the same place and style.
The translated client reads loose files under data/ before the archive, so
OUTDIR is the game folder, or a folder whose contents are copied into it."""
import io, json, os, sys
from PIL import Image
from grf import Grf

HERE = os.path.dirname(os.path.abspath(__file__))
FONTS = json.load(open(os.path.join(HERE, 'font.json')))
SPEC = json.load(open(os.path.join(HERE, 'images.json'), encoding='utf-8'))

def lum(c):
    return c[0] * 299 + c[1] * 587 + c[2] * 114

def pixels(font, text):
    """Set pixels of text as (x, y), y counted from the top of the line; and the line's width."""
    f = FONTS[font]; pts = []; x = 0
    for ch in text:
        rows = f['glyphs'][ch].split('/')
        for y, row in enumerate(rows):
            pts += [(x + i, y) for i, c in enumerate(row) if c == '#']
        x += len(rows[0]) + f['gap']
    return pts, x

def placed(font, text, at=None, center=None, rot=None):
    """Pixels of text at a position: 'at' is the top left of the line, 'center' the middle of its ink."""
    pts, _ = pixels(font, text)
    if rot == 'ccw':                       # reads upwards
        pts = [(y, -x) for x, y in pts]
    x0 = min(p[0] for p in pts); x1 = max(p[0] for p in pts)
    y0 = min(p[1] for p in pts); y1 = max(p[1] for p in pts)
    if center:
        dx = center[0] - (x0 + x1 + 1) // 2; dy = center[1] - (y0 + y1 + 1) // 2
    elif rot:
        dx = at[0] - x0; dy = at[1] - y0
    else:
        dx, dy = at
    return {(x + dx, y + dy) for x, y in pts}

def paint(im, pts, colour=None, factor=None):
    px = im.load(); w, h = im.size
    for x, y in pts:
        if 0 <= x < w and 0 <= y < h:
            px[x, y] = colour if colour else tuple(int(c * factor) for c in px[x, y])

def label(im, font, text, state, top):
    """Centred label in the style of the original buttons."""
    w, h = im.size
    pts, _ = pixels(font, text)
    x0 = min(p[0] for p in pts); x1 = max(p[0] for p in pts)
    dx = (w - (x1 - x0 + 1)) // 2 - x0
    if x1 - x0 + 1 > w - 6:
        print('  warning: "%s" is %d px wide in a %d px picture' % (text, x1 - x0 + 1, w))
    pts = {(x + dx, y + top) for x, y in pts}
    down = {(x + 1, y + 1) for x, y in pts}
    if state == 'pressed':
        paint(im, down, (49, 49, 49))
    elif state == 'disabled':
        halo = {(x + i, y + j) for x, y in pts for i in (-1, 0, 1) for j in (-1, 0, 1)}
        paint(im, halo - pts, (255, 255, 255)); paint(im, pts, (110, 110, 110))
    else:
        paint(im, down - pts, factor=0.88)
        paint(im, pts, (54, 54, 54) if state == 'normal' else (49, 49, 49))

def median(images):
    """What same-sized pictures share: per pixel, a value on the light side of the middle, so
    that the dark labels, which differ from picture to picture, drop out."""
    w, h = images[0].size; out = Image.new('RGB', (w, h)); px = out.load()
    srcs = [im.load() for im in images]
    for y in range(h):
        for x in range(w):
            vals = sorted((s[x, y] for s in srcs), key=lum)
            px[x, y] = vals[len(vals) * 7 // 10]
    # where most labels put ink on the same pixel a speck survives: smooth each row over it
    src = out.copy().load()
    for y in range(h):
        for x in range(CAP, w - CAP):
            near = sorted((src[i, y] for i in range(max(CAP, x - 6), min(w - CAP, x + 7))), key=lum)
            px[x, y] = near[len(near) // 2]
    return out

def rowfill(im, x0, x1, src, y0=0, y1=None, src2=None):
    """Fill columns x0..x1-1 of each row with its colour at column src, or with a blend from
    column src to column src2."""
    px = im.load()
    for y in range(y0, im.size[1] if y1 is None else y1):
        a = px[src, y]; b = px[src2, y] if src2 is not None else a
        for x in range(x0, x1):
            t = (x - x0) / max(1, x1 - 1 - x0)
            px[x, y] = tuple(int(round(a[i] + (b[i] - a[i]) * t)) for i in range(3))

BUTTON_STATES = {'': 'normal', '_a': 'hover', '_b': 'pressed', '_dis': 'disabled'}
CAP = 6                                    # columns at each end that hold the rounded border

def buttons(g, save):
    found = []                             # (folder, name, suffix, picture)
    for grp in SPEC['buttons']:
        for name in grp['labels']:
            for suffix in BUTTON_STATES:
                e = g.find('%s/%s%s.bmp' % (grp['dir'], name, suffix))
                if e: found.append((grp, name, suffix, Image.open(io.BytesIO(g.read(e))).convert('RGB')))
    groups = {}
    for grp, name, suffix, im in found:
        groups.setdefault((grp['dir'], suffix, im.size), []).append(im)
    templates = {k: median(v) for k, v in groups.items() if len(v) >= 8}
    main = SPEC['buttons'][0]['dir']
    for grp, name, suffix, im in found:
        w, h = im.size; state = BUTTON_STATES[suffix]
        tpl = templates.get((grp['dir'], suffix, im.size))
        if tpl is None:                    # too few of this size to compare: stretch the common one
            tpl = next((t for (d, s, size), t in sorted(templates.items(), key=lambda kv: kv[0][0] != main)
                        if s == suffix and size[1] == h), None)
        if tpl is None:
            rowfill(im, CAP, w - CAP, CAP - 1)
        else:                              # both ends as they are, the middle stretched to the width
            tw = tpl.size[0]
            im.paste(tpl.crop((0, 0, CAP, h)), (0, 0))
            im.paste(tpl.crop((tw - CAP, 0, tw, h)), (w - CAP, 0))
            im.paste(tpl.crop((CAP, 0, tw - CAP, h)).resize((w - 2 * CAP, h), Image.BILINEAR), (CAP, 0))
        label(im, grp['font'], grp['labels'][name], state, grp['top'])
        save('%s/%s%s.bmp' % (grp['dir'], name, suffix), im)

def bars(g, save):
    for grp in SPEC['bars']:
        for name, text in grp['labels'].items():
            for suffix, state in grp['states'].items():
                e = g.find('%s/%s%s.bmp' % (grp['dir'], name, suffix))
                if not e: continue
                im = Image.open(io.BytesIO(g.read(e))).convert('RGB')
                rowfill(im, grp['margin'], im.size[0] - grp['margin'], grp['margin'] - 1, src2=im.size[0] - grp['margin'])
                label(im, grp['font'], text, state, grp['top'])
                save('%s/%s%s.bmp' % (grp['dir'], name, suffix), im)

def patches(g, save):
    for p in SPEC['patches']:
        e = g.find(p['file'])
        if not e:
            print('  not in the archive:', p['file']); continue
        im = Image.open(io.BytesIO(g.read(e))).convert('RGB')
        for op in p['ops']:
            if 'fill' in op:
                x0, y0, x1, y1 = op['fill']
                if 'col' in op: rowfill(im, x0, x1, op['col'], y0, y1)
                else: im.paste(tuple(op['colour']), (x0, y0, x1, y1))
            else:
                pts = placed(op['font'], op['text'], op.get('at'), op.get('center'), op.get('rot'))
                if 'shadow' in op:
                    paint(im, {(x + 1, y + 1) for x, y in pts} - pts, tuple(op['shadow']))
                paint(im, pts, tuple(op['colour']))
        save(p['file'], im)

def main(grf, out):
    g = Grf(grf); n = [0]
    def save(name, im):
        dst = os.path.join(out, *name.split('/'))
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        im.save(dst, 'BMP'); n[0] += 1
    buttons(g, save); bars(g, save); patches(g, save)
    print(n[0], 'pictures written under', out)

if __name__ == '__main__':
    if len(sys.argv) != 3: sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
