import sys
p='preview_array.html'
s=open(p,'r',encoding='utf-8').read()
pairs={'(':')','[':']','{':'}'}
stack=[]
errors=[]
line=1;col=0
for i,ch in enumerate(s):
    if ch=='\n': line+=1; col=0; continue
    col+=1
    if ch in pairs:
        stack.append((ch,line,col))
    elif ch in pairs.values():
        if not stack:
            errors.append(f'unmatched closing {ch} at {line}:{col}')
        else:
            op,ol,oc=stack.pop()
            if pairs[op]!=ch:
                errors.append(f'mismatch {op}@{ol}:{oc} closed by {ch} at {line}:{col}')

# naive quote check
in_s=False; in_d=False; in_b=False; esc=False
line=1;col=0
for ch in s:
    if ch=='\n': line+=1;col=0; esc=False; continue
    col+=1
    if esc:
        esc=False; continue
    if ch=='\\': esc=True; continue
    if ch=="'": in_s=not in_s
    if ch=='"': in_d=not in_d
    if ch=='`': in_b=not in_b

if errors:
    print('BRACKET_ERRORS:')
    for e in errors[:50]: print(' -',e)
else:
    print('No bracket close errors')

if stack:
    print('Unclosed openings (top 20):')
    for op,ln,cn in stack[-20:]: print(f' - {op} opened at {ln}:{cn}')
else:
    print('No unclosed openings')

print('Quotes state: single=',in_s,' double=',in_d,' backtick=',in_b)
print('Length',len(s),'lines approx',s.count('\n')+1)

# print surrounding lines for any last unmatched opening
if stack:
    op,ln,cn=stack[-1]
    print('\nContext around last open:')
    lines=s.splitlines()
    for L in range(max(1,ln-3), min(len(lines),ln+2)+1):
        mark = '>>' if L==ln else '  '
        print(f"{mark} {L:4d}: {lines[L-1]}")

sys.exit(0)
