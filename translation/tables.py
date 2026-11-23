#!/usr/bin/env python3
"""English names and descriptions in the game's skill and item tables.

    tables.py GRF          check tables.json against the tables in the archive

tables.json holds the English text by skill constant and item number. Only
the entries listed there change; the rest of each table stays as it is.
data.py packs the patched tables into the archive the translated client loads."""
import json, os, re, sys
from grf import Grf

HERE = os.path.dirname(os.path.abspath(__file__))

# The item description tables are stored with the letters and one byte range mirrored, and the
# client mirrors each line back as it loads it. The mapping is its own inverse.
MIRROR = bytes(97 + 122 - c if 97 <= c <= 122 else 65 + 90 - c if 65 <= c <= 90 else
               0xb0 + 0xbf - c if 0xb0 <= c <= 0xbf else c for c in range(256))

#        part of tables.json      field         table                                  kind     mirrored
TABLES = [('skill_names', None,        'data/skillnametable.txt',             'names', False),
          ('skill_desc',  None,        'data/skilldesctable.txt',             'descs', False),
          ('items',       'name',      'data/idnum2itemdisplaynametable.txt', 'names', False),
          ('items',       'unid_name', 'data/num2itemdisplaynametable.txt',   'names', False),
          ('items',       'desc',      'data/idnum2itemdesctable.txt',        'descs', True),
          ('items',       'unid_desc', 'data/num2itemdesctable.txt',          'descs', True)]

def check(key, line):
    # a line of digits alone would be read as the next item's number
    if not line.isascii() or '#' in line or '//' in line or '\t' in line or line.strip().isdigit():
        raise SystemExit('%s: not usable in a table: %r' % (key, line))
    return line.encode('ascii')

def names(data, entries):
    """key#name# lines: put the new name in the second field."""
    out = []; seen = set()
    for line in data.splitlines(True):
        m = re.match(rb'([^#\r\n]+)#[^#\r\n]*#', line)
        key = m and m.group(1).decode('cp949', 'replace')
        if key in entries:
            line = m.group(1) + b'#' + check(key, entries[key]) + b'#' + line[m.end():]
            seen.add(key)
        out.append(line)
    return b''.join(out), seen

def descs(data, entries, mirrored):
    """key# on a line, the description lines, # on a line: put the new lines in between."""
    out = []; seen = set(); key = None
    for line in data.splitlines(True):
        if key is None:
            m = re.match(rb'([^#\r\n]+)#\s*$', line)
            if m: key = m.group(1).decode('cp949', 'replace')
            out.append(line)
            if key in entries:
                for text in entries[key]:
                    text = check(key, text)
                    out.append((text.translate(MIRROR) if mirrored else text) + b'\r\n')
                seen.add(key)
        elif line.strip() == b'#':
            key = None; out.append(line)
        elif key not in entries:
            out.append(line)
    return b''.join(out), seen

def build(g):
    """{name in the archive: patched table}."""
    t = json.load(open(os.path.join(HERE, 'tables.json'), encoding='utf-8'))
    files = {}
    for part, field, table, kind, mirrored in TABLES:
        entries = {k: v if field is None else v[field] for k, v in t[part].items()}
        entries = {k: v for k, v in entries.items() if v is not None}
        e = g.find(table)
        data, seen = names(g.read(e), entries) if kind == 'names' else descs(g.read(e), entries, mirrored)
        missing = sorted(set(entries) - seen)
        if missing:
            print('  %s has no entry for: %s' % (table, ' '.join(missing)))
        files[e[0]] = data
    return files

if __name__ == '__main__':
    if len(sys.argv) != 2: sys.exit(__doc__)
    for name, data in build(Grf(sys.argv[1])).items():
        print('%8d bytes  %s' % (len(data), name.decode('cp949')))
