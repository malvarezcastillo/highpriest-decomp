#!/usr/bin/env python3
"""Korean-to-English string tooling for the translation branch.

  tr.py extract [REV]  list the Korean literals of REV (default main) into strings/
  tr.py apply          rewrite the literals of the working tree that have an "en"
  tr.py status         count translated, kept and open entries; check formats

strings/<source>.json maps "line:column" on REV to {"ko", "en" or "keep", ...}.
"keep" gives the reason a literal stays Korean (a file name, a lookup key).
Literals are CP949; the JSON files are UTF-8.
"""
import bisect, json, os, re, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
STRINGS = os.path.join(HERE, 'strings')
HEX = b'0123456789abcdefABCDEF'
SIMPLE = {ord(k): v for k, v in {'n': 10, 't': 9, 'r': 13, '0': 0, 'a': 7, 'b': 8, 'f': 12, 'v': 11}.items()}
SPEC = re.compile(r'%[-+ #0]*(?:\d+|\*)?(?:\.(?:\d+|\*))?(?:hh|h|ll|l|I64|L)?[diouxXcsfeEgGp%]')


def unescape(b):
    o = bytearray(); i = 0
    while i < len(b):
        c = b[i]
        if c != 92:
            o.append(c); i += 1; continue
        n = b[i + 1]
        if n == ord('x'):
            j = i + 2; v = 0
            while j < len(b) and b[j] in HEX:
                v = v * 16 + int(chr(b[j]), 16); j += 1
            o.append(v & 255); i = j
        elif chr(n) in '01234567':
            j = i + 1; v = 0
            while j < len(b) and j < i + 4 and chr(b[j]) in '01234567':
                v = v * 8 + b[j] - 48; j += 1
            o.append(v & 255); i = j
        else:
            o.append(SIMPLE.get(n, n)); i += 2
    return bytes(o)


def literal(text):
    """A C string literal for text, CP949, safe from long hex escapes and trigraphs."""
    out = ['"']; hexed = False; prev = ''
    for c in text.encode('cp949'):
        ch = chr(c)
        if c >= 0x80 or (c < 0x20 and ch not in '\n\t\r'):
            out.append('\\x%02x' % c); hexed = True; prev = ''; continue
        if hexed and c in HEX:
            out.append('" "')
        hexed = False
        if ch == '?' and prev == '?':
            out.append('\\?')
        else:
            out.append({'"': '\\"', '\\': '\\\\', '\n': '\\n', '\t': '\\t', '\r': '\\r'}.get(ch, ch))
        prev = ch
    return ''.join(out) + '"'


def groups(d):
    """(start, end, text bytes) of each run of adjacent string literals outside
    comments, character literals and preprocessor lines."""
    res = []; i = 0; n = len(d)
    while i < n:
        c = d[i]
        if c == 47 and d[i + 1:i + 2] == b'/':
            j = d.find(b'\n', i); i = n if j < 0 else j
        elif c == 47 and d[i + 1:i + 2] == b'*':
            j = d.find(b'*/', i + 2); i = n if j < 0 else j + 2
        elif c == 39:
            j = i + 1
            while j < n and d[j] != 39:
                j += 2 if d[j] == 92 else 1
            i = j + 1
        elif c == 34:
            j = i + 1
            while j < n and d[j] != 34:
                j += 2 if d[j] == 92 else 1
            ls = d.rfind(b'\n', 0, i) + 1
            if not d[ls:i].lstrip().startswith(b'#'):
                raw = unescape(d[i + 1:j])
                if res and d[res[-1][1]:i].strip() == b'':
                    res[-1] = (res[-1][0], j + 1, res[-1][2] + raw)
                else:
                    res.append((i, j + 1, raw))
            i = j + 1
        else:
            i += 1
    return res


def line_starts(d):
    return [0] + [m.end() for m in re.finditer(b'\n', d)]


def sources(rev):
    out = subprocess.run(['git', '-C', ROOT, 'ls-tree', '-r', '--name-only', rev, 'src'],
                         capture_output=True, text=True, check=True).stdout.split('\n')
    return [f for f in out if f.endswith(('.cpp', '.h', '.c')) and not f.startswith('src/library/')]


def table_path(src):
    return os.path.join(STRINGS, src[4:].replace('/', '__') + '.json')


def load(src):
    p = table_path(src)
    return json.load(open(p, encoding='utf-8')) if os.path.exists(p) else {}


def save(src, table):
    key = lambda k: tuple(int(x) for x in k.split(':'))
    with open(table_path(src), 'w', encoding='utf-8') as f:
        json.dump({k: table[k] for k in sorted(table, key=key)}, f, ensure_ascii=False, indent=1)
        f.write('\n')


def tables():
    for name in sorted(os.listdir(STRINGS)):
        src = 'src/' + name[:-5].replace('__', '/')
        yield src, load(src)


def extract(rev):
    total = 0
    for src in sources(rev):
        d = subprocess.run(['git', '-C', ROOT, 'show', '%s:%s' % (rev, src)], capture_output=True, check=True).stdout
        starts = line_starts(d); old = load(src); table = {}
        for s, e, raw in groups(d):
            if not any(c > 127 for c in raw):
                continue
            ln = bisect.bisect_right(starts, s)
            key = '%d:%d' % (ln, s - starts[ln - 1])
            entry = old.get(key, {})
            try:
                entry['ko'] = raw.decode('cp949')
            except UnicodeDecodeError:
                entry = {'ko': raw.decode('latin-1'), 'keep': 'binary data'}
            table[key] = entry
        if table:
            save(src, table); total += len(table)
    print(total, 'Korean literals')


def apply():
    done = 0; errors = 0
    for src, table in tables():
        path = os.path.join(ROOT, src)
        d = open(path, 'rb').read(); starts = line_starts(d)
        byline = {}
        for g in groups(d):
            byline.setdefault(bisect.bisect_right(starts, g[0]), []).append(g)
        edits = []
        for key, entry in table.items():
            if 'en' not in entry:
                continue
            ln = int(key.split(':')[0]); ko = entry['ko'].encode('cp949'); en = entry['en'].encode('cp949')
            cand = byline.get(ln, [])
            hit = next((g for g in cand if g[2] == ko), None)
            if hit:
                cand.remove(hit)
                edits.append((hit[0], hit[1], literal(entry['en']).encode('ascii') + b'\n' * d.count(b'\n', hit[0], hit[1])))
            else:
                was = next((g for g in cand if g[2] == en), None)
                if was:
                    cand.remove(was)
                else:
                    print('%s:%s: literal not found' % (src, key)); errors += 1
        for s, e, new in sorted(edits, reverse=True):
            d = d[:s] + new + d[e:]
        if edits:
            open(path, 'wb').write(d); done += len(edits)
    print(done, 'literals rewritten,', errors, 'errors')
    return 1 if errors else 0


def status():
    n = {'en': 0, 'keep': 0, 'open': 0}; bad = 0
    for src, table in tables():
        for key, entry in table.items():
            kind = 'en' if 'en' in entry else 'keep' if 'keep' in entry else 'open'
            n[kind] += 1
            if kind != 'en':
                continue
            en = entry['en']
            try:
                en.encode('cp949')
            except UnicodeEncodeError:
                print('%s:%s: not CP949: %s' % (src, key, en)); bad += 1
            if entry.get('format', True) and SPEC.findall(entry['ko']) != SPEC.findall(en):
                print('%s:%s: format differs: %s | %s' % (src, key, entry['ko'], en)); bad += 1
    print('%(en)d translated, %(keep)d kept, %(open)d open' % n, '-', bad, 'problems')
    return 1 if bad else 0


if __name__ == '__main__':
    cmd = sys.argv[1] if len(sys.argv) > 1 else ''
    if cmd == 'extract':
        extract(sys.argv[2] if len(sys.argv) > 2 else 'main')
    elif cmd == 'apply':
        sys.exit(apply())
    elif cmd == 'status':
        sys.exit(status())
    else:
        sys.exit(__doc__)
